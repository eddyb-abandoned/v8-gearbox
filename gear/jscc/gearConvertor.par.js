/*
	Default driver template for JS/CC generated parsers for V8
	
	Features:
	- Parser trace messages
	- Step-by-step parsing
	- Integrated panic-mode error recovery
	- Pseudo-graphical parse tree generation
	
	Written 2007 by Jan Max Meyer, J.M.K S.F. Software Technologies
        Modified 2008 from driver.js_ to support V8 by Louis P.Santillan
			<lpsantil@gmail.com>
	
	This is in the public domain.
*/


/* Gathering utilities */

function createClass(name, childs) {
    var out = {type:'class', name:name, classes:{}, vars:{}, varsNative:{}, functions:{}};
    for(c in childs) {
        var node = childs[c];
        switch(node.type) {
            case 'class':
                out.classes[node.name] = {classes:node.classes, vars:node.vars, varsNative:node.varsNative, functions:node.functions};
                break;
            case 'function':
                if(!out.functions[node.name])
                    out.functions[node.name] = [{args:node.args, code:node.code}];
                else
                    out.functions[node.name].push({args:node.args, code:node.code});
                break;
            case 'native-var':
                out.varsNative[node.name] = {real:node.real, defval:node.defval};
                break;
            case 'var':
                out.vars[node.name] = {real:node.real, defval:(node.defval||{real:node.real, value:baseTypesDefaults[node.real]})};
                break;
            case 'native-block':
                throw Error("TODO: Native blocks in classes");
        }
    }
    
    return out;
}

function createNamespace(name, childs) {
    var out = {type:'namespace', name:name, namespaces:{}, classes:{}, vars:{}, functions:{}, prelude:""};
    for(c in childs) {
        var node = childs[c];
        switch(node.type) {
            case 'namespace':
                out.namespaces[node.name] = {namespaces:node.namespaces, classes:node.classes, vars:node.vars, functions:node.functions, prelude:node.prelude};
                break;
            case 'class':
                out.classes[node.name] = {classes:node.classes, vars:node.vars, varsNative:node.varsNative, functions:node.functions};
                break;
            case 'function':
                if(!out.functions[node.name])
                    out.functions[node.name] = [{args:node.args, code:node.code}];
                else
                    out.functions[node.name].push({args:node.args, code:node.code});
                break;
            case 'native-var':
                throw Error("TODO: Native Vars in namespaces");
                break;
            case 'var':
                out.vars[node.name] = {real:node.real, defval:(node.defval||{real:node.real, value:baseTypesDefaults[node.real]})};
                break;
            case 'native-block':
                if(c == 0)
                    out.prelude += node.code + "\n";
        }
    }
    
    return out;
}

/* Generating code utilities */

var baseTypesDefaults = {int:0, string:"", bool:false};
var baseTypesCtors = {
    int: function(val) {
        return "v8::Integer::New(" + Math.floor(val) + ")";
    },
    string: function(val) {
        return "v8::String::New(\"" + val.replace(/"/g,"\\\"").replace(/\n/g,"\\n").replace(/\t/g,"\\t") + "\")";
    },
    bool: function(val) {
        return "v8::Boolean::New(" + !!val + ")";
    }
};

function makeObjectReplacements(vars, objPath, getCode) {
    var replaces = [];
    for(v in vars) {
        var varName = vars[v].name || v;
        var varPath = "\\b" + objPath.concat([varName]).join("\\b\\.\\b") + "\\b";
        
        if(vars[v].real == "string") {
            replaces.push({regex:varPath+"\\s*=\\s*([^;]*);", replace:getCode+"->V8Set(\""+varName+"\", v8::String::New($1));"});
            replaces.push({regex:varPath+"\\.\\blength\\b", replace:"v8::String::Utf8Value("+getCode+"->V8Get(\""+varName+"\")).length()"});
            replaces.push({regex:varPath, replace:"(*v8::String::Utf8Value("+getCode+"->V8Get(\""+varName+"\")))"});
        } else if(vars[v].real == "int") {
            replaces.push({regex:varPath+"\\s*=\\s*([^;]*);", replace:getCode+"->V8Set(\""+varName+"\", v8::Integer::New($1));"});
            replaces.push({regex:varPath, replace:getCode+"->V8Get(\""+varName+"\")->IntegerValue()"});
        }
        else if(vars[v].real == "bool") {
            replaces.push({regex:varPath+"\\s*=\\s*([^;]*);", replace:getCode+"->V8Set(\""+varName+"\", v8::Boolean::New($1));"});
            replaces.push({regex:varPath, replace:getCode+"->V8Get(\""+varName+"\")->BooleanValue()"});
        }
        else if(vars[v].real.obj)
            replaces = replaces.concat(makeObjectReplacements(vars[v].real.childs, objPath.concat([varName]), getCode+"->V8Get(\""+varName+"\")"));
    }
    return replaces;
}

function makeArgumentReplacements(args) {
    var replaces = [], n = 0;
    for(arg in args) {
        if(args[arg].real == "string") {
            replaces.push({regex:"\\b"+args[arg].name+"\\b\\.\\blength\\b", replace:"v8::String::Utf8Value(args["+n+"]).length()"});
            replaces.push({regex:"\\b"+args[arg].name+"\\b", replace:"(*v8::String::Utf8Value(args["+n+"]))"});
        } else if(args[arg].real == "int")
            replaces.push({regex:"\\b"+args[arg].name+"\\b", replace:"args["+n+"]->IntegerValue()"});
        else if(args[arg].real == "bool")
            replaces.push({regex:"\\b"+args[arg].name+"\\b", replace:"args["+n+"]->BooleanValue()"});
        else if(args[arg].real == "function") {
            replaces.push({regex:"\\b"+args[arg].name+"\\b\\s*\\(\\s*\\)", replace:"V8FuncCall0(args.This(), args["+n+"])"});
            replaces.push({regex:"\\b"+args[arg].name+"\\b\\s*\\(", replace:"V8FuncCall(args.This(), args["+n+"], "});
        } else if(args[arg].real.obj)
            replaces = replaces.concat(makeObjectReplacements(args[arg].real.childs, [args[arg].name], "args["+n+"]->ToObject()"));
        
        n++;
    }
    return replaces;
}

function makeNativeReplacements(natives, objPath, getCode) {
    var replaces = [], n = 0;
    for(_native in natives) {
        var varPath = "\\b" + objPath.concat([_native]).join("\\b\\.\\b") + "\\b";
        if(natives[_native].real.indexOf("*") > -1) {
            replaces.push({regex:varPath+"\\s*=\\s*([^;]*);", replace:getCode+"->SetPointerInInternalField("+n+", $1);"});
            replaces.push({regex:varPath, replace:"("+natives[_native].real+")("+getCode+"->GetPointerFromInternalField("+n+"))"});
        }
        
        n++;
    }
    return replaces;
}

function makeTabs(n, ch) {
    var s = "";
    for(var i = 0; i < n; i++)
        s += ch;
    return s;
}

function generateFunctionCode(functions, name, parentObjName, code, class) {
    var objName = parentObjName + "_" + name, replaces = [], funcCode = "", hasNoArgsVer = false;
    functions.sort(function(a, b) {return b.args.length - a.args.length;});
    for(f in functions) {
        var func = functions[f], replaces = [], tbs = (func.args.length ? "\t\t" : "\t");
        var actualCode = "\n" + tbs + func.code.trim() + "\n";
        
        replaces = replaces.concat(makeArgumentReplacements(func.args));
        if(class) {
            replaces = replaces.concat(makeObjectReplacements(class.vars, ["this"], "args.This()"));
            replaces = replaces.concat(makeNativeReplacements(class.varsNative, ["this"], "args.This()"));
        }
        
        actualCode += tbs + "return undefined;\n";
        replaces.push({regex:"\n" + makeTabs(parentObjName.split("_").length, "    "), replace:"\n" + tbs});
        replaces.push({regex:"\\b(String|Integer|Boolean)\\b\\s*\\(\\s*([^\\)]*)\\s*\\)", replace:"v8::$1::New($2)"});
        replaces.push({regex:"\\bundefined\\b", replace:"v8::Undefined()"});
        //print("===============  " + name + "  ===============");
        //print(JSON.stringify(replaces, 0, 4));
        
        for(r in replaces) {
            var replace = replaces[r];
            actualCode = actualCode.replace(new RegExp(replace.regex, "g"), replace.replace);
        }
        
        if(func.args.length)
            funcCode += "\n\tif(args.Length() >= " + func.args.length + ")\n\t{" + actualCode + "\t}\n";
        else {
            funcCode += actualCode;
            hasNoArgsVer = true;
        }
    }
    
    if(!hasNoArgsVer)
        funcCode += "\tV8Assert(false, \"Invalid call to " + parentObjName.replace(/_/g, ".") + (class?".prototype":"") + "." + name + "\")\n";
    
    code.func += "V8FuncDef(" + objName + ")\n{" + funcCode + "}\n\n";
}

function generateClassCode(class, name, parentObjName, code) {
    var objName = parentObjName + "_" + name;
    
    code.addClass(objName, name);
    
    if(Object.keys(class.varsNative).length)
        code.setNativeVarNum(objName, Object.keys(class.varsNative).length);
    
    for(className in class.classes)
        generateClassCode(class.classes[className], className, objName, code);
    
    for(funcName in class.functions) {
        if(funcName != name)
            code.setPrototype(objName, funcName, "V8Func(" + (objName + "_" + funcName) + ")->GetFunction()");
        generateFunctionCode(class.functions[funcName], funcName, objName, code, class);
    }
    
    for(varName in class.vars) {
        var defval = class.vars[varName].defval;
        code.setPrototype(objName, varName, baseTypesCtors[defval.real](defval.value));
    }
    
    code.setStatic(parentObjName, name, objName + "->GetFunction()");
}

function generateNamespaceCode(namespace, name, parentObjName, code) {
    var objName = parentObjName + "_" + name;
    
    code.addNamespace(objName);
    code.setStatic(parentObjName, name, objName);
    
    for(className in namespace.classes)
        generateClassCode(namespace.classes[className], className, objName, code);
    
    for(funcName in namespace.functions) {
        code.setStatic(objName, funcName, "V8Func(" + (objName + "_" + funcName) + ")->GetFunction()");
        generateFunctionCode(namespace.functions[funcName], funcName, objName, code);
    }
    
    for(varName in namespace.vars) {
        var defval = namespace.vars[varName].defval;
        code.setPrototype(objName, varName, baseTypesCtors[defval.real](defval.value));
    }
}

function generateCode(global) {
    var code = {
        func:"", init:"",
        addNamespace: function(objName) {
            this.init += "\tv8::Handle<v8::Object> " + objName + " = v8::Object::New();\n";
        },
        addClass: function(objName, ctor) {
            this.init += "\tv8::Handle<v8::FunctionTemplate> " + objName + " = V8Func(" + objName + "_" + ctor + ");\n";
        },
        setStatic: function(parentObjName, name, value) {
            this.init += "\t" + parentObjName + "->V8Set(\"" + name + "\", " + value + ");\n";
        },
        setPrototype: function(parentObjName, name, value) {
            this.init += "\t" + parentObjName + "->PrototypeTemplate()->V8Set(\"" + name + "\", " + value + ");\n";
        },
        setNativeVarNum: function(objName, nativeVarNum) {
            this.init += "\t" + objName + "->InstanceTemplate()->SetInternalFieldCount(" + nativeVarNum + ");\n";
        }
    };
    
    var namespaces = Object.keys(global.namespaces);
    
    if(!namespaces.length)
        throw Error("No namespace");
    else if(namespaces.length > 1)
        throw Error("Too may namespaces");
    else {
        generateNamespaceCode(global.namespaces[namespaces[0]], namespaces[0], "global", code);
        
        var genCode = "\n"  + global.prelude.trim().replace(/\n    /g, "\n") + (global.prelude.trim()?"\n\n":"") + code.func + "\nvoid Setup" + namespaces[0] + "(v8::Handle<v8::Object> global)\n{\n" + code.init + "}";
        genCode = genCode.replace(/\t/g, "    ");
        print(genCode);
        return;
    }
    
}



var v8_dbg_withparsetree	= false;
var v8_dbg_withtrace		= false;
var v8_dbg_withstepbystep	= false;

function __v8dbg_print( text )
{
	print( text );
}

function __v8dbg_wait()
{
   var v = read_line();
}

function __v8lex( info )
{
	var state		= 0;
	var match		= -1;
	var match_pos	= 0;
	var start		= 0;
	var pos			= info.offset + 1;

	do
	{
		pos--;
		state = 0;
		match = -2;
		start = pos;

		if( info.src.length <= start )
			return 40;

		do
		{

switch( state )
{
	case 0:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 38 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 9 && info.src.charCodeAt( pos ) <= 10 ) || info.src.charCodeAt( pos ) == 13 || info.src.charCodeAt( pos ) == 32 ) state = 2;
		else if( info.src.charCodeAt( pos ) == 40 ) state = 3;
		else if( info.src.charCodeAt( pos ) == 41 ) state = 4;
		else if( info.src.charCodeAt( pos ) == 42 ) state = 5;
		else if( info.src.charCodeAt( pos ) == 44 ) state = 6;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) ) state = 7;
		else if( info.src.charCodeAt( pos ) == 59 ) state = 8;
		else if( info.src.charCodeAt( pos ) == 61 ) state = 9;
		else if( ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || info.src.charCodeAt( pos ) == 97 || ( info.src.charCodeAt( pos ) >= 100 && info.src.charCodeAt( pos ) <= 101 ) || ( info.src.charCodeAt( pos ) >= 103 && info.src.charCodeAt( pos ) <= 104 ) || ( info.src.charCodeAt( pos ) >= 106 && info.src.charCodeAt( pos ) <= 109 ) || ( info.src.charCodeAt( pos ) >= 112 && info.src.charCodeAt( pos ) <= 114 ) || ( info.src.charCodeAt( pos ) >= 117 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 123 ) state = 11;
		else if( info.src.charCodeAt( pos ) == 125 ) state = 12;
		else if( info.src.charCodeAt( pos ) == 39 ) state = 25;
		else if( info.src.charCodeAt( pos ) == 105 ) state = 40;
		else if( info.src.charCodeAt( pos ) == 98 ) state = 51;
		else if( info.src.charCodeAt( pos ) == 116 ) state = 52;
		else if( info.src.charCodeAt( pos ) == 99 ) state = 60;
		else if( info.src.charCodeAt( pos ) == 102 ) state = 61;
		else if( info.src.charCodeAt( pos ) == 110 ) state = 67;
		else if( info.src.charCodeAt( pos ) == 111 ) state = 68;
		else if( info.src.charCodeAt( pos ) == 115 ) state = 69;
		else state = -1;
		break;

	case 1:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 58 ) || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 122 ) || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else state = -1;
		match = 23;
		match_pos = pos;
		break;

	case 2:
		if( ( info.src.charCodeAt( pos ) >= 9 && info.src.charCodeAt( pos ) <= 10 ) || info.src.charCodeAt( pos ) == 13 || info.src.charCodeAt( pos ) == 32 ) state = 2;
		else state = -1;
		match = 1;
		match_pos = pos;
		break;

	case 3:
		state = -1;
		match = 14;
		match_pos = pos;
		break;

	case 4:
		state = -1;
		match = 15;
		match_pos = pos;
		break;

	case 5:
		state = -1;
		match = 19;
		match_pos = pos;
		break;

	case 6:
		state = -1;
		match = 18;
		match_pos = pos;
		break;

	case 7:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 122 ) || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) ) state = 7;
		else state = -1;
		match = 22;
		match_pos = pos;
		break;

	case 8:
		state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 9:
		state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 10:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 11:
		state = -1;
		match = 12;
		match_pos = pos;
		break;

	case 12:
		state = -1;
		match = 13;
		match_pos = pos;
		break;

	case 13:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 58 ) || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 122 ) || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else state = -1;
		match = 21;
		match_pos = pos;
		break;

	case 14:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 5;
		match_pos = pos;
		break;

	case 15:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 7;
		match_pos = pos;
		break;

	case 16:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 9;
		match_pos = pos;
		break;

	case 17:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 3;
		match_pos = pos;
		break;

	case 18:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 10;
		match_pos = pos;
		break;

	case 19:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 4;
		match_pos = pos;
		break;

	case 20:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 8;
		match_pos = pos;
		break;

	case 21:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 6;
		match_pos = pos;
		break;

	case 22:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 11;
		match_pos = pos;
		break;

	case 23:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 2;
		match_pos = pos;
		break;

	case 24:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 38 ) || ( info.src.charCodeAt( pos ) >= 40 && info.src.charCodeAt( pos ) <= 91 ) || ( info.src.charCodeAt( pos ) >= 93 && info.src.charCodeAt( pos ) <= 254 ) ) state = 24;
		else if( info.src.charCodeAt( pos ) == 39 ) state = 27;
		else if( info.src.charCodeAt( pos ) == 92 ) state = 41;
		else state = -1;
		break;

	case 25:
		if( info.src.charCodeAt( pos ) == 39 ) state = 13;
		else if( ( info.src.charCodeAt( pos ) >= 9 && info.src.charCodeAt( pos ) <= 10 ) || info.src.charCodeAt( pos ) == 13 || info.src.charCodeAt( pos ) == 32 || ( info.src.charCodeAt( pos ) >= 40 && info.src.charCodeAt( pos ) <= 42 ) || info.src.charCodeAt( pos ) == 44 || info.src.charCodeAt( pos ) == 59 || info.src.charCodeAt( pos ) == 61 || info.src.charCodeAt( pos ) == 123 || info.src.charCodeAt( pos ) == 125 ) state = 24;
		else if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 38 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 58 ) || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 91 ) || ( info.src.charCodeAt( pos ) >= 93 && info.src.charCodeAt( pos ) <= 122 ) || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 25;
		else if( info.src.charCodeAt( pos ) == 92 ) state = 28;
		else state = -1;
		match = 23;
		match_pos = pos;
		break;

	case 26:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 115 ) || ( info.src.charCodeAt( pos ) >= 117 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 116 ) state = 14;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 27:
		state = -1;
		match = 21;
		match_pos = pos;
		break;

	case 28:
		if( ( info.src.charCodeAt( pos ) >= 9 && info.src.charCodeAt( pos ) <= 10 ) || info.src.charCodeAt( pos ) == 13 || info.src.charCodeAt( pos ) == 32 || ( info.src.charCodeAt( pos ) >= 40 && info.src.charCodeAt( pos ) <= 42 ) || info.src.charCodeAt( pos ) == 44 || info.src.charCodeAt( pos ) == 59 || info.src.charCodeAt( pos ) == 61 || info.src.charCodeAt( pos ) == 123 || info.src.charCodeAt( pos ) == 125 ) state = 24;
		else if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 38 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 58 ) || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 91 ) || ( info.src.charCodeAt( pos ) >= 93 && info.src.charCodeAt( pos ) <= 122 ) || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 25;
		else if( info.src.charCodeAt( pos ) == 92 ) state = 28;
		else if( info.src.charCodeAt( pos ) == 39 ) state = 30;
		else state = -1;
		match = 23;
		match_pos = pos;
		break;

	case 29:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 107 ) || ( info.src.charCodeAt( pos ) >= 109 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 108 ) state = 15;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 30:
		if( info.src.charCodeAt( pos ) == 39 ) state = 13;
		else if( ( info.src.charCodeAt( pos ) >= 9 && info.src.charCodeAt( pos ) <= 10 ) || info.src.charCodeAt( pos ) == 13 || info.src.charCodeAt( pos ) == 32 || ( info.src.charCodeAt( pos ) >= 40 && info.src.charCodeAt( pos ) <= 42 ) || info.src.charCodeAt( pos ) == 44 || info.src.charCodeAt( pos ) == 59 || info.src.charCodeAt( pos ) == 61 || info.src.charCodeAt( pos ) == 123 || info.src.charCodeAt( pos ) == 125 ) state = 24;
		else if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 38 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 58 ) || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 91 ) || ( info.src.charCodeAt( pos ) >= 93 && info.src.charCodeAt( pos ) <= 122 ) || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 25;
		else if( info.src.charCodeAt( pos ) == 92 ) state = 28;
		else state = -1;
		match = 21;
		match_pos = pos;
		break;

	case 31:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 100 ) || ( info.src.charCodeAt( pos ) >= 102 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 101 ) state = 16;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 32:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 38 ) || ( info.src.charCodeAt( pos ) >= 40 && info.src.charCodeAt( pos ) <= 91 ) || ( info.src.charCodeAt( pos ) >= 93 && info.src.charCodeAt( pos ) <= 254 ) ) state = 24;
		else if( info.src.charCodeAt( pos ) == 39 ) state = 27;
		else if( info.src.charCodeAt( pos ) == 92 ) state = 41;
		else state = -1;
		match = 21;
		match_pos = pos;
		break;

	case 33:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 114 ) || ( info.src.charCodeAt( pos ) >= 116 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 115 ) state = 17;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 34:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 100 ) || ( info.src.charCodeAt( pos ) >= 102 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 101 ) state = 18;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 35:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 100 ) || ( info.src.charCodeAt( pos ) >= 102 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 101 ) state = 19;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 36:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 115 ) || ( info.src.charCodeAt( pos ) >= 117 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 116 ) state = 20;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 37:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 102 ) || ( info.src.charCodeAt( pos ) >= 104 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 103 ) state = 21;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 38:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 109 ) || ( info.src.charCodeAt( pos ) >= 111 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 110 ) state = 22;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 39:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 100 ) || ( info.src.charCodeAt( pos ) >= 102 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 101 ) state = 23;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 40:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 109 ) || ( info.src.charCodeAt( pos ) >= 111 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 110 ) state = 26;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 41:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 38 ) || ( info.src.charCodeAt( pos ) >= 40 && info.src.charCodeAt( pos ) <= 91 ) || ( info.src.charCodeAt( pos ) >= 93 && info.src.charCodeAt( pos ) <= 254 ) ) state = 24;
		else if( info.src.charCodeAt( pos ) == 39 ) state = 32;
		else if( info.src.charCodeAt( pos ) == 92 ) state = 41;
		else state = -1;
		break;

	case 42:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 110 ) || ( info.src.charCodeAt( pos ) >= 112 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 111 ) state = 29;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 43:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 116 ) || ( info.src.charCodeAt( pos ) >= 118 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 117 ) state = 31;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 44:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 114 ) || ( info.src.charCodeAt( pos ) >= 116 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 115 ) state = 33;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 45:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 114 ) || ( info.src.charCodeAt( pos ) >= 116 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 115 ) state = 34;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 46:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 117 ) || ( info.src.charCodeAt( pos ) >= 119 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 118 ) state = 35;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 47:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 98 ) || ( info.src.charCodeAt( pos ) >= 100 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 99 ) state = 36;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 48:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 109 ) || ( info.src.charCodeAt( pos ) >= 111 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 110 ) state = 37;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 49:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 110 ) || ( info.src.charCodeAt( pos ) >= 112 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 111 ) state = 38;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 50:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 98 ) || ( info.src.charCodeAt( pos ) >= 100 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 99 ) state = 39;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 51:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 110 ) || ( info.src.charCodeAt( pos ) >= 112 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 111 ) state = 42;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 52:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 113 ) || ( info.src.charCodeAt( pos ) >= 115 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 114 ) state = 43;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 53:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 98 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 97 ) state = 44;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 54:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 107 ) || ( info.src.charCodeAt( pos ) >= 109 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 108 ) state = 45;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 55:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 104 ) || ( info.src.charCodeAt( pos ) >= 106 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 105 ) state = 46;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 56:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 100 ) || ( info.src.charCodeAt( pos ) >= 102 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 101 ) state = 47;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 57:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 104 ) || ( info.src.charCodeAt( pos ) >= 106 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 105 ) state = 48;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 58:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 104 ) || ( info.src.charCodeAt( pos ) >= 106 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 105 ) state = 49;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 59:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 98 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 97 ) state = 50;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 60:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 107 ) || ( info.src.charCodeAt( pos ) >= 109 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 108 ) state = 53;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 61:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 98 && info.src.charCodeAt( pos ) <= 116 ) || ( info.src.charCodeAt( pos ) >= 118 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 97 ) state = 54;
		else if( info.src.charCodeAt( pos ) == 117 ) state = 72;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 62:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 108 ) || ( info.src.charCodeAt( pos ) >= 110 && info.src.charCodeAt( pos ) <= 115 ) || ( info.src.charCodeAt( pos ) >= 117 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 116 ) state = 55;
		else if( info.src.charCodeAt( pos ) == 109 ) state = 73;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 63:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 105 ) || ( info.src.charCodeAt( pos ) >= 107 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 106 ) state = 56;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 64:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 113 ) || ( info.src.charCodeAt( pos ) >= 115 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 114 ) state = 57;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 65:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 115 ) || ( info.src.charCodeAt( pos ) >= 117 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 116 ) state = 58;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 66:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 111 ) || ( info.src.charCodeAt( pos ) >= 113 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 112 ) state = 59;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 67:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 98 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 97 ) state = 62;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 68:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || info.src.charCodeAt( pos ) == 97 || ( info.src.charCodeAt( pos ) >= 99 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 98 ) state = 63;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 69:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 115 ) || ( info.src.charCodeAt( pos ) >= 117 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 116 ) state = 64;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 70:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 98 ) || ( info.src.charCodeAt( pos ) >= 100 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 99 ) state = 65;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 71:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 114 ) || ( info.src.charCodeAt( pos ) >= 116 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 115 ) state = 66;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 72:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 109 ) || ( info.src.charCodeAt( pos ) >= 111 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 110 ) state = 70;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

	case 73:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 100 ) || ( info.src.charCodeAt( pos ) >= 102 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 101 ) state = 71;
		else state = -1;
		match = 20;
		match_pos = pos;
		break;

}


			pos++;

		}
		while( state > -1 );

	}
	while( -1 > -1 && match == -1 );

	if( match > -1 )
	{
		info.att = info.src.substr( start, match_pos - start );
		info.offset = match_pos;
		
switch( match )
{
	case 21:
		{
		 info.att = info.att.substr(1, info.att.length - 2).replace(/\\\'/g, "'" ); 
		}
		break;

}


	}
	else
	{
		info.att = new String();
		match = -1;
	}

	return match;
}


function __v8parse( src, err_off, err_la )
{
	var		sstack			= new Array();
	var		vstack			= new Array();
	var 	err_cnt			= 0;
	var		act;
	var		go;
	var		la;
	var		rval;
	var 	parseinfo		= new Function( "", "var offset; var src; var att;" );
	var		info			= new parseinfo();
	
	//Visual parse tree generation
	var 	treenode		= new Function( "", "var sym; var att; var child;" );
	var		treenodes		= new Array();
	var		tree			= new Array();
	var		tmptree			= null;

/* Pop-Table */
var pop_tab = new Array(
	new Array( 0/* Global' */, 1 ),
	new Array( 25/* Global */, 1 ),
	new Array( 27/* Namespace */, 9 ),
	new Array( 24/* NamespaceContents */, 4 ),
	new Array( 24/* NamespaceContents */, 1 ),
	new Array( 28/* NamespaceContent */, 1 ),
	new Array( 28/* NamespaceContent */, 1 ),
	new Array( 28/* NamespaceContent */, 1 ),
	new Array( 28/* NamespaceContent */, 1 ),
	new Array( 28/* NamespaceContent */, 1 ),
	new Array( 29/* Class */, 9 ),
	new Array( 33/* ClassContents */, 4 ),
	new Array( 33/* ClassContents */, 1 ),
	new Array( 34/* ClassContent */, 1 ),
	new Array( 34/* ClassContent */, 1 ),
	new Array( 34/* ClassContent */, 1 ),
	new Array( 34/* ClassContent */, 1 ),
	new Array( 30/* VariableDef */, 5 ),
	new Array( 30/* VariableDef */, 9 ),
	new Array( 35/* Variable */, 3 ),
	new Array( 35/* Variable */, 5 ),
	new Array( 35/* Variable */, 1 ),
	new Array( 35/* Variable */, 1 ),
	new Array( 35/* Variable */, 1 ),
	new Array( 35/* Variable */, 1 ),
	new Array( 35/* Variable */, 7 ),
	new Array( 36/* Immediate */, 1 ),
	new Array( 36/* Immediate */, 1 ),
	new Array( 36/* Immediate */, 1 ),
	new Array( 36/* Immediate */, 1 ),
	new Array( 31/* Function */, 13 ),
	new Array( 31/* Function */, 11 ),
	new Array( 37/* ArgumentList */, 7 ),
	new Array( 37/* ArgumentList */, 3 ),
	new Array( 32/* NativeBlock */, 5 ),
	new Array( 38/* NativeCode */, 2 ),
	new Array( 38/* NativeCode */, 0 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 39/* PossibleJunk */, 1 ),
	new Array( 26/* W */, 1 ),
	new Array( 26/* W */, 0 )
);

/* Action-Table */
var act_tab = new Array(
	/* State 0 */ new Array( 1/* "WHTS" */,4 , 40/* "$" */,-60 , 2/* "namespace" */,-60 , 3/* "class" */,-60 , 11/* "function" */,-60 , 4/* "native" */,-60 , 5/* "int" */,-60 , 6/* "string" */,-60 , 7/* "bool" */,-60 , 8/* "object" */,-60 ),
	/* State 1 */ new Array( 40/* "$" */,0 ),
	/* State 2 */ new Array( 1/* "WHTS" */,4 , 40/* "$" */,-1 , 2/* "namespace" */,-60 , 3/* "class" */,-60 , 11/* "function" */,-60 , 4/* "native" */,-60 , 5/* "int" */,-60 , 6/* "string" */,-60 , 7/* "bool" */,-60 , 8/* "object" */,-60 ),
	/* State 3 */ new Array( 40/* "$" */,-4 , 1/* "WHTS" */,-4 , 2/* "namespace" */,-4 , 3/* "class" */,-4 , 11/* "function" */,-4 , 4/* "native" */,-4 , 5/* "int" */,-4 , 6/* "string" */,-4 , 7/* "bool" */,-4 , 8/* "object" */,-4 , 13/* "}" */,-4 ),
	/* State 4 */ new Array( 40/* "$" */,-59 , 1/* "WHTS" */,-59 , 2/* "namespace" */,-59 , 3/* "class" */,-59 , 11/* "function" */,-59 , 4/* "native" */,-59 , 5/* "int" */,-59 , 6/* "string" */,-59 , 7/* "bool" */,-59 , 8/* "object" */,-59 , 20/* "Identifier" */,-59 , 12/* "{" */,-59 , 14/* "(" */,-59 , 17/* ";" */,-59 , 16/* "=" */,-59 , 19/* "*" */,-59 , 13/* "}" */,-59 , 23/* "Junk" */,-59 , 9/* "true" */,-59 , 10/* "false" */,-59 , 15/* ")" */,-59 , 18/* "," */,-59 , 21/* "String" */,-59 , 22/* "Integer" */,-59 ),
	/* State 5 */ new Array( 2/* "namespace" */,12 , 3/* "class" */,13 , 11/* "function" */,15 , 4/* "native" */,16 , 5/* "int" */,17 , 6/* "string" */,18 , 7/* "bool" */,19 , 8/* "object" */,20 ),
	/* State 6 */ new Array( 1/* "WHTS" */,4 , 40/* "$" */,-60 , 2/* "namespace" */,-60 , 3/* "class" */,-60 , 11/* "function" */,-60 , 4/* "native" */,-60 , 5/* "int" */,-60 , 6/* "string" */,-60 , 7/* "bool" */,-60 , 8/* "object" */,-60 ),
	/* State 7 */ new Array( 1/* "WHTS" */,-5 , 40/* "$" */,-5 , 2/* "namespace" */,-5 , 3/* "class" */,-5 , 11/* "function" */,-5 , 4/* "native" */,-5 , 5/* "int" */,-5 , 6/* "string" */,-5 , 7/* "bool" */,-5 , 8/* "object" */,-5 , 13/* "}" */,-5 ),
	/* State 8 */ new Array( 1/* "WHTS" */,-6 , 40/* "$" */,-6 , 2/* "namespace" */,-6 , 3/* "class" */,-6 , 11/* "function" */,-6 , 4/* "native" */,-6 , 5/* "int" */,-6 , 6/* "string" */,-6 , 7/* "bool" */,-6 , 8/* "object" */,-6 , 13/* "}" */,-6 ),
	/* State 9 */ new Array( 1/* "WHTS" */,-7 , 40/* "$" */,-7 , 2/* "namespace" */,-7 , 3/* "class" */,-7 , 11/* "function" */,-7 , 4/* "native" */,-7 , 5/* "int" */,-7 , 6/* "string" */,-7 , 7/* "bool" */,-7 , 8/* "object" */,-7 , 13/* "}" */,-7 ),
	/* State 10 */ new Array( 1/* "WHTS" */,-8 , 40/* "$" */,-8 , 2/* "namespace" */,-8 , 3/* "class" */,-8 , 11/* "function" */,-8 , 4/* "native" */,-8 , 5/* "int" */,-8 , 6/* "string" */,-8 , 7/* "bool" */,-8 , 8/* "object" */,-8 , 13/* "}" */,-8 ),
	/* State 11 */ new Array( 1/* "WHTS" */,-9 , 40/* "$" */,-9 , 2/* "namespace" */,-9 , 3/* "class" */,-9 , 11/* "function" */,-9 , 4/* "native" */,-9 , 5/* "int" */,-9 , 6/* "string" */,-9 , 7/* "bool" */,-9 , 8/* "object" */,-9 , 13/* "}" */,-9 ),
	/* State 12 */ new Array( 1/* "WHTS" */,4 , 20/* "Identifier" */,-60 ),
	/* State 13 */ new Array( 1/* "WHTS" */,4 , 20/* "Identifier" */,-60 ),
	/* State 14 */ new Array( 1/* "WHTS" */,4 , 20/* "Identifier" */,-60 ),
	/* State 15 */ new Array( 1/* "WHTS" */,4 , 20/* "Identifier" */,-24 ),
	/* State 16 */ new Array( 1/* "WHTS" */,4 , 12/* "{" */,-60 , 20/* "Identifier" */,-60 ),
	/* State 17 */ new Array( 1/* "WHTS" */,-21 , 20/* "Identifier" */,-21 ),
	/* State 18 */ new Array( 1/* "WHTS" */,-22 , 20/* "Identifier" */,-22 ),
	/* State 19 */ new Array( 1/* "WHTS" */,-23 , 20/* "Identifier" */,-23 ),
	/* State 20 */ new Array( 1/* "WHTS" */,4 , 14/* "(" */,-60 ),
	/* State 21 */ new Array( 40/* "$" */,-3 , 1/* "WHTS" */,-3 , 2/* "namespace" */,-3 , 3/* "class" */,-3 , 11/* "function" */,-3 , 4/* "native" */,-3 , 5/* "int" */,-3 , 6/* "string" */,-3 , 7/* "bool" */,-3 , 8/* "object" */,-3 , 13/* "}" */,-3 ),
	/* State 22 */ new Array( 20/* "Identifier" */,28 ),
	/* State 23 */ new Array( 20/* "Identifier" */,29 ),
	/* State 24 */ new Array( 20/* "Identifier" */,30 ),
	/* State 25 */ new Array( 20/* "Identifier" */,31 ),
	/* State 26 */ new Array( 12/* "{" */,32 , 20/* "Identifier" */,33 ),
	/* State 27 */ new Array( 14/* "(" */,34 ),
	/* State 28 */ new Array( 1/* "WHTS" */,4 , 12/* "{" */,-60 ),
	/* State 29 */ new Array( 1/* "WHTS" */,4 , 12/* "{" */,-60 ),
	/* State 30 */ new Array( 1/* "WHTS" */,4 , 17/* ";" */,-60 , 16/* "=" */,-60 ),
	/* State 31 */ new Array( 1/* "WHTS" */,4 , 14/* "(" */,-60 ),
	/* State 32 */ new Array( 13/* "}" */,-36 , 23/* "Junk" */,-36 , 2/* "namespace" */,-36 , 3/* "class" */,-36 , 4/* "native" */,-36 , 5/* "int" */,-36 , 6/* "string" */,-36 , 7/* "bool" */,-36 , 9/* "true" */,-36 , 10/* "false" */,-36 , 11/* "function" */,-36 , 12/* "{" */,-36 , 14/* "(" */,-36 , 15/* ")" */,-36 , 16/* "=" */,-36 , 17/* ";" */,-36 , 18/* "," */,-36 , 19/* "*" */,-36 , 20/* "Identifier" */,-36 , 21/* "String" */,-36 , 22/* "Integer" */,-36 , 1/* "WHTS" */,-36 ),
	/* State 33 */ new Array( 1/* "WHTS" */,4 , 20/* "Identifier" */,-19 , 19/* "*" */,-60 ),
	/* State 34 */ new Array( 1/* "WHTS" */,4 , 4/* "native" */,-60 , 5/* "int" */,-60 , 6/* "string" */,-60 , 7/* "bool" */,-60 , 11/* "function" */,-60 , 8/* "object" */,-60 ),
	/* State 35 */ new Array( 12/* "{" */,42 ),
	/* State 36 */ new Array( 12/* "{" */,43 ),
	/* State 37 */ new Array( 17/* ";" */,44 , 16/* "=" */,45 ),
	/* State 38 */ new Array( 14/* "(" */,46 ),
	/* State 39 */ new Array( 13/* "}" */,48 , 23/* "Junk" */,49 , 2/* "namespace" */,50 , 3/* "class" */,51 , 4/* "native" */,52 , 5/* "int" */,53 , 6/* "string" */,54 , 7/* "bool" */,55 , 9/* "true" */,56 , 10/* "false" */,57 , 11/* "function" */,58 , 12/* "{" */,59 , 14/* "(" */,60 , 15/* ")" */,61 , 16/* "=" */,62 , 17/* ";" */,63 , 18/* "," */,64 , 19/* "*" */,65 , 20/* "Identifier" */,66 , 21/* "String" */,67 , 22/* "Integer" */,68 , 1/* "WHTS" */,4 ),
	/* State 40 */ new Array( 19/* "*" */,70 ),
	/* State 41 */ new Array( 4/* "native" */,73 , 5/* "int" */,17 , 6/* "string" */,18 , 7/* "bool" */,19 , 11/* "function" */,74 , 8/* "object" */,20 ),
	/* State 42 */ new Array( 1/* "WHTS" */,4 , 2/* "namespace" */,-60 , 3/* "class" */,-60 , 11/* "function" */,-60 , 4/* "native" */,-60 , 5/* "int" */,-60 , 6/* "string" */,-60 , 7/* "bool" */,-60 , 8/* "object" */,-60 , 13/* "}" */,-60 ),
	/* State 43 */ new Array( 1/* "WHTS" */,4 , 3/* "class" */,-60 , 11/* "function" */,-60 , 4/* "native" */,-60 , 5/* "int" */,-60 , 6/* "string" */,-60 , 7/* "bool" */,-60 , 8/* "object" */,-60 , 13/* "}" */,-60 ),
	/* State 44 */ new Array( 1/* "WHTS" */,-17 , 40/* "$" */,-17 , 2/* "namespace" */,-17 , 3/* "class" */,-17 , 11/* "function" */,-17 , 4/* "native" */,-17 , 5/* "int" */,-17 , 6/* "string" */,-17 , 7/* "bool" */,-17 , 8/* "object" */,-17 , 13/* "}" */,-17 ),
	/* State 45 */ new Array( 1/* "WHTS" */,4 , 21/* "String" */,-60 , 22/* "Integer" */,-60 , 9/* "true" */,-60 , 10/* "false" */,-60 ),
	/* State 46 */ new Array( 1/* "WHTS" */,4 , 4/* "native" */,-60 , 5/* "int" */,-60 , 6/* "string" */,-60 , 7/* "bool" */,-60 , 11/* "function" */,-60 , 8/* "object" */,-60 , 15/* ")" */,-60 ),
	/* State 47 */ new Array( 13/* "}" */,-35 , 23/* "Junk" */,-35 , 2/* "namespace" */,-35 , 3/* "class" */,-35 , 4/* "native" */,-35 , 5/* "int" */,-35 , 6/* "string" */,-35 , 7/* "bool" */,-35 , 9/* "true" */,-35 , 10/* "false" */,-35 , 11/* "function" */,-35 , 12/* "{" */,-35 , 14/* "(" */,-35 , 15/* ")" */,-35 , 16/* "=" */,-35 , 17/* ";" */,-35 , 18/* "," */,-35 , 19/* "*" */,-35 , 20/* "Identifier" */,-35 , 21/* "String" */,-35 , 22/* "Integer" */,-35 , 1/* "WHTS" */,-35 ),
	/* State 48 */ new Array( 1/* "WHTS" */,-34 , 40/* "$" */,-34 , 2/* "namespace" */,-34 , 3/* "class" */,-34 , 11/* "function" */,-34 , 4/* "native" */,-34 , 5/* "int" */,-34 , 6/* "string" */,-34 , 7/* "bool" */,-34 , 8/* "object" */,-34 , 13/* "}" */,-34 , 23/* "Junk" */,-48 , 9/* "true" */,-48 , 10/* "false" */,-48 , 12/* "{" */,-48 , 14/* "(" */,-48 , 15/* ")" */,-48 , 16/* "=" */,-48 , 17/* ";" */,-48 , 18/* "," */,-48 , 19/* "*" */,-48 , 20/* "Identifier" */,-48 , 21/* "String" */,-48 , 22/* "Integer" */,-48 ),
	/* State 49 */ new Array( 13/* "}" */,-37 , 23/* "Junk" */,-37 , 2/* "namespace" */,-37 , 3/* "class" */,-37 , 4/* "native" */,-37 , 5/* "int" */,-37 , 6/* "string" */,-37 , 7/* "bool" */,-37 , 9/* "true" */,-37 , 10/* "false" */,-37 , 11/* "function" */,-37 , 12/* "{" */,-37 , 14/* "(" */,-37 , 15/* ")" */,-37 , 16/* "=" */,-37 , 17/* ";" */,-37 , 18/* "," */,-37 , 19/* "*" */,-37 , 20/* "Identifier" */,-37 , 21/* "String" */,-37 , 22/* "Integer" */,-37 , 1/* "WHTS" */,-37 ),
	/* State 50 */ new Array( 13/* "}" */,-38 , 23/* "Junk" */,-38 , 2/* "namespace" */,-38 , 3/* "class" */,-38 , 4/* "native" */,-38 , 5/* "int" */,-38 , 6/* "string" */,-38 , 7/* "bool" */,-38 , 9/* "true" */,-38 , 10/* "false" */,-38 , 11/* "function" */,-38 , 12/* "{" */,-38 , 14/* "(" */,-38 , 15/* ")" */,-38 , 16/* "=" */,-38 , 17/* ";" */,-38 , 18/* "," */,-38 , 19/* "*" */,-38 , 20/* "Identifier" */,-38 , 21/* "String" */,-38 , 22/* "Integer" */,-38 , 1/* "WHTS" */,-38 ),
	/* State 51 */ new Array( 13/* "}" */,-39 , 23/* "Junk" */,-39 , 2/* "namespace" */,-39 , 3/* "class" */,-39 , 4/* "native" */,-39 , 5/* "int" */,-39 , 6/* "string" */,-39 , 7/* "bool" */,-39 , 9/* "true" */,-39 , 10/* "false" */,-39 , 11/* "function" */,-39 , 12/* "{" */,-39 , 14/* "(" */,-39 , 15/* ")" */,-39 , 16/* "=" */,-39 , 17/* ";" */,-39 , 18/* "," */,-39 , 19/* "*" */,-39 , 20/* "Identifier" */,-39 , 21/* "String" */,-39 , 22/* "Integer" */,-39 , 1/* "WHTS" */,-39 ),
	/* State 52 */ new Array( 13/* "}" */,-40 , 23/* "Junk" */,-40 , 2/* "namespace" */,-40 , 3/* "class" */,-40 , 4/* "native" */,-40 , 5/* "int" */,-40 , 6/* "string" */,-40 , 7/* "bool" */,-40 , 9/* "true" */,-40 , 10/* "false" */,-40 , 11/* "function" */,-40 , 12/* "{" */,-40 , 14/* "(" */,-40 , 15/* ")" */,-40 , 16/* "=" */,-40 , 17/* ";" */,-40 , 18/* "," */,-40 , 19/* "*" */,-40 , 20/* "Identifier" */,-40 , 21/* "String" */,-40 , 22/* "Integer" */,-40 , 1/* "WHTS" */,-40 ),
	/* State 53 */ new Array( 13/* "}" */,-41 , 23/* "Junk" */,-41 , 2/* "namespace" */,-41 , 3/* "class" */,-41 , 4/* "native" */,-41 , 5/* "int" */,-41 , 6/* "string" */,-41 , 7/* "bool" */,-41 , 9/* "true" */,-41 , 10/* "false" */,-41 , 11/* "function" */,-41 , 12/* "{" */,-41 , 14/* "(" */,-41 , 15/* ")" */,-41 , 16/* "=" */,-41 , 17/* ";" */,-41 , 18/* "," */,-41 , 19/* "*" */,-41 , 20/* "Identifier" */,-41 , 21/* "String" */,-41 , 22/* "Integer" */,-41 , 1/* "WHTS" */,-41 ),
	/* State 54 */ new Array( 13/* "}" */,-42 , 23/* "Junk" */,-42 , 2/* "namespace" */,-42 , 3/* "class" */,-42 , 4/* "native" */,-42 , 5/* "int" */,-42 , 6/* "string" */,-42 , 7/* "bool" */,-42 , 9/* "true" */,-42 , 10/* "false" */,-42 , 11/* "function" */,-42 , 12/* "{" */,-42 , 14/* "(" */,-42 , 15/* ")" */,-42 , 16/* "=" */,-42 , 17/* ";" */,-42 , 18/* "," */,-42 , 19/* "*" */,-42 , 20/* "Identifier" */,-42 , 21/* "String" */,-42 , 22/* "Integer" */,-42 , 1/* "WHTS" */,-42 ),
	/* State 55 */ new Array( 13/* "}" */,-43 , 23/* "Junk" */,-43 , 2/* "namespace" */,-43 , 3/* "class" */,-43 , 4/* "native" */,-43 , 5/* "int" */,-43 , 6/* "string" */,-43 , 7/* "bool" */,-43 , 9/* "true" */,-43 , 10/* "false" */,-43 , 11/* "function" */,-43 , 12/* "{" */,-43 , 14/* "(" */,-43 , 15/* ")" */,-43 , 16/* "=" */,-43 , 17/* ";" */,-43 , 18/* "," */,-43 , 19/* "*" */,-43 , 20/* "Identifier" */,-43 , 21/* "String" */,-43 , 22/* "Integer" */,-43 , 1/* "WHTS" */,-43 ),
	/* State 56 */ new Array( 13/* "}" */,-44 , 23/* "Junk" */,-44 , 2/* "namespace" */,-44 , 3/* "class" */,-44 , 4/* "native" */,-44 , 5/* "int" */,-44 , 6/* "string" */,-44 , 7/* "bool" */,-44 , 9/* "true" */,-44 , 10/* "false" */,-44 , 11/* "function" */,-44 , 12/* "{" */,-44 , 14/* "(" */,-44 , 15/* ")" */,-44 , 16/* "=" */,-44 , 17/* ";" */,-44 , 18/* "," */,-44 , 19/* "*" */,-44 , 20/* "Identifier" */,-44 , 21/* "String" */,-44 , 22/* "Integer" */,-44 , 1/* "WHTS" */,-44 ),
	/* State 57 */ new Array( 13/* "}" */,-45 , 23/* "Junk" */,-45 , 2/* "namespace" */,-45 , 3/* "class" */,-45 , 4/* "native" */,-45 , 5/* "int" */,-45 , 6/* "string" */,-45 , 7/* "bool" */,-45 , 9/* "true" */,-45 , 10/* "false" */,-45 , 11/* "function" */,-45 , 12/* "{" */,-45 , 14/* "(" */,-45 , 15/* ")" */,-45 , 16/* "=" */,-45 , 17/* ";" */,-45 , 18/* "," */,-45 , 19/* "*" */,-45 , 20/* "Identifier" */,-45 , 21/* "String" */,-45 , 22/* "Integer" */,-45 , 1/* "WHTS" */,-45 ),
	/* State 58 */ new Array( 13/* "}" */,-46 , 23/* "Junk" */,-46 , 2/* "namespace" */,-46 , 3/* "class" */,-46 , 4/* "native" */,-46 , 5/* "int" */,-46 , 6/* "string" */,-46 , 7/* "bool" */,-46 , 9/* "true" */,-46 , 10/* "false" */,-46 , 11/* "function" */,-46 , 12/* "{" */,-46 , 14/* "(" */,-46 , 15/* ")" */,-46 , 16/* "=" */,-46 , 17/* ";" */,-46 , 18/* "," */,-46 , 19/* "*" */,-46 , 20/* "Identifier" */,-46 , 21/* "String" */,-46 , 22/* "Integer" */,-46 , 1/* "WHTS" */,-46 ),
	/* State 59 */ new Array( 13/* "}" */,-47 , 23/* "Junk" */,-47 , 2/* "namespace" */,-47 , 3/* "class" */,-47 , 4/* "native" */,-47 , 5/* "int" */,-47 , 6/* "string" */,-47 , 7/* "bool" */,-47 , 9/* "true" */,-47 , 10/* "false" */,-47 , 11/* "function" */,-47 , 12/* "{" */,-47 , 14/* "(" */,-47 , 15/* ")" */,-47 , 16/* "=" */,-47 , 17/* ";" */,-47 , 18/* "," */,-47 , 19/* "*" */,-47 , 20/* "Identifier" */,-47 , 21/* "String" */,-47 , 22/* "Integer" */,-47 , 1/* "WHTS" */,-47 ),
	/* State 60 */ new Array( 13/* "}" */,-49 , 23/* "Junk" */,-49 , 2/* "namespace" */,-49 , 3/* "class" */,-49 , 4/* "native" */,-49 , 5/* "int" */,-49 , 6/* "string" */,-49 , 7/* "bool" */,-49 , 9/* "true" */,-49 , 10/* "false" */,-49 , 11/* "function" */,-49 , 12/* "{" */,-49 , 14/* "(" */,-49 , 15/* ")" */,-49 , 16/* "=" */,-49 , 17/* ";" */,-49 , 18/* "," */,-49 , 19/* "*" */,-49 , 20/* "Identifier" */,-49 , 21/* "String" */,-49 , 22/* "Integer" */,-49 , 1/* "WHTS" */,-49 ),
	/* State 61 */ new Array( 13/* "}" */,-50 , 23/* "Junk" */,-50 , 2/* "namespace" */,-50 , 3/* "class" */,-50 , 4/* "native" */,-50 , 5/* "int" */,-50 , 6/* "string" */,-50 , 7/* "bool" */,-50 , 9/* "true" */,-50 , 10/* "false" */,-50 , 11/* "function" */,-50 , 12/* "{" */,-50 , 14/* "(" */,-50 , 15/* ")" */,-50 , 16/* "=" */,-50 , 17/* ";" */,-50 , 18/* "," */,-50 , 19/* "*" */,-50 , 20/* "Identifier" */,-50 , 21/* "String" */,-50 , 22/* "Integer" */,-50 , 1/* "WHTS" */,-50 ),
	/* State 62 */ new Array( 13/* "}" */,-51 , 23/* "Junk" */,-51 , 2/* "namespace" */,-51 , 3/* "class" */,-51 , 4/* "native" */,-51 , 5/* "int" */,-51 , 6/* "string" */,-51 , 7/* "bool" */,-51 , 9/* "true" */,-51 , 10/* "false" */,-51 , 11/* "function" */,-51 , 12/* "{" */,-51 , 14/* "(" */,-51 , 15/* ")" */,-51 , 16/* "=" */,-51 , 17/* ";" */,-51 , 18/* "," */,-51 , 19/* "*" */,-51 , 20/* "Identifier" */,-51 , 21/* "String" */,-51 , 22/* "Integer" */,-51 , 1/* "WHTS" */,-51 ),
	/* State 63 */ new Array( 13/* "}" */,-52 , 23/* "Junk" */,-52 , 2/* "namespace" */,-52 , 3/* "class" */,-52 , 4/* "native" */,-52 , 5/* "int" */,-52 , 6/* "string" */,-52 , 7/* "bool" */,-52 , 9/* "true" */,-52 , 10/* "false" */,-52 , 11/* "function" */,-52 , 12/* "{" */,-52 , 14/* "(" */,-52 , 15/* ")" */,-52 , 16/* "=" */,-52 , 17/* ";" */,-52 , 18/* "," */,-52 , 19/* "*" */,-52 , 20/* "Identifier" */,-52 , 21/* "String" */,-52 , 22/* "Integer" */,-52 , 1/* "WHTS" */,-52 ),
	/* State 64 */ new Array( 13/* "}" */,-53 , 23/* "Junk" */,-53 , 2/* "namespace" */,-53 , 3/* "class" */,-53 , 4/* "native" */,-53 , 5/* "int" */,-53 , 6/* "string" */,-53 , 7/* "bool" */,-53 , 9/* "true" */,-53 , 10/* "false" */,-53 , 11/* "function" */,-53 , 12/* "{" */,-53 , 14/* "(" */,-53 , 15/* ")" */,-53 , 16/* "=" */,-53 , 17/* ";" */,-53 , 18/* "," */,-53 , 19/* "*" */,-53 , 20/* "Identifier" */,-53 , 21/* "String" */,-53 , 22/* "Integer" */,-53 , 1/* "WHTS" */,-53 ),
	/* State 65 */ new Array( 13/* "}" */,-54 , 23/* "Junk" */,-54 , 2/* "namespace" */,-54 , 3/* "class" */,-54 , 4/* "native" */,-54 , 5/* "int" */,-54 , 6/* "string" */,-54 , 7/* "bool" */,-54 , 9/* "true" */,-54 , 10/* "false" */,-54 , 11/* "function" */,-54 , 12/* "{" */,-54 , 14/* "(" */,-54 , 15/* ")" */,-54 , 16/* "=" */,-54 , 17/* ";" */,-54 , 18/* "," */,-54 , 19/* "*" */,-54 , 20/* "Identifier" */,-54 , 21/* "String" */,-54 , 22/* "Integer" */,-54 , 1/* "WHTS" */,-54 ),
	/* State 66 */ new Array( 13/* "}" */,-55 , 23/* "Junk" */,-55 , 2/* "namespace" */,-55 , 3/* "class" */,-55 , 4/* "native" */,-55 , 5/* "int" */,-55 , 6/* "string" */,-55 , 7/* "bool" */,-55 , 9/* "true" */,-55 , 10/* "false" */,-55 , 11/* "function" */,-55 , 12/* "{" */,-55 , 14/* "(" */,-55 , 15/* ")" */,-55 , 16/* "=" */,-55 , 17/* ";" */,-55 , 18/* "," */,-55 , 19/* "*" */,-55 , 20/* "Identifier" */,-55 , 21/* "String" */,-55 , 22/* "Integer" */,-55 , 1/* "WHTS" */,-55 ),
	/* State 67 */ new Array( 13/* "}" */,-56 , 23/* "Junk" */,-56 , 2/* "namespace" */,-56 , 3/* "class" */,-56 , 4/* "native" */,-56 , 5/* "int" */,-56 , 6/* "string" */,-56 , 7/* "bool" */,-56 , 9/* "true" */,-56 , 10/* "false" */,-56 , 11/* "function" */,-56 , 12/* "{" */,-56 , 14/* "(" */,-56 , 15/* ")" */,-56 , 16/* "=" */,-56 , 17/* ";" */,-56 , 18/* "," */,-56 , 19/* "*" */,-56 , 20/* "Identifier" */,-56 , 21/* "String" */,-56 , 22/* "Integer" */,-56 , 1/* "WHTS" */,-56 ),
	/* State 68 */ new Array( 13/* "}" */,-57 , 23/* "Junk" */,-57 , 2/* "namespace" */,-57 , 3/* "class" */,-57 , 4/* "native" */,-57 , 5/* "int" */,-57 , 6/* "string" */,-57 , 7/* "bool" */,-57 , 9/* "true" */,-57 , 10/* "false" */,-57 , 11/* "function" */,-57 , 12/* "{" */,-57 , 14/* "(" */,-57 , 15/* ")" */,-57 , 16/* "=" */,-57 , 17/* ";" */,-57 , 18/* "," */,-57 , 19/* "*" */,-57 , 20/* "Identifier" */,-57 , 21/* "String" */,-57 , 22/* "Integer" */,-57 , 1/* "WHTS" */,-57 ),
	/* State 69 */ new Array( 13/* "}" */,-58 , 23/* "Junk" */,-58 , 2/* "namespace" */,-58 , 3/* "class" */,-58 , 4/* "native" */,-58 , 5/* "int" */,-58 , 6/* "string" */,-58 , 7/* "bool" */,-58 , 9/* "true" */,-58 , 10/* "false" */,-58 , 11/* "function" */,-58 , 12/* "{" */,-58 , 14/* "(" */,-58 , 15/* ")" */,-58 , 16/* "=" */,-58 , 17/* ";" */,-58 , 18/* "," */,-58 , 19/* "*" */,-58 , 20/* "Identifier" */,-58 , 21/* "String" */,-58 , 22/* "Integer" */,-58 , 1/* "WHTS" */,-58 ),
	/* State 70 */ new Array( 1/* "WHTS" */,-20 , 20/* "Identifier" */,-20 ),
	/* State 71 */ new Array( 1/* "WHTS" */,4 , 15/* ")" */,-60 , 18/* "," */,-60 ),
	/* State 72 */ new Array( 1/* "WHTS" */,4 , 20/* "Identifier" */,-60 ),
	/* State 73 */ new Array( 1/* "WHTS" */,4 , 20/* "Identifier" */,-60 ),
	/* State 74 */ new Array( 1/* "WHTS" */,-24 , 20/* "Identifier" */,-24 ),
	/* State 75 */ new Array( 1/* "WHTS" */,4 , 13/* "}" */,-60 , 2/* "namespace" */,-60 , 3/* "class" */,-60 , 11/* "function" */,-60 , 4/* "native" */,-60 , 5/* "int" */,-60 , 6/* "string" */,-60 , 7/* "bool" */,-60 , 8/* "object" */,-60 ),
	/* State 76 */ new Array( 1/* "WHTS" */,4 , 13/* "}" */,-60 , 3/* "class" */,-60 , 11/* "function" */,-60 , 4/* "native" */,-60 , 5/* "int" */,-60 , 6/* "string" */,-60 , 7/* "bool" */,-60 , 8/* "object" */,-60 ),
	/* State 77 */ new Array( 21/* "String" */,86 , 22/* "Integer" */,87 , 9/* "true" */,88 , 10/* "false" */,89 ),
	/* State 78 */ new Array( 15/* ")" */,91 , 4/* "native" */,73 , 5/* "int" */,17 , 6/* "string" */,18 , 7/* "bool" */,19 , 11/* "function" */,74 , 8/* "object" */,20 ),
	/* State 79 */ new Array( 15/* ")" */,92 , 18/* "," */,93 ),
	/* State 80 */ new Array( 20/* "Identifier" */,94 ),
	/* State 81 */ new Array( 20/* "Identifier" */,33 ),
	/* State 82 */ new Array( 1/* "WHTS" */,4 , 13/* "}" */,-60 , 2/* "namespace" */,-60 , 3/* "class" */,-60 , 11/* "function" */,-60 , 4/* "native" */,-60 , 5/* "int" */,-60 , 6/* "string" */,-60 , 7/* "bool" */,-60 , 8/* "object" */,-60 ),
	/* State 83 */ new Array( 1/* "WHTS" */,4 , 13/* "}" */,-60 , 3/* "class" */,-60 , 11/* "function" */,-60 , 4/* "native" */,-60 , 5/* "int" */,-60 , 6/* "string" */,-60 , 7/* "bool" */,-60 , 8/* "object" */,-60 ),
	/* State 84 */ new Array( 1/* "WHTS" */,-12 , 13/* "}" */,-12 , 3/* "class" */,-12 , 11/* "function" */,-12 , 4/* "native" */,-12 , 5/* "int" */,-12 , 6/* "string" */,-12 , 7/* "bool" */,-12 , 8/* "object" */,-12 ),
	/* State 85 */ new Array( 1/* "WHTS" */,4 , 17/* ";" */,-60 ),
	/* State 86 */ new Array( 1/* "WHTS" */,-26 , 17/* ";" */,-26 ),
	/* State 87 */ new Array( 1/* "WHTS" */,-27 , 17/* ";" */,-27 ),
	/* State 88 */ new Array( 1/* "WHTS" */,-28 , 17/* ";" */,-28 ),
	/* State 89 */ new Array( 1/* "WHTS" */,-29 , 17/* ";" */,-29 ),
	/* State 90 */ new Array( 1/* "WHTS" */,4 , 15/* ")" */,-60 , 18/* "," */,-60 ),
	/* State 91 */ new Array( 1/* "WHTS" */,4 , 12/* "{" */,-60 ),
	/* State 92 */ new Array( 1/* "WHTS" */,-25 , 20/* "Identifier" */,-25 ),
	/* State 93 */ new Array( 1/* "WHTS" */,4 , 4/* "native" */,-60 , 5/* "int" */,-60 , 6/* "string" */,-60 , 7/* "bool" */,-60 , 11/* "function" */,-60 , 8/* "object" */,-60 ),
	/* State 94 */ new Array( 1/* "WHTS" */,-33 , 15/* ")" */,-33 , 18/* "," */,-33 ),
	/* State 95 */ new Array( 13/* "}" */,101 , 2/* "namespace" */,12 , 3/* "class" */,13 , 11/* "function" */,15 , 4/* "native" */,16 , 5/* "int" */,17 , 6/* "string" */,18 , 7/* "bool" */,19 , 8/* "object" */,20 ),
	/* State 96 */ new Array( 13/* "}" */,102 , 3/* "class" */,13 , 11/* "function" */,15 , 4/* "native" */,16 , 5/* "int" */,17 , 6/* "string" */,18 , 7/* "bool" */,19 , 8/* "object" */,20 ),
	/* State 97 */ new Array( 17/* ";" */,108 ),
	/* State 98 */ new Array( 15/* ")" */,109 , 18/* "," */,93 ),
	/* State 99 */ new Array( 12/* "{" */,110 ),
	/* State 100 */ new Array( 4/* "native" */,73 , 5/* "int" */,17 , 6/* "string" */,18 , 7/* "bool" */,19 , 11/* "function" */,74 , 8/* "object" */,20 ),
	/* State 101 */ new Array( 1/* "WHTS" */,-2 , 40/* "$" */,-2 , 2/* "namespace" */,-2 , 3/* "class" */,-2 , 11/* "function" */,-2 , 4/* "native" */,-2 , 5/* "int" */,-2 , 6/* "string" */,-2 , 7/* "bool" */,-2 , 8/* "object" */,-2 , 13/* "}" */,-2 ),
	/* State 102 */ new Array( 1/* "WHTS" */,-10 , 40/* "$" */,-10 , 2/* "namespace" */,-10 , 3/* "class" */,-10 , 11/* "function" */,-10 , 4/* "native" */,-10 , 5/* "int" */,-10 , 6/* "string" */,-10 , 7/* "bool" */,-10 , 8/* "object" */,-10 , 13/* "}" */,-10 ),
	/* State 103 */ new Array( 1/* "WHTS" */,4 , 13/* "}" */,-60 , 3/* "class" */,-60 , 11/* "function" */,-60 , 4/* "native" */,-60 , 5/* "int" */,-60 , 6/* "string" */,-60 , 7/* "bool" */,-60 , 8/* "object" */,-60 ),
	/* State 104 */ new Array( 1/* "WHTS" */,-13 , 13/* "}" */,-13 , 3/* "class" */,-13 , 11/* "function" */,-13 , 4/* "native" */,-13 , 5/* "int" */,-13 , 6/* "string" */,-13 , 7/* "bool" */,-13 , 8/* "object" */,-13 ),
	/* State 105 */ new Array( 1/* "WHTS" */,-14 , 13/* "}" */,-14 , 3/* "class" */,-14 , 11/* "function" */,-14 , 4/* "native" */,-14 , 5/* "int" */,-14 , 6/* "string" */,-14 , 7/* "bool" */,-14 , 8/* "object" */,-14 ),
	/* State 106 */ new Array( 1/* "WHTS" */,-15 , 13/* "}" */,-15 , 3/* "class" */,-15 , 11/* "function" */,-15 , 4/* "native" */,-15 , 5/* "int" */,-15 , 6/* "string" */,-15 , 7/* "bool" */,-15 , 8/* "object" */,-15 ),
	/* State 107 */ new Array( 1/* "WHTS" */,-16 , 13/* "}" */,-16 , 3/* "class" */,-16 , 11/* "function" */,-16 , 4/* "native" */,-16 , 5/* "int" */,-16 , 6/* "string" */,-16 , 7/* "bool" */,-16 , 8/* "object" */,-16 ),
	/* State 108 */ new Array( 1/* "WHTS" */,-18 , 40/* "$" */,-18 , 2/* "namespace" */,-18 , 3/* "class" */,-18 , 11/* "function" */,-18 , 4/* "native" */,-18 , 5/* "int" */,-18 , 6/* "string" */,-18 , 7/* "bool" */,-18 , 8/* "object" */,-18 , 13/* "}" */,-18 ),
	/* State 109 */ new Array( 1/* "WHTS" */,4 , 12/* "{" */,-60 ),
	/* State 110 */ new Array( 13/* "}" */,-36 , 23/* "Junk" */,-36 , 2/* "namespace" */,-36 , 3/* "class" */,-36 , 4/* "native" */,-36 , 5/* "int" */,-36 , 6/* "string" */,-36 , 7/* "bool" */,-36 , 9/* "true" */,-36 , 10/* "false" */,-36 , 11/* "function" */,-36 , 12/* "{" */,-36 , 14/* "(" */,-36 , 15/* ")" */,-36 , 16/* "=" */,-36 , 17/* ";" */,-36 , 18/* "," */,-36 , 19/* "*" */,-36 , 20/* "Identifier" */,-36 , 21/* "String" */,-36 , 22/* "Integer" */,-36 , 1/* "WHTS" */,-36 ),
	/* State 111 */ new Array( 1/* "WHTS" */,4 , 20/* "Identifier" */,-60 ),
	/* State 112 */ new Array( 1/* "WHTS" */,-11 , 13/* "}" */,-11 , 3/* "class" */,-11 , 11/* "function" */,-11 , 4/* "native" */,-11 , 5/* "int" */,-11 , 6/* "string" */,-11 , 7/* "bool" */,-11 , 8/* "object" */,-11 ),
	/* State 113 */ new Array( 12/* "{" */,116 ),
	/* State 114 */ new Array( 13/* "}" */,117 , 23/* "Junk" */,49 , 2/* "namespace" */,50 , 3/* "class" */,51 , 4/* "native" */,52 , 5/* "int" */,53 , 6/* "string" */,54 , 7/* "bool" */,55 , 9/* "true" */,56 , 10/* "false" */,57 , 11/* "function" */,58 , 12/* "{" */,59 , 14/* "(" */,60 , 15/* ")" */,61 , 16/* "=" */,62 , 17/* ";" */,63 , 18/* "," */,64 , 19/* "*" */,65 , 20/* "Identifier" */,66 , 21/* "String" */,67 , 22/* "Integer" */,68 , 1/* "WHTS" */,4 ),
	/* State 115 */ new Array( 20/* "Identifier" */,118 ),
	/* State 116 */ new Array( 13/* "}" */,-36 , 23/* "Junk" */,-36 , 2/* "namespace" */,-36 , 3/* "class" */,-36 , 4/* "native" */,-36 , 5/* "int" */,-36 , 6/* "string" */,-36 , 7/* "bool" */,-36 , 9/* "true" */,-36 , 10/* "false" */,-36 , 11/* "function" */,-36 , 12/* "{" */,-36 , 14/* "(" */,-36 , 15/* ")" */,-36 , 16/* "=" */,-36 , 17/* ";" */,-36 , 18/* "," */,-36 , 19/* "*" */,-36 , 20/* "Identifier" */,-36 , 21/* "String" */,-36 , 22/* "Integer" */,-36 , 1/* "WHTS" */,-36 ),
	/* State 117 */ new Array( 1/* "WHTS" */,-31 , 40/* "$" */,-31 , 2/* "namespace" */,-31 , 3/* "class" */,-31 , 11/* "function" */,-31 , 4/* "native" */,-31 , 5/* "int" */,-31 , 6/* "string" */,-31 , 7/* "bool" */,-31 , 8/* "object" */,-31 , 13/* "}" */,-31 , 23/* "Junk" */,-48 , 9/* "true" */,-48 , 10/* "false" */,-48 , 12/* "{" */,-48 , 14/* "(" */,-48 , 15/* ")" */,-48 , 16/* "=" */,-48 , 17/* ";" */,-48 , 18/* "," */,-48 , 19/* "*" */,-48 , 20/* "Identifier" */,-48 , 21/* "String" */,-48 , 22/* "Integer" */,-48 ),
	/* State 118 */ new Array( 1/* "WHTS" */,-32 , 15/* ")" */,-32 , 18/* "," */,-32 ),
	/* State 119 */ new Array( 13/* "}" */,120 , 23/* "Junk" */,49 , 2/* "namespace" */,50 , 3/* "class" */,51 , 4/* "native" */,52 , 5/* "int" */,53 , 6/* "string" */,54 , 7/* "bool" */,55 , 9/* "true" */,56 , 10/* "false" */,57 , 11/* "function" */,58 , 12/* "{" */,59 , 14/* "(" */,60 , 15/* ")" */,61 , 16/* "=" */,62 , 17/* ";" */,63 , 18/* "," */,64 , 19/* "*" */,65 , 20/* "Identifier" */,66 , 21/* "String" */,67 , 22/* "Integer" */,68 , 1/* "WHTS" */,4 ),
	/* State 120 */ new Array( 1/* "WHTS" */,-30 , 40/* "$" */,-30 , 2/* "namespace" */,-30 , 3/* "class" */,-30 , 11/* "function" */,-30 , 4/* "native" */,-30 , 5/* "int" */,-30 , 6/* "string" */,-30 , 7/* "bool" */,-30 , 8/* "object" */,-30 , 13/* "}" */,-30 , 23/* "Junk" */,-48 , 9/* "true" */,-48 , 10/* "false" */,-48 , 12/* "{" */,-48 , 14/* "(" */,-48 , 15/* ")" */,-48 , 16/* "=" */,-48 , 17/* ";" */,-48 , 18/* "," */,-48 , 19/* "*" */,-48 , 20/* "Identifier" */,-48 , 21/* "String" */,-48 , 22/* "Integer" */,-48 )
);

/* Goto-Table */
var goto_tab = new Array(
	/* State 0 */ new Array( 25/* Global */,1 , 24/* NamespaceContents */,2 , 26/* W */,3 ),
	/* State 1 */ new Array(  ),
	/* State 2 */ new Array( 26/* W */,5 ),
	/* State 3 */ new Array(  ),
	/* State 4 */ new Array(  ),
	/* State 5 */ new Array( 28/* NamespaceContent */,6 , 27/* Namespace */,7 , 29/* Class */,8 , 30/* VariableDef */,9 , 31/* Function */,10 , 32/* NativeBlock */,11 , 35/* Variable */,14 ),
	/* State 6 */ new Array( 26/* W */,21 ),
	/* State 7 */ new Array(  ),
	/* State 8 */ new Array(  ),
	/* State 9 */ new Array(  ),
	/* State 10 */ new Array(  ),
	/* State 11 */ new Array(  ),
	/* State 12 */ new Array( 26/* W */,22 ),
	/* State 13 */ new Array( 26/* W */,23 ),
	/* State 14 */ new Array( 26/* W */,24 ),
	/* State 15 */ new Array( 26/* W */,25 ),
	/* State 16 */ new Array( 26/* W */,26 ),
	/* State 17 */ new Array(  ),
	/* State 18 */ new Array(  ),
	/* State 19 */ new Array(  ),
	/* State 20 */ new Array( 26/* W */,27 ),
	/* State 21 */ new Array(  ),
	/* State 22 */ new Array(  ),
	/* State 23 */ new Array(  ),
	/* State 24 */ new Array(  ),
	/* State 25 */ new Array(  ),
	/* State 26 */ new Array(  ),
	/* State 27 */ new Array(  ),
	/* State 28 */ new Array( 26/* W */,35 ),
	/* State 29 */ new Array( 26/* W */,36 ),
	/* State 30 */ new Array( 26/* W */,37 ),
	/* State 31 */ new Array( 26/* W */,38 ),
	/* State 32 */ new Array( 38/* NativeCode */,39 ),
	/* State 33 */ new Array( 26/* W */,40 ),
	/* State 34 */ new Array( 26/* W */,41 ),
	/* State 35 */ new Array(  ),
	/* State 36 */ new Array(  ),
	/* State 37 */ new Array(  ),
	/* State 38 */ new Array(  ),
	/* State 39 */ new Array( 39/* PossibleJunk */,47 , 26/* W */,69 ),
	/* State 40 */ new Array(  ),
	/* State 41 */ new Array( 37/* ArgumentList */,71 , 35/* Variable */,72 ),
	/* State 42 */ new Array( 26/* W */,75 ),
	/* State 43 */ new Array( 26/* W */,76 ),
	/* State 44 */ new Array(  ),
	/* State 45 */ new Array( 26/* W */,77 ),
	/* State 46 */ new Array( 26/* W */,78 ),
	/* State 47 */ new Array(  ),
	/* State 48 */ new Array(  ),
	/* State 49 */ new Array(  ),
	/* State 50 */ new Array(  ),
	/* State 51 */ new Array(  ),
	/* State 52 */ new Array(  ),
	/* State 53 */ new Array(  ),
	/* State 54 */ new Array(  ),
	/* State 55 */ new Array(  ),
	/* State 56 */ new Array(  ),
	/* State 57 */ new Array(  ),
	/* State 58 */ new Array(  ),
	/* State 59 */ new Array(  ),
	/* State 60 */ new Array(  ),
	/* State 61 */ new Array(  ),
	/* State 62 */ new Array(  ),
	/* State 63 */ new Array(  ),
	/* State 64 */ new Array(  ),
	/* State 65 */ new Array(  ),
	/* State 66 */ new Array(  ),
	/* State 67 */ new Array(  ),
	/* State 68 */ new Array(  ),
	/* State 69 */ new Array(  ),
	/* State 70 */ new Array(  ),
	/* State 71 */ new Array( 26/* W */,79 ),
	/* State 72 */ new Array( 26/* W */,80 ),
	/* State 73 */ new Array( 26/* W */,81 ),
	/* State 74 */ new Array(  ),
	/* State 75 */ new Array( 24/* NamespaceContents */,82 , 26/* W */,3 ),
	/* State 76 */ new Array( 33/* ClassContents */,83 , 26/* W */,84 ),
	/* State 77 */ new Array( 36/* Immediate */,85 ),
	/* State 78 */ new Array( 37/* ArgumentList */,90 , 35/* Variable */,72 ),
	/* State 79 */ new Array(  ),
	/* State 80 */ new Array(  ),
	/* State 81 */ new Array(  ),
	/* State 82 */ new Array( 26/* W */,95 ),
	/* State 83 */ new Array( 26/* W */,96 ),
	/* State 84 */ new Array(  ),
	/* State 85 */ new Array( 26/* W */,97 ),
	/* State 86 */ new Array(  ),
	/* State 87 */ new Array(  ),
	/* State 88 */ new Array(  ),
	/* State 89 */ new Array(  ),
	/* State 90 */ new Array( 26/* W */,98 ),
	/* State 91 */ new Array( 26/* W */,99 ),
	/* State 92 */ new Array(  ),
	/* State 93 */ new Array( 26/* W */,100 ),
	/* State 94 */ new Array(  ),
	/* State 95 */ new Array( 28/* NamespaceContent */,6 , 27/* Namespace */,7 , 29/* Class */,8 , 30/* VariableDef */,9 , 31/* Function */,10 , 32/* NativeBlock */,11 , 35/* Variable */,14 ),
	/* State 96 */ new Array( 34/* ClassContent */,103 , 29/* Class */,104 , 30/* VariableDef */,105 , 31/* Function */,106 , 32/* NativeBlock */,107 , 35/* Variable */,14 ),
	/* State 97 */ new Array(  ),
	/* State 98 */ new Array(  ),
	/* State 99 */ new Array(  ),
	/* State 100 */ new Array( 35/* Variable */,111 ),
	/* State 101 */ new Array(  ),
	/* State 102 */ new Array(  ),
	/* State 103 */ new Array( 26/* W */,112 ),
	/* State 104 */ new Array(  ),
	/* State 105 */ new Array(  ),
	/* State 106 */ new Array(  ),
	/* State 107 */ new Array(  ),
	/* State 108 */ new Array(  ),
	/* State 109 */ new Array( 26/* W */,113 ),
	/* State 110 */ new Array( 38/* NativeCode */,114 ),
	/* State 111 */ new Array( 26/* W */,115 ),
	/* State 112 */ new Array(  ),
	/* State 113 */ new Array(  ),
	/* State 114 */ new Array( 39/* PossibleJunk */,47 , 26/* W */,69 ),
	/* State 115 */ new Array(  ),
	/* State 116 */ new Array( 38/* NativeCode */,119 ),
	/* State 117 */ new Array(  ),
	/* State 118 */ new Array(  ),
	/* State 119 */ new Array( 39/* PossibleJunk */,47 , 26/* W */,69 ),
	/* State 120 */ new Array(  )
);



/* Symbol labels */
var labels = new Array(
	"Global'" /* Non-terminal symbol */,
	"WHTS" /* Terminal symbol */,
	"namespace" /* Terminal symbol */,
	"class" /* Terminal symbol */,
	"native" /* Terminal symbol */,
	"int" /* Terminal symbol */,
	"string" /* Terminal symbol */,
	"bool" /* Terminal symbol */,
	"object" /* Terminal symbol */,
	"true" /* Terminal symbol */,
	"false" /* Terminal symbol */,
	"function" /* Terminal symbol */,
	"{" /* Terminal symbol */,
	"}" /* Terminal symbol */,
	"(" /* Terminal symbol */,
	")" /* Terminal symbol */,
	"=" /* Terminal symbol */,
	";" /* Terminal symbol */,
	"," /* Terminal symbol */,
	"*" /* Terminal symbol */,
	"Identifier" /* Terminal symbol */,
	"String" /* Terminal symbol */,
	"Integer" /* Terminal symbol */,
	"Junk" /* Terminal symbol */,
	"NamespaceContents" /* Non-terminal symbol */,
	"Global" /* Non-terminal symbol */,
	"W" /* Non-terminal symbol */,
	"Namespace" /* Non-terminal symbol */,
	"NamespaceContent" /* Non-terminal symbol */,
	"Class" /* Non-terminal symbol */,
	"VariableDef" /* Non-terminal symbol */,
	"Function" /* Non-terminal symbol */,
	"NativeBlock" /* Non-terminal symbol */,
	"ClassContents" /* Non-terminal symbol */,
	"ClassContent" /* Non-terminal symbol */,
	"Variable" /* Non-terminal symbol */,
	"Immediate" /* Non-terminal symbol */,
	"ArgumentList" /* Non-terminal symbol */,
	"NativeCode" /* Non-terminal symbol */,
	"PossibleJunk" /* Non-terminal symbol */,
	"$" /* Terminal symbol */
);


	
	info.offset = 0;
	info.src = src;
	info.att = new String();
	
	if( !err_off )
		err_off	= new Array();
	if( !err_la )
	err_la = new Array();
	
	sstack.push( 0 );
	vstack.push( 0 );
	
	la = __v8lex( info );
			
	while( true )
	{
		act = 122;
		for( var i = 0; i < act_tab[sstack[sstack.length-1]].length; i+=2 )
		{
			if( act_tab[sstack[sstack.length-1]][i] == la )
			{
				act = act_tab[sstack[sstack.length-1]][i+1];
				break;
			}
		}

		/*
		_print( "state " + sstack[sstack.length-1] + " la = " + la + " info.att = >" +
				info.att + "< act = " + act + " src = >" + info.src.substr( info.offset, 30 ) + "..." + "<" +
					" sstack = " + sstack.join() );
		*/
		
		if( v8_dbg_withtrace && sstack.length > 0 )
		{
			__v8dbg_print( "\nState " + sstack[sstack.length-1] + "\n" +
							"\tLookahead: " + labels[la] + " (\"" + info.att + "\")\n" +
							"\tAction: " + act + "\n" + 
							"\tSource: \"" + info.src.substr( info.offset, 30 ) + ( ( info.offset + 30 < info.src.length ) ?
									"..." : "" ) + "\"\n" +
							"\tStack: " + sstack.join() + "\n" +
							"\tValue stack: " + vstack.join() + "\n" );
			
			if( v8_dbg_withstepbystep )
				__v8dbg_wait();
		}
		
			
		//Panic-mode: Try recovery when parse-error occurs!
		if( act == 122 )
		{
			if( v8_dbg_withtrace )
				__v8dbg_print( "Error detected: There is no reduce or shift on the symbol " + labels[la] );
			
			err_cnt++;
			err_off.push( info.offset - info.att.length );			
			err_la.push( new Array() );
			for( var i = 0; i < act_tab[sstack[sstack.length-1]].length; i+=2 )
				err_la[err_la.length-1].push( labels[act_tab[sstack[sstack.length-1]][i]] );
			
			//Remember the original stack!
			var rsstack = new Array();
			var rvstack = new Array();
			for( var i = 0; i < sstack.length; i++ )
			{
				rsstack[i] = sstack[i];
				rvstack[i] = vstack[i];
			}
			
			while( act == 122 && la != 40 )
			{
				if( v8_dbg_withtrace )
					__v8dbg_print( "\tError recovery\n" +
									"Current lookahead: " + labels[la] + " (" + info.att + ")\n" +
									"Action: " + act + "\n\n" );
				if( la == -1 )
					info.offset++;
					
				while( act == 122 && sstack.length > 0 )
				{
					sstack.pop();
					vstack.pop();
					
					if( sstack.length == 0 )
						break;
						
					act = 122;
					for( var i = 0; i < act_tab[sstack[sstack.length-1]].length; i+=2 )
					{
						if( act_tab[sstack[sstack.length-1]][i] == la )
						{
							act = act_tab[sstack[sstack.length-1]][i+1];
							break;
						}
					}
				}
				
				if( act != 122 )
					break;
				
				for( var i = 0; i < rsstack.length; i++ )
				{
					sstack.push( rsstack[i] );
					vstack.push( rvstack[i] );
				}
				
				la = __v8lex( info );
			}
			
			if( act == 122 )
			{
				if( v8_dbg_withtrace )
					__v8dbg_print( "\tError recovery failed, terminating parse process..." );
				break;
			}


			if( v8_dbg_withtrace )
				__v8dbg_print( "\tError recovery succeeded, continuing" );
		}
		
		/*
		if( act == 122 )
			break;
		*/
		
		
		//Shift
		if( act > 0 )
		{
			//Parse tree generation
			if( v8_dbg_withparsetree )
			{
				var node = new treenode();
				node.sym = labels[ la ];
				node.att = info.att;
				node.child = new Array();
				tree.push( treenodes.length );
				treenodes.push( node );
			}
			
			if( v8_dbg_withtrace )
				__v8dbg_print( "Shifting symbol: " + labels[la] + " (" + info.att + ")" );
		
			sstack.push( act );
			vstack.push( info.att );
			
			la = __v8lex( info );
			
			if( v8_dbg_withtrace )
				__v8dbg_print( "\tNew lookahead symbol: " + labels[la] + " (" + info.att + ")" );
		}
		//Reduce
		else
		{		
			act *= -1;
			
			if( v8_dbg_withtrace )
				__v8dbg_print( "Reducing by producution: " + act );
			
			rval = void(0);
			
			if( v8_dbg_withtrace )
				__v8dbg_print( "\tPerforming semantic action..." );
			
switch( act )
{
	case 0:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 1:
	{
		 generateCode(createNamespace("global", vstack[ vstack.length - 1 ])); 
	}
	break;
	case 2:
	{
		 rval = createNamespace(vstack[ vstack.length - 7 ], vstack[ vstack.length - 3 ]); 
	}
	break;
	case 3:
	{
		 rval = vstack[ vstack.length - 4 ].concat([vstack[ vstack.length - 2 ]]); 
	}
	break;
	case 4:
	{
		 rval = []; 
	}
	break;
	case 5:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 6:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 7:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 8:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 9:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 10:
	{
		 rval = createClass(vstack[ vstack.length - 7 ], vstack[ vstack.length - 3 ]); 
	}
	break;
	case 11:
	{
		 rval = vstack[ vstack.length - 4 ].concat([vstack[ vstack.length - 2 ]]); 
	}
	break;
	case 12:
	{
		 rval = []; 
	}
	break;
	case 13:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 14:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 15:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 16:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 17:
	{
		 rval = {type:(vstack[ vstack.length - 5 ])['native']?'native-var':'var', real:(vstack[ vstack.length - 5 ]).real, name:vstack[ vstack.length - 3 ]}; 
	}
	break;
	case 18:
	{
		 rval = {type:(vstack[ vstack.length - 9 ])['native']?'native-var':'var', real:(vstack[ vstack.length - 9 ]).real, name:vstack[ vstack.length - 7 ], defval:vstack[ vstack.length - 3 ]}; 
	}
	break;
	case 19:
	{
		 rval = {'native':true, real:vstack[ vstack.length - 1 ]}; 
	}
	break;
	case 20:
	{
		 rval = {'native':true, real:vstack[ vstack.length - 3 ]+'*'}; 
	}
	break;
	case 21:
	{
		 rval = {real:vstack[ vstack.length - 1 ]}; 
	}
	break;
	case 22:
	{
		 rval = {real:vstack[ vstack.length - 1 ]}; 
	}
	break;
	case 23:
	{
		 rval = {real:vstack[ vstack.length - 1 ]}; 
	}
	break;
	case 24:
	{
		 rval = {real:vstack[ vstack.length - 1 ]}; 
	}
	break;
	case 25:
	{
		 rval = {real:{obj:true, childs:vstack[ vstack.length - 3 ]}}; 
	}
	break;
	case 26:
	{
		 rval = {real:'string', value:vstack[ vstack.length - 1 ]}; 
	}
	break;
	case 27:
	{
		 rval = {real:'int', value:vstack[ vstack.length - 1 ]}; 
	}
	break;
	case 28:
	{
		 rval = {real:'bool', value:true}; 
	}
	break;
	case 29:
	{
		 rval = {real:'bool', value:false}; 
	}
	break;
	case 30:
	{
		 rval = {type:'function', name:vstack[ vstack.length - 11 ], args:vstack[ vstack.length - 7 ], code:vstack[ vstack.length - 2 ]}; 
	}
	break;
	case 31:
	{
		 rval = {type:'function', name:vstack[ vstack.length - 9 ], args:[], code:vstack[ vstack.length - 2 ]}; 
	}
	break;
	case 32:
	{
		 if((vstack[ vstack.length - 3 ])['native'])throw Error("No natives allowed as arguments");rval = (vstack[ vstack.length - 7 ]).concat([{real:(vstack[ vstack.length - 3 ]).real, name:vstack[ vstack.length - 1 ]}]); 
	}
	break;
	case 33:
	{
		 if((vstack[ vstack.length - 3 ])['native'])throw Error("No natives allowed as arguments");rval = [{real:(vstack[ vstack.length - 3 ]).real, name:vstack[ vstack.length - 1 ]}]; 
	}
	break;
	case 34:
	{
		 rval = {type:'native-block', code:vstack[ vstack.length - 2 ]}; 
	}
	break;
	case 35:
	{
		 rval = vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 36:
	{
		 rval = ""; 
	}
	break;
	case 37:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 38:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 39:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 40:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 41:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 42:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 43:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 44:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 45:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 46:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 47:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 48:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 49:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 50:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 51:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 52:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 53:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 54:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 55:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 56:
	{
		 rval = "'"+(vstack[ vstack.length - 1 ]).replace(/'/g, "\\'")+"'"; 
	}
	break;
	case 57:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 58:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 59:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 60:
	{
		rval = vstack[ vstack.length - 0 ];
	}
	break;
}


			
			if( v8_dbg_withparsetree )
				tmptree = new Array();

			if( v8_dbg_withtrace )
				__v8dbg_print( "\tPopping " + pop_tab[act][1] + " off the stack..." );
				
			for( var i = 0; i < pop_tab[act][1]; i++ )
			{
				if( v8_dbg_withparsetree )
					tmptree.push( tree.pop() );
					
				sstack.pop();
				vstack.pop();
			}
									
			go = -1;
			for( var i = 0; i < goto_tab[sstack[sstack.length-1]].length; i+=2 )
			{
				if( goto_tab[sstack[sstack.length-1]][i] == pop_tab[act][0] )
				{
					go = goto_tab[sstack[sstack.length-1]][i+1];
					break;
				}
			}
			
			if( v8_dbg_withparsetree )
			{
				var node = new treenode();
				node.sym = labels[ pop_tab[act][0] ];
				node.att = new String();
				node.child = tmptree.reverse();
				tree.push( treenodes.length );
				treenodes.push( node );
			}
			
			if( act == 0 )
				break;
				
			if( v8_dbg_withtrace )
				__v8dbg_print( "\tPushing non-terminal " + labels[ pop_tab[act][0] ] );
				
			sstack.push( go );
			vstack.push( rval );			
		}
	}

	if( v8_dbg_withtrace )
		__v8dbg_print( "\nParse complete." );

	if( v8_dbg_withparsetree )
	{
		if( err_cnt == 0 )
		{
			__v8dbg_print( "\n\n--- Parse tree ---" );
			__v8dbg_parsetree( 0, treenodes, tree );
		}
		else
		{
			__v8dbg_print( "\n\nParse tree cannot be viewed. There where parse errors." );
		}
	}
	
	return err_cnt;
}


function __v8dbg_parsetree( indent, nodes, tree )
{
	var str = new String();
	for( var i = 0; i < tree.length; i++ )
	{
		str = "";
		for( var j = indent; j > 0; j-- )
			str += "\t";
		
		str += nodes[ tree[i] ].sym;
		if( nodes[ tree[i] ].att != "" )
			str += " >" + nodes[ tree[i] ].att + "<" ;
			
		__v8dbg_print( str );
		if( nodes[ tree[i] ].child.length > 0 )
			__v8dbg_parsetree( indent + 1, nodes, nodes[ tree[i] ].child );
	}
}


function read_file(file)
{
    var src = new String();
    
    if(file_exists(file))
        src = file_read(file);
    else
    {
        print("unable to open file '" + file + "'");
        quit();
    }
    
    return src;
}

//v8_dbg_withtrace = true;
//v8_dbg_withparsetree = true;
//v8_dbg_withstepbystep = true;

if( arguments.length > 0 )
{
    var str         = read_file( arguments[0] );
    var error_cnt   = 0;
    var error_off   = new Array();
    var error_la    = new Array();
    
    if( ( error_cnt = __v8parse( str, error_off, error_la ) ) > 0 )
    {
        var i;
        for( i = 0; i < error_cnt; i++ )
            print( "Parse error near >" + str.substr( error_off[i], 30 ) + "<, expecting \"" + error_la[i].join() + "\"" );
    }
}
else
{
    print( 'usage: ./gearConvertor <filename>' );
}

