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

#include <stdlib.h>
#include <v8.h>

#include "global.h"
#include "shell.h"
#include "Gearbox.h"

using namespace Gearbox;

#ifndef GEARBOX_APACHE_MOD
    #include <readline/readline.h>
    #include <readline/history.h>
    #include <stdio.h>
    void RunShell(v8::Handle<v8::Context> context);
    #define _STR_NEWLINE "\n"
    #define _STR_SPACE " "
#else
    #include "apache2/mod_gearbox.h"
    extern ApacheRequestRec *g_pRequest;
    #define printf if(g_pRequest)g_pRequest->rprintf
    #define _STR_NEWLINE "<br>"
    #define _STR_SPACE "&nbsp;"
#endif

#ifndef GEARBOX_APACHE_MOD

int RunMain(int argc, char* argv[]) {
    v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
    v8::HandleScope handle_scope;
    
    // Create and enter the context
    v8::Handle<v8::Context> context = v8::Context::New();
    v8::Context::Scope context_scope(context);
    var global = context->Global();

    // Setup global context
    SetupGlobal(v8::Handle<v8::Object>::Cast(global.operator v8::Handle<v8::Value>()));
    
    // Set the arguments array
    var arguments = Array();//v8::Array::New(argc - i);
    global["arguments"] = arguments;

    bool run_shell = (argc == 1);
    for (int i = 1; i < argc; i++) {
        char* str = argv[i];
        if(strcmp(str, "--shell") == 0)
            run_shell = true;
        else if(strcmp(str, "-f") == 0)
            continue;
        else if((!strcmp(str, "-e") || !strcmp(str, "--eval")) && i <= argc) {
            // Execute argument given to -e / --eval option directly
            v8::HandleScope handle_scope;
            if(ExecuteString(argv[++i], "unnamed") == null)
                return 1;
        } 
        else if(!strncmp(str, "--", 2))
            printf("Warning: unknown flag %s." _STR_NEWLINE "Try --help for options" _STR_NEWLINE, str);
        else {
            // Use all other arguments as names of files to load and run.
            v8::HandleScope handle_scope;
            String file_name = str;
            String source = ReadFile(str);
            if(source.empty())
            {
                printf("Error reading '%s'" _STR_NEWLINE , str);
                return 1;
            }
            
            // Set the rest of the arguments into the array
            for(int j = i; j < argc; j++)
                arguments[j - i] = argv[j];
            
            if(ExecuteString(source, file_name) == null)
                return 1;
            return 0; // Execute only one file
        }
    }
    if(run_shell)
        RunShell(context);
    return 0;
}

#define GEARBOX_HISTORY_FILE "/.gearbox_history"

void RunShell(v8::Handle<v8::Context> context) {
    String history_file = String::concat(getenv("HOME"), GEARBOX_HISTORY_FILE);
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
            
            var result = ExecuteString(str, "(shell)");
            if(result != null)
                printf("%s" _STR_NEWLINE, *result.to<String>());
        }
        delete str;
    }
    printf(_STR_NEWLINE);
}


int main(int argc, char* argv[]) {
    int result = RunMain(argc, argv);
    v8::V8::Dispose();
    return result;
}

#else

#define FixHttpVar(x) ExecuteString(\
x "=(function(d){var e={};d.split('&').forEach(function(a){if((a=a.split('='))[0]){var b=decodeURIComponent(a.shift().replace(/\\+/g, '%20')).replace(/\\[\\]$/,'');var c=a.join('=');if(c!=undefined)c=decodeURIComponent(c.replace(/\\+/g, '%20'));if(b in e){if(!Array.isArray(e[b]))e[b]=[e[b]];e[b].push(c)}else e[b]=c}});return e})(" x ");"\
, "Fix " x)

bool RunScript(const char *sScript) {
    v8::HandleScope handle_scope;
    
    // Create and enter the context
    v8::Handle<v8::Context> context = v8::Context::New();
    v8::Context::Scope context_scope(context);
    var global = context->Global();
    
    // Setup global context
    SetupGlobal(global);
    
    // Empty arguments array
    global["arguments"] = Array();
    
    if(g_pRequest && g_pRequest->args()) {
        global["GET"] = g_pRequest->args();
        FixHttpVar("GET");
    }
    else
        global["GET"] = Object();
    
    if(g_pRequest && g_pRequest->method_number() == M_POST) {
        ap_setup_client_block(g_pRequest->get_request_rec(), REQUEST_CHUNKED_ERROR);
        ap_should_client_block(g_pRequest->get_request_rec());
        size_t nPostBytes = g_pRequest->remaining();
        char *pPostData = new char[nPostBytes];
        g_pRequest->get_client_block(pPostData, nPostBytes);
        global["POST"] = String(pPostData, nPostBytes));
        delete pPostData;
        FixHttpVar("POST");
    }
    else
        global["POST"] = Object();
    
    // Use all other arguments as names of files to load and run.
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


// Reads a file into a v8 string.
String Gearbox::ReadFile(String name) {
    FILE* file = fopen(name, "rb");
    if (file == NULL)
        return String();

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);

    char* chars = new char[size + 1];
    for(int i = 0; i < size;)
        i += fread(&chars[i], 1, size - i, file);
    chars[size] = '\0';
    fclose(file);
    
    String result(chars, size);
    delete [] chars;
    return result;
}

// Executes a string within the current v8 context.
Value Gearbox::ExecuteString(String source, String name)
{
    v8::HandleScope handle_scope;
    v8::TryCatch try_catch;
    v8::Handle<v8::Script> script = v8::Script::Compile(source, name);
    if(script.IsEmpty())
    {
        // Print errors that happened during compilation.
        ReportException(&try_catch);
        return null;
    }
    else
    {
        v8::Handle<v8::Value> result = script->Run();
        if(result.IsEmpty())
        {
            // Print errors that happened during execution.
            ReportException(&try_catch);
            return null;
        }
        else
            return result;
    }
}


void Gearbox::ReportException(v8::TryCatch* try_catch)
{
    v8::HandleScope handle_scope;
    String exception = Value(try_catch->Exception());
    v8::Handle<v8::Message> message = try_catch->Message();
    if (message.IsEmpty())
    {
        // V8 didn't provide any extra information about this error; just
        // print the exception.
        printf("%s" _STR_NEWLINE, *exception);
    }
    else
    {
        // Print (filename):(line number): (message).
        String filename = Value(message->GetScriptResourceName());
        printf("%s:%i: %s" _STR_NEWLINE, *filename, message->GetLineNumber(), *exception);
        // Print line of source code.
        String sourceline = Value(message->GetSourceLine());
        printf("%s" _STR_NEWLINE, *sourceline);
        
        // Print wavy underline (GetUnderline is deprecated).
        int start = message->GetStartColumn();
        int end = message->GetEndColumn();
        for (int i = 0; i < start; i++)
            printf(_STR_SPACE);
        for (int i = start; i < end; i++)
            printf("^");
        printf(_STR_NEWLINE);
    }
}
