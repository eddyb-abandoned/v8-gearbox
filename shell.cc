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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/types.h>

#include "Ncurses.h"
#include "Network.h"
#include "shell.h"

#define V8GlobFuncList(f) \
f(print);\
f(read);\
f(load);\
f(quit);\
f(version);\

void RunShell(v8::Handle<v8::Context> context);
bool ExecuteString(v8::Handle<v8::String> source,
                   v8::Handle<v8::Value> name,
                   bool print_result,
                   bool report_exceptions);

// Native global functions declaration
V8GlobFuncList(V8FuncDef);
v8::Handle<v8::String> ReadFile(const char* name);
void ReportException(v8::TryCatch* handler);

int RunMain(int argc, char* argv[])
{
    v8::V8::SetFlagsFromCommandLine(&argc, argv, true);
    v8::HandleScope handle_scope;

    // Create a template for the global object.
    v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();

    // Native global functions install
    V8GlobFuncList(global->V8FuncSet);

    // Create and enter the context
    v8::Handle<v8::Context> context = v8::Context::New(NULL, global);
    v8::Context::Scope context_scope(context);

    // Setup Network functions
    SetupNetwork(context->Global());
    // Setup Ncurses functions
    SetupNcurses(context->Global());

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
            if (!ExecuteString(source, file_name, false, true))
                return 1;
        }
    }
    if(run_shell)
        RunShell(context);
    return 0;
}


int main(int argc, char* argv[])
{
    int result = RunMain(argc, argv);
    v8::V8::Dispose();
    return result;
}


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
    bool first = true;
    for (int i = 0; i < args.Length(); i++)
    {
        v8::HandleScope handle_scope;
        if (first)
            first = false;
        else
            printf(" ");
        v8::String::Utf8Value str(args[i]);
        const char* cstr = ToCString(str);
        printf("%s", cstr);
        //syslog(LOG_NOTICE, "%s", cstr);
    }
    printf("\n");
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


// The read-eval-execute loop of the shell.
void RunShell(v8::Handle<v8::Context> context)
{
    printf("V8 version %s\n", v8::V8::GetVersion());
    static const int kBufferSize = 256;
    while (true)
    {
        char buffer[kBufferSize];
        printf("> ");
        char* str = fgets(buffer, kBufferSize, stdin);
        if (str == NULL)
            break;
        v8::HandleScope handle_scope;
        ExecuteString(V8Str(str),
                       V8Str("(shell)"),
                       true,
                       true);
    }
    printf("\n");
}


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
        printf("%s\n", exception_string);
    }
    else
    {
        // Print (filename):(line number): (message).
        v8::String::Utf8Value filename(message->GetScriptResourceName());
        const char* filename_string = ToCString(filename);
        int linenum = message->GetLineNumber();
        printf("%s:%i: %s\n", filename_string, linenum, exception_string);
        // Print line of source code.
        v8::String::Utf8Value sourceline(message->GetSourceLine());
        const char* sourceline_string = ToCString(sourceline);
        printf("%s\n", sourceline_string);
        // Print wavy underline (GetUnderline is deprecated).
        int start = message->GetStartColumn();
        for (int i = 0; i < start; i++)
            printf(" ");
        int end = message->GetEndColumn();
        for (int i = start; i < end; i++)
            printf("^");
        printf("\n");
    }
}
