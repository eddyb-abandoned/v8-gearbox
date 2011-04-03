
#include <v8-gearbox.h>

#include <cstdlib>

#include "modules/Io.h"
#include "modules/MySQL.h"
#include "modules/Network.h"
#include "modules/Ncurses.h"
#include "modules/SDL.h"
#include "modules/GL.h"

namespace Gearbox {
    Context *Context::m_pCurrentContext = 0;
    
    v8::Handle<v8::Value> _print(const v8::Arguments& args) {
        if(args.Length()) {
            for(int i = 0; i < args.Length(); i++)
                printf(i ? " %s" : "%s", *Value(args[i]).to<String>());
            printf(_STR_NEWLINE);
            return undefined;
        }
        return Throw(Error("Invalid call to print"));
    }
    
    v8::Handle<v8::Value> _load(const v8::Arguments& args) {
        if(args.Length() >= 1) {
            String file = Value(args[0]);
            if(file.empty())
                return Throw(Error("Error loading file"));
            
            String source = ReadFile(file);
            if(source.empty())
                return Throw(Error("Error loading file"));
            
            Context *pCurrentContext = Context::getCurrent();
            if(!pCurrentContext)
                return Throw(Error("No Context is in use"));
            
            return pCurrentContext->runScript(source, file);
        }
        return Throw(Error("Invalid call to load"));
    }
    
    v8::Handle<v8::Value> _exit(const v8::Arguments& args) {
        std::_Exit(Value(args[0]));
        return undefined;
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
        
        _global["print"] = Function(_print, "print");
        _global["load"] = Function(_load, "load");
        _global["exit"] = Function(_exit, "exit");
        
        _global["global"] = _global;
        
        // Setup GL functions
        SetupGL(_global);
        // Setup Io functions
        SetupIo(_global);
        // Setup MySQL functions
        SetupMySQL(_global);
        // Setup Network functions
        SetupNetwork(_global);
        // Setup Ncurses functions
        SetupNcurses(_global);
        // Setup SDL functions
        SetupSDL(_global);
    }
}
