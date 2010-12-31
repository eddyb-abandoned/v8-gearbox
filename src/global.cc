
#include "global.h"
#include "Gearbox.h"
using namespace Gearbox;

/** \file global.cc */

#include <stdlib.h>

#include "modules/Io.h"
#include "modules/MySQL.h"
#include "modules/Network.h"
#include "modules/Ncurses.h"
#include "modules/SDL.h"
#include "modules/GL.h"

v8::Handle<v8::Value> __global_print(const v8::Arguments& args) {
    if(args.Length())
    {
        for(int i = 0; i < args.Length(); i++)
            printf("%s\n", *Value(args[i]).to<String>());
        return undefined;
    }
    return Error("Invalid call to print");
}

v8::Handle<v8::Value> __global_load(const v8::Arguments& args) {
    if(args.Length() >= 1)
    {
        v8::HandleScope handle_scope;
        String file = Value(args[0]);
        if(file.empty())
            return Error("Error loading file");
        String source = ReadFile(file);
        if(source.empty())
            return Error("Error loading file");
        return ExecuteString(source, file);
    }
    return Error("Invalid call to load");
}

v8::Handle<v8::Value> __global_quit(const v8::Arguments& args) {
    exit(Value(args[0]));
    return undefined;
}

v8::Handle<v8::Value> __global_version(const v8::Arguments& args) {
    return String(v8::V8::GetVersion());
}

void SetupGlobal(v8::Handle<v8::Object> global) {
#define GEARBOX_SET_FUNCTION(base, func) base->Set(String(#func), Function(__##base##_##func, #func))
    GEARBOX_SET_FUNCTION(global, print);
    GEARBOX_SET_FUNCTION(global, load);
    GEARBOX_SET_FUNCTION(global, quit);
    GEARBOX_SET_FUNCTION(global, version);
#undef GEARBOX_SET_FUNCTION
    
    global->Set(String("global"), global);
    
    // Setup GL functions
    SetupGL(global);
    // Setup Io functions
    SetupIo(global);
    // Setup MySQL functions
    SetupMySQL(global);
    // Setup Network functions
    SetupNetwork(global);
    // Setup Ncurses functions
    SetupNcurses(global);
    // Setup SDL functions
    SetupSDL(global);
}