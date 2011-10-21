//"use strict";

var exports = {}; // TODO: Remove this later when it becomes a module

exports.Codeaze = function Codeaze(options) {
    options = options || {};
    this.symbols = options.symbols || {};
    this.mainSymbol = options.mainSymbol || "main";
    this.spaceSymbol = options.spaceSymbol || "space";
    this.spaceDefault = options.spaceDefault || "*";
    this.spaceIgnore = options.spaceIgnore === undefined ? true : options.spaceIgnore;
    this.init = options.init || function(){};
};

exports.Codeaze.prototype.parse = function parse(text) {
    if(!this.hasSymbolsFixed) {
        this.fixSymbols();
        this.hasSymbolsFixed = true;
    }
    var state = {text:text, offset:0, symbols:this.symbols, main:this.symbols[this.mainSymbol], space:this.symbols[this.spaceSymbol]};
    this.init.call(state);
    return state.main.parse(state);
};

exports.Codeaze.prototype.fixSymbols = function fixSymbols() {
    // The special space symbol
    this.symbols[" "] = {symbol:this.symbols[this.spaceSymbol], ignore:this.spaceIgnore, min:0, max:Infinity};
    if(this.spaceDefault == "?")
        this.symbols[" "].max = 1;
    if(this.spaceDefault == "+")
        this.symbols[" "].min = 1;
    
    for(var i in this.symbols) {
        if(i == " ")
            continue;
        if(this.symbols[i].fixSymbol)
            this.symbols[i] = this.symbols[i].fixSymbol;
        else
            this.symbols[i].fixSymbols(this.symbols);
    }
    
    delete this.symbols[" "];
};

exports.Symbol = function Symbol(sym, proc) {
    // Reference to another symbol, to be fixed later
    if(typeof sym == "string" && /^([a-zA-Z_]\w*| )$/.test(sym))
        return {fixSymbol:sym};
    
    // Forward Symbol(...) to new Symbol(...)
    if(!(this instanceof exports.Symbol))
        return new exports.Symbol(sym, proc);
    
    this.proc = proc;
    
    // RegExp Symbols
    if(sym instanceof RegExp) {
        this.regexp = RegExp("^"+sym.source);
        return;
    }
    
    // Parse the Symbol
    var state = {text:sym, offset:0, choiceLists:[], choiceList:new exports.ChoiceList()};

    state.makeChoice = function(obj, defaults) {
        if(this.offset >= this.text.length)
            return defaults ? new exports.Choice(obj, defaults.min, defaults.max) : obj;
        if(this.text[this.offset] == "?")
            return new exports.Choice(obj, 0, 1);
        if(this.text[this.offset] == "*")
            return new exports.Choice(obj, 0, Infinity);
        if(this.text[this.offset] == "+")
            return new exports.Choice(obj, 1, Infinity);
        if(this.text[this.offset] == "{") {
            var min = "", max = "";
            for(this.offset++; this.offset < text.length && this.text[this.offset] != ","; this.offset++)
                min += this.text[this.offset];
            for(this.offset++; this.offset < text.length && this.text[this.offset] != "}"; this.offset++)
                max += this.text[this.offset];
            return new exports.Choice(obj, +min, max.length ? +max : Infinity);
        }
        this.offset--;
        return defaults ? new exports.Choice(obj, defaults.min, defaults.max) : obj;
    };
    
    for(state.offset = 0; state.offset < state.text.length; state.offset++) {
        var c = state.text[state.offset];
        
        // RegExps
        if(c == "/") {
            var regexp = "";
            for(state.offset++; state.offset < state.text.length; state.offset++) {
                if(state.text[state.offset] == "/")
                    break;
                regexp += state.text[state.offset];
                if(state.text[state.offset] == "\\")
                    regexp += state.text[++state.offset];
            }
            state.offset++;
            state.choiceList.push(state.makeChoice(new exports.Symbol(new RegExp(regexp))));
            continue;
        }
        
        // References to other symbols
        if(/[a-zA-Z_]/.test(c)) {
            var symbol = c;
            for(state.offset++; state.offset < state.text.length && /\w/.test(state.text[state.offset]); state.offset++)
                symbol += state.text[state.offset];
            state.choiceList.push(state.makeChoice(exports.Symbol(symbol)));
            continue;
        }
        
        // Any kinds of space
        if(/\s/.test(c)) {
            for(state.offset++; state.offset < state.text.length && /\s/.test(state.text[state.offset]); state.offset++);
            state.choiceList.push(state.makeChoice(exports.Symbol(" "), {min:-Infinity, max:Infinity}));
            continue;
        }
        
        // Parallel choices
        if(c == "|") {
            if(!state.choiceLists.length || !state.choiceLists[state.choiceLists.length - 1].parallel)
                state.choiceLists.push(new exports.ChoiceList(true));
            state.choiceLists[state.choiceLists.length - 1].push(state.choiceList);
            state.choiceList = new exports.ChoiceList();
            continue;
        }
        
        // Start of serial choices
        if(c == "(") {
            state.choiceLists.push(state.choiceList);
            state.choiceList = new exports.ChoiceList();
            continue;
        }
        
        // End of serial choices
        if(c == ")") {
            var closingChoiceList = state.choiceList;
            state.choiceList = state.choiceLists.pop();
            if(state.choiceList.parallel) {
                state.choiceList.push(closingChoiceList);
                closingChoiceList = state.choiceList;
                state.choiceList = state.choiceLists.pop();
            }
            state.offset++;
            state.choiceList.push(state.makeChoice(closingChoiceList));
            continue;
        }
        
        // Separator
        if(c == ",")
            continue;
        
        throw new Error("Unrecognized token '" + c + "' in '" + state.text + "'");
    }
    
    if(state.choiceLists.length > 1)
        throw new Error("Too many choice lists left ("+state.choiceLists.length+")");
    
    if(state.choiceLists.length && state.choiceLists[0].parallel) {
        state.choiceLists[0].push(state.choiceList);
        state.choiceList = state.choiceLists[0];
    }
    this.choiceList = state.choiceList;
};

exports.Symbol.prototype.parse = function parse(state) {
    if(this.regexp) {
        var match = this.regexp.exec(state.text.substr(state.offset));
        if(!match)
            return false;
        state.offset += match[0].length;
        if(this.proc)
            return this.proc(match);
        return match;
    }
    if(this.choiceList) {
        var match = this.choiceList.parse(state);
        if(match === false)
            return false;
        if(this.proc)
            return this.proc(match);
        return match;
    }
};

exports.Symbol.prototype.fixSymbols = function fixSymbols(symbols) {
    if(this.choiceList)
        this.choiceList.fixSymbols(symbols);
};

exports.Choice = function Choice(obj, min, max) {
    this.obj = obj;
    this.min = min;
    this.max = max;
};

exports.Choice.prototype.parse = function parse(state) {
    var matches = [], oldOffset = state.offset;
    while(matches.length < this.max) {
        var match = this.obj.parse(state);
        if(match === false)
            break;
        matches.push(match);
    }
    if(matches.length < this.min) {
        state.offset = oldOffset;
        return false;
    }
    
    // Let's not make X? return [X] but X or null
    if(this.min === 0 && this.max === 1)
        return matches[0] || null;
    return matches;
};

exports.Choice.prototype.fixSymbols = function fixSymbols(symbols) {
    if(this.obj.fixSymbol) {
        if(this.obj.fixSymbol == " ") {
            this.obj = symbols[" "].symbol;
            this.ignore = symbols[" "].ignore;
            if(this.min === -Infinity && this.max === Infinity) {
                this.min = symbols[" "].min;
                this.max = symbols[" "].max;
            }
        } else
            this.obj = symbols[this.obj.fixSymbol];
    } else
        this.obj.fixSymbols(symbols);
};

exports.ChoiceList = function ChoiceList(parallel) {
    this.parallel = !!parallel;
};

exports.ChoiceList.prototype = [];

exports.ChoiceList.prototype.parse = function parse(state) {
    if(this.parallel) {
        var match = false;
        for(var i = 0; i < this.length; i++)
            if((match = this[i].parse(state)) !== false)
                return match;
        return false;
    }
    
    var matches = [], oldOffset = state.offset;
    for(var i = 0; i < this.length; i++) {
        var match = this[i].parse(state);
        if(match === false) {
            state.offset = oldOffset;
            return false;
        }
        if(!this[i].ignore)
            matches.push(match);
    }
    return matches;
};

exports.ChoiceList.prototype.fixSymbols = function fixSymbols(symbols) {
    for(var i = 0; i < this.length; i++) {
        if(this[i].fixSymbol)
            this[i] = symbols[this[i].fixSymbol];
        else
            this[i].fixSymbols(symbols);
    }
};

exports; // TODO: Remove this later when it becomes a module
