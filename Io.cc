
#include <string>
#include <fstream>
#include <streambuf>

#include "Io.h"
#include "shell.h"

V8FuncDef(global_Io_readFileContents)
{
    if(args.Length() >= 1)
    {
        std::ifstream file((*v8::String::Utf8Value(args[0])));
        std::string str;
        
        file.seekg(0, std::ios::end);   
        str.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        
        str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        return v8::String::New(str.c_str());
        return v8::Undefined();
    }
    V8Assert(false, "Invalid call to global.Io.readFileContents")
}

V8FuncDef(global_Io_writeFileContents)
{
    if(args.Length() >= 2)
    {
        std::ofstream file((*v8::String::Utf8Value(args[0])));
        file.write((*v8::String::Utf8Value(args[1])), v8::String::Utf8Value(args[1]).length());
        return v8::Undefined();
    }
    V8Assert(false, "Invalid call to global.Io.writeFileContents")
}


void SetupIo(v8::Handle<v8::Object> global)
{
    v8::Handle<v8::Object> global_Io = v8::Object::New();
    global->V8Set("Io", global_Io);
    global_Io->V8Set("readFileContents", V8Func(global_Io_readFileContents)->GetFunction());
    global_Io->V8Set("writeFileContents", V8Func(global_Io_writeFileContents)->GetFunction());
}
