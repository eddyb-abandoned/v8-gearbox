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
    var out = {type:'class', name:name, classes:{}, vars:{}, staticVars:{}, functions:{}};
    for(c in childs) {
        var node = childs[c];
        switch(node.type) {
            case 'class':
                out.classes[node.name] = {classes:node.classes, vars:node.vars, staticVars:node.staticVars, functions:node.functions};
                break;
            case 'function':
                if(!out.functions[node.name])
                    out.functions[node.name] = [{args:node.args, code:node.code, line:node.line}];
                else
                    out.functions[node.name].push({args:node.args, code:node.code, line:node.line});
                break;
            case 'var':
                out.vars[node.name] = {val:node.val};
                break;
            case 'static-var':
                out.staticVars[node.name] = {val:node.val};
                break;
            case 'native-block':
                throw Error("TODO: Native blocks in classes");
        }
    }
    
    return out;
}

function createNamespace(name, childs, line) {
    var out = {type:'namespace', name:name, namespaces:{}, classes:{}, vars:{}, functions:{}, header:""};
    for(c in childs) {
        var node = childs[c];
        switch(node.type) {
            case 'namespace':
                out.namespaces[node.name] = {namespaces:node.namespaces, classes:node.classes, vars:node.vars, functions:node.functions, header:node.header};
                break;
            case 'class':
                out.classes[node.name] = {classes:node.classes, vars:node.vars, staticVars:node.staticVars, functions:node.functions};
                break;
            case 'function':
                if(!out.functions.hasOwnProperty(node.name))
                    out.functions[node.name] = [{args:node.args, code:node.code, line:node.line}];
                else
                    out.functions[node.name].push({args:node.args, code:node.code, line:node.line});
                break;
            case 'var':
                out.vars[node.name] = {val:node.val};
                break;
            case 'native-block':
                if(node.which == 'header')
                    out.header += node.code + "\n";
                else
                    throw Error("TODO: Native block `" + node.which + "`");
        }
    }
    
    if(!out.functions.hasOwnProperty("toString"))
        out.functions["toString"] = [{args:[], code:"return String(\"[object "+name+"]\");", line:line}];
    
    return out;
}

function nLines(s){return s.replace(/[^\n]/g,"").length;}
function nCols(s){return s.replace(/^(.*\n)+/,"").length;}

function makeTabs(n, ch) {
    var s = "";
    for(var i = 0; i < n; i++)
        s += ch;
    return s;
}

function makeLine(tbs, line) {
    return "\n" + tbs + "#line " + line + " \"" + gear.gear + "\"";
}

var lineNumber = 1;
function generateFunctionCode(functions, name, parentObjName, code, class, ctor) {
    var objName = parentObjName + "_" + name, replaces = [], funcCode = "", hasNoArgsVer = false;
    functions.sort(function(a, b) {return b.args.length - a.args.length;});
    for(f in functions) {
        var func = functions[f], replaces = [], tbs = (func.args.length ? "\t\t" : "\t");
        var actualCode = "\n" + tbs + func.code.trim() + "\n";
        
        var argsLine = "";
        for(var _arg in func.args)
            argsLine += (argsLine ? ", " : "") + func.args[_arg].name + "(args[" + _arg + "])";
        if(argsLine)
            actualCode = makeLine(tbs, func.line) + "\n" + tbs + "Value " + argsLine + ";" + actualCode;
        else
            actualCode = makeLine(tbs, func.line + 1) + actualCode;
        
        replaces.push({regex:"\n" + makeTabs(parentObjName.split("_").length, "    "), replace:"\n" + tbs});
        replaces.push({regex:"\\breturn\\b\\s*;", replace:"return undefined;"});
        replaces.push({regex:"\\bthis\\b", replace:"This"});
        
        for(r in replaces) {
            var replace = replaces[r];
            actualCode = actualCode.replace(new RegExp(replace.regex, "g"), replace.replace);
        }
        if(!RegExp("\n"+tbs+"\\breturn\\b[^;]*;\\s*$").exec(actualCode))
            actualCode += tbs + "return undefined;\n";
        
        if(func.args.length)
            funcCode += "\n\tif(args.Length() >= " + func.args.length + ")\n\t{" + actualCode + "\t}\n";
        else {
            funcCode += actualCode;
            hasNoArgsVer = true;
        }
    }
    
    if(class)
        funcCode = "\n\tValue This(args.This());"+funcCode;
    
    if(!hasNoArgsVer)
        funcCode += "\treturn Error(\"Invalid call to " + parentObjName.replace(/_/g, ".").replace(/^global\./, "") + (ctor ? "" : (class?".prototype":"") + "." + name) + "\");\n";
    
    code.func += "v8::Handle<v8::Value> __" + objName + "(const v8::Arguments& args) {" + funcCode + "}\n\n";
}

function generateClassCode(class, name, parentObjName, code) {
    var objName = parentObjName + "_" + name;
    
    code.addClass(objName, name);
    
    for(className in class.classes)
        generateClassCode(class.classes[className], className, objName, code);
    
    for(funcName in class.functions) {
        if(funcName != name)
            code.setPrototype(objName, funcName, "Function(__" + objName + "_" + funcName + ", \"" + funcName + "\")");
        generateFunctionCode(class.functions[funcName], funcName, objName, code, class, funcName == name);
    }
    
    for(varName in class.vars) {
        var val = class.vars[varName].val;
        code.setPrototype(objName, varName, /^\s*\b[A-Z]\w+\b\(.+\)$/.test(val) ? val : "Value(" + val + ")");
    }
    
    for(varName in class.staticVars) {
        var val = class.staticVars[varName].val;
        code.setStatic(objName + "->GetFunction()", varName, /^\s*\b[A-Z]\w+\b\(.+\)$/.test(val) ? val : "Value(" + val + ")");
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
        code.setStatic(objName, funcName, "Function(__" + objName + "_" + funcName + ", \"" + funcName + "\")");
        generateFunctionCode(namespace.functions[funcName], funcName, objName, code);
    }
    
    for(varName in namespace.vars) {
        var val = namespace.vars[varName].val;
        code.setStatic(objName, varName, /^\s*\b[A-Z]\w+\b\(.+\)$/.test(val) ? val : "Value(" + val + ")");
    }
}

function generateCode(global) {
    var code = {
        func:"", init:"",
        addNamespace: function(objName) {
            this.init += "\tv8::Handle<v8::Object> " + objName + " = v8::Object::New();\n";
        },
        addFunction: function(objName, funcName) {
            objName += "_" + funcName;
            this.init += "\tv8::Handle<v8::Function> " + objName + " = v8::FunctionTemplate::New(__" + objName + ")->GetFunction();\n";
            this.init += "\t" + objName + "->SetName(String(\"" + funcName + "\"));\n";
        },
        addClass: function(objName, ctor) {
            this.init += "\tv8::Handle<v8::FunctionTemplate> " + objName + " = v8::FunctionTemplate::New(__" + objName + "_" + ctor + ");\n";
            this.init += "\t" + objName + "->SetClassName(String(\"" + ctor + "\"));\n";
        },
        setStatic: function(parentObjName, name, value) {
            this.init += "\t" + parentObjName + "->Set(String(\"" + name + "\"), " + value + ");\n";
        },
        setPrototype: function(parentObjName, name, value) {
            this.init += "\t" + parentObjName + "->PrototypeTemplate()->Set(\"" + name + "\", " + value + ");\n";
        },
    };
    
    var namespaces = Object.keys(global.namespaces);
    
    if(!namespaces.length)
        throw Error("No namespace");
    else if(namespaces.length > 1)
        throw Error("Too may namespaces");
    else {
        generateNamespaceCode(global.namespaces[namespaces[0]], namespaces[0], "global", code);
        
        var ccCode =
        "\n#include \"../Gearbox.h\"\n"+
        "#include \""+baseName+".h\"\n"+
        "using namespace Gearbox;\n\n"+
        "/** \\file "+baseName+".cc */\n"+
        makeLine("",1) + "\n" +
        global.header.trim().replace(/\n    /g, "\n") +
        (global.header.trim()?"\n\n":"") + code.func;
        ccCode += makeLine("",nLines(ccCode)+2).replace(".gear",".cc") + "\nvoid Setup" + baseName + "(v8::Handle<v8::Object> global) {\n" + code.init + "}";
        ccCode = ccCode.replace(/\t/g, "    ");
        Io.writeFileContents(gear.cc, ccCode);
        
        var hCode =
        "#ifndef MODULE_"+baseName.toUpperCase()+"_H\n"+
        "#define MODULE_"+baseName.toUpperCase()+"_H\n\n"+
        "#include <v8.h>\n\n"+
        "void Setup"+baseName+"(v8::Handle<v8::Object> global);\n\n"+
        "#endif\n";
        Io.writeFileContents(gear.h, hCode);
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
			return 35;

		do
		{

switch( state )
{
	case 0:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 46 ) || ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 58 ) || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 9 && info.src.charCodeAt( pos ) <= 10 ) || info.src.charCodeAt( pos ) == 32 ) state = 2;
		else if( info.src.charCodeAt( pos ) == 40 ) state = 3;
		else if( info.src.charCodeAt( pos ) == 41 ) state = 4;
		else if( info.src.charCodeAt( pos ) == 42 ) state = 5;
		else if( info.src.charCodeAt( pos ) == 44 ) state = 6;
		else if( info.src.charCodeAt( pos ) == 47 ) state = 7;
		else if( info.src.charCodeAt( pos ) == 59 ) state = 8;
		else if( info.src.charCodeAt( pos ) == 61 ) state = 9;
		else if( ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 98 ) || ( info.src.charCodeAt( pos ) >= 100 && info.src.charCodeAt( pos ) <= 101 ) || ( info.src.charCodeAt( pos ) >= 103 && info.src.charCodeAt( pos ) <= 109 ) || ( info.src.charCodeAt( pos ) >= 111 && info.src.charCodeAt( pos ) <= 114 ) || ( info.src.charCodeAt( pos ) >= 116 && info.src.charCodeAt( pos ) <= 117 ) || ( info.src.charCodeAt( pos ) >= 119 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 123 ) state = 11;
		else if( info.src.charCodeAt( pos ) == 125 ) state = 12;
		else if( info.src.charCodeAt( pos ) == 118 ) state = 23;
		else if( info.src.charCodeAt( pos ) == 99 ) state = 32;
		else if( info.src.charCodeAt( pos ) == 115 ) state = 36;
		else if( info.src.charCodeAt( pos ) == 102 ) state = 41;
		else if( info.src.charCodeAt( pos ) == 110 ) state = 43;
		else state = -1;
		break;

	case 1:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 58 ) || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 122 ) || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 2:
		if( ( info.src.charCodeAt( pos ) >= 9 && info.src.charCodeAt( pos ) <= 10 ) || info.src.charCodeAt( pos ) == 32 ) state = 2;
		else state = -1;
		match = 1;
		match_pos = pos;
		break;

	case 3:
		state = -1;
		match = 9;
		match_pos = pos;
		break;

	case 4:
		state = -1;
		match = 10;
		match_pos = pos;
		break;

	case 5:
		state = -1;
		match = 14;
		match_pos = pos;
		break;

	case 6:
		state = -1;
		match = 13;
		match_pos = pos;
		break;

	case 7:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 58 ) || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 122 ) || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else state = -1;
		match = 15;
		match_pos = pos;
		break;

	case 8:
		state = -1;
		match = 12;
		match_pos = pos;
		break;

	case 9:
		state = -1;
		match = 11;
		match_pos = pos;
		break;

	case 10:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 11:
		state = -1;
		match = 7;
		match_pos = pos;
		break;

	case 12:
		state = -1;
		match = 8;
		match_pos = pos;
		break;

	case 13:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 6;
		match_pos = pos;
		break;

	case 14:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 3;
		match_pos = pos;
		break;

	case 15:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 5;
		match_pos = pos;
		break;

	case 16:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 4;
		match_pos = pos;
		break;

	case 17:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 2;
		match_pos = pos;
		break;

	case 18:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 113 ) || ( info.src.charCodeAt( pos ) >= 115 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 114 ) state = 13;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 19:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 114 ) || ( info.src.charCodeAt( pos ) >= 116 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 115 ) state = 14;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 20:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 98 ) || ( info.src.charCodeAt( pos ) >= 100 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 99 ) state = 15;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 21:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 109 ) || ( info.src.charCodeAt( pos ) >= 111 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 110 ) state = 16;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 22:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 100 ) || ( info.src.charCodeAt( pos ) >= 102 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 101 ) state = 17;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 23:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 98 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 97 ) state = 18;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 24:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 114 ) || ( info.src.charCodeAt( pos ) >= 116 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 115 ) state = 19;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 25:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 104 ) || ( info.src.charCodeAt( pos ) >= 106 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 105 ) state = 20;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 26:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 110 ) || ( info.src.charCodeAt( pos ) >= 112 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 111 ) state = 21;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 27:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 98 ) || ( info.src.charCodeAt( pos ) >= 100 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 99 ) state = 22;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 28:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 98 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 97 ) state = 24;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 29:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 115 ) || ( info.src.charCodeAt( pos ) >= 117 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 116 ) state = 25;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 30:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 104 ) || ( info.src.charCodeAt( pos ) >= 106 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 105 ) state = 26;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 31:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 98 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 97 ) state = 27;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 32:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 107 ) || ( info.src.charCodeAt( pos ) >= 109 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 108 ) state = 28;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 33:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 98 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 97 ) state = 29;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 34:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 115 ) || ( info.src.charCodeAt( pos ) >= 117 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 116 ) state = 30;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 35:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 111 ) || ( info.src.charCodeAt( pos ) >= 113 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 112 ) state = 31;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 36:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 115 ) || ( info.src.charCodeAt( pos ) >= 117 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 116 ) state = 33;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 37:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 98 ) || ( info.src.charCodeAt( pos ) >= 100 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 99 ) state = 34;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 38:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 114 ) || ( info.src.charCodeAt( pos ) >= 116 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 115 ) state = 35;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 39:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 109 ) || ( info.src.charCodeAt( pos ) >= 111 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 110 ) state = 37;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 40:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 100 ) || ( info.src.charCodeAt( pos ) >= 102 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 101 ) state = 38;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 41:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 116 ) || ( info.src.charCodeAt( pos ) >= 118 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 117 ) state = 39;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 42:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 108 ) || ( info.src.charCodeAt( pos ) >= 110 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 109 ) state = 40;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 43:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 98 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 97 ) state = 42;
		else state = -1;
		match = 16;
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
	new Array( 19/* Global */, 1 ),
	new Array( 21/* Namespace */, 9 ),
	new Array( 18/* NamespaceContents */, 4 ),
	new Array( 18/* NamespaceContents */, 1 ),
	new Array( 22/* NamespaceContent */, 1 ),
	new Array( 22/* NamespaceContent */, 1 ),
	new Array( 22/* NamespaceContent */, 1 ),
	new Array( 22/* NamespaceContent */, 1 ),
	new Array( 22/* NamespaceContent */, 1 ),
	new Array( 23/* Class */, 9 ),
	new Array( 27/* ClassContents */, 3 ),
	new Array( 27/* ClassContents */, 1 ),
	new Array( 28/* ClassContent */, 1 ),
	new Array( 28/* ClassContent */, 1 ),
	new Array( 28/* ClassContent */, 1 ),
	new Array( 28/* ClassContent */, 1 ),
	new Array( 24/* VariableDef */, 9 ),
	new Array( 24/* VariableDef */, 11 ),
	new Array( 25/* Function */, 13 ),
	new Array( 25/* Function */, 11 ),
	new Array( 30/* ArgumentList */, 5 ),
	new Array( 30/* ArgumentList */, 1 ),
	new Array( 26/* NativeBlock */, 5 ),
	new Array( 29/* NativeCodeInline */, 2 ),
	new Array( 29/* NativeCodeInline */, 4 ),
	new Array( 29/* NativeCodeInline */, 4 ),
	new Array( 29/* NativeCodeInline */, 0 ),
	new Array( 31/* NativeCode */, 2 ),
	new Array( 31/* NativeCode */, 2 ),
	new Array( 31/* NativeCode */, 4 ),
	new Array( 31/* NativeCode */, 4 ),
	new Array( 31/* NativeCode */, 0 ),
	new Array( 20/* W */, 2 ),
	new Array( 20/* W */, 6 ),
	new Array( 20/* W */, 0 ),
	new Array( 34/* MLComment */, 2 ),
	new Array( 34/* MLComment */, 2 ),
	new Array( 34/* MLComment */, 2 ),
	new Array( 34/* MLComment */, 2 ),
	new Array( 34/* MLComment */, 2 ),
	new Array( 34/* MLComment */, 2 ),
	new Array( 34/* MLComment */, 0 ),
	new Array( 32/* PossibleJunk */, 1 ),
	new Array( 32/* PossibleJunk */, 1 ),
	new Array( 32/* PossibleJunk */, 1 ),
	new Array( 32/* PossibleJunk */, 1 ),
	new Array( 32/* PossibleJunk */, 1 ),
	new Array( 32/* PossibleJunk */, 1 ),
	new Array( 32/* PossibleJunk */, 1 ),
	new Array( 32/* PossibleJunk */, 1 ),
	new Array( 32/* PossibleJunk */, 1 ),
	new Array( 32/* PossibleJunk */, 1 ),
	new Array( 32/* PossibleJunk */, 1 ),
	new Array( 32/* PossibleJunk */, 1 ),
	new Array( 33/* _W */, 1 ),
	new Array( 33/* _W */, 0 )
);

/* Action-Table */
var act_tab = new Array(
	/* State 0 */ new Array( 35/* "$" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 , 2/* "namespace" */,-35 , 3/* "class" */,-35 , 6/* "var" */,-35 , 5/* "static" */,-35 , 4/* "function" */,-35 , 16/* "Identifier" */,-35 ),
	/* State 1 */ new Array( 35/* "$" */,0 ),
	/* State 2 */ new Array( 35/* "$" */,-1 , 2/* "namespace" */,-35 , 3/* "class" */,-35 , 6/* "var" */,-35 , 5/* "static" */,-35 , 4/* "function" */,-35 , 16/* "Identifier" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 3 */ new Array( 15/* "/" */,5 , 1/* "WHTS" */,7 , 35/* "$" */,-4 , 2/* "namespace" */,-4 , 3/* "class" */,-4 , 6/* "var" */,-4 , 5/* "static" */,-4 , 4/* "function" */,-4 , 16/* "Identifier" */,-4 , 8/* "}" */,-4 ),
	/* State 4 */ new Array( 15/* "/" */,5 , 1/* "WHTS" */,7 , 2/* "namespace" */,14 , 3/* "class" */,15 , 6/* "var" */,16 , 5/* "static" */,17 , 4/* "function" */,18 , 16/* "Identifier" */,19 ),
	/* State 5 */ new Array( 14/* "*" */,20 ),
	/* State 6 */ new Array( 35/* "$" */,-33 , 1/* "WHTS" */,-33 , 15/* "/" */,-33 , 2/* "namespace" */,-33 , 3/* "class" */,-33 , 6/* "var" */,-33 , 5/* "static" */,-33 , 4/* "function" */,-33 , 16/* "Identifier" */,-33 , 7/* "{" */,-33 , 11/* "=" */,-33 , 9/* "(" */,-33 , 8/* "}" */,-33 , 17/* "Junk" */,-33 , 13/* "," */,-33 , 14/* "*" */,-33 , 12/* ";" */,-33 , 10/* ")" */,-33 ),
	/* State 7 */ new Array( 35/* "$" */,-55 , 1/* "WHTS" */,-55 , 15/* "/" */,-55 , 2/* "namespace" */,-55 , 3/* "class" */,-55 , 6/* "var" */,-55 , 5/* "static" */,-55 , 4/* "function" */,-55 , 16/* "Identifier" */,-55 , 7/* "{" */,-55 , 14/* "*" */,-55 , 17/* "Junk" */,-55 , 11/* "=" */,-55 , 13/* "," */,-55 , 12/* ";" */,-55 , 8/* "}" */,-55 , 9/* "(" */,-55 , 10/* ")" */,-55 ),
	/* State 8 */ new Array( 35/* "$" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 , 2/* "namespace" */,-35 , 3/* "class" */,-35 , 6/* "var" */,-35 , 5/* "static" */,-35 , 4/* "function" */,-35 , 16/* "Identifier" */,-35 ),
	/* State 9 */ new Array( 1/* "WHTS" */,-5 , 15/* "/" */,-5 , 35/* "$" */,-5 , 2/* "namespace" */,-5 , 3/* "class" */,-5 , 6/* "var" */,-5 , 5/* "static" */,-5 , 4/* "function" */,-5 , 16/* "Identifier" */,-5 , 8/* "}" */,-5 ),
	/* State 10 */ new Array( 1/* "WHTS" */,-6 , 15/* "/" */,-6 , 35/* "$" */,-6 , 2/* "namespace" */,-6 , 3/* "class" */,-6 , 6/* "var" */,-6 , 5/* "static" */,-6 , 4/* "function" */,-6 , 16/* "Identifier" */,-6 , 8/* "}" */,-6 ),
	/* State 11 */ new Array( 1/* "WHTS" */,-7 , 15/* "/" */,-7 , 35/* "$" */,-7 , 2/* "namespace" */,-7 , 3/* "class" */,-7 , 6/* "var" */,-7 , 5/* "static" */,-7 , 4/* "function" */,-7 , 16/* "Identifier" */,-7 , 8/* "}" */,-7 ),
	/* State 12 */ new Array( 1/* "WHTS" */,-8 , 15/* "/" */,-8 , 35/* "$" */,-8 , 2/* "namespace" */,-8 , 3/* "class" */,-8 , 6/* "var" */,-8 , 5/* "static" */,-8 , 4/* "function" */,-8 , 16/* "Identifier" */,-8 , 8/* "}" */,-8 ),
	/* State 13 */ new Array( 1/* "WHTS" */,-9 , 15/* "/" */,-9 , 35/* "$" */,-9 , 2/* "namespace" */,-9 , 3/* "class" */,-9 , 6/* "var" */,-9 , 5/* "static" */,-9 , 4/* "function" */,-9 , 16/* "Identifier" */,-9 , 8/* "}" */,-9 ),
	/* State 14 */ new Array( 16/* "Identifier" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 15 */ new Array( 16/* "Identifier" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 16 */ new Array( 16/* "Identifier" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 17 */ new Array( 6/* "var" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 18 */ new Array( 16/* "Identifier" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 19 */ new Array( 7/* "{" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 20 */ new Array( 14/* "*" */,-42 , 17/* "Junk" */,-42 , 2/* "namespace" */,-42 , 3/* "class" */,-42 , 4/* "function" */,-42 , 5/* "static" */,-42 , 6/* "var" */,-42 , 11/* "=" */,-42 , 13/* "," */,-42 , 15/* "/" */,-42 , 16/* "Identifier" */,-42 , 1/* "WHTS" */,-42 , 12/* ";" */,-42 , 7/* "{" */,-42 , 8/* "}" */,-42 , 9/* "(" */,-42 , 10/* ")" */,-42 ),
	/* State 21 */ new Array( 15/* "/" */,5 , 1/* "WHTS" */,7 , 35/* "$" */,-3 , 2/* "namespace" */,-3 , 3/* "class" */,-3 , 6/* "var" */,-3 , 5/* "static" */,-3 , 4/* "function" */,-3 , 16/* "Identifier" */,-3 , 8/* "}" */,-3 ),
	/* State 22 */ new Array( 15/* "/" */,5 , 16/* "Identifier" */,29 , 1/* "WHTS" */,7 ),
	/* State 23 */ new Array( 15/* "/" */,5 , 16/* "Identifier" */,30 , 1/* "WHTS" */,7 ),
	/* State 24 */ new Array( 15/* "/" */,5 , 16/* "Identifier" */,31 , 1/* "WHTS" */,7 ),
	/* State 25 */ new Array( 15/* "/" */,5 , 6/* "var" */,32 , 1/* "WHTS" */,7 ),
	/* State 26 */ new Array( 15/* "/" */,5 , 16/* "Identifier" */,33 , 1/* "WHTS" */,7 ),
	/* State 27 */ new Array( 15/* "/" */,5 , 7/* "{" */,34 , 1/* "WHTS" */,7 ),
	/* State 28 */ new Array( 10/* ")" */,35 , 9/* "(" */,36 , 8/* "}" */,37 , 7/* "{" */,38 , 12/* ";" */,39 , 14/* "*" */,41 , 17/* "Junk" */,42 , 2/* "namespace" */,43 , 3/* "class" */,44 , 4/* "function" */,45 , 5/* "static" */,46 , 6/* "var" */,47 , 11/* "=" */,48 , 13/* "," */,49 , 15/* "/" */,50 , 16/* "Identifier" */,51 , 1/* "WHTS" */,7 ),
	/* State 29 */ new Array( 7/* "{" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 30 */ new Array( 7/* "{" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 31 */ new Array( 11/* "=" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 32 */ new Array( 16/* "Identifier" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 33 */ new Array( 9/* "(" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 34 */ new Array( 8/* "}" */,-32 , 17/* "Junk" */,-32 , 2/* "namespace" */,-32 , 3/* "class" */,-32 , 4/* "function" */,-32 , 5/* "static" */,-32 , 6/* "var" */,-32 , 11/* "=" */,-32 , 13/* "," */,-32 , 14/* "*" */,-32 , 15/* "/" */,-32 , 16/* "Identifier" */,-32 , 1/* "WHTS" */,-32 , 12/* ";" */,-32 , 7/* "{" */,-32 , 9/* "(" */,-32 ),
	/* State 35 */ new Array( 14/* "*" */,-41 , 17/* "Junk" */,-41 , 2/* "namespace" */,-41 , 3/* "class" */,-41 , 4/* "function" */,-41 , 5/* "static" */,-41 , 6/* "var" */,-41 , 11/* "=" */,-41 , 13/* "," */,-41 , 15/* "/" */,-41 , 16/* "Identifier" */,-41 , 1/* "WHTS" */,-41 , 12/* ";" */,-41 , 7/* "{" */,-41 , 8/* "}" */,-41 , 9/* "(" */,-41 , 10/* ")" */,-41 ),
	/* State 36 */ new Array( 14/* "*" */,-40 , 17/* "Junk" */,-40 , 2/* "namespace" */,-40 , 3/* "class" */,-40 , 4/* "function" */,-40 , 5/* "static" */,-40 , 6/* "var" */,-40 , 11/* "=" */,-40 , 13/* "," */,-40 , 15/* "/" */,-40 , 16/* "Identifier" */,-40 , 1/* "WHTS" */,-40 , 12/* ";" */,-40 , 7/* "{" */,-40 , 8/* "}" */,-40 , 9/* "(" */,-40 , 10/* ")" */,-40 ),
	/* State 37 */ new Array( 14/* "*" */,-39 , 17/* "Junk" */,-39 , 2/* "namespace" */,-39 , 3/* "class" */,-39 , 4/* "function" */,-39 , 5/* "static" */,-39 , 6/* "var" */,-39 , 11/* "=" */,-39 , 13/* "," */,-39 , 15/* "/" */,-39 , 16/* "Identifier" */,-39 , 1/* "WHTS" */,-39 , 12/* ";" */,-39 , 7/* "{" */,-39 , 8/* "}" */,-39 , 9/* "(" */,-39 , 10/* ")" */,-39 ),
	/* State 38 */ new Array( 14/* "*" */,-38 , 17/* "Junk" */,-38 , 2/* "namespace" */,-38 , 3/* "class" */,-38 , 4/* "function" */,-38 , 5/* "static" */,-38 , 6/* "var" */,-38 , 11/* "=" */,-38 , 13/* "," */,-38 , 15/* "/" */,-38 , 16/* "Identifier" */,-38 , 1/* "WHTS" */,-38 , 12/* ";" */,-38 , 7/* "{" */,-38 , 8/* "}" */,-38 , 9/* "(" */,-38 , 10/* ")" */,-38 ),
	/* State 39 */ new Array( 14/* "*" */,-37 , 17/* "Junk" */,-37 , 2/* "namespace" */,-37 , 3/* "class" */,-37 , 4/* "function" */,-37 , 5/* "static" */,-37 , 6/* "var" */,-37 , 11/* "=" */,-37 , 13/* "," */,-37 , 15/* "/" */,-37 , 16/* "Identifier" */,-37 , 1/* "WHTS" */,-37 , 12/* ";" */,-37 , 7/* "{" */,-37 , 8/* "}" */,-37 , 9/* "(" */,-37 , 10/* ")" */,-37 ),
	/* State 40 */ new Array( 14/* "*" */,-36 , 17/* "Junk" */,-36 , 2/* "namespace" */,-36 , 3/* "class" */,-36 , 4/* "function" */,-36 , 5/* "static" */,-36 , 6/* "var" */,-36 , 11/* "=" */,-36 , 13/* "," */,-36 , 15/* "/" */,-36 , 16/* "Identifier" */,-36 , 1/* "WHTS" */,-36 , 12/* ";" */,-36 , 7/* "{" */,-36 , 8/* "}" */,-36 , 9/* "(" */,-36 , 10/* ")" */,-36 ),
	/* State 41 */ new Array( 15/* "/" */,59 , 14/* "*" */,-51 , 17/* "Junk" */,-51 , 2/* "namespace" */,-51 , 3/* "class" */,-51 , 4/* "function" */,-51 , 5/* "static" */,-51 , 6/* "var" */,-51 , 11/* "=" */,-51 , 13/* "," */,-51 , 16/* "Identifier" */,-51 , 1/* "WHTS" */,-51 , 12/* ";" */,-51 , 7/* "{" */,-51 , 8/* "}" */,-51 , 9/* "(" */,-51 , 10/* ")" */,-51 ),
	/* State 42 */ new Array( 14/* "*" */,-43 , 17/* "Junk" */,-43 , 2/* "namespace" */,-43 , 3/* "class" */,-43 , 4/* "function" */,-43 , 5/* "static" */,-43 , 6/* "var" */,-43 , 11/* "=" */,-43 , 13/* "," */,-43 , 15/* "/" */,-43 , 16/* "Identifier" */,-43 , 1/* "WHTS" */,-43 , 12/* ";" */,-43 , 7/* "{" */,-43 , 8/* "}" */,-43 , 9/* "(" */,-43 , 10/* ")" */,-43 ),
	/* State 43 */ new Array( 14/* "*" */,-44 , 17/* "Junk" */,-44 , 2/* "namespace" */,-44 , 3/* "class" */,-44 , 4/* "function" */,-44 , 5/* "static" */,-44 , 6/* "var" */,-44 , 11/* "=" */,-44 , 13/* "," */,-44 , 15/* "/" */,-44 , 16/* "Identifier" */,-44 , 1/* "WHTS" */,-44 , 12/* ";" */,-44 , 7/* "{" */,-44 , 8/* "}" */,-44 , 9/* "(" */,-44 , 10/* ")" */,-44 ),
	/* State 44 */ new Array( 14/* "*" */,-45 , 17/* "Junk" */,-45 , 2/* "namespace" */,-45 , 3/* "class" */,-45 , 4/* "function" */,-45 , 5/* "static" */,-45 , 6/* "var" */,-45 , 11/* "=" */,-45 , 13/* "," */,-45 , 15/* "/" */,-45 , 16/* "Identifier" */,-45 , 1/* "WHTS" */,-45 , 12/* ";" */,-45 , 7/* "{" */,-45 , 8/* "}" */,-45 , 9/* "(" */,-45 , 10/* ")" */,-45 ),
	/* State 45 */ new Array( 14/* "*" */,-46 , 17/* "Junk" */,-46 , 2/* "namespace" */,-46 , 3/* "class" */,-46 , 4/* "function" */,-46 , 5/* "static" */,-46 , 6/* "var" */,-46 , 11/* "=" */,-46 , 13/* "," */,-46 , 15/* "/" */,-46 , 16/* "Identifier" */,-46 , 1/* "WHTS" */,-46 , 12/* ";" */,-46 , 7/* "{" */,-46 , 8/* "}" */,-46 , 9/* "(" */,-46 , 10/* ")" */,-46 ),
	/* State 46 */ new Array( 14/* "*" */,-47 , 17/* "Junk" */,-47 , 2/* "namespace" */,-47 , 3/* "class" */,-47 , 4/* "function" */,-47 , 5/* "static" */,-47 , 6/* "var" */,-47 , 11/* "=" */,-47 , 13/* "," */,-47 , 15/* "/" */,-47 , 16/* "Identifier" */,-47 , 1/* "WHTS" */,-47 , 12/* ";" */,-47 , 7/* "{" */,-47 , 8/* "}" */,-47 , 9/* "(" */,-47 , 10/* ")" */,-47 ),
	/* State 47 */ new Array( 14/* "*" */,-48 , 17/* "Junk" */,-48 , 2/* "namespace" */,-48 , 3/* "class" */,-48 , 4/* "function" */,-48 , 5/* "static" */,-48 , 6/* "var" */,-48 , 11/* "=" */,-48 , 13/* "," */,-48 , 15/* "/" */,-48 , 16/* "Identifier" */,-48 , 1/* "WHTS" */,-48 , 12/* ";" */,-48 , 7/* "{" */,-48 , 8/* "}" */,-48 , 9/* "(" */,-48 , 10/* ")" */,-48 ),
	/* State 48 */ new Array( 14/* "*" */,-49 , 17/* "Junk" */,-49 , 2/* "namespace" */,-49 , 3/* "class" */,-49 , 4/* "function" */,-49 , 5/* "static" */,-49 , 6/* "var" */,-49 , 11/* "=" */,-49 , 13/* "," */,-49 , 15/* "/" */,-49 , 16/* "Identifier" */,-49 , 1/* "WHTS" */,-49 , 12/* ";" */,-49 , 7/* "{" */,-49 , 8/* "}" */,-49 , 9/* "(" */,-49 , 10/* ")" */,-49 ),
	/* State 49 */ new Array( 14/* "*" */,-50 , 17/* "Junk" */,-50 , 2/* "namespace" */,-50 , 3/* "class" */,-50 , 4/* "function" */,-50 , 5/* "static" */,-50 , 6/* "var" */,-50 , 11/* "=" */,-50 , 13/* "," */,-50 , 15/* "/" */,-50 , 16/* "Identifier" */,-50 , 1/* "WHTS" */,-50 , 12/* ";" */,-50 , 7/* "{" */,-50 , 8/* "}" */,-50 , 9/* "(" */,-50 , 10/* ")" */,-50 ),
	/* State 50 */ new Array( 14/* "*" */,-52 , 17/* "Junk" */,-52 , 2/* "namespace" */,-52 , 3/* "class" */,-52 , 4/* "function" */,-52 , 5/* "static" */,-52 , 6/* "var" */,-52 , 11/* "=" */,-52 , 13/* "," */,-52 , 15/* "/" */,-52 , 16/* "Identifier" */,-52 , 1/* "WHTS" */,-52 , 12/* ";" */,-52 , 7/* "{" */,-52 , 8/* "}" */,-52 , 9/* "(" */,-52 , 10/* ")" */,-52 ),
	/* State 51 */ new Array( 14/* "*" */,-53 , 17/* "Junk" */,-53 , 2/* "namespace" */,-53 , 3/* "class" */,-53 , 4/* "function" */,-53 , 5/* "static" */,-53 , 6/* "var" */,-53 , 11/* "=" */,-53 , 13/* "," */,-53 , 15/* "/" */,-53 , 16/* "Identifier" */,-53 , 1/* "WHTS" */,-53 , 12/* ";" */,-53 , 7/* "{" */,-53 , 8/* "}" */,-53 , 9/* "(" */,-53 , 10/* ")" */,-53 ),
	/* State 52 */ new Array( 14/* "*" */,-54 , 17/* "Junk" */,-54 , 2/* "namespace" */,-54 , 3/* "class" */,-54 , 4/* "function" */,-54 , 5/* "static" */,-54 , 6/* "var" */,-54 , 11/* "=" */,-54 , 13/* "," */,-54 , 15/* "/" */,-54 , 16/* "Identifier" */,-54 , 1/* "WHTS" */,-54 , 12/* ";" */,-54 , 7/* "{" */,-54 , 8/* "}" */,-54 , 9/* "(" */,-54 , 10/* ")" */,-54 ),
	/* State 53 */ new Array( 15/* "/" */,5 , 7/* "{" */,60 , 1/* "WHTS" */,7 ),
	/* State 54 */ new Array( 15/* "/" */,5 , 7/* "{" */,61 , 1/* "WHTS" */,7 ),
	/* State 55 */ new Array( 15/* "/" */,5 , 11/* "=" */,62 , 1/* "WHTS" */,7 ),
	/* State 56 */ new Array( 15/* "/" */,5 , 16/* "Identifier" */,63 , 1/* "WHTS" */,7 ),
	/* State 57 */ new Array( 15/* "/" */,5 , 9/* "(" */,64 , 1/* "WHTS" */,7 ),
	/* State 58 */ new Array( 9/* "(" */,65 , 7/* "{" */,66 , 12/* ";" */,67 , 8/* "}" */,69 , 17/* "Junk" */,42 , 2/* "namespace" */,43 , 3/* "class" */,44 , 4/* "function" */,45 , 5/* "static" */,46 , 6/* "var" */,47 , 11/* "=" */,48 , 13/* "," */,49 , 14/* "*" */,70 , 15/* "/" */,50 , 16/* "Identifier" */,51 , 1/* "WHTS" */,7 ),
	/* State 59 */ new Array( 35/* "$" */,-34 , 1/* "WHTS" */,-34 , 15/* "/" */,-34 , 2/* "namespace" */,-34 , 3/* "class" */,-34 , 6/* "var" */,-34 , 5/* "static" */,-34 , 4/* "function" */,-34 , 16/* "Identifier" */,-34 , 7/* "{" */,-34 , 11/* "=" */,-34 , 9/* "(" */,-34 , 8/* "}" */,-34 , 17/* "Junk" */,-34 , 13/* "," */,-34 , 14/* "*" */,-34 , 12/* ";" */,-34 , 10/* ")" */,-34 ),
	/* State 60 */ new Array( 1/* "WHTS" */,-35 , 15/* "/" */,-35 , 2/* "namespace" */,-35 , 3/* "class" */,-35 , 6/* "var" */,-35 , 5/* "static" */,-35 , 4/* "function" */,-35 , 16/* "Identifier" */,-35 , 8/* "}" */,-35 ),
	/* State 61 */ new Array( 1/* "WHTS" */,-35 , 15/* "/" */,-35 , 3/* "class" */,-35 , 6/* "var" */,-35 , 5/* "static" */,-35 , 4/* "function" */,-35 , 16/* "Identifier" */,-35 , 8/* "}" */,-35 ),
	/* State 62 */ new Array( 17/* "Junk" */,-35 , 2/* "namespace" */,-35 , 3/* "class" */,-35 , 4/* "function" */,-35 , 5/* "static" */,-35 , 6/* "var" */,-35 , 11/* "=" */,-35 , 13/* "," */,-35 , 14/* "*" */,-35 , 15/* "/" */,-35 , 16/* "Identifier" */,-35 , 7/* "{" */,-35 , 9/* "(" */,-35 , 12/* ";" */,-35 , 1/* "WHTS" */,-35 ),
	/* State 63 */ new Array( 11/* "=" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 64 */ new Array( 16/* "Identifier" */,-35 , 10/* ")" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 65 */ new Array( 10/* ")" */,-32 , 17/* "Junk" */,-32 , 2/* "namespace" */,-32 , 3/* "class" */,-32 , 4/* "function" */,-32 , 5/* "static" */,-32 , 6/* "var" */,-32 , 11/* "=" */,-32 , 13/* "," */,-32 , 14/* "*" */,-32 , 15/* "/" */,-32 , 16/* "Identifier" */,-32 , 1/* "WHTS" */,-32 , 12/* ";" */,-32 , 7/* "{" */,-32 , 9/* "(" */,-32 ),
	/* State 66 */ new Array( 8/* "}" */,-32 , 17/* "Junk" */,-32 , 2/* "namespace" */,-32 , 3/* "class" */,-32 , 4/* "function" */,-32 , 5/* "static" */,-32 , 6/* "var" */,-32 , 11/* "=" */,-32 , 13/* "," */,-32 , 14/* "*" */,-32 , 15/* "/" */,-32 , 16/* "Identifier" */,-32 , 1/* "WHTS" */,-32 , 12/* ";" */,-32 , 7/* "{" */,-32 , 9/* "(" */,-32 ),
	/* State 67 */ new Array( 8/* "}" */,-29 , 17/* "Junk" */,-29 , 2/* "namespace" */,-29 , 3/* "class" */,-29 , 4/* "function" */,-29 , 5/* "static" */,-29 , 6/* "var" */,-29 , 11/* "=" */,-29 , 13/* "," */,-29 , 14/* "*" */,-29 , 15/* "/" */,-29 , 16/* "Identifier" */,-29 , 1/* "WHTS" */,-29 , 12/* ";" */,-29 , 7/* "{" */,-29 , 9/* "(" */,-29 , 10/* ")" */,-29 ),
	/* State 68 */ new Array( 8/* "}" */,-28 , 17/* "Junk" */,-28 , 2/* "namespace" */,-28 , 3/* "class" */,-28 , 4/* "function" */,-28 , 5/* "static" */,-28 , 6/* "var" */,-28 , 11/* "=" */,-28 , 13/* "," */,-28 , 14/* "*" */,-28 , 15/* "/" */,-28 , 16/* "Identifier" */,-28 , 1/* "WHTS" */,-28 , 12/* ";" */,-28 , 7/* "{" */,-28 , 9/* "(" */,-28 , 10/* ")" */,-28 ),
	/* State 69 */ new Array( 1/* "WHTS" */,-23 , 15/* "/" */,-23 , 35/* "$" */,-23 , 2/* "namespace" */,-23 , 3/* "class" */,-23 , 6/* "var" */,-23 , 5/* "static" */,-23 , 4/* "function" */,-23 , 16/* "Identifier" */,-23 , 8/* "}" */,-23 ),
	/* State 70 */ new Array( 8/* "}" */,-51 , 17/* "Junk" */,-51 , 2/* "namespace" */,-51 , 3/* "class" */,-51 , 4/* "function" */,-51 , 5/* "static" */,-51 , 6/* "var" */,-51 , 11/* "=" */,-51 , 13/* "," */,-51 , 14/* "*" */,-51 , 15/* "/" */,-51 , 16/* "Identifier" */,-51 , 1/* "WHTS" */,-51 , 12/* ";" */,-51 , 7/* "{" */,-51 , 9/* "(" */,-51 , 10/* ")" */,-51 ),
	/* State 71 */ new Array( 15/* "/" */,5 , 1/* "WHTS" */,7 , 2/* "namespace" */,-35 , 3/* "class" */,-35 , 6/* "var" */,-35 , 5/* "static" */,-35 , 4/* "function" */,-35 , 16/* "Identifier" */,-35 , 8/* "}" */,-35 ),
	/* State 72 */ new Array( 15/* "/" */,5 , 1/* "WHTS" */,7 , 3/* "class" */,-35 , 6/* "var" */,-35 , 5/* "static" */,-35 , 4/* "function" */,-35 , 16/* "Identifier" */,-35 , 8/* "}" */,-35 ),
	/* State 73 */ new Array( 15/* "/" */,5 , 1/* "WHTS" */,7 , 12/* ";" */,-27 , 17/* "Junk" */,-32 , 2/* "namespace" */,-32 , 3/* "class" */,-32 , 4/* "function" */,-32 , 5/* "static" */,-32 , 6/* "var" */,-32 , 11/* "=" */,-32 , 13/* "," */,-32 , 14/* "*" */,-32 , 16/* "Identifier" */,-32 , 7/* "{" */,-32 , 9/* "(" */,-32 ),
	/* State 74 */ new Array( 15/* "/" */,5 , 11/* "=" */,83 , 1/* "WHTS" */,7 ),
	/* State 75 */ new Array( 15/* "/" */,5 , 10/* ")" */,85 , 16/* "Identifier" */,86 , 1/* "WHTS" */,7 ),
	/* State 76 */ new Array( 9/* "(" */,65 , 7/* "{" */,66 , 12/* ";" */,67 , 10/* ")" */,87 , 17/* "Junk" */,42 , 2/* "namespace" */,43 , 3/* "class" */,44 , 4/* "function" */,45 , 5/* "static" */,46 , 6/* "var" */,47 , 11/* "=" */,48 , 13/* "," */,49 , 14/* "*" */,70 , 15/* "/" */,50 , 16/* "Identifier" */,51 , 1/* "WHTS" */,7 ),
	/* State 77 */ new Array( 9/* "(" */,65 , 7/* "{" */,66 , 12/* ";" */,67 , 8/* "}" */,88 , 17/* "Junk" */,42 , 2/* "namespace" */,43 , 3/* "class" */,44 , 4/* "function" */,45 , 5/* "static" */,46 , 6/* "var" */,47 , 11/* "=" */,48 , 13/* "," */,49 , 14/* "*" */,70 , 15/* "/" */,50 , 16/* "Identifier" */,51 , 1/* "WHTS" */,7 ),
	/* State 78 */ new Array( 8/* "}" */,-35 , 2/* "namespace" */,-35 , 3/* "class" */,-35 , 6/* "var" */,-35 , 5/* "static" */,-35 , 4/* "function" */,-35 , 16/* "Identifier" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 79 */ new Array( 8/* "}" */,-35 , 3/* "class" */,-35 , 6/* "var" */,-35 , 5/* "static" */,-35 , 4/* "function" */,-35 , 16/* "Identifier" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 80 */ new Array( 15/* "/" */,5 , 1/* "WHTS" */,7 , 8/* "}" */,-12 , 3/* "class" */,-12 , 6/* "var" */,-12 , 5/* "static" */,-12 , 4/* "function" */,-12 , 16/* "Identifier" */,-12 ),
	/* State 81 */ new Array( 12/* ";" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 82 */ new Array( 9/* "(" */,92 , 7/* "{" */,93 , 12/* ";" */,67 , 17/* "Junk" */,42 , 2/* "namespace" */,43 , 3/* "class" */,44 , 4/* "function" */,45 , 5/* "static" */,46 , 6/* "var" */,47 , 11/* "=" */,48 , 13/* "," */,49 , 14/* "*" */,70 , 15/* "/" */,50 , 16/* "Identifier" */,51 , 1/* "WHTS" */,7 ),
	/* State 83 */ new Array( 17/* "Junk" */,-35 , 2/* "namespace" */,-35 , 3/* "class" */,-35 , 4/* "function" */,-35 , 5/* "static" */,-35 , 6/* "var" */,-35 , 11/* "=" */,-35 , 13/* "," */,-35 , 14/* "*" */,-35 , 15/* "/" */,-35 , 16/* "Identifier" */,-35 , 7/* "{" */,-35 , 9/* "(" */,-35 , 12/* ";" */,-35 , 1/* "WHTS" */,-35 ),
	/* State 84 */ new Array( 10/* ")" */,-35 , 13/* "," */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 85 */ new Array( 7/* "{" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 86 */ new Array( 1/* "WHTS" */,-22 , 15/* "/" */,-22 , 10/* ")" */,-22 , 13/* "," */,-22 ),
	/* State 87 */ new Array( 8/* "}" */,-31 , 17/* "Junk" */,-31 , 2/* "namespace" */,-31 , 3/* "class" */,-31 , 4/* "function" */,-31 , 5/* "static" */,-31 , 6/* "var" */,-31 , 11/* "=" */,-31 , 13/* "," */,-31 , 14/* "*" */,-31 , 15/* "/" */,-31 , 16/* "Identifier" */,-31 , 1/* "WHTS" */,-31 , 12/* ";" */,-31 , 7/* "{" */,-31 , 9/* "(" */,-31 , 10/* ")" */,-31 ),
	/* State 88 */ new Array( 8/* "}" */,-30 , 17/* "Junk" */,-30 , 2/* "namespace" */,-30 , 3/* "class" */,-30 , 4/* "function" */,-30 , 5/* "static" */,-30 , 6/* "var" */,-30 , 11/* "=" */,-30 , 13/* "," */,-30 , 14/* "*" */,-30 , 15/* "/" */,-30 , 16/* "Identifier" */,-30 , 1/* "WHTS" */,-30 , 12/* ";" */,-30 , 7/* "{" */,-30 , 9/* "(" */,-30 , 10/* ")" */,-30 ),
	/* State 89 */ new Array( 15/* "/" */,5 , 8/* "}" */,98 , 1/* "WHTS" */,7 , 2/* "namespace" */,14 , 3/* "class" */,15 , 6/* "var" */,16 , 5/* "static" */,17 , 4/* "function" */,18 , 16/* "Identifier" */,19 ),
	/* State 90 */ new Array( 15/* "/" */,5 , 8/* "}" */,99 , 1/* "WHTS" */,7 , 3/* "class" */,15 , 6/* "var" */,16 , 5/* "static" */,17 , 4/* "function" */,18 , 16/* "Identifier" */,19 ),
	/* State 91 */ new Array( 15/* "/" */,5 , 12/* ";" */,105 , 1/* "WHTS" */,7 ),
	/* State 92 */ new Array( 10/* ")" */,-32 , 17/* "Junk" */,-32 , 2/* "namespace" */,-32 , 3/* "class" */,-32 , 4/* "function" */,-32 , 5/* "static" */,-32 , 6/* "var" */,-32 , 11/* "=" */,-32 , 13/* "," */,-32 , 14/* "*" */,-32 , 15/* "/" */,-32 , 16/* "Identifier" */,-32 , 1/* "WHTS" */,-32 , 12/* ";" */,-32 , 7/* "{" */,-32 , 9/* "(" */,-32 ),
	/* State 93 */ new Array( 8/* "}" */,-32 , 17/* "Junk" */,-32 , 2/* "namespace" */,-32 , 3/* "class" */,-32 , 4/* "function" */,-32 , 5/* "static" */,-32 , 6/* "var" */,-32 , 11/* "=" */,-32 , 13/* "," */,-32 , 14/* "*" */,-32 , 15/* "/" */,-32 , 16/* "Identifier" */,-32 , 1/* "WHTS" */,-32 , 12/* ";" */,-32 , 7/* "{" */,-32 , 9/* "(" */,-32 ),
	/* State 94 */ new Array( 17/* "Junk" */,-28 , 2/* "namespace" */,-28 , 3/* "class" */,-28 , 4/* "function" */,-28 , 5/* "static" */,-28 , 6/* "var" */,-28 , 11/* "=" */,-28 , 13/* "," */,-28 , 14/* "*" */,-28 , 15/* "/" */,-24 , 16/* "Identifier" */,-28 , 1/* "WHTS" */,-24 , 12/* ";" */,-24 , 7/* "{" */,-28 , 9/* "(" */,-28 ),
	/* State 95 */ new Array( 15/* "/" */,5 , 1/* "WHTS" */,7 , 12/* ";" */,-27 , 17/* "Junk" */,-32 , 2/* "namespace" */,-32 , 3/* "class" */,-32 , 4/* "function" */,-32 , 5/* "static" */,-32 , 6/* "var" */,-32 , 11/* "=" */,-32 , 13/* "," */,-32 , 14/* "*" */,-32 , 16/* "Identifier" */,-32 , 7/* "{" */,-32 , 9/* "(" */,-32 ),
	/* State 96 */ new Array( 15/* "/" */,5 , 10/* ")" */,109 , 13/* "," */,110 , 1/* "WHTS" */,7 ),
	/* State 97 */ new Array( 15/* "/" */,5 , 7/* "{" */,111 , 1/* "WHTS" */,7 ),
	/* State 98 */ new Array( 1/* "WHTS" */,-2 , 15/* "/" */,-2 , 35/* "$" */,-2 , 2/* "namespace" */,-2 , 3/* "class" */,-2 , 6/* "var" */,-2 , 5/* "static" */,-2 , 4/* "function" */,-2 , 16/* "Identifier" */,-2 , 8/* "}" */,-2 ),
	/* State 99 */ new Array( 1/* "WHTS" */,-10 , 15/* "/" */,-10 , 35/* "$" */,-10 , 2/* "namespace" */,-10 , 3/* "class" */,-10 , 6/* "var" */,-10 , 5/* "static" */,-10 , 4/* "function" */,-10 , 16/* "Identifier" */,-10 , 8/* "}" */,-10 ),
	/* State 100 */ new Array( 1/* "WHTS" */,-11 , 15/* "/" */,-11 , 8/* "}" */,-11 , 3/* "class" */,-11 , 6/* "var" */,-11 , 5/* "static" */,-11 , 4/* "function" */,-11 , 16/* "Identifier" */,-11 ),
	/* State 101 */ new Array( 1/* "WHTS" */,-13 , 15/* "/" */,-13 , 8/* "}" */,-13 , 3/* "class" */,-13 , 6/* "var" */,-13 , 5/* "static" */,-13 , 4/* "function" */,-13 , 16/* "Identifier" */,-13 ),
	/* State 102 */ new Array( 1/* "WHTS" */,-14 , 15/* "/" */,-14 , 8/* "}" */,-14 , 3/* "class" */,-14 , 6/* "var" */,-14 , 5/* "static" */,-14 , 4/* "function" */,-14 , 16/* "Identifier" */,-14 ),
	/* State 103 */ new Array( 1/* "WHTS" */,-15 , 15/* "/" */,-15 , 8/* "}" */,-15 , 3/* "class" */,-15 , 6/* "var" */,-15 , 5/* "static" */,-15 , 4/* "function" */,-15 , 16/* "Identifier" */,-15 ),
	/* State 104 */ new Array( 1/* "WHTS" */,-16 , 15/* "/" */,-16 , 8/* "}" */,-16 , 3/* "class" */,-16 , 6/* "var" */,-16 , 5/* "static" */,-16 , 4/* "function" */,-16 , 16/* "Identifier" */,-16 ),
	/* State 105 */ new Array( 1/* "WHTS" */,-17 , 15/* "/" */,-17 , 35/* "$" */,-17 , 2/* "namespace" */,-17 , 3/* "class" */,-17 , 6/* "var" */,-17 , 5/* "static" */,-17 , 4/* "function" */,-17 , 16/* "Identifier" */,-17 , 8/* "}" */,-17 ),
	/* State 106 */ new Array( 9/* "(" */,65 , 7/* "{" */,66 , 12/* ";" */,67 , 10/* ")" */,112 , 17/* "Junk" */,42 , 2/* "namespace" */,43 , 3/* "class" */,44 , 4/* "function" */,45 , 5/* "static" */,46 , 6/* "var" */,47 , 11/* "=" */,48 , 13/* "," */,49 , 14/* "*" */,70 , 15/* "/" */,50 , 16/* "Identifier" */,51 , 1/* "WHTS" */,7 ),
	/* State 107 */ new Array( 9/* "(" */,65 , 7/* "{" */,66 , 12/* ";" */,67 , 8/* "}" */,113 , 17/* "Junk" */,42 , 2/* "namespace" */,43 , 3/* "class" */,44 , 4/* "function" */,45 , 5/* "static" */,46 , 6/* "var" */,47 , 11/* "=" */,48 , 13/* "," */,49 , 14/* "*" */,70 , 15/* "/" */,50 , 16/* "Identifier" */,51 , 1/* "WHTS" */,7 ),
	/* State 108 */ new Array( 12/* ";" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 109 */ new Array( 7/* "{" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 110 */ new Array( 16/* "Identifier" */,-35 , 1/* "WHTS" */,-35 , 15/* "/" */,-35 ),
	/* State 111 */ new Array( 8/* "}" */,-32 , 17/* "Junk" */,-32 , 2/* "namespace" */,-32 , 3/* "class" */,-32 , 4/* "function" */,-32 , 5/* "static" */,-32 , 6/* "var" */,-32 , 11/* "=" */,-32 , 13/* "," */,-32 , 14/* "*" */,-32 , 15/* "/" */,-32 , 16/* "Identifier" */,-32 , 1/* "WHTS" */,-32 , 12/* ";" */,-32 , 7/* "{" */,-32 , 9/* "(" */,-32 ),
	/* State 112 */ new Array( 17/* "Junk" */,-31 , 2/* "namespace" */,-31 , 3/* "class" */,-31 , 4/* "function" */,-31 , 5/* "static" */,-31 , 6/* "var" */,-31 , 11/* "=" */,-31 , 13/* "," */,-31 , 14/* "*" */,-31 , 15/* "/" */,-26 , 16/* "Identifier" */,-31 , 1/* "WHTS" */,-26 , 12/* ";" */,-26 , 7/* "{" */,-31 , 9/* "(" */,-31 ),
	/* State 113 */ new Array( 17/* "Junk" */,-30 , 2/* "namespace" */,-30 , 3/* "class" */,-30 , 4/* "function" */,-30 , 5/* "static" */,-30 , 6/* "var" */,-30 , 11/* "=" */,-30 , 13/* "," */,-30 , 14/* "*" */,-30 , 15/* "/" */,-25 , 16/* "Identifier" */,-30 , 1/* "WHTS" */,-25 , 12/* ";" */,-25 , 7/* "{" */,-30 , 9/* "(" */,-30 ),
	/* State 114 */ new Array( 15/* "/" */,5 , 12/* ";" */,118 , 1/* "WHTS" */,7 ),
	/* State 115 */ new Array( 15/* "/" */,5 , 7/* "{" */,119 , 1/* "WHTS" */,7 ),
	/* State 116 */ new Array( 15/* "/" */,5 , 16/* "Identifier" */,120 , 1/* "WHTS" */,7 ),
	/* State 117 */ new Array( 9/* "(" */,65 , 7/* "{" */,66 , 12/* ";" */,67 , 8/* "}" */,121 , 17/* "Junk" */,42 , 2/* "namespace" */,43 , 3/* "class" */,44 , 4/* "function" */,45 , 5/* "static" */,46 , 6/* "var" */,47 , 11/* "=" */,48 , 13/* "," */,49 , 14/* "*" */,70 , 15/* "/" */,50 , 16/* "Identifier" */,51 , 1/* "WHTS" */,7 ),
	/* State 118 */ new Array( 1/* "WHTS" */,-18 , 15/* "/" */,-18 , 35/* "$" */,-18 , 2/* "namespace" */,-18 , 3/* "class" */,-18 , 6/* "var" */,-18 , 5/* "static" */,-18 , 4/* "function" */,-18 , 16/* "Identifier" */,-18 , 8/* "}" */,-18 ),
	/* State 119 */ new Array( 8/* "}" */,-32 , 17/* "Junk" */,-32 , 2/* "namespace" */,-32 , 3/* "class" */,-32 , 4/* "function" */,-32 , 5/* "static" */,-32 , 6/* "var" */,-32 , 11/* "=" */,-32 , 13/* "," */,-32 , 14/* "*" */,-32 , 15/* "/" */,-32 , 16/* "Identifier" */,-32 , 1/* "WHTS" */,-32 , 12/* ";" */,-32 , 7/* "{" */,-32 , 9/* "(" */,-32 ),
	/* State 120 */ new Array( 1/* "WHTS" */,-21 , 15/* "/" */,-21 , 10/* ")" */,-21 , 13/* "," */,-21 ),
	/* State 121 */ new Array( 1/* "WHTS" */,-20 , 15/* "/" */,-20 , 35/* "$" */,-20 , 2/* "namespace" */,-20 , 3/* "class" */,-20 , 6/* "var" */,-20 , 5/* "static" */,-20 , 4/* "function" */,-20 , 16/* "Identifier" */,-20 , 8/* "}" */,-20 ),
	/* State 122 */ new Array( 9/* "(" */,65 , 7/* "{" */,66 , 12/* ";" */,67 , 8/* "}" */,123 , 17/* "Junk" */,42 , 2/* "namespace" */,43 , 3/* "class" */,44 , 4/* "function" */,45 , 5/* "static" */,46 , 6/* "var" */,47 , 11/* "=" */,48 , 13/* "," */,49 , 14/* "*" */,70 , 15/* "/" */,50 , 16/* "Identifier" */,51 , 1/* "WHTS" */,7 ),
	/* State 123 */ new Array( 1/* "WHTS" */,-19 , 15/* "/" */,-19 , 35/* "$" */,-19 , 2/* "namespace" */,-19 , 3/* "class" */,-19 , 6/* "var" */,-19 , 5/* "static" */,-19 , 4/* "function" */,-19 , 16/* "Identifier" */,-19 , 8/* "}" */,-19 )
);

/* Goto-Table */
var goto_tab = new Array(
	/* State 0 */ new Array( 19/* Global */,1 , 18/* NamespaceContents */,2 , 20/* W */,3 ),
	/* State 1 */ new Array(  ),
	/* State 2 */ new Array( 20/* W */,4 ),
	/* State 3 */ new Array( 33/* _W */,6 ),
	/* State 4 */ new Array( 33/* _W */,6 , 22/* NamespaceContent */,8 , 21/* Namespace */,9 , 23/* Class */,10 , 24/* VariableDef */,11 , 25/* Function */,12 , 26/* NativeBlock */,13 ),
	/* State 5 */ new Array(  ),
	/* State 6 */ new Array(  ),
	/* State 7 */ new Array(  ),
	/* State 8 */ new Array( 20/* W */,21 ),
	/* State 9 */ new Array(  ),
	/* State 10 */ new Array(  ),
	/* State 11 */ new Array(  ),
	/* State 12 */ new Array(  ),
	/* State 13 */ new Array(  ),
	/* State 14 */ new Array( 20/* W */,22 ),
	/* State 15 */ new Array( 20/* W */,23 ),
	/* State 16 */ new Array( 20/* W */,24 ),
	/* State 17 */ new Array( 20/* W */,25 ),
	/* State 18 */ new Array( 20/* W */,26 ),
	/* State 19 */ new Array( 20/* W */,27 ),
	/* State 20 */ new Array( 34/* MLComment */,28 ),
	/* State 21 */ new Array( 33/* _W */,6 ),
	/* State 22 */ new Array( 33/* _W */,6 ),
	/* State 23 */ new Array( 33/* _W */,6 ),
	/* State 24 */ new Array( 33/* _W */,6 ),
	/* State 25 */ new Array( 33/* _W */,6 ),
	/* State 26 */ new Array( 33/* _W */,6 ),
	/* State 27 */ new Array( 33/* _W */,6 ),
	/* State 28 */ new Array( 32/* PossibleJunk */,40 , 33/* _W */,52 ),
	/* State 29 */ new Array( 20/* W */,53 ),
	/* State 30 */ new Array( 20/* W */,54 ),
	/* State 31 */ new Array( 20/* W */,55 ),
	/* State 32 */ new Array( 20/* W */,56 ),
	/* State 33 */ new Array( 20/* W */,57 ),
	/* State 34 */ new Array( 31/* NativeCode */,58 ),
	/* State 35 */ new Array(  ),
	/* State 36 */ new Array(  ),
	/* State 37 */ new Array(  ),
	/* State 38 */ new Array(  ),
	/* State 39 */ new Array(  ),
	/* State 40 */ new Array(  ),
	/* State 41 */ new Array(  ),
	/* State 42 */ new Array(  ),
	/* State 43 */ new Array(  ),
	/* State 44 */ new Array(  ),
	/* State 45 */ new Array(  ),
	/* State 46 */ new Array(  ),
	/* State 47 */ new Array(  ),
	/* State 48 */ new Array(  ),
	/* State 49 */ new Array(  ),
	/* State 50 */ new Array(  ),
	/* State 51 */ new Array(  ),
	/* State 52 */ new Array(  ),
	/* State 53 */ new Array( 33/* _W */,6 ),
	/* State 54 */ new Array( 33/* _W */,6 ),
	/* State 55 */ new Array( 33/* _W */,6 ),
	/* State 56 */ new Array( 33/* _W */,6 ),
	/* State 57 */ new Array( 33/* _W */,6 ),
	/* State 58 */ new Array( 32/* PossibleJunk */,68 , 33/* _W */,52 ),
	/* State 59 */ new Array(  ),
	/* State 60 */ new Array( 20/* W */,71 ),
	/* State 61 */ new Array( 20/* W */,72 ),
	/* State 62 */ new Array( 20/* W */,73 ),
	/* State 63 */ new Array( 20/* W */,74 ),
	/* State 64 */ new Array( 20/* W */,75 ),
	/* State 65 */ new Array( 31/* NativeCode */,76 ),
	/* State 66 */ new Array( 31/* NativeCode */,77 ),
	/* State 67 */ new Array(  ),
	/* State 68 */ new Array(  ),
	/* State 69 */ new Array(  ),
	/* State 70 */ new Array(  ),
	/* State 71 */ new Array( 33/* _W */,6 , 18/* NamespaceContents */,78 , 20/* W */,3 ),
	/* State 72 */ new Array( 33/* _W */,6 , 27/* ClassContents */,79 , 20/* W */,80 ),
	/* State 73 */ new Array( 33/* _W */,6 , 29/* NativeCodeInline */,81 , 31/* NativeCode */,82 ),
	/* State 74 */ new Array( 33/* _W */,6 ),
	/* State 75 */ new Array( 33/* _W */,6 , 30/* ArgumentList */,84 ),
	/* State 76 */ new Array( 32/* PossibleJunk */,68 , 33/* _W */,52 ),
	/* State 77 */ new Array( 32/* PossibleJunk */,68 , 33/* _W */,52 ),
	/* State 78 */ new Array( 20/* W */,89 ),
	/* State 79 */ new Array( 20/* W */,90 ),
	/* State 80 */ new Array( 33/* _W */,6 ),
	/* State 81 */ new Array( 20/* W */,91 ),
	/* State 82 */ new Array( 32/* PossibleJunk */,94 , 33/* _W */,52 ),
	/* State 83 */ new Array( 20/* W */,95 ),
	/* State 84 */ new Array( 20/* W */,96 ),
	/* State 85 */ new Array( 20/* W */,97 ),
	/* State 86 */ new Array(  ),
	/* State 87 */ new Array(  ),
	/* State 88 */ new Array(  ),
	/* State 89 */ new Array( 33/* _W */,6 , 22/* NamespaceContent */,8 , 21/* Namespace */,9 , 23/* Class */,10 , 24/* VariableDef */,11 , 25/* Function */,12 , 26/* NativeBlock */,13 ),
	/* State 90 */ new Array( 33/* _W */,6 , 28/* ClassContent */,100 , 23/* Class */,101 , 24/* VariableDef */,102 , 25/* Function */,103 , 26/* NativeBlock */,104 ),
	/* State 91 */ new Array( 33/* _W */,6 ),
	/* State 92 */ new Array( 31/* NativeCode */,106 ),
	/* State 93 */ new Array( 31/* NativeCode */,107 ),
	/* State 94 */ new Array(  ),
	/* State 95 */ new Array( 33/* _W */,6 , 29/* NativeCodeInline */,108 , 31/* NativeCode */,82 ),
	/* State 96 */ new Array( 33/* _W */,6 ),
	/* State 97 */ new Array( 33/* _W */,6 ),
	/* State 98 */ new Array(  ),
	/* State 99 */ new Array(  ),
	/* State 100 */ new Array(  ),
	/* State 101 */ new Array(  ),
	/* State 102 */ new Array(  ),
	/* State 103 */ new Array(  ),
	/* State 104 */ new Array(  ),
	/* State 105 */ new Array(  ),
	/* State 106 */ new Array( 32/* PossibleJunk */,68 , 33/* _W */,52 ),
	/* State 107 */ new Array( 32/* PossibleJunk */,68 , 33/* _W */,52 ),
	/* State 108 */ new Array( 20/* W */,114 ),
	/* State 109 */ new Array( 20/* W */,115 ),
	/* State 110 */ new Array( 20/* W */,116 ),
	/* State 111 */ new Array( 31/* NativeCode */,117 ),
	/* State 112 */ new Array(  ),
	/* State 113 */ new Array(  ),
	/* State 114 */ new Array( 33/* _W */,6 ),
	/* State 115 */ new Array( 33/* _W */,6 ),
	/* State 116 */ new Array( 33/* _W */,6 ),
	/* State 117 */ new Array( 32/* PossibleJunk */,68 , 33/* _W */,52 ),
	/* State 118 */ new Array(  ),
	/* State 119 */ new Array( 31/* NativeCode */,122 ),
	/* State 120 */ new Array(  ),
	/* State 121 */ new Array(  ),
	/* State 122 */ new Array( 32/* PossibleJunk */,68 , 33/* _W */,52 ),
	/* State 123 */ new Array(  )
);



/* Symbol labels */
var labels = new Array(
	"Global'" /* Non-terminal symbol */,
	"WHTS" /* Terminal symbol */,
	"namespace" /* Terminal symbol */,
	"class" /* Terminal symbol */,
	"function" /* Terminal symbol */,
	"static" /* Terminal symbol */,
	"var" /* Terminal symbol */,
	"{" /* Terminal symbol */,
	"}" /* Terminal symbol */,
	"(" /* Terminal symbol */,
	")" /* Terminal symbol */,
	"=" /* Terminal symbol */,
	";" /* Terminal symbol */,
	"," /* Terminal symbol */,
	"*" /* Terminal symbol */,
	"/" /* Terminal symbol */,
	"Identifier" /* Terminal symbol */,
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
	"NativeCodeInline" /* Non-terminal symbol */,
	"ArgumentList" /* Non-terminal symbol */,
	"NativeCode" /* Non-terminal symbol */,
	"PossibleJunk" /* Non-terminal symbol */,
	"_W" /* Non-terminal symbol */,
	"MLComment" /* Non-terminal symbol */,
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
		act = 125;
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
		if( act == 125 )
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
			
			while( act == 125 && la != 35 )
			{
				if( v8_dbg_withtrace )
					__v8dbg_print( "\tError recovery\n" +
									"Current lookahead: " + labels[la] + " (" + info.att + ")\n" +
									"Action: " + act + "\n\n" );
				if( la == -1 )
					info.offset++;
					
				while( act == 125 && sstack.length > 0 )
				{
					sstack.pop();
					vstack.pop();
					
					if( sstack.length == 0 )
						break;
						
					act = 125;
					for( var i = 0; i < act_tab[sstack[sstack.length-1]].length; i+=2 )
					{
						if( act_tab[sstack[sstack.length-1]][i] == la )
						{
							act = act_tab[sstack[sstack.length-1]][i+1];
							break;
						}
					}
				}
				
				if( act != 125 )
					break;
				
				for( var i = 0; i < rsstack.length; i++ )
				{
					sstack.push( rsstack[i] );
					vstack.push( rvstack[i] );
				}
				
				la = __v8lex( info );
			}
			
			if( act == 125 )
			{
				if( v8_dbg_withtrace )
					__v8dbg_print( "\tError recovery failed, terminating parse process..." );
				break;
			}


			if( v8_dbg_withtrace )
				__v8dbg_print( "\tError recovery succeeded, continuing" );
		}
		
		/*
		if( act == 125 )
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
		 rval = createNamespace(vstack[ vstack.length - 7 ], vstack[ vstack.length - 3 ], vstack[ vstack.length - 8 ].line); 
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
		 rval = vstack[ vstack.length - 3 ].concat([vstack[ vstack.length - 1 ]]); 
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
		 rval = {type:'var', name:vstack[ vstack.length - 7 ], val:vstack[ vstack.length - 3 ]}; 
	}
	break;
	case 18:
	{
		 rval = {type:'static-var', name:vstack[ vstack.length - 7 ], val:vstack[ vstack.length - 3 ]}; 
	}
	break;
	case 19:
	{
		 rval = {type:'function', name:vstack[ vstack.length - 11 ], args:vstack[ vstack.length - 7 ], code:vstack[ vstack.length - 2 ], line:vstack[ vstack.length - 4 ].line}; 
	}
	break;
	case 20:
	{
		 rval = {type:'function', name:vstack[ vstack.length - 9 ], args:[], code:vstack[ vstack.length - 2 ], line:vstack[ vstack.length - 4 ].line}; 
	}
	break;
	case 21:
	{
		 rval = (vstack[ vstack.length - 5 ]).concat([{name:vstack[ vstack.length - 1 ]}]); 
	}
	break;
	case 22:
	{
		 rval = [{name:vstack[ vstack.length - 1 ]}]; 
	}
	break;
	case 23:
	{
		 rval = {type:'native-block', which:vstack[ vstack.length - 5 ], code:vstack[ vstack.length - 2 ]}; 
	}
	break;
	case 24:
	{
		 rval = vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 25:
	{
		 rval = vstack[ vstack.length - 4 ] + vstack[ vstack.length - 3 ] + vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 26:
	{
		 rval = vstack[ vstack.length - 4 ] + vstack[ vstack.length - 3 ] + vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 27:
	{
		 rval = ""; 
	}
	break;
	case 28:
	{
		 rval = vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 29:
	{
		 rval = vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 30:
	{
		 rval = vstack[ vstack.length - 4 ] + vstack[ vstack.length - 3 ] + vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 31:
	{
		 rval = vstack[ vstack.length - 4 ] + vstack[ vstack.length - 3 ] + vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 32:
	{
		 rval = ""; 
	}
	break;
	case 33:
	{
		 rval = {s:vstack[ vstack.length - 2 ].s + vstack[ vstack.length - 1 ].s, line:vstack[ vstack.length - 1 ].line}; 
	}
	break;
	case 34:
	{
		 rval = {s:vstack[ vstack.length - 6 ].s, line:vstack[ vstack.length - 6 ].line}; 
	}
	break;
	case 35:
	{
		 rval = {s:"",line:lineNumber}; 
	}
	break;
	case 36:
	{
		rval = vstack[ vstack.length - 2 ];
	}
	break;
	case 37:
	{
		rval = vstack[ vstack.length - 2 ];
	}
	break;
	case 38:
	{
		rval = vstack[ vstack.length - 2 ];
	}
	break;
	case 39:
	{
		rval = vstack[ vstack.length - 2 ];
	}
	break;
	case 40:
	{
		rval = vstack[ vstack.length - 2 ];
	}
	break;
	case 41:
	{
		rval = vstack[ vstack.length - 2 ];
	}
	break;
	case 42:
	{
		rval = vstack[ vstack.length - 0 ];
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
		 rval = vstack[ vstack.length - 1 ].s; 
	}
	break;
	case 55:
	{
		 lineNumber += vstack[ vstack.length - 1 ].replace(/[^\n]/g,"").length; rval = {s:vstack[ vstack.length - 1 ],line:lineNumber};
	}
	break;
	case 56:
	{
		 rval={s:"",line:lineNumber}; 
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



//v8_dbg_withtrace = true;
//v8_dbg_withparsetree = true;
//v8_dbg_withstepbystep = true;
var baseDir = arguments[1], baseName = arguments[2];
var gear = {gear:baseDir+baseName+".gear", cc:baseDir+baseName+".cc", h:baseDir+baseName+".h"};
if( arguments.length == 3 )
{
    var str         = Io.readFileContents(gear.gear);
    var error_cnt   = 0;
    var error_off   = [];
    var error_la    = [];
    
    if( ( error_cnt = __v8parse( str, error_off, error_la ) ) > 0 )
        for(var i = 0; i < error_cnt; i++) {
            var bf = str.substr(0, error_off[i]);
            print(gear.gear+":"+(nLines(bf)+1)+":"+(nCols(bf)+1)+": Error near >" + str.substr(error_off[i], 30) + "<, expecting \"" + error_la[i].join() + "\"" );
        }
}
else
    print("usage: " + arguments[0] + " <directory> <baseName>");

