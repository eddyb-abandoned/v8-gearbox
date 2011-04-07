/*
 * Copyright (c) 2011 Eduard Burtescu
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITRTLSS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, RTLGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONRTLCTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <v8-gearbox.h>
#include <modules/Io.h>

#include <cstdlib>

namespace Gearbox {
    using namespace Modules;
    
    Context *Context::m_pCurrentContext = 0;
    
    static v8::Handle<v8::Value> _exit(const v8::Arguments& args) {
        std::_Exit(Value(args[0]));
        return undefined;
    }
    
    static v8::Handle<v8::Value> _load(const v8::Arguments& args) {
        if(args.Length() >= 1) {
            Value file(args[0]);
            
            TryCatch tryCatch;
            String source = Io::read(file);
            
            // Report exceptions caught while reading the file
            if(tryCatch.hasCaught())
                return undefined;
            
            Context *pCurrentContext = Context::getCurrent();
            if(!pCurrentContext)
                return Throw(Error("No Context is in use"));
            
            return pCurrentContext->runScript(source, file);
        }
        return Throw(Error("Invalid call to load"));
    }
    
    static v8::Handle<v8::Value> _print(const v8::Arguments& args) {
        if(args.Length()) {
            for(int i = 0; i < args.Length(); i++)
                printf(i ? " %s" : "%s", *Value(args[i]).to<String>());
            printf(_STR_NEWLINE);
            return undefined;
        }
        return Throw(Error("Invalid call to print"));
    }
    
    static v8::Handle<v8::Value> _require(const v8::Arguments& args) {
        if(args.Length() >= 1)
            return Module::require(Value(args[0]));
        return Throw(Error("Invalid call to require"));
    }
    
    Context::Context() {
        // Save the previous context
        m_pPreviousContext = m_pCurrentContext;
        
        // Create the context
        m_hContext = v8::Context::New();
        
        // Enter this context
        m_hContext->Enter();
        
        // We're in this context
        m_pCurrentContext = this;
        
        // Setup the context
        setup();
    }
    
    Context::~Context() {
        // Exit this context
        m_hContext->Exit();
        
        // We're in the previous context
        m_pCurrentContext = m_pPreviousContext;
        
        // Dispose this context
        m_hContext.Dispose();
    }
    
    Value Context::runScript(String source, String name) {
        TryCatch tryCatch;
        
        // Compile the script source
        v8::Handle<v8::Script> script = v8::Script::Compile(source, name);
        
        // Check for any errors that could have happened at compile time
        if(script.IsEmpty() || tryCatch.hasCaught())
            return undefined;
        
        // Exceptions can be thrown, we are inside JavaScript
        bool bCanThrowBefore = TryCatch::canThrow(true);
        
        // Run the script and get the result
        var result = script->Run();
        
        // We are back from JavaScript
        TryCatch::canThrow(bCanThrowBefore);
        
        // Return the result
        return result;
    }
    
    void Context::setup() {
        // Get the global object
        var _global = global();
        
        _global["exit"] = Function(_exit, "exit");
        _global["load"] = Function(_load, "load");
        _global["print"] = Function(_print, "print");
        _global["require"] = Function(_require, "require");
        
        _global["global"] = _global;
    }
}
