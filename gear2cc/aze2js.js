if(arguments.length <= 2)
    exit(print('Usage: '+arguments[0]+' <Codeaze source file> <JavaScript output file>'));

var sourceFile = arguments[1], outputFile = arguments[2];

var Io = require('Io');
var source = Io.read(sourceFile), output = 'try {var codeaze = load("Codeaze.js");}catch(e){var codeaze = load("gear2cc/Codeaze.js");} /** @todo require("./Codeaze.js") */\nvar $aze = new codeaze.Codeaze();\n\n';

function scanRegExp(flags) {
    var regexp = '/';
    for(i++; i < source.length; i++) {
        regexp += source[i]
        if(source[i] == '/')
            break;
        if(source[i] == '[')
            for(i++; i < source.length; i++) {
                regexp += source[i];
                if(source[i] == ']')
                    break;
                if(source[i] == '\\' && source[i+1] == ']')
                    regexp += source[++i];
            }
        if(source[i] == '\\')
            regexp += source[++i];
    }
    if(flags) {
        for(i++; i < source.length && /\w/.test(source[i]); i++)
            regexp += source[i];
        i--;
    }
    return regexp;
}

function scanPattern() {
    var pattern = '';
    for(i++; i < source.length; i++) {
        if(source[i] == '/') {
            pattern += scanRegExp();
            continue;
        }
        if(source[i] == '`')
            break;
        pattern += source[i];
    }
    return "'"+pattern.replace(/(['\\])/g, '\\$1')+"'";
}

function scanCode() {
    var code = '', depth = 0;
    for(i++; i < source.length; i++) {
        if(source[i] == '}')
            depth--;
        if(depth < 0)
            break;
        
        // Single line comments
        if(source[i] == '/' && source[i+1] == '/') {
            for(; i < source.length-1 && source[i] != '\n'; i++)
                code += source[i];
            code += source[i];
            continue;
        }
        // Multi line comments
        if(source[i] == '/' && source[i+1] == '*') {
            code += source[i++];
            for(; i < source.length-1 && !(source[i] == '*' && source[i+1] == '/'); i++)
                code += source[i];
            code += source[i];
            code += source[++i];
            continue;
        }
            
        // RegExps
        if(source[i] == '/' && !/(\w|\))\s*$/.test(source)) {
            code += scanRegExp(true);
            continue;
        }
            
        code += source[i];
        
        if(source[i] == '{')
            depth++;
        
        // Single quote strings
        if(source[i] == "'") {
            for(i++; i < source.length; i++) {
                code += source[i];
                if(source[i] == "'" || source[i] == '\n')
                    break;
                if(source[i] == '\\')
                    code += source[++i];
            }
            continue;
        }
        // Double quote strings
        if(source[i] == '"') {
            for(i++; i < source.length; i++) {
                code += source[i];
                if(source[i] == '"')
                    break;
                if(source[i] == '\\')
                    code += source[++i];
            }
            continue;
        }
    }
    return code;
}

for(var i = 0; i < source.length; i++) {
    var c = source[i];
    
    // Any kinds of space
    if(/\s/.test(c)) {
        for(i++; i < source.length && /\s/.test(source[i]); i++);
        i--;
        continue;
    }
    
    // JavaScript code
    if(c == '{') {
        output += scanCode().replace(/^    /gm,'');
        continue;
    }
    
    // Rule definition
    if(/[a-zA-Z_]/.test(c)) {
        var ruleName = c, pattern, proc = '';
        for(i++; i < source.length && /\w/.test(source[i]); i++)
            ruleName += source[i];
        for(; i < source.length && /\s/.test(source[i]); i++);
        if(source[i] != ':')
            throw new Error('Expected : after rule indentifier, got \''+source[i]+'\'');
        for(i++; i < source.length && /\s/.test(source[i]); i++);
        if(source[i] == '/')
            pattern = scanRegExp();
        else if(source[i] == '`')
            pattern = scanPattern();
        else
            throw new Error('Expected ` or / after :, got \''+source[i]+'\'');
        for(i++; i < source.length && /\s/.test(source[i]); i++);
        if(source[i] == '{') {
            proc = ', function($){var $$;'+scanCode().replace(/\$(\d+)/g, '$[$1]').replace(/([^;])$/, '$1;')+'return $$;}';
            i++;
        }
        for(; i < source.length && /\s/.test(source[i]); i++);
        if(source[i] != ';')
            throw new Error('Expected ; after rule declaration, got \''+source[i]+'\'');
        output += '$aze.symbols.'+ruleName+' = new codeaze.Symbol('+pattern+proc+');\n';
        continue;
    }
    
    // Parallel choices
    /*if(c == '|') {
        if(!state.choiceLists.length || !state.choiceLists[state.choiceLists.length - 1].parallel)
            state.choiceLists.push(new exports.ChoiceList(true));
        state.choiceLists[state.choiceLists.length - 1].push(state.choiceList);
        state.choiceList = new exports.ChoiceList();
        continue;
    }*/
    throw new Error('Unrecognized token \'' + c + '\' in "' + source + '"');
}

Io.write(outputFile, output);