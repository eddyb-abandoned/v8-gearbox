// Copyright 2009 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <v8-gearbox.h>
#include "global.h"
#include <fstream>

using namespace Gearbox;

#ifndef GEARBOX_APACHE_MOD

#include <readline/readline.h>
#include <readline/history.h>

#define GEARBOX_HISTORY_FILE "/.gearbox_history"

void RunShell(v8::Handle<v8::Context> context) {
    TryCatch tryCatch;
    
    String history_file = String::concat(std::getenv("HOME"), GEARBOX_HISTORY_FILE);
    read_history(history_file);
    
    printf("v8-gearbox [v8 version %s]" _STR_NEWLINE, v8::V8::GetVersion());
    
    while(true) {
        char* str = readline("gearbox> ");
        if(!str)
            continue;
        if(*str) {
            HIST_ENTRY *lastEntry = history_get(history_length);
            if(!lastEntry || strcmp(str, lastEntry->line)) {
                add_history(str);
                append_history(1, history_file);
            }
            
            // Execute the expression
            var result = ExecuteString(str, "(shell)");
            // Check for exceptions
            if(tryCatch.hasCaught())
                tryCatch.reportException();
            else
                printf("%s" _STR_NEWLINE, *result.to<String>());
        }
        delete str;
    }
    printf(_STR_NEWLINE);
}


int main(int argc, char* argv[]) {
    v8::HandleScope handleScope;
    
    v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
    
    // Create and enter the context
    v8::Handle<v8::Context> context = v8::Context::New();
    v8::Context::Scope contextScope(context);
    var global = context->Global();
    
    // Setup global context
    SetupGlobal(global.to<v8::Handle<v8::Object>>());
    
    // Set the arguments array
    var arguments = Array();
    global["arguments"] = arguments;
    
    TryCatch tryCatch;
    
    bool runShell = (argc == 1);
    for(int i = 1; i < argc; i++) {
        String arg = argv[i];
        if(arg == "-s" || arg == "--shell")
            runShell = true;
        else if((arg == "-e" || arg ==  "--eval") && i <= argc) {
            ExecuteString(argv[++i], "unnamed");
            if(tryCatch.hasCaught())
                return 1;
        } 
        else if(arg.compare("--", 2))
            printf("Warning: unknown flag %s." _STR_NEWLINE "Try --help for options" _STR_NEWLINE, *arg);
        else {
            String source = ReadFile(*arg);
            if(source.empty()) {
                printf("Error reading '%s'" _STR_NEWLINE , *arg);
                return 1;
            }
            
            // Set the rest of the arguments into the array
            for(int j = i; j < argc; j++)
                arguments[j - i] = argv[j];
            
            ExecuteString(source, arg);
            return tryCatch.hasCaught() ? 1 : 0;
        }
    }
    if(runShell)
        RunShell(context);
    return 0;
}

#else

const char *sFixHttpVar = "function(d){var e={};d.split('&').forEach(function(a){if((a=a.split('='))[0]){var b=decodeURIComponent(a.shift().replace(/\\+/g, '%20')).replace(/\\[\\]$/,'');var c=a.join('=');if(c!=undefined)c=decodeURIComponent(c.replace(/\\+/g, '%20'));if(b in e){if(!Array.isArray(e[b]))e[b]=[e[b]];e[b].push(c)}else e[b]=c}});return e}";

bool RunScript(const char *sScript) {
    v8::HandleScope handleScope;
    
    // Create and enter the context
    v8::Handle<v8::Context> context = v8::Context::New();
    v8::Context::Scope context_scope(context);
    var global = context->Global();
    
    // Setup global context
    SetupGlobal(global);
    
    // Empty arguments array
    global["arguments"] = Array();
    
    // HTTP VAR fix function
    var fixHttpVar = ExecuteString(sFixHttpVar, "");
    
    if(g_pRequest && g_pRequest->args())
        global["GET"] = fixHttpVar(g_pRequest->args());
    else
        global["GET"] = Object();
    
    if(g_pRequest && g_pRequest->method_number() == M_POST) {
        ap_setup_client_block(g_pRequest->get_request_rec(), REQUEST_CHUNKED_ERROR);
        ap_should_client_block(g_pRequest->get_request_rec());
        size_t nPostBytes = g_pRequest->remaining();
        char *pPostData = new char[nPostBytes];
        g_pRequest->get_client_block(pPostData, nPostBytes);
        global["POST"] = fixHttpVar(String(pPostData, nPostBytes));
        delete pPostData;
    }
    else
        global["POST"] = Object();
    
    // Try to read the file
    String source = ReadFile(sScript);
    if(source.empty()) {
        v8::V8::Dispose();
        return false;
    }
    ExecuteString(source, sScript);
    v8::V8::Dispose();
    return true;
}

#endif

// Reads a file into a String.
String Gearbox::ReadFile(String path) {
    std::ifstream file(path, std::ifstream::in | std::ifstream::binary);
    if(!file.good())
        return String();
    
    file.seekg(0, std::ios::end);
    size_t length = file.tellg();
    file.seekg(0, std::ios::beg);
    
    char *pBuffer = new char [length];
    
    file.read(pBuffer, length);
    String contents(pBuffer, length);
    
    delete [] pBuffer;
    return contents;
}

// Executes a string within the current v8 context.
Value Gearbox::ExecuteString(String source, String name) {
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
