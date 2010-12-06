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

#include <v8.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/types.h>
#include <string>

using namespace std;

#include "modules/Io.h"
#include "modules/MySQL.h"
#include "modules/Network.h"
#include "modules/Ncurses.h"
#include "modules/SDL.h"
#include "modules/GL.h"
#include "shell.h"

#define V8GlobFuncList(f) \
f(print);\
f(read);\
f(load);\
f(quit);\
f(version);\

#ifndef APACHE_MODULE_GB_
void RunShell(v8::Handle<v8::Context> context);
#else
#include "apache2/mod_gearbox.h"
extern ApacheRequestRec *g_pRequest;
#define printf if(g_pRequest)g_pRequest->rprintf
#endif

bool ExecuteString(v8::Handle<v8::String> source,
                   v8::Handle<v8::Value> name,
                   bool print_result,
                   bool report_exceptions);

// Native global functions declaration
V8GlobFuncList(V8FuncDef);
v8::Handle<v8::String> ReadFile(const char* name);
void ReportException(v8::TryCatch* handler);

#ifndef APACHE_MODULE_GB_

int RunMain(int argc, char* argv[])
{
    v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
    v8::HandleScope handle_scope;
    
    // Create and enter the context
    v8::Handle<v8::Context> context = v8::Context::New();
    v8::Context::Scope context_scope(context);
    v8::Handle<v8::Object> global = context->Global();

    // Setup global functions
    V8GlobFuncList(global->V8FuncSet);
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
    // Setup GL functions
    SetupGL(global);

    bool run_shell = (argc == 1);
    for (int i = 1; i < argc; i++)
    {
        const char* str = argv[i];
        if (strcmp(str, "--shell") == 0)
            run_shell = true;
        else if (strcmp(str, "-f") == 0)
            // Ignore any -f flags for compatibility with the other stand-
            // alone JavaScript engines.
            continue;
        else if (strncmp(str, "--", 2) == 0)
            printf("Warning: unknown flag %s.\nTry --help for options\n", str);
        else if (strcmp(str, "-e") == 0 && i + 1 < argc)
        {
            // Execute argument given to -e option directly
            v8::HandleScope handle_scope;
            v8::Handle<v8::String> file_name = V8Str("unnamed");
            v8::Handle<v8::String> source = V8Str(argv[i + 1]);
            if (!ExecuteString(source, file_name, false, true))
                return 1;
            i++;
        } else
        {
            // Use all other arguments as names of files to load and run.
            v8::HandleScope handle_scope;
            v8::Handle<v8::String> file_name = V8Str(str);
            v8::Handle<v8::String> source = ReadFile(str);
            if (source.IsEmpty())
            {
                printf("Error reading '%s'\n", str);
                return 1;
            }
            
            // Setup "arguments"
            v8::Handle<v8::Array> arguments = v8::Array::New(argc - i);
            for(int j = i; j < argc; j++)
                arguments->Set(j - i, v8::String::New(argv[j]));
            global->Set(v8::String::New("arguments"), arguments);
            
            if (!ExecuteString(source, file_name, false, true))
                return 1;
            return 0; // Execute only one file
        }
    }
    if(run_shell)
        RunShell(context);
    return 0;
}


int main(int argc, char* argv[])
{
    v8::V8::Dispose();
    int result = RunMain(argc, argv);
    v8::V8::Dispose();
    return result;
}

#else

#define FixHttpVar(x) ExecuteString(V8Str(\
#x "=(function(d){var e={};d.split('&').forEach(function(a){if((a=a.split('='))[0]){var b=decodeURIComponent(a.shift().replace(/\\+/g, '%20')).replace(/\\[\\]$/,'');var c=a.join('=');if(c!=undefined)c=decodeURIComponent(c.replace(/\\+/g, '%20'));if(b in e){if(!Array.isArray(e[b]))e[b]=[e[b]];e[b].push(c)}else e[b]=c}});return e})(" #x ");"\
)), V8Str("Fix " #x), false, true)

bool RunScript(const char *sScript)
{
    v8::V8::Dispose();
    v8::HandleScope handle_scope;
    
    // Create and enter the context
    v8::Handle<v8::Context> context = v8::Context::New();
    v8::Context::Scope context_scope(context);
    v8::Handle<v8::Object> global = context->Global();
    
    // Setup global functions
    V8GlobFuncList(global->V8FuncSet);
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
    
    if(g_pRequest && g_pRequest->args()) {
        global->V8Set("GET", V8Str(g_pRequest->args()));
        FixHttpVar(GET);
    }
    else
        global->V8Set("GET", v8::Object::New());
    
    if(g_pRequest && g_pRequest->method_number() == M_POST) {
        ap_setup_client_block(g_pRequest->get_request_rec(), REQUEST_CHUNKED_ERROR);
        ap_should_client_block(g_pRequest->get_request_rec());
        size_t nPostBytes = g_pRequest->remaining();
        char *pPostData = new char[nPostBytes];
        g_pRequest->get_client_block(pPostData, nPostBytes);
        global->V8Set("POST", V8Str(pPostData, nPostBytes));
        FixHttpVar(POST);
    }
    else
        global->V8Set("POST", v8::Object::New());
    
    // Use all other arguments as names of files to load and run.
    v8::Handle<v8::String> source = ReadFile(sScript);
    if(source.IsEmpty()) {
        v8::V8::Dispose();
        return false;
    }
    ExecuteString(source, V8Str(sScript), false, true);
    v8::V8::Dispose();
    return true;
}

#endif


// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value)
{
    return *value ? *value : "<string conversion failed>";
}


// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called.  Prints its arguments on stdout separated by
// spaces and ending with a newline.
V8FuncDef(print)
{
    if(!args.Length())
        return v8::Undefined();
    
    printf("%s\n", ToCString(v8::String::Utf8Value(args[0])));
    fflush(stdout);
    
    return v8::Undefined();
}


// The callback that is invoked by v8 whenever the JavaScript 'read'
// function is called.  This function loads the content of the file named in
// the argument into a JavaScript string.
V8FuncDef(read)
{
    V8AssertArgs(1);
    v8::String::Utf8Value file(args[0]);
    if (*file == NULL)
        return v8::ThrowException(V8Str("Error loading file"));
    v8::Handle<v8::String> source = ReadFile(*file);
    if (source.IsEmpty())
        return v8::ThrowException(V8Str("Error loading file"));
    return source;
}


// The callback that is invoked by v8 whenever the JavaScript 'load'
// function is called.  Loads, compiles and executes its argument
// JavaScript file.
V8FuncDef(load)
{
    for (int i = 0; i < args.Length(); i++)
    {
        v8::HandleScope handle_scope;
        v8::String::Utf8Value file(args[i]);
        if (*file == NULL)
            return v8::ThrowException(V8Str("Error loading file"));
        v8::Handle<v8::String> source = ReadFile(*file);
        if (source.IsEmpty())
            return v8::ThrowException(V8Str("Error loading file"));
        if (!ExecuteString(source, V8Str(*file), false, false))
            return v8::ThrowException(V8Str("Error executing file"));
    }
    return v8::Undefined();
}


// The callback that is invoked by v8 whenever the JavaScript 'quit'
// function is called.  Quits.
V8FuncDef(quit)
{
    // If not arguments are given args[0] will yield undefined which
    // converts to the integer value 0.
    exit(args[0]->Int32Value());
    return v8::Undefined();
}

V8FuncDef(version)
{
    return V8Str(v8::V8::GetVersion());
}


// Reads a file into a v8 string.
v8::Handle<v8::String> ReadFile(const char* name)
{
    FILE* file = fopen(name, "rb");
    if (file == NULL)
        return v8::Handle<v8::String>();

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);

    char* chars = new char[size + 1];
    chars[size] = '\0';
    for (int i = 0; i < size;)
    {
        int read = fread(&chars[i], 1, size - i, file);
        i += read;
    }
    fclose(file);
    v8::Handle<v8::String> result = V8Str(chars, size);
    delete[] chars;
    return result;
}

#ifndef APACHE_MODULE_GB_

// The read-eval-execute loop of the shell.
void RunShell(v8::Handle<v8::Context> context)
{
    printf("v8-gearbox [v8 version %s]\n", v8::V8::GetVersion());
    while (true)
    {
        char* str = readline("> ");
        if(!str)
            continue;
        if(*str) {
            add_history(str);
            v8::HandleScope handle_scope;
            ExecuteString(V8Str(str), V8Str("(shell)"), true, true);
        }
        delete str;
    }
    printf("\n");
}

#endif

// Executes a string within the current v8 context.
bool ExecuteString(v8::Handle<v8::String> source,
                   v8::Handle<v8::Value> name,
                   bool print_result,
                   bool report_exceptions)
{
    v8::HandleScope handle_scope;
    v8::TryCatch try_catch;
    v8::Handle<v8::Script> script = v8::Script::Compile(source, name);
    if (script.IsEmpty())
    {
        // Print errors that happened during compilation.
        if (report_exceptions)
            ReportException(&try_catch);
        return false;
    }
    else
    {
        v8::Handle<v8::Value> result = script->Run();
        if (result.IsEmpty())
        {
            // Print errors that happened during execution.
            if (report_exceptions)
                ReportException(&try_catch);
            return false;
        }
        else
        {
            if (print_result && !result->IsUndefined())
            {
                // If all went well and the result wasn't undefined then print
                // the returned value.
                v8::String::Utf8Value str(result);
                const char* cstr = ToCString(str);
                printf("%s\n", cstr);
            }
            return true;
        }
    }
}


void ReportException(v8::TryCatch* try_catch)
{
    v8::HandleScope handle_scope;
    v8::String::Utf8Value exception(try_catch->Exception());
    const char* exception_string = ToCString(exception);
    v8::Handle<v8::Message> message = try_catch->Message();
    if (message.IsEmpty())
    {
        // V8 didn't provide any extra information about this error; just
        // print the exception.
#ifndef APACHE_MODULE_GB_
        printf("%s\n", exception_string);
#else
        printf("%s<br>", exception_string);
#endif
    }
    else
    {
        // Print (filename):(line number): (message).
        v8::String::Utf8Value filename(message->GetScriptResourceName());
        const char* filename_string = ToCString(filename);
        int linenum = message->GetLineNumber();
#ifndef APACHE_MODULE_GB_
        printf("%s:%i: %s\n", filename_string, linenum, exception_string);
#else
        printf("%s:%i: %s<br>", filename_string, linenum, exception_string);
#endif
        // Print line of source code.
        v8::String::Utf8Value sourceline(message->GetSourceLine());
        const char* sourceline_string = ToCString(sourceline);
#ifndef APACHE_MODULE_GB_
        printf("%s\n", sourceline_string);
#else
        printf("%s<br>", sourceline_string);
#endif
        // Print wavy underline (GetUnderline is deprecated).
        int start = message->GetStartColumn();
        for (int i = 0; i < start; i++)
#ifndef APACHE_MODULE_GB_
            printf(" ");
#else
            printf("&nbsp;");
#endif
        int end = message->GetEndColumn();
        for (int i = start; i < end; i++)
            printf("^");
#ifndef APACHE_MODULE_GB_
        printf("\n");
#else
        printf("<br>");
#endif
    }
}
