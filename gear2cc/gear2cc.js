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

function createObject(name, childs, line, isModule) {
    var out = {type:isModule?'module':'object', name:name, objects:{}, modules:{}, classes:{}, vars:{}, functions:{}, header:"", top:"", license:""};
    for(c in childs) {
        var node = childs[c];
        switch(node.type) {
            case 'object':
                out.objects[node.name] = {objects:node.objects, classes:node.classes, vars:node.vars, functions:node.functions, header:node.header};
                break;
            case 'module':
                out.modules[node.name] = {objects:node.objects, classes:node.classes, vars:node.vars, functions:node.functions, header:node.header};
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
                else if(node.which == 'top')
                    out.top += node.code + "\n";
                else if(node.which == 'license')
                    out.license += node.code + "\n";
                else
                    throw Error("TODO: Native block `" + node.which + "`");
        }
    }
    
    if(!out.functions.hasOwnProperty("toString"))
        out.functions["toString"] = [{args:[], code:"return String(\"["+(isModule?'module':'object')+" "+name+"]\");", line:line}];
    
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
function generateFunctionCode(functions, name, parentPrefix, parentPath, code, class, ctor) {
    var prefix = parentPrefix + "_" + name, path = parentPath + "[\"" + name + "\"]", replaces = [], funcCode = "", hasNoArgsVer = false;
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
        
        replaces.push({regex:"\n" + makeTabs(prefix.split("_").length-1, "    "), replace:"\n" + tbs});
        replaces.push({regex:"\\breturn\\b\\s*;", replace:"return undefined;"});
        replaces.push({regex:"\\bthis\\b", replace:"This"});
        
        for(r in replaces) {
            var replace = replaces[r];
            actualCode = actualCode.replace(new RegExp(replace.regex, "g"), replace.replace);
        }
        if(!RegExp("\n"+tbs+"\\breturn\\b[^;]*;\\s*$").exec(actualCode))
            actualCode += tbs + "return undefined;\n";
        
        if(func.args.length)
            funcCode += "\n\tif(args.Length() >= " + func.args.length + ") {" + actualCode + "\t}\n";
        else {
            funcCode += actualCode;
            hasNoArgsVer = true;
        }
    }
    
    if(class)
        funcCode = "\n\tValue This(args.This());"+funcCode;
    
    if(!hasNoArgsVer)
        funcCode += "\treturn Throw(Error(\"Invalid call to " + parentPrefix.replace(/_/g, ".").replace(/^\./, "") + (ctor ? "" : (class?".prototype":"") + "." + name) + "\"));\n";
    
    code.func += "static v8::Handle<v8::Value> " + prefix + "(const v8::Arguments& args) {" + funcCode + "}\n\n";
}

function generateClassCode(class, name, parentPrefix, parentPath, code) {
    var prefix = parentPrefix + "_" + name, path = parentPath + "[\"" + name + "\"]";
    
    code.addClass(prefix, name);
    
    for(className in class.classes)
        generateClassCode(class.classes[className], className, prefix, prefix, code);
    
    for(funcName in class.functions) {
        if(funcName != name)
            code.setPrototype(prefix, funcName, code.makeFunction(prefix + "_" + funcName, funcName));
        generateFunctionCode(class.functions[funcName], funcName, prefix, prefix, code, class, funcName == name);
    }
    
    for(varName in class.vars) {
        var val = class.vars[varName].val;
        code.setPrototype(prefix, varName, /^\s*\b[A-Z]\w+\b\(.+\)$/.test(val) ? val : "Value(" + val + ")");
    }
    
    code.setStatic(parentPath, name, prefix + "->GetFunction()");
    for(varName in class.staticVars) {
        var val = class.staticVars[varName].val;
        code.setStatic(path, varName, /^\s*\b[A-Z]\w+\b\(.+\)$/.test(val) ? val : "Value(" + val + ")");
    }
}

function generateObjectCode(object, name, parentPrefix, parentPath, code) {
    var prefix = parentPrefix + "_" + name, path = parentPath + "[\"" + name + "\"]";
    
    for(className in object.classes)
        generateClassCode(object.classes[className], className, prefix, path, code);
    
    for(funcName in object.functions) {
        code.setStatic(path, funcName, code.makeFunction(prefix + "_" + funcName, funcName));
        generateFunctionCode(object.functions[funcName], funcName, prefix, path, code);
    }
    
    for(varName in object.vars) {
        var val = object.vars[varName].val;
        code.setStatic(path, varName, /^\s*\b[A-Z]\w+\b\(.+\)$/.test(val) ? val : "Value(" + val + ")");
    }
}

function generateModuleCode(object, name, parentPrefix, parentPath, code) {
    var prefix = parentPrefix + "_" + name, path = parentPath;
    
    for(className in object.classes)
        generateClassCode(object.classes[className], className, prefix, path, code);
    
    for(funcName in object.functions) {
        code.setStatic(path, funcName, code.makeFunction(prefix + "_" + funcName, funcName));
        generateFunctionCode(object.functions[funcName], funcName, prefix, path, code);
    }
    
    for(varName in object.vars) {
        var val = object.vars[varName].val;
        code.setStatic(path, varName, /^\s*\b[A-Z]\w+\b\(.+\)$/.test(val) ? val : "Value(" + val + ")");
    }
}

function generateCode(global) {
    var code = {
        func:"", init:"",
        addObject: function(path) {
            this.init += "\tvar " + path + " = Object();\n";
        },
        addClass: function(objName, ctor) {
            this.init += "\tv8::Handle<v8::FunctionTemplate> " + objName + " = v8::FunctionTemplate::New(" + objName + "_" + ctor + ");\n";
            this.init += "\t" + objName + "->SetClassName(String(\"" + ctor + "\"));\n";
        },
        makeFunction: function(prefix, name) {
            return "Function(" + prefix + ", \"" + name + "\")";
        },
        setStatic: function(parentObjName, name, value) {
            this.init += "\t" + parentObjName + "[\"" + name + "\"] = " + value + ";\n";
        },
        setPrototype: function(parentObjName, name, value) {
            this.init += "\t" + parentObjName + "->PrototypeTemplate()->Set(\"" + name + "\", " + value + ");\n";
        },
    };
    
    var modules = Object.keys(global.modules);
    
    if(!modules.length)
        throw Error("No modules");
    else if(modules.length > 1)
        throw Error("More than one module");
    else {
        var moduleName = modules[0], module = global.modules[moduleName];
        generateModuleCode(module, moduleName, "", "_exports", code);
        
        var license = global.license.trim().replace(/\n    /g, "\n") + (global.license.trim()?"\n\n":"\n"),
            top = global.top.trim().replace(/\n    /g, "\n") + (global.top.trim()?"\n\n":"\n");
        var ccCode = license+'\
#include <v8-gearbox.h>\n\
#include "'+baseName+'.h"\n\
\n\
using namespace Gearbox;\n\
\n\
/** \\file '+baseName+'.cc converted from '+baseName+'.gear */\n'+
        makeLine("",1) + "\n" + top + code.func;
        
        ccCode += makeLine("",nLines(ccCode)+2).replace(".gear",".cc") + "\nstatic void _setup_" + moduleName + "(Value _exports) {\n" + code.init + "}\nstatic Module _module_" + moduleName + "(\""+moduleName+"\", _setup_" + moduleName + ");";
        ccCode = ccCode.replace(/\t/g, "    ");
        Io.write(gear.cc, ccCode);
        
        var hCode = license+'\
#ifndef V8_GEARBOX_MODULES_'+baseName.toUpperCase()+'_H\n\
#define V8_GEARBOX_MODULES_'+baseName.toUpperCase()+'_H\n\n\
#include <v8.h>\n\n'+
//void Setup'+baseName+'(v8::Handle<v8::Object> global);\n\n\
'#endif\n';
        Io.write(gear.h, hCode);
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
			return 37;

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
		else if( ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 98 ) || ( info.src.charCodeAt( pos ) >= 100 && info.src.charCodeAt( pos ) <= 101 ) || ( info.src.charCodeAt( pos ) >= 103 && info.src.charCodeAt( pos ) <= 108 ) || info.src.charCodeAt( pos ) == 110 || ( info.src.charCodeAt( pos ) >= 112 && info.src.charCodeAt( pos ) <= 114 ) || ( info.src.charCodeAt( pos ) >= 116 && info.src.charCodeAt( pos ) <= 117 ) || ( info.src.charCodeAt( pos ) >= 119 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 123 ) state = 11;
		else if( info.src.charCodeAt( pos ) == 125 ) state = 12;
		else if( info.src.charCodeAt( pos ) == 118 ) state = 25;
		else if( info.src.charCodeAt( pos ) == 99 ) state = 36;
		else if( info.src.charCodeAt( pos ) == 109 ) state = 41;
		else if( info.src.charCodeAt( pos ) == 111 ) state = 42;
		else if( info.src.charCodeAt( pos ) == 115 ) state = 43;
		else if( info.src.charCodeAt( pos ) == 102 ) state = 46;
		else state = -1;
		break;

	case 1:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 58 ) || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 122 ) || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else state = -1;
		match = 18;
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
		match = 10;
		match_pos = pos;
		break;

	case 4:
		state = -1;
		match = 11;
		match_pos = pos;
		break;

	case 5:
		state = -1;
		match = 15;
		match_pos = pos;
		break;

	case 6:
		state = -1;
		match = 14;
		match_pos = pos;
		break;

	case 7:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 58 ) || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 122 ) || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else state = -1;
		match = 16;
		match_pos = pos;
		break;

	case 8:
		state = -1;
		match = 13;
		match_pos = pos;
		break;

	case 9:
		state = -1;
		match = 12;
		match_pos = pos;
		break;

	case 10:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 11:
		state = -1;
		match = 8;
		match_pos = pos;
		break;

	case 12:
		state = -1;
		match = 9;
		match_pos = pos;
		break;

	case 13:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 7;
		match_pos = pos;
		break;

	case 14:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 4;
		match_pos = pos;
		break;

	case 15:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 2;
		match_pos = pos;
		break;

	case 16:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 3;
		match_pos = pos;
		break;

	case 17:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 6;
		match_pos = pos;
		break;

	case 18:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else state = -1;
		match = 5;
		match_pos = pos;
		break;

	case 19:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 113 ) || ( info.src.charCodeAt( pos ) >= 115 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 114 ) state = 13;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 20:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 114 ) || ( info.src.charCodeAt( pos ) >= 116 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 115 ) state = 14;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 21:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 100 ) || ( info.src.charCodeAt( pos ) >= 102 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 101 ) state = 15;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 22:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 115 ) || ( info.src.charCodeAt( pos ) >= 117 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 116 ) state = 16;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 23:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 98 ) || ( info.src.charCodeAt( pos ) >= 100 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 99 ) state = 17;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 24:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 109 ) || ( info.src.charCodeAt( pos ) >= 111 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 110 ) state = 18;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 25:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 98 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 97 ) state = 19;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 26:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 114 ) || ( info.src.charCodeAt( pos ) >= 116 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 115 ) state = 20;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 27:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 107 ) || ( info.src.charCodeAt( pos ) >= 109 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 108 ) state = 21;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 28:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 98 ) || ( info.src.charCodeAt( pos ) >= 100 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 99 ) state = 22;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 29:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 104 ) || ( info.src.charCodeAt( pos ) >= 106 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 105 ) state = 23;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 30:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 110 ) || ( info.src.charCodeAt( pos ) >= 112 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 111 ) state = 24;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 31:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 98 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 97 ) state = 26;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 32:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 116 ) || ( info.src.charCodeAt( pos ) >= 118 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 117 ) state = 27;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 33:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 100 ) || ( info.src.charCodeAt( pos ) >= 102 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 101 ) state = 28;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 34:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 115 ) || ( info.src.charCodeAt( pos ) >= 117 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 116 ) state = 29;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 35:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 104 ) || ( info.src.charCodeAt( pos ) >= 106 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 105 ) state = 30;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 36:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 107 ) || ( info.src.charCodeAt( pos ) >= 109 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 108 ) state = 31;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 37:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 99 ) || ( info.src.charCodeAt( pos ) >= 101 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 100 ) state = 32;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 38:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 105 ) || ( info.src.charCodeAt( pos ) >= 107 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 106 ) state = 33;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 39:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 98 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 97 ) state = 34;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 40:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 115 ) || ( info.src.charCodeAt( pos ) >= 117 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 116 ) state = 35;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 41:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 110 ) || ( info.src.charCodeAt( pos ) >= 112 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 111 ) state = 37;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 42:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || info.src.charCodeAt( pos ) == 97 || ( info.src.charCodeAt( pos ) >= 99 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 98 ) state = 38;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 43:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 115 ) || ( info.src.charCodeAt( pos ) >= 117 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 116 ) state = 39;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 44:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 98 ) || ( info.src.charCodeAt( pos ) >= 100 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 99 ) state = 40;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 45:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 109 ) || ( info.src.charCodeAt( pos ) >= 111 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 110 ) state = 44;
		else state = -1;
		match = 17;
		match_pos = pos;
		break;

	case 46:
		if( ( info.src.charCodeAt( pos ) >= 0 && info.src.charCodeAt( pos ) <= 8 ) || ( info.src.charCodeAt( pos ) >= 11 && info.src.charCodeAt( pos ) <= 12 ) || ( info.src.charCodeAt( pos ) >= 14 && info.src.charCodeAt( pos ) <= 31 ) || ( info.src.charCodeAt( pos ) >= 33 && info.src.charCodeAt( pos ) <= 39 ) || info.src.charCodeAt( pos ) == 43 || ( info.src.charCodeAt( pos ) >= 45 && info.src.charCodeAt( pos ) <= 47 ) || info.src.charCodeAt( pos ) == 58 || info.src.charCodeAt( pos ) == 60 || ( info.src.charCodeAt( pos ) >= 62 && info.src.charCodeAt( pos ) <= 64 ) || ( info.src.charCodeAt( pos ) >= 91 && info.src.charCodeAt( pos ) <= 94 ) || info.src.charCodeAt( pos ) == 96 || info.src.charCodeAt( pos ) == 124 || ( info.src.charCodeAt( pos ) >= 126 && info.src.charCodeAt( pos ) <= 254 ) ) state = 1;
		else if( ( info.src.charCodeAt( pos ) >= 48 && info.src.charCodeAt( pos ) <= 57 ) || ( info.src.charCodeAt( pos ) >= 65 && info.src.charCodeAt( pos ) <= 90 ) || info.src.charCodeAt( pos ) == 95 || ( info.src.charCodeAt( pos ) >= 97 && info.src.charCodeAt( pos ) <= 116 ) || ( info.src.charCodeAt( pos ) >= 118 && info.src.charCodeAt( pos ) <= 122 ) ) state = 10;
		else if( info.src.charCodeAt( pos ) == 117 ) state = 45;
		else state = -1;
		match = 17;
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
	new Array( 20/* Global */, 1 ),
	new Array( 22/* Module */, 9 ),
	new Array( 23/* Object */, 9 ),
	new Array( 19/* ObjectContents */, 4 ),
	new Array( 19/* ObjectContents */, 1 ),
	new Array( 24/* ObjectContent */, 1 ),
	new Array( 24/* ObjectContent */, 1 ),
	new Array( 24/* ObjectContent */, 1 ),
	new Array( 24/* ObjectContent */, 1 ),
	new Array( 24/* ObjectContent */, 1 ),
	new Array( 24/* ObjectContent */, 1 ),
	new Array( 25/* Class */, 9 ),
	new Array( 29/* ClassContents */, 3 ),
	new Array( 29/* ClassContents */, 1 ),
	new Array( 30/* ClassContent */, 1 ),
	new Array( 30/* ClassContent */, 1 ),
	new Array( 30/* ClassContent */, 1 ),
	new Array( 30/* ClassContent */, 1 ),
	new Array( 26/* VariableDef */, 9 ),
	new Array( 26/* VariableDef */, 11 ),
	new Array( 27/* Function */, 13 ),
	new Array( 27/* Function */, 11 ),
	new Array( 32/* ArgumentList */, 5 ),
	new Array( 32/* ArgumentList */, 1 ),
	new Array( 28/* NativeBlock */, 5 ),
	new Array( 31/* NativeCodeInline */, 2 ),
	new Array( 31/* NativeCodeInline */, 4 ),
	new Array( 31/* NativeCodeInline */, 4 ),
	new Array( 31/* NativeCodeInline */, 0 ),
	new Array( 33/* NativeCode */, 2 ),
	new Array( 33/* NativeCode */, 2 ),
	new Array( 33/* NativeCode */, 4 ),
	new Array( 33/* NativeCode */, 4 ),
	new Array( 33/* NativeCode */, 0 ),
	new Array( 21/* W */, 2 ),
	new Array( 21/* W */, 6 ),
	new Array( 21/* W */, 0 ),
	new Array( 36/* MLComment */, 2 ),
	new Array( 36/* MLComment */, 2 ),
	new Array( 36/* MLComment */, 2 ),
	new Array( 36/* MLComment */, 2 ),
	new Array( 36/* MLComment */, 2 ),
	new Array( 36/* MLComment */, 2 ),
	new Array( 36/* MLComment */, 0 ),
	new Array( 34/* PossibleJunk */, 1 ),
	new Array( 34/* PossibleJunk */, 1 ),
	new Array( 34/* PossibleJunk */, 1 ),
	new Array( 34/* PossibleJunk */, 1 ),
	new Array( 34/* PossibleJunk */, 1 ),
	new Array( 34/* PossibleJunk */, 1 ),
	new Array( 34/* PossibleJunk */, 1 ),
	new Array( 34/* PossibleJunk */, 1 ),
	new Array( 34/* PossibleJunk */, 1 ),
	new Array( 34/* PossibleJunk */, 1 ),
	new Array( 34/* PossibleJunk */, 1 ),
	new Array( 34/* PossibleJunk */, 1 ),
	new Array( 34/* PossibleJunk */, 1 ),
	new Array( 35/* _W */, 1 ),
	new Array( 35/* _W */, 0 )
);

/* Action-Table */
var act_tab = new Array(
	/* State 0 */ new Array( 37/* "$" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 , 2/* "module" */,-37 , 3/* "object" */,-37 , 4/* "class" */,-37 , 7/* "var" */,-37 , 6/* "static" */,-37 , 5/* "function" */,-37 , 17/* "Identifier" */,-37 ),
	/* State 1 */ new Array( 37/* "$" */,0 ),
	/* State 2 */ new Array( 37/* "$" */,-1 , 2/* "module" */,-37 , 3/* "object" */,-37 , 4/* "class" */,-37 , 7/* "var" */,-37 , 6/* "static" */,-37 , 5/* "function" */,-37 , 17/* "Identifier" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 3 */ new Array( 16/* "/" */,5 , 1/* "WHTS" */,7 , 37/* "$" */,-5 , 2/* "module" */,-5 , 3/* "object" */,-5 , 4/* "class" */,-5 , 7/* "var" */,-5 , 6/* "static" */,-5 , 5/* "function" */,-5 , 17/* "Identifier" */,-5 , 9/* "}" */,-5 ),
	/* State 4 */ new Array( 16/* "/" */,5 , 1/* "WHTS" */,7 , 2/* "module" */,15 , 3/* "object" */,16 , 4/* "class" */,17 , 7/* "var" */,18 , 6/* "static" */,19 , 5/* "function" */,20 , 17/* "Identifier" */,21 ),
	/* State 5 */ new Array( 15/* "*" */,22 ),
	/* State 6 */ new Array( 37/* "$" */,-35 , 1/* "WHTS" */,-35 , 16/* "/" */,-35 , 2/* "module" */,-35 , 3/* "object" */,-35 , 4/* "class" */,-35 , 7/* "var" */,-35 , 6/* "static" */,-35 , 5/* "function" */,-35 , 17/* "Identifier" */,-35 , 8/* "{" */,-35 , 12/* "=" */,-35 , 10/* "(" */,-35 , 9/* "}" */,-35 , 18/* "Junk" */,-35 , 14/* "," */,-35 , 15/* "*" */,-35 , 13/* ";" */,-35 , 11/* ")" */,-35 ),
	/* State 7 */ new Array( 37/* "$" */,-58 , 1/* "WHTS" */,-58 , 16/* "/" */,-58 , 2/* "module" */,-58 , 3/* "object" */,-58 , 4/* "class" */,-58 , 7/* "var" */,-58 , 6/* "static" */,-58 , 5/* "function" */,-58 , 17/* "Identifier" */,-58 , 8/* "{" */,-58 , 15/* "*" */,-58 , 18/* "Junk" */,-58 , 12/* "=" */,-58 , 14/* "," */,-58 , 13/* ";" */,-58 , 9/* "}" */,-58 , 10/* "(" */,-58 , 11/* ")" */,-58 ),
	/* State 8 */ new Array( 37/* "$" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 , 2/* "module" */,-37 , 3/* "object" */,-37 , 4/* "class" */,-37 , 7/* "var" */,-37 , 6/* "static" */,-37 , 5/* "function" */,-37 , 17/* "Identifier" */,-37 ),
	/* State 9 */ new Array( 1/* "WHTS" */,-6 , 16/* "/" */,-6 , 37/* "$" */,-6 , 2/* "module" */,-6 , 3/* "object" */,-6 , 4/* "class" */,-6 , 7/* "var" */,-6 , 6/* "static" */,-6 , 5/* "function" */,-6 , 17/* "Identifier" */,-6 , 9/* "}" */,-6 ),
	/* State 10 */ new Array( 1/* "WHTS" */,-7 , 16/* "/" */,-7 , 37/* "$" */,-7 , 2/* "module" */,-7 , 3/* "object" */,-7 , 4/* "class" */,-7 , 7/* "var" */,-7 , 6/* "static" */,-7 , 5/* "function" */,-7 , 17/* "Identifier" */,-7 , 9/* "}" */,-7 ),
	/* State 11 */ new Array( 1/* "WHTS" */,-8 , 16/* "/" */,-8 , 37/* "$" */,-8 , 2/* "module" */,-8 , 3/* "object" */,-8 , 4/* "class" */,-8 , 7/* "var" */,-8 , 6/* "static" */,-8 , 5/* "function" */,-8 , 17/* "Identifier" */,-8 , 9/* "}" */,-8 ),
	/* State 12 */ new Array( 1/* "WHTS" */,-9 , 16/* "/" */,-9 , 37/* "$" */,-9 , 2/* "module" */,-9 , 3/* "object" */,-9 , 4/* "class" */,-9 , 7/* "var" */,-9 , 6/* "static" */,-9 , 5/* "function" */,-9 , 17/* "Identifier" */,-9 , 9/* "}" */,-9 ),
	/* State 13 */ new Array( 1/* "WHTS" */,-10 , 16/* "/" */,-10 , 37/* "$" */,-10 , 2/* "module" */,-10 , 3/* "object" */,-10 , 4/* "class" */,-10 , 7/* "var" */,-10 , 6/* "static" */,-10 , 5/* "function" */,-10 , 17/* "Identifier" */,-10 , 9/* "}" */,-10 ),
	/* State 14 */ new Array( 1/* "WHTS" */,-11 , 16/* "/" */,-11 , 37/* "$" */,-11 , 2/* "module" */,-11 , 3/* "object" */,-11 , 4/* "class" */,-11 , 7/* "var" */,-11 , 6/* "static" */,-11 , 5/* "function" */,-11 , 17/* "Identifier" */,-11 , 9/* "}" */,-11 ),
	/* State 15 */ new Array( 17/* "Identifier" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 16 */ new Array( 17/* "Identifier" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 17 */ new Array( 17/* "Identifier" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 18 */ new Array( 17/* "Identifier" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 19 */ new Array( 7/* "var" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 20 */ new Array( 17/* "Identifier" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 21 */ new Array( 8/* "{" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 22 */ new Array( 15/* "*" */,-44 , 18/* "Junk" */,-44 , 2/* "module" */,-44 , 3/* "object" */,-44 , 4/* "class" */,-44 , 5/* "function" */,-44 , 6/* "static" */,-44 , 7/* "var" */,-44 , 12/* "=" */,-44 , 14/* "," */,-44 , 16/* "/" */,-44 , 17/* "Identifier" */,-44 , 1/* "WHTS" */,-44 , 13/* ";" */,-44 , 8/* "{" */,-44 , 9/* "}" */,-44 , 10/* "(" */,-44 , 11/* ")" */,-44 ),
	/* State 23 */ new Array( 16/* "/" */,5 , 1/* "WHTS" */,7 , 37/* "$" */,-4 , 2/* "module" */,-4 , 3/* "object" */,-4 , 4/* "class" */,-4 , 7/* "var" */,-4 , 6/* "static" */,-4 , 5/* "function" */,-4 , 17/* "Identifier" */,-4 , 9/* "}" */,-4 ),
	/* State 24 */ new Array( 16/* "/" */,5 , 17/* "Identifier" */,32 , 1/* "WHTS" */,7 ),
	/* State 25 */ new Array( 16/* "/" */,5 , 17/* "Identifier" */,33 , 1/* "WHTS" */,7 ),
	/* State 26 */ new Array( 16/* "/" */,5 , 17/* "Identifier" */,34 , 1/* "WHTS" */,7 ),
	/* State 27 */ new Array( 16/* "/" */,5 , 17/* "Identifier" */,35 , 1/* "WHTS" */,7 ),
	/* State 28 */ new Array( 16/* "/" */,5 , 7/* "var" */,36 , 1/* "WHTS" */,7 ),
	/* State 29 */ new Array( 16/* "/" */,5 , 17/* "Identifier" */,37 , 1/* "WHTS" */,7 ),
	/* State 30 */ new Array( 16/* "/" */,5 , 8/* "{" */,38 , 1/* "WHTS" */,7 ),
	/* State 31 */ new Array( 11/* ")" */,39 , 10/* "(" */,40 , 9/* "}" */,41 , 8/* "{" */,42 , 13/* ";" */,43 , 15/* "*" */,45 , 18/* "Junk" */,46 , 2/* "module" */,47 , 3/* "object" */,48 , 4/* "class" */,49 , 5/* "function" */,50 , 6/* "static" */,51 , 7/* "var" */,52 , 12/* "=" */,53 , 14/* "," */,54 , 16/* "/" */,55 , 17/* "Identifier" */,56 , 1/* "WHTS" */,7 ),
	/* State 32 */ new Array( 8/* "{" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 33 */ new Array( 8/* "{" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 34 */ new Array( 8/* "{" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 35 */ new Array( 12/* "=" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 36 */ new Array( 17/* "Identifier" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 37 */ new Array( 10/* "(" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 38 */ new Array( 9/* "}" */,-34 , 18/* "Junk" */,-34 , 2/* "module" */,-34 , 3/* "object" */,-34 , 4/* "class" */,-34 , 5/* "function" */,-34 , 6/* "static" */,-34 , 7/* "var" */,-34 , 12/* "=" */,-34 , 14/* "," */,-34 , 15/* "*" */,-34 , 16/* "/" */,-34 , 17/* "Identifier" */,-34 , 1/* "WHTS" */,-34 , 13/* ";" */,-34 , 8/* "{" */,-34 , 10/* "(" */,-34 ),
	/* State 39 */ new Array( 15/* "*" */,-43 , 18/* "Junk" */,-43 , 2/* "module" */,-43 , 3/* "object" */,-43 , 4/* "class" */,-43 , 5/* "function" */,-43 , 6/* "static" */,-43 , 7/* "var" */,-43 , 12/* "=" */,-43 , 14/* "," */,-43 , 16/* "/" */,-43 , 17/* "Identifier" */,-43 , 1/* "WHTS" */,-43 , 13/* ";" */,-43 , 8/* "{" */,-43 , 9/* "}" */,-43 , 10/* "(" */,-43 , 11/* ")" */,-43 ),
	/* State 40 */ new Array( 15/* "*" */,-42 , 18/* "Junk" */,-42 , 2/* "module" */,-42 , 3/* "object" */,-42 , 4/* "class" */,-42 , 5/* "function" */,-42 , 6/* "static" */,-42 , 7/* "var" */,-42 , 12/* "=" */,-42 , 14/* "," */,-42 , 16/* "/" */,-42 , 17/* "Identifier" */,-42 , 1/* "WHTS" */,-42 , 13/* ";" */,-42 , 8/* "{" */,-42 , 9/* "}" */,-42 , 10/* "(" */,-42 , 11/* ")" */,-42 ),
	/* State 41 */ new Array( 15/* "*" */,-41 , 18/* "Junk" */,-41 , 2/* "module" */,-41 , 3/* "object" */,-41 , 4/* "class" */,-41 , 5/* "function" */,-41 , 6/* "static" */,-41 , 7/* "var" */,-41 , 12/* "=" */,-41 , 14/* "," */,-41 , 16/* "/" */,-41 , 17/* "Identifier" */,-41 , 1/* "WHTS" */,-41 , 13/* ";" */,-41 , 8/* "{" */,-41 , 9/* "}" */,-41 , 10/* "(" */,-41 , 11/* ")" */,-41 ),
	/* State 42 */ new Array( 15/* "*" */,-40 , 18/* "Junk" */,-40 , 2/* "module" */,-40 , 3/* "object" */,-40 , 4/* "class" */,-40 , 5/* "function" */,-40 , 6/* "static" */,-40 , 7/* "var" */,-40 , 12/* "=" */,-40 , 14/* "," */,-40 , 16/* "/" */,-40 , 17/* "Identifier" */,-40 , 1/* "WHTS" */,-40 , 13/* ";" */,-40 , 8/* "{" */,-40 , 9/* "}" */,-40 , 10/* "(" */,-40 , 11/* ")" */,-40 ),
	/* State 43 */ new Array( 15/* "*" */,-39 , 18/* "Junk" */,-39 , 2/* "module" */,-39 , 3/* "object" */,-39 , 4/* "class" */,-39 , 5/* "function" */,-39 , 6/* "static" */,-39 , 7/* "var" */,-39 , 12/* "=" */,-39 , 14/* "," */,-39 , 16/* "/" */,-39 , 17/* "Identifier" */,-39 , 1/* "WHTS" */,-39 , 13/* ";" */,-39 , 8/* "{" */,-39 , 9/* "}" */,-39 , 10/* "(" */,-39 , 11/* ")" */,-39 ),
	/* State 44 */ new Array( 15/* "*" */,-38 , 18/* "Junk" */,-38 , 2/* "module" */,-38 , 3/* "object" */,-38 , 4/* "class" */,-38 , 5/* "function" */,-38 , 6/* "static" */,-38 , 7/* "var" */,-38 , 12/* "=" */,-38 , 14/* "," */,-38 , 16/* "/" */,-38 , 17/* "Identifier" */,-38 , 1/* "WHTS" */,-38 , 13/* ";" */,-38 , 8/* "{" */,-38 , 9/* "}" */,-38 , 10/* "(" */,-38 , 11/* ")" */,-38 ),
	/* State 45 */ new Array( 16/* "/" */,65 , 15/* "*" */,-54 , 18/* "Junk" */,-54 , 2/* "module" */,-54 , 3/* "object" */,-54 , 4/* "class" */,-54 , 5/* "function" */,-54 , 6/* "static" */,-54 , 7/* "var" */,-54 , 12/* "=" */,-54 , 14/* "," */,-54 , 17/* "Identifier" */,-54 , 1/* "WHTS" */,-54 , 13/* ";" */,-54 , 8/* "{" */,-54 , 9/* "}" */,-54 , 10/* "(" */,-54 , 11/* ")" */,-54 ),
	/* State 46 */ new Array( 15/* "*" */,-45 , 18/* "Junk" */,-45 , 2/* "module" */,-45 , 3/* "object" */,-45 , 4/* "class" */,-45 , 5/* "function" */,-45 , 6/* "static" */,-45 , 7/* "var" */,-45 , 12/* "=" */,-45 , 14/* "," */,-45 , 16/* "/" */,-45 , 17/* "Identifier" */,-45 , 1/* "WHTS" */,-45 , 13/* ";" */,-45 , 8/* "{" */,-45 , 9/* "}" */,-45 , 10/* "(" */,-45 , 11/* ")" */,-45 ),
	/* State 47 */ new Array( 15/* "*" */,-46 , 18/* "Junk" */,-46 , 2/* "module" */,-46 , 3/* "object" */,-46 , 4/* "class" */,-46 , 5/* "function" */,-46 , 6/* "static" */,-46 , 7/* "var" */,-46 , 12/* "=" */,-46 , 14/* "," */,-46 , 16/* "/" */,-46 , 17/* "Identifier" */,-46 , 1/* "WHTS" */,-46 , 13/* ";" */,-46 , 8/* "{" */,-46 , 9/* "}" */,-46 , 10/* "(" */,-46 , 11/* ")" */,-46 ),
	/* State 48 */ new Array( 15/* "*" */,-47 , 18/* "Junk" */,-47 , 2/* "module" */,-47 , 3/* "object" */,-47 , 4/* "class" */,-47 , 5/* "function" */,-47 , 6/* "static" */,-47 , 7/* "var" */,-47 , 12/* "=" */,-47 , 14/* "," */,-47 , 16/* "/" */,-47 , 17/* "Identifier" */,-47 , 1/* "WHTS" */,-47 , 13/* ";" */,-47 , 8/* "{" */,-47 , 9/* "}" */,-47 , 10/* "(" */,-47 , 11/* ")" */,-47 ),
	/* State 49 */ new Array( 15/* "*" */,-48 , 18/* "Junk" */,-48 , 2/* "module" */,-48 , 3/* "object" */,-48 , 4/* "class" */,-48 , 5/* "function" */,-48 , 6/* "static" */,-48 , 7/* "var" */,-48 , 12/* "=" */,-48 , 14/* "," */,-48 , 16/* "/" */,-48 , 17/* "Identifier" */,-48 , 1/* "WHTS" */,-48 , 13/* ";" */,-48 , 8/* "{" */,-48 , 9/* "}" */,-48 , 10/* "(" */,-48 , 11/* ")" */,-48 ),
	/* State 50 */ new Array( 15/* "*" */,-49 , 18/* "Junk" */,-49 , 2/* "module" */,-49 , 3/* "object" */,-49 , 4/* "class" */,-49 , 5/* "function" */,-49 , 6/* "static" */,-49 , 7/* "var" */,-49 , 12/* "=" */,-49 , 14/* "," */,-49 , 16/* "/" */,-49 , 17/* "Identifier" */,-49 , 1/* "WHTS" */,-49 , 13/* ";" */,-49 , 8/* "{" */,-49 , 9/* "}" */,-49 , 10/* "(" */,-49 , 11/* ")" */,-49 ),
	/* State 51 */ new Array( 15/* "*" */,-50 , 18/* "Junk" */,-50 , 2/* "module" */,-50 , 3/* "object" */,-50 , 4/* "class" */,-50 , 5/* "function" */,-50 , 6/* "static" */,-50 , 7/* "var" */,-50 , 12/* "=" */,-50 , 14/* "," */,-50 , 16/* "/" */,-50 , 17/* "Identifier" */,-50 , 1/* "WHTS" */,-50 , 13/* ";" */,-50 , 8/* "{" */,-50 , 9/* "}" */,-50 , 10/* "(" */,-50 , 11/* ")" */,-50 ),
	/* State 52 */ new Array( 15/* "*" */,-51 , 18/* "Junk" */,-51 , 2/* "module" */,-51 , 3/* "object" */,-51 , 4/* "class" */,-51 , 5/* "function" */,-51 , 6/* "static" */,-51 , 7/* "var" */,-51 , 12/* "=" */,-51 , 14/* "," */,-51 , 16/* "/" */,-51 , 17/* "Identifier" */,-51 , 1/* "WHTS" */,-51 , 13/* ";" */,-51 , 8/* "{" */,-51 , 9/* "}" */,-51 , 10/* "(" */,-51 , 11/* ")" */,-51 ),
	/* State 53 */ new Array( 15/* "*" */,-52 , 18/* "Junk" */,-52 , 2/* "module" */,-52 , 3/* "object" */,-52 , 4/* "class" */,-52 , 5/* "function" */,-52 , 6/* "static" */,-52 , 7/* "var" */,-52 , 12/* "=" */,-52 , 14/* "," */,-52 , 16/* "/" */,-52 , 17/* "Identifier" */,-52 , 1/* "WHTS" */,-52 , 13/* ";" */,-52 , 8/* "{" */,-52 , 9/* "}" */,-52 , 10/* "(" */,-52 , 11/* ")" */,-52 ),
	/* State 54 */ new Array( 15/* "*" */,-53 , 18/* "Junk" */,-53 , 2/* "module" */,-53 , 3/* "object" */,-53 , 4/* "class" */,-53 , 5/* "function" */,-53 , 6/* "static" */,-53 , 7/* "var" */,-53 , 12/* "=" */,-53 , 14/* "," */,-53 , 16/* "/" */,-53 , 17/* "Identifier" */,-53 , 1/* "WHTS" */,-53 , 13/* ";" */,-53 , 8/* "{" */,-53 , 9/* "}" */,-53 , 10/* "(" */,-53 , 11/* ")" */,-53 ),
	/* State 55 */ new Array( 15/* "*" */,-55 , 18/* "Junk" */,-55 , 2/* "module" */,-55 , 3/* "object" */,-55 , 4/* "class" */,-55 , 5/* "function" */,-55 , 6/* "static" */,-55 , 7/* "var" */,-55 , 12/* "=" */,-55 , 14/* "," */,-55 , 16/* "/" */,-55 , 17/* "Identifier" */,-55 , 1/* "WHTS" */,-55 , 13/* ";" */,-55 , 8/* "{" */,-55 , 9/* "}" */,-55 , 10/* "(" */,-55 , 11/* ")" */,-55 ),
	/* State 56 */ new Array( 15/* "*" */,-56 , 18/* "Junk" */,-56 , 2/* "module" */,-56 , 3/* "object" */,-56 , 4/* "class" */,-56 , 5/* "function" */,-56 , 6/* "static" */,-56 , 7/* "var" */,-56 , 12/* "=" */,-56 , 14/* "," */,-56 , 16/* "/" */,-56 , 17/* "Identifier" */,-56 , 1/* "WHTS" */,-56 , 13/* ";" */,-56 , 8/* "{" */,-56 , 9/* "}" */,-56 , 10/* "(" */,-56 , 11/* ")" */,-56 ),
	/* State 57 */ new Array( 15/* "*" */,-57 , 18/* "Junk" */,-57 , 2/* "module" */,-57 , 3/* "object" */,-57 , 4/* "class" */,-57 , 5/* "function" */,-57 , 6/* "static" */,-57 , 7/* "var" */,-57 , 12/* "=" */,-57 , 14/* "," */,-57 , 16/* "/" */,-57 , 17/* "Identifier" */,-57 , 1/* "WHTS" */,-57 , 13/* ";" */,-57 , 8/* "{" */,-57 , 9/* "}" */,-57 , 10/* "(" */,-57 , 11/* ")" */,-57 ),
	/* State 58 */ new Array( 16/* "/" */,5 , 8/* "{" */,66 , 1/* "WHTS" */,7 ),
	/* State 59 */ new Array( 16/* "/" */,5 , 8/* "{" */,67 , 1/* "WHTS" */,7 ),
	/* State 60 */ new Array( 16/* "/" */,5 , 8/* "{" */,68 , 1/* "WHTS" */,7 ),
	/* State 61 */ new Array( 16/* "/" */,5 , 12/* "=" */,69 , 1/* "WHTS" */,7 ),
	/* State 62 */ new Array( 16/* "/" */,5 , 17/* "Identifier" */,70 , 1/* "WHTS" */,7 ),
	/* State 63 */ new Array( 16/* "/" */,5 , 10/* "(" */,71 , 1/* "WHTS" */,7 ),
	/* State 64 */ new Array( 10/* "(" */,72 , 8/* "{" */,73 , 13/* ";" */,74 , 9/* "}" */,76 , 18/* "Junk" */,46 , 2/* "module" */,47 , 3/* "object" */,48 , 4/* "class" */,49 , 5/* "function" */,50 , 6/* "static" */,51 , 7/* "var" */,52 , 12/* "=" */,53 , 14/* "," */,54 , 15/* "*" */,77 , 16/* "/" */,55 , 17/* "Identifier" */,56 , 1/* "WHTS" */,7 ),
	/* State 65 */ new Array( 37/* "$" */,-36 , 1/* "WHTS" */,-36 , 16/* "/" */,-36 , 2/* "module" */,-36 , 3/* "object" */,-36 , 4/* "class" */,-36 , 7/* "var" */,-36 , 6/* "static" */,-36 , 5/* "function" */,-36 , 17/* "Identifier" */,-36 , 8/* "{" */,-36 , 12/* "=" */,-36 , 10/* "(" */,-36 , 9/* "}" */,-36 , 18/* "Junk" */,-36 , 14/* "," */,-36 , 15/* "*" */,-36 , 13/* ";" */,-36 , 11/* ")" */,-36 ),
	/* State 66 */ new Array( 1/* "WHTS" */,-37 , 16/* "/" */,-37 , 2/* "module" */,-37 , 3/* "object" */,-37 , 4/* "class" */,-37 , 7/* "var" */,-37 , 6/* "static" */,-37 , 5/* "function" */,-37 , 17/* "Identifier" */,-37 , 9/* "}" */,-37 ),
	/* State 67 */ new Array( 1/* "WHTS" */,-37 , 16/* "/" */,-37 , 2/* "module" */,-37 , 3/* "object" */,-37 , 4/* "class" */,-37 , 7/* "var" */,-37 , 6/* "static" */,-37 , 5/* "function" */,-37 , 17/* "Identifier" */,-37 , 9/* "}" */,-37 ),
	/* State 68 */ new Array( 1/* "WHTS" */,-37 , 16/* "/" */,-37 , 4/* "class" */,-37 , 7/* "var" */,-37 , 6/* "static" */,-37 , 5/* "function" */,-37 , 17/* "Identifier" */,-37 , 9/* "}" */,-37 ),
	/* State 69 */ new Array( 18/* "Junk" */,-37 , 2/* "module" */,-37 , 3/* "object" */,-37 , 4/* "class" */,-37 , 5/* "function" */,-37 , 6/* "static" */,-37 , 7/* "var" */,-37 , 12/* "=" */,-37 , 14/* "," */,-37 , 15/* "*" */,-37 , 16/* "/" */,-37 , 17/* "Identifier" */,-37 , 8/* "{" */,-37 , 10/* "(" */,-37 , 13/* ";" */,-37 , 1/* "WHTS" */,-37 ),
	/* State 70 */ new Array( 12/* "=" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 71 */ new Array( 17/* "Identifier" */,-37 , 11/* ")" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 72 */ new Array( 11/* ")" */,-34 , 18/* "Junk" */,-34 , 2/* "module" */,-34 , 3/* "object" */,-34 , 4/* "class" */,-34 , 5/* "function" */,-34 , 6/* "static" */,-34 , 7/* "var" */,-34 , 12/* "=" */,-34 , 14/* "," */,-34 , 15/* "*" */,-34 , 16/* "/" */,-34 , 17/* "Identifier" */,-34 , 1/* "WHTS" */,-34 , 13/* ";" */,-34 , 8/* "{" */,-34 , 10/* "(" */,-34 ),
	/* State 73 */ new Array( 9/* "}" */,-34 , 18/* "Junk" */,-34 , 2/* "module" */,-34 , 3/* "object" */,-34 , 4/* "class" */,-34 , 5/* "function" */,-34 , 6/* "static" */,-34 , 7/* "var" */,-34 , 12/* "=" */,-34 , 14/* "," */,-34 , 15/* "*" */,-34 , 16/* "/" */,-34 , 17/* "Identifier" */,-34 , 1/* "WHTS" */,-34 , 13/* ";" */,-34 , 8/* "{" */,-34 , 10/* "(" */,-34 ),
	/* State 74 */ new Array( 9/* "}" */,-31 , 18/* "Junk" */,-31 , 2/* "module" */,-31 , 3/* "object" */,-31 , 4/* "class" */,-31 , 5/* "function" */,-31 , 6/* "static" */,-31 , 7/* "var" */,-31 , 12/* "=" */,-31 , 14/* "," */,-31 , 15/* "*" */,-31 , 16/* "/" */,-31 , 17/* "Identifier" */,-31 , 1/* "WHTS" */,-31 , 13/* ";" */,-31 , 8/* "{" */,-31 , 10/* "(" */,-31 , 11/* ")" */,-31 ),
	/* State 75 */ new Array( 9/* "}" */,-30 , 18/* "Junk" */,-30 , 2/* "module" */,-30 , 3/* "object" */,-30 , 4/* "class" */,-30 , 5/* "function" */,-30 , 6/* "static" */,-30 , 7/* "var" */,-30 , 12/* "=" */,-30 , 14/* "," */,-30 , 15/* "*" */,-30 , 16/* "/" */,-30 , 17/* "Identifier" */,-30 , 1/* "WHTS" */,-30 , 13/* ";" */,-30 , 8/* "{" */,-30 , 10/* "(" */,-30 , 11/* ")" */,-30 ),
	/* State 76 */ new Array( 1/* "WHTS" */,-25 , 16/* "/" */,-25 , 37/* "$" */,-25 , 2/* "module" */,-25 , 3/* "object" */,-25 , 4/* "class" */,-25 , 7/* "var" */,-25 , 6/* "static" */,-25 , 5/* "function" */,-25 , 17/* "Identifier" */,-25 , 9/* "}" */,-25 ),
	/* State 77 */ new Array( 9/* "}" */,-54 , 18/* "Junk" */,-54 , 2/* "module" */,-54 , 3/* "object" */,-54 , 4/* "class" */,-54 , 5/* "function" */,-54 , 6/* "static" */,-54 , 7/* "var" */,-54 , 12/* "=" */,-54 , 14/* "," */,-54 , 15/* "*" */,-54 , 16/* "/" */,-54 , 17/* "Identifier" */,-54 , 1/* "WHTS" */,-54 , 13/* ";" */,-54 , 8/* "{" */,-54 , 10/* "(" */,-54 , 11/* ")" */,-54 ),
	/* State 78 */ new Array( 16/* "/" */,5 , 1/* "WHTS" */,7 , 2/* "module" */,-37 , 3/* "object" */,-37 , 4/* "class" */,-37 , 7/* "var" */,-37 , 6/* "static" */,-37 , 5/* "function" */,-37 , 17/* "Identifier" */,-37 , 9/* "}" */,-37 ),
	/* State 79 */ new Array( 16/* "/" */,5 , 1/* "WHTS" */,7 , 2/* "module" */,-37 , 3/* "object" */,-37 , 4/* "class" */,-37 , 7/* "var" */,-37 , 6/* "static" */,-37 , 5/* "function" */,-37 , 17/* "Identifier" */,-37 , 9/* "}" */,-37 ),
	/* State 80 */ new Array( 16/* "/" */,5 , 1/* "WHTS" */,7 , 4/* "class" */,-37 , 7/* "var" */,-37 , 6/* "static" */,-37 , 5/* "function" */,-37 , 17/* "Identifier" */,-37 , 9/* "}" */,-37 ),
	/* State 81 */ new Array( 16/* "/" */,5 , 1/* "WHTS" */,7 , 13/* ";" */,-29 , 18/* "Junk" */,-34 , 2/* "module" */,-34 , 3/* "object" */,-34 , 4/* "class" */,-34 , 5/* "function" */,-34 , 6/* "static" */,-34 , 7/* "var" */,-34 , 12/* "=" */,-34 , 14/* "," */,-34 , 15/* "*" */,-34 , 17/* "Identifier" */,-34 , 8/* "{" */,-34 , 10/* "(" */,-34 ),
	/* State 82 */ new Array( 16/* "/" */,5 , 12/* "=" */,92 , 1/* "WHTS" */,7 ),
	/* State 83 */ new Array( 16/* "/" */,5 , 11/* ")" */,94 , 17/* "Identifier" */,95 , 1/* "WHTS" */,7 ),
	/* State 84 */ new Array( 10/* "(" */,72 , 8/* "{" */,73 , 13/* ";" */,74 , 11/* ")" */,96 , 18/* "Junk" */,46 , 2/* "module" */,47 , 3/* "object" */,48 , 4/* "class" */,49 , 5/* "function" */,50 , 6/* "static" */,51 , 7/* "var" */,52 , 12/* "=" */,53 , 14/* "," */,54 , 15/* "*" */,77 , 16/* "/" */,55 , 17/* "Identifier" */,56 , 1/* "WHTS" */,7 ),
	/* State 85 */ new Array( 10/* "(" */,72 , 8/* "{" */,73 , 13/* ";" */,74 , 9/* "}" */,97 , 18/* "Junk" */,46 , 2/* "module" */,47 , 3/* "object" */,48 , 4/* "class" */,49 , 5/* "function" */,50 , 6/* "static" */,51 , 7/* "var" */,52 , 12/* "=" */,53 , 14/* "," */,54 , 15/* "*" */,77 , 16/* "/" */,55 , 17/* "Identifier" */,56 , 1/* "WHTS" */,7 ),
	/* State 86 */ new Array( 9/* "}" */,-37 , 2/* "module" */,-37 , 3/* "object" */,-37 , 4/* "class" */,-37 , 7/* "var" */,-37 , 6/* "static" */,-37 , 5/* "function" */,-37 , 17/* "Identifier" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 87 */ new Array( 9/* "}" */,-37 , 2/* "module" */,-37 , 3/* "object" */,-37 , 4/* "class" */,-37 , 7/* "var" */,-37 , 6/* "static" */,-37 , 5/* "function" */,-37 , 17/* "Identifier" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 88 */ new Array( 9/* "}" */,-37 , 4/* "class" */,-37 , 7/* "var" */,-37 , 6/* "static" */,-37 , 5/* "function" */,-37 , 17/* "Identifier" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 89 */ new Array( 16/* "/" */,5 , 1/* "WHTS" */,7 , 9/* "}" */,-14 , 4/* "class" */,-14 , 7/* "var" */,-14 , 6/* "static" */,-14 , 5/* "function" */,-14 , 17/* "Identifier" */,-14 ),
	/* State 90 */ new Array( 13/* ";" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 91 */ new Array( 10/* "(" */,102 , 8/* "{" */,103 , 13/* ";" */,74 , 18/* "Junk" */,46 , 2/* "module" */,47 , 3/* "object" */,48 , 4/* "class" */,49 , 5/* "function" */,50 , 6/* "static" */,51 , 7/* "var" */,52 , 12/* "=" */,53 , 14/* "," */,54 , 15/* "*" */,77 , 16/* "/" */,55 , 17/* "Identifier" */,56 , 1/* "WHTS" */,7 ),
	/* State 92 */ new Array( 18/* "Junk" */,-37 , 2/* "module" */,-37 , 3/* "object" */,-37 , 4/* "class" */,-37 , 5/* "function" */,-37 , 6/* "static" */,-37 , 7/* "var" */,-37 , 12/* "=" */,-37 , 14/* "," */,-37 , 15/* "*" */,-37 , 16/* "/" */,-37 , 17/* "Identifier" */,-37 , 8/* "{" */,-37 , 10/* "(" */,-37 , 13/* ";" */,-37 , 1/* "WHTS" */,-37 ),
	/* State 93 */ new Array( 11/* ")" */,-37 , 14/* "," */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 94 */ new Array( 8/* "{" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 95 */ new Array( 1/* "WHTS" */,-24 , 16/* "/" */,-24 , 11/* ")" */,-24 , 14/* "," */,-24 ),
	/* State 96 */ new Array( 9/* "}" */,-33 , 18/* "Junk" */,-33 , 2/* "module" */,-33 , 3/* "object" */,-33 , 4/* "class" */,-33 , 5/* "function" */,-33 , 6/* "static" */,-33 , 7/* "var" */,-33 , 12/* "=" */,-33 , 14/* "," */,-33 , 15/* "*" */,-33 , 16/* "/" */,-33 , 17/* "Identifier" */,-33 , 1/* "WHTS" */,-33 , 13/* ";" */,-33 , 8/* "{" */,-33 , 10/* "(" */,-33 , 11/* ")" */,-33 ),
	/* State 97 */ new Array( 9/* "}" */,-32 , 18/* "Junk" */,-32 , 2/* "module" */,-32 , 3/* "object" */,-32 , 4/* "class" */,-32 , 5/* "function" */,-32 , 6/* "static" */,-32 , 7/* "var" */,-32 , 12/* "=" */,-32 , 14/* "," */,-32 , 15/* "*" */,-32 , 16/* "/" */,-32 , 17/* "Identifier" */,-32 , 1/* "WHTS" */,-32 , 13/* ";" */,-32 , 8/* "{" */,-32 , 10/* "(" */,-32 , 11/* ")" */,-32 ),
	/* State 98 */ new Array( 16/* "/" */,5 , 9/* "}" */,108 , 1/* "WHTS" */,7 , 2/* "module" */,15 , 3/* "object" */,16 , 4/* "class" */,17 , 7/* "var" */,18 , 6/* "static" */,19 , 5/* "function" */,20 , 17/* "Identifier" */,21 ),
	/* State 99 */ new Array( 16/* "/" */,5 , 9/* "}" */,109 , 1/* "WHTS" */,7 , 2/* "module" */,15 , 3/* "object" */,16 , 4/* "class" */,17 , 7/* "var" */,18 , 6/* "static" */,19 , 5/* "function" */,20 , 17/* "Identifier" */,21 ),
	/* State 100 */ new Array( 16/* "/" */,5 , 9/* "}" */,110 , 1/* "WHTS" */,7 , 4/* "class" */,17 , 7/* "var" */,18 , 6/* "static" */,19 , 5/* "function" */,20 , 17/* "Identifier" */,21 ),
	/* State 101 */ new Array( 16/* "/" */,5 , 13/* ";" */,116 , 1/* "WHTS" */,7 ),
	/* State 102 */ new Array( 11/* ")" */,-34 , 18/* "Junk" */,-34 , 2/* "module" */,-34 , 3/* "object" */,-34 , 4/* "class" */,-34 , 5/* "function" */,-34 , 6/* "static" */,-34 , 7/* "var" */,-34 , 12/* "=" */,-34 , 14/* "," */,-34 , 15/* "*" */,-34 , 16/* "/" */,-34 , 17/* "Identifier" */,-34 , 1/* "WHTS" */,-34 , 13/* ";" */,-34 , 8/* "{" */,-34 , 10/* "(" */,-34 ),
	/* State 103 */ new Array( 9/* "}" */,-34 , 18/* "Junk" */,-34 , 2/* "module" */,-34 , 3/* "object" */,-34 , 4/* "class" */,-34 , 5/* "function" */,-34 , 6/* "static" */,-34 , 7/* "var" */,-34 , 12/* "=" */,-34 , 14/* "," */,-34 , 15/* "*" */,-34 , 16/* "/" */,-34 , 17/* "Identifier" */,-34 , 1/* "WHTS" */,-34 , 13/* ";" */,-34 , 8/* "{" */,-34 , 10/* "(" */,-34 ),
	/* State 104 */ new Array( 18/* "Junk" */,-30 , 2/* "module" */,-30 , 3/* "object" */,-30 , 4/* "class" */,-30 , 5/* "function" */,-30 , 6/* "static" */,-30 , 7/* "var" */,-30 , 12/* "=" */,-30 , 14/* "," */,-30 , 15/* "*" */,-30 , 16/* "/" */,-26 , 17/* "Identifier" */,-30 , 1/* "WHTS" */,-26 , 13/* ";" */,-26 , 8/* "{" */,-30 , 10/* "(" */,-30 ),
	/* State 105 */ new Array( 16/* "/" */,5 , 1/* "WHTS" */,7 , 13/* ";" */,-29 , 18/* "Junk" */,-34 , 2/* "module" */,-34 , 3/* "object" */,-34 , 4/* "class" */,-34 , 5/* "function" */,-34 , 6/* "static" */,-34 , 7/* "var" */,-34 , 12/* "=" */,-34 , 14/* "," */,-34 , 15/* "*" */,-34 , 17/* "Identifier" */,-34 , 8/* "{" */,-34 , 10/* "(" */,-34 ),
	/* State 106 */ new Array( 16/* "/" */,5 , 11/* ")" */,120 , 14/* "," */,121 , 1/* "WHTS" */,7 ),
	/* State 107 */ new Array( 16/* "/" */,5 , 8/* "{" */,122 , 1/* "WHTS" */,7 ),
	/* State 108 */ new Array( 1/* "WHTS" */,-2 , 16/* "/" */,-2 , 37/* "$" */,-2 , 2/* "module" */,-2 , 3/* "object" */,-2 , 4/* "class" */,-2 , 7/* "var" */,-2 , 6/* "static" */,-2 , 5/* "function" */,-2 , 17/* "Identifier" */,-2 , 9/* "}" */,-2 ),
	/* State 109 */ new Array( 1/* "WHTS" */,-3 , 16/* "/" */,-3 , 37/* "$" */,-3 , 2/* "module" */,-3 , 3/* "object" */,-3 , 4/* "class" */,-3 , 7/* "var" */,-3 , 6/* "static" */,-3 , 5/* "function" */,-3 , 17/* "Identifier" */,-3 , 9/* "}" */,-3 ),
	/* State 110 */ new Array( 1/* "WHTS" */,-12 , 16/* "/" */,-12 , 37/* "$" */,-12 , 2/* "module" */,-12 , 3/* "object" */,-12 , 4/* "class" */,-12 , 7/* "var" */,-12 , 6/* "static" */,-12 , 5/* "function" */,-12 , 17/* "Identifier" */,-12 , 9/* "}" */,-12 ),
	/* State 111 */ new Array( 1/* "WHTS" */,-13 , 16/* "/" */,-13 , 9/* "}" */,-13 , 4/* "class" */,-13 , 7/* "var" */,-13 , 6/* "static" */,-13 , 5/* "function" */,-13 , 17/* "Identifier" */,-13 ),
	/* State 112 */ new Array( 1/* "WHTS" */,-15 , 16/* "/" */,-15 , 9/* "}" */,-15 , 4/* "class" */,-15 , 7/* "var" */,-15 , 6/* "static" */,-15 , 5/* "function" */,-15 , 17/* "Identifier" */,-15 ),
	/* State 113 */ new Array( 1/* "WHTS" */,-16 , 16/* "/" */,-16 , 9/* "}" */,-16 , 4/* "class" */,-16 , 7/* "var" */,-16 , 6/* "static" */,-16 , 5/* "function" */,-16 , 17/* "Identifier" */,-16 ),
	/* State 114 */ new Array( 1/* "WHTS" */,-17 , 16/* "/" */,-17 , 9/* "}" */,-17 , 4/* "class" */,-17 , 7/* "var" */,-17 , 6/* "static" */,-17 , 5/* "function" */,-17 , 17/* "Identifier" */,-17 ),
	/* State 115 */ new Array( 1/* "WHTS" */,-18 , 16/* "/" */,-18 , 9/* "}" */,-18 , 4/* "class" */,-18 , 7/* "var" */,-18 , 6/* "static" */,-18 , 5/* "function" */,-18 , 17/* "Identifier" */,-18 ),
	/* State 116 */ new Array( 1/* "WHTS" */,-19 , 16/* "/" */,-19 , 37/* "$" */,-19 , 2/* "module" */,-19 , 3/* "object" */,-19 , 4/* "class" */,-19 , 7/* "var" */,-19 , 6/* "static" */,-19 , 5/* "function" */,-19 , 17/* "Identifier" */,-19 , 9/* "}" */,-19 ),
	/* State 117 */ new Array( 10/* "(" */,72 , 8/* "{" */,73 , 13/* ";" */,74 , 11/* ")" */,123 , 18/* "Junk" */,46 , 2/* "module" */,47 , 3/* "object" */,48 , 4/* "class" */,49 , 5/* "function" */,50 , 6/* "static" */,51 , 7/* "var" */,52 , 12/* "=" */,53 , 14/* "," */,54 , 15/* "*" */,77 , 16/* "/" */,55 , 17/* "Identifier" */,56 , 1/* "WHTS" */,7 ),
	/* State 118 */ new Array( 10/* "(" */,72 , 8/* "{" */,73 , 13/* ";" */,74 , 9/* "}" */,124 , 18/* "Junk" */,46 , 2/* "module" */,47 , 3/* "object" */,48 , 4/* "class" */,49 , 5/* "function" */,50 , 6/* "static" */,51 , 7/* "var" */,52 , 12/* "=" */,53 , 14/* "," */,54 , 15/* "*" */,77 , 16/* "/" */,55 , 17/* "Identifier" */,56 , 1/* "WHTS" */,7 ),
	/* State 119 */ new Array( 13/* ";" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 120 */ new Array( 8/* "{" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 121 */ new Array( 17/* "Identifier" */,-37 , 1/* "WHTS" */,-37 , 16/* "/" */,-37 ),
	/* State 122 */ new Array( 9/* "}" */,-34 , 18/* "Junk" */,-34 , 2/* "module" */,-34 , 3/* "object" */,-34 , 4/* "class" */,-34 , 5/* "function" */,-34 , 6/* "static" */,-34 , 7/* "var" */,-34 , 12/* "=" */,-34 , 14/* "," */,-34 , 15/* "*" */,-34 , 16/* "/" */,-34 , 17/* "Identifier" */,-34 , 1/* "WHTS" */,-34 , 13/* ";" */,-34 , 8/* "{" */,-34 , 10/* "(" */,-34 ),
	/* State 123 */ new Array( 18/* "Junk" */,-33 , 2/* "module" */,-33 , 3/* "object" */,-33 , 4/* "class" */,-33 , 5/* "function" */,-33 , 6/* "static" */,-33 , 7/* "var" */,-33 , 12/* "=" */,-33 , 14/* "," */,-33 , 15/* "*" */,-33 , 16/* "/" */,-28 , 17/* "Identifier" */,-33 , 1/* "WHTS" */,-28 , 13/* ";" */,-28 , 8/* "{" */,-33 , 10/* "(" */,-33 ),
	/* State 124 */ new Array( 18/* "Junk" */,-32 , 2/* "module" */,-32 , 3/* "object" */,-32 , 4/* "class" */,-32 , 5/* "function" */,-32 , 6/* "static" */,-32 , 7/* "var" */,-32 , 12/* "=" */,-32 , 14/* "," */,-32 , 15/* "*" */,-32 , 16/* "/" */,-27 , 17/* "Identifier" */,-32 , 1/* "WHTS" */,-27 , 13/* ";" */,-27 , 8/* "{" */,-32 , 10/* "(" */,-32 ),
	/* State 125 */ new Array( 16/* "/" */,5 , 13/* ";" */,129 , 1/* "WHTS" */,7 ),
	/* State 126 */ new Array( 16/* "/" */,5 , 8/* "{" */,130 , 1/* "WHTS" */,7 ),
	/* State 127 */ new Array( 16/* "/" */,5 , 17/* "Identifier" */,131 , 1/* "WHTS" */,7 ),
	/* State 128 */ new Array( 10/* "(" */,72 , 8/* "{" */,73 , 13/* ";" */,74 , 9/* "}" */,132 , 18/* "Junk" */,46 , 2/* "module" */,47 , 3/* "object" */,48 , 4/* "class" */,49 , 5/* "function" */,50 , 6/* "static" */,51 , 7/* "var" */,52 , 12/* "=" */,53 , 14/* "," */,54 , 15/* "*" */,77 , 16/* "/" */,55 , 17/* "Identifier" */,56 , 1/* "WHTS" */,7 ),
	/* State 129 */ new Array( 1/* "WHTS" */,-20 , 16/* "/" */,-20 , 37/* "$" */,-20 , 2/* "module" */,-20 , 3/* "object" */,-20 , 4/* "class" */,-20 , 7/* "var" */,-20 , 6/* "static" */,-20 , 5/* "function" */,-20 , 17/* "Identifier" */,-20 , 9/* "}" */,-20 ),
	/* State 130 */ new Array( 9/* "}" */,-34 , 18/* "Junk" */,-34 , 2/* "module" */,-34 , 3/* "object" */,-34 , 4/* "class" */,-34 , 5/* "function" */,-34 , 6/* "static" */,-34 , 7/* "var" */,-34 , 12/* "=" */,-34 , 14/* "," */,-34 , 15/* "*" */,-34 , 16/* "/" */,-34 , 17/* "Identifier" */,-34 , 1/* "WHTS" */,-34 , 13/* ";" */,-34 , 8/* "{" */,-34 , 10/* "(" */,-34 ),
	/* State 131 */ new Array( 1/* "WHTS" */,-23 , 16/* "/" */,-23 , 11/* ")" */,-23 , 14/* "," */,-23 ),
	/* State 132 */ new Array( 1/* "WHTS" */,-22 , 16/* "/" */,-22 , 37/* "$" */,-22 , 2/* "module" */,-22 , 3/* "object" */,-22 , 4/* "class" */,-22 , 7/* "var" */,-22 , 6/* "static" */,-22 , 5/* "function" */,-22 , 17/* "Identifier" */,-22 , 9/* "}" */,-22 ),
	/* State 133 */ new Array( 10/* "(" */,72 , 8/* "{" */,73 , 13/* ";" */,74 , 9/* "}" */,134 , 18/* "Junk" */,46 , 2/* "module" */,47 , 3/* "object" */,48 , 4/* "class" */,49 , 5/* "function" */,50 , 6/* "static" */,51 , 7/* "var" */,52 , 12/* "=" */,53 , 14/* "," */,54 , 15/* "*" */,77 , 16/* "/" */,55 , 17/* "Identifier" */,56 , 1/* "WHTS" */,7 ),
	/* State 134 */ new Array( 1/* "WHTS" */,-21 , 16/* "/" */,-21 , 37/* "$" */,-21 , 2/* "module" */,-21 , 3/* "object" */,-21 , 4/* "class" */,-21 , 7/* "var" */,-21 , 6/* "static" */,-21 , 5/* "function" */,-21 , 17/* "Identifier" */,-21 , 9/* "}" */,-21 )
);

/* Goto-Table */
var goto_tab = new Array(
	/* State 0 */ new Array( 20/* Global */,1 , 19/* ObjectContents */,2 , 21/* W */,3 ),
	/* State 1 */ new Array(  ),
	/* State 2 */ new Array( 21/* W */,4 ),
	/* State 3 */ new Array( 35/* _W */,6 ),
	/* State 4 */ new Array( 35/* _W */,6 , 24/* ObjectContent */,8 , 22/* Module */,9 , 23/* Object */,10 , 25/* Class */,11 , 26/* VariableDef */,12 , 27/* Function */,13 , 28/* NativeBlock */,14 ),
	/* State 5 */ new Array(  ),
	/* State 6 */ new Array(  ),
	/* State 7 */ new Array(  ),
	/* State 8 */ new Array( 21/* W */,23 ),
	/* State 9 */ new Array(  ),
	/* State 10 */ new Array(  ),
	/* State 11 */ new Array(  ),
	/* State 12 */ new Array(  ),
	/* State 13 */ new Array(  ),
	/* State 14 */ new Array(  ),
	/* State 15 */ new Array( 21/* W */,24 ),
	/* State 16 */ new Array( 21/* W */,25 ),
	/* State 17 */ new Array( 21/* W */,26 ),
	/* State 18 */ new Array( 21/* W */,27 ),
	/* State 19 */ new Array( 21/* W */,28 ),
	/* State 20 */ new Array( 21/* W */,29 ),
	/* State 21 */ new Array( 21/* W */,30 ),
	/* State 22 */ new Array( 36/* MLComment */,31 ),
	/* State 23 */ new Array( 35/* _W */,6 ),
	/* State 24 */ new Array( 35/* _W */,6 ),
	/* State 25 */ new Array( 35/* _W */,6 ),
	/* State 26 */ new Array( 35/* _W */,6 ),
	/* State 27 */ new Array( 35/* _W */,6 ),
	/* State 28 */ new Array( 35/* _W */,6 ),
	/* State 29 */ new Array( 35/* _W */,6 ),
	/* State 30 */ new Array( 35/* _W */,6 ),
	/* State 31 */ new Array( 34/* PossibleJunk */,44 , 35/* _W */,57 ),
	/* State 32 */ new Array( 21/* W */,58 ),
	/* State 33 */ new Array( 21/* W */,59 ),
	/* State 34 */ new Array( 21/* W */,60 ),
	/* State 35 */ new Array( 21/* W */,61 ),
	/* State 36 */ new Array( 21/* W */,62 ),
	/* State 37 */ new Array( 21/* W */,63 ),
	/* State 38 */ new Array( 33/* NativeCode */,64 ),
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
	/* State 53 */ new Array(  ),
	/* State 54 */ new Array(  ),
	/* State 55 */ new Array(  ),
	/* State 56 */ new Array(  ),
	/* State 57 */ new Array(  ),
	/* State 58 */ new Array( 35/* _W */,6 ),
	/* State 59 */ new Array( 35/* _W */,6 ),
	/* State 60 */ new Array( 35/* _W */,6 ),
	/* State 61 */ new Array( 35/* _W */,6 ),
	/* State 62 */ new Array( 35/* _W */,6 ),
	/* State 63 */ new Array( 35/* _W */,6 ),
	/* State 64 */ new Array( 34/* PossibleJunk */,75 , 35/* _W */,57 ),
	/* State 65 */ new Array(  ),
	/* State 66 */ new Array( 21/* W */,78 ),
	/* State 67 */ new Array( 21/* W */,79 ),
	/* State 68 */ new Array( 21/* W */,80 ),
	/* State 69 */ new Array( 21/* W */,81 ),
	/* State 70 */ new Array( 21/* W */,82 ),
	/* State 71 */ new Array( 21/* W */,83 ),
	/* State 72 */ new Array( 33/* NativeCode */,84 ),
	/* State 73 */ new Array( 33/* NativeCode */,85 ),
	/* State 74 */ new Array(  ),
	/* State 75 */ new Array(  ),
	/* State 76 */ new Array(  ),
	/* State 77 */ new Array(  ),
	/* State 78 */ new Array( 35/* _W */,6 , 19/* ObjectContents */,86 , 21/* W */,3 ),
	/* State 79 */ new Array( 35/* _W */,6 , 19/* ObjectContents */,87 , 21/* W */,3 ),
	/* State 80 */ new Array( 35/* _W */,6 , 29/* ClassContents */,88 , 21/* W */,89 ),
	/* State 81 */ new Array( 35/* _W */,6 , 31/* NativeCodeInline */,90 , 33/* NativeCode */,91 ),
	/* State 82 */ new Array( 35/* _W */,6 ),
	/* State 83 */ new Array( 35/* _W */,6 , 32/* ArgumentList */,93 ),
	/* State 84 */ new Array( 34/* PossibleJunk */,75 , 35/* _W */,57 ),
	/* State 85 */ new Array( 34/* PossibleJunk */,75 , 35/* _W */,57 ),
	/* State 86 */ new Array( 21/* W */,98 ),
	/* State 87 */ new Array( 21/* W */,99 ),
	/* State 88 */ new Array( 21/* W */,100 ),
	/* State 89 */ new Array( 35/* _W */,6 ),
	/* State 90 */ new Array( 21/* W */,101 ),
	/* State 91 */ new Array( 34/* PossibleJunk */,104 , 35/* _W */,57 ),
	/* State 92 */ new Array( 21/* W */,105 ),
	/* State 93 */ new Array( 21/* W */,106 ),
	/* State 94 */ new Array( 21/* W */,107 ),
	/* State 95 */ new Array(  ),
	/* State 96 */ new Array(  ),
	/* State 97 */ new Array(  ),
	/* State 98 */ new Array( 35/* _W */,6 , 24/* ObjectContent */,8 , 22/* Module */,9 , 23/* Object */,10 , 25/* Class */,11 , 26/* VariableDef */,12 , 27/* Function */,13 , 28/* NativeBlock */,14 ),
	/* State 99 */ new Array( 35/* _W */,6 , 24/* ObjectContent */,8 , 22/* Module */,9 , 23/* Object */,10 , 25/* Class */,11 , 26/* VariableDef */,12 , 27/* Function */,13 , 28/* NativeBlock */,14 ),
	/* State 100 */ new Array( 35/* _W */,6 , 30/* ClassContent */,111 , 25/* Class */,112 , 26/* VariableDef */,113 , 27/* Function */,114 , 28/* NativeBlock */,115 ),
	/* State 101 */ new Array( 35/* _W */,6 ),
	/* State 102 */ new Array( 33/* NativeCode */,117 ),
	/* State 103 */ new Array( 33/* NativeCode */,118 ),
	/* State 104 */ new Array(  ),
	/* State 105 */ new Array( 35/* _W */,6 , 31/* NativeCodeInline */,119 , 33/* NativeCode */,91 ),
	/* State 106 */ new Array( 35/* _W */,6 ),
	/* State 107 */ new Array( 35/* _W */,6 ),
	/* State 108 */ new Array(  ),
	/* State 109 */ new Array(  ),
	/* State 110 */ new Array(  ),
	/* State 111 */ new Array(  ),
	/* State 112 */ new Array(  ),
	/* State 113 */ new Array(  ),
	/* State 114 */ new Array(  ),
	/* State 115 */ new Array(  ),
	/* State 116 */ new Array(  ),
	/* State 117 */ new Array( 34/* PossibleJunk */,75 , 35/* _W */,57 ),
	/* State 118 */ new Array( 34/* PossibleJunk */,75 , 35/* _W */,57 ),
	/* State 119 */ new Array( 21/* W */,125 ),
	/* State 120 */ new Array( 21/* W */,126 ),
	/* State 121 */ new Array( 21/* W */,127 ),
	/* State 122 */ new Array( 33/* NativeCode */,128 ),
	/* State 123 */ new Array(  ),
	/* State 124 */ new Array(  ),
	/* State 125 */ new Array( 35/* _W */,6 ),
	/* State 126 */ new Array( 35/* _W */,6 ),
	/* State 127 */ new Array( 35/* _W */,6 ),
	/* State 128 */ new Array( 34/* PossibleJunk */,75 , 35/* _W */,57 ),
	/* State 129 */ new Array(  ),
	/* State 130 */ new Array( 33/* NativeCode */,133 ),
	/* State 131 */ new Array(  ),
	/* State 132 */ new Array(  ),
	/* State 133 */ new Array( 34/* PossibleJunk */,75 , 35/* _W */,57 ),
	/* State 134 */ new Array(  )
);



/* Symbol labels */
var labels = new Array(
	"Global'" /* Non-terminal symbol */,
	"WHTS" /* Terminal symbol */,
	"module" /* Terminal symbol */,
	"object" /* Terminal symbol */,
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
	"ObjectContents" /* Non-terminal symbol */,
	"Global" /* Non-terminal symbol */,
	"W" /* Non-terminal symbol */,
	"Module" /* Non-terminal symbol */,
	"Object" /* Non-terminal symbol */,
	"ObjectContent" /* Non-terminal symbol */,
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
		act = 136;
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
		if( act == 136 )
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
			
			while( act == 136 && la != 37 )
			{
				if( v8_dbg_withtrace )
					__v8dbg_print( "\tError recovery\n" +
									"Current lookahead: " + labels[la] + " (" + info.att + ")\n" +
									"Action: " + act + "\n\n" );
				if( la == -1 )
					info.offset++;
					
				while( act == 136 && sstack.length > 0 )
				{
					sstack.pop();
					vstack.pop();
					
					if( sstack.length == 0 )
						break;
						
					act = 136;
					for( var i = 0; i < act_tab[sstack[sstack.length-1]].length; i+=2 )
					{
						if( act_tab[sstack[sstack.length-1]][i] == la )
						{
							act = act_tab[sstack[sstack.length-1]][i+1];
							break;
						}
					}
				}
				
				if( act != 136 )
					break;
				
				for( var i = 0; i < rsstack.length; i++ )
				{
					sstack.push( rsstack[i] );
					vstack.push( rvstack[i] );
				}
				
				la = __v8lex( info );
			}
			
			if( act == 136 )
			{
				if( v8_dbg_withtrace )
					__v8dbg_print( "\tError recovery failed, terminating parse process..." );
				break;
			}


			if( v8_dbg_withtrace )
				__v8dbg_print( "\tError recovery succeeded, continuing" );
		}
		
		/*
		if( act == 136 )
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
		 generateCode(createObject("global", vstack[ vstack.length - 1 ]), false); 
	}
	break;
	case 2:
	{
		 rval = createObject(vstack[ vstack.length - 7 ], vstack[ vstack.length - 3 ], vstack[ vstack.length - 8 ].line, true); 
	}
	break;
	case 3:
	{
		 rval = createObject(vstack[ vstack.length - 7 ], vstack[ vstack.length - 3 ], vstack[ vstack.length - 8 ].line, false); 
	}
	break;
	case 4:
	{
		 rval = vstack[ vstack.length - 4 ].concat([vstack[ vstack.length - 2 ]]); 
	}
	break;
	case 5:
	{
		 rval = []; 
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
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 11:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 12:
	{
		 rval = createClass(vstack[ vstack.length - 7 ], vstack[ vstack.length - 3 ]); 
	}
	break;
	case 13:
	{
		 rval = vstack[ vstack.length - 3 ].concat([vstack[ vstack.length - 1 ]]); 
	}
	break;
	case 14:
	{
		 rval = []; 
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
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 18:
	{
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 19:
	{
		 rval = {type:'var', name:vstack[ vstack.length - 7 ], val:vstack[ vstack.length - 3 ]}; 
	}
	break;
	case 20:
	{
		 rval = {type:'static-var', name:vstack[ vstack.length - 7 ], val:vstack[ vstack.length - 3 ]}; 
	}
	break;
	case 21:
	{
		 rval = {type:'function', name:vstack[ vstack.length - 11 ], args:vstack[ vstack.length - 7 ], code:vstack[ vstack.length - 2 ], line:vstack[ vstack.length - 4 ].line}; 
	}
	break;
	case 22:
	{
		 rval = {type:'function', name:vstack[ vstack.length - 9 ], args:[], code:vstack[ vstack.length - 2 ], line:vstack[ vstack.length - 4 ].line}; 
	}
	break;
	case 23:
	{
		 rval = (vstack[ vstack.length - 5 ]).concat([{name:vstack[ vstack.length - 1 ]}]); 
	}
	break;
	case 24:
	{
		 rval = [{name:vstack[ vstack.length - 1 ]}]; 
	}
	break;
	case 25:
	{
		 rval = {type:'native-block', which:vstack[ vstack.length - 5 ], code:vstack[ vstack.length - 2 ]}; 
	}
	break;
	case 26:
	{
		 rval = vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 27:
	{
		 rval = vstack[ vstack.length - 4 ] + vstack[ vstack.length - 3 ] + vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 28:
	{
		 rval = vstack[ vstack.length - 4 ] + vstack[ vstack.length - 3 ] + vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 29:
	{
		 rval = ""; 
	}
	break;
	case 30:
	{
		 rval = vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 31:
	{
		 rval = vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 32:
	{
		 rval = vstack[ vstack.length - 4 ] + vstack[ vstack.length - 3 ] + vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 33:
	{
		 rval = vstack[ vstack.length - 4 ] + vstack[ vstack.length - 3 ] + vstack[ vstack.length - 2 ] + vstack[ vstack.length - 1 ]; 
	}
	break;
	case 34:
	{
		 rval = ""; 
	}
	break;
	case 35:
	{
		 rval = {s:vstack[ vstack.length - 2 ].s + vstack[ vstack.length - 1 ].s, line:vstack[ vstack.length - 1 ].line}; 
	}
	break;
	case 36:
	{
		 rval = {s:vstack[ vstack.length - 6 ].s, line:vstack[ vstack.length - 6 ].line}; 
	}
	break;
	case 37:
	{
		 rval = {s:"",line:lineNumber}; 
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
		rval = vstack[ vstack.length - 2 ];
	}
	break;
	case 43:
	{
		rval = vstack[ vstack.length - 2 ];
	}
	break;
	case 44:
	{
		rval = vstack[ vstack.length - 0 ];
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
		rval = vstack[ vstack.length - 1 ];
	}
	break;
	case 57:
	{
		 rval = vstack[ vstack.length - 1 ].s; 
	}
	break;
	case 58:
	{
		 lineNumber += vstack[ vstack.length - 1 ].replace(/[^\n]/g,"").length; rval = {s:vstack[ vstack.length - 1 ],line:lineNumber};
	}
	break;
	case 59:
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
var Io = require("Io");
var lastSlash = arguments[1].lastIndexOf('/')+1;
var baseDir = arguments[1].substr(0, lastSlash), baseName = arguments[1].substr(lastSlash).replace(/\.gear$/, '');
var gear = {gear:baseDir+baseName+".gear", cc:baseDir+baseName+".cc", h:baseDir+baseName+".h"};
if(arguments.length == 2) {
    var str         = Io.read(gear.gear);
    var error_cnt   = 0;
    var error_off   = [];
    var error_la    = [];
    
    if((error_cnt = __v8parse(str, error_off, error_la)) > 0)
        for(var i = 0; i < error_cnt; i++) {
            var bf = str.substr(0, error_off[i]);
            print(gear.gear+":"+(nLines(bf)+1)+":"+(nCols(bf)+1)+": Error near >" + str.substr(error_off[i], 30) + "<, expecting \"" + error_la[i].join() + "\"" );
        }
}
else
    print("usage: " + arguments[0] + " <file>");
exit(); // Just in case v8-gearbox is a (bit) broken

