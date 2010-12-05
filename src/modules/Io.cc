
#include <string>
#include <fstream>
#include <streambuf>
#include <stdio.h>

#include "Io.h"
#include "../shell.h"

using namespace std;

#define fread_num(x, fp, dw) do {x i;fread(&i, sizeof(x), 1, fp);dw;} while(0)

V8FuncDef(global_Io_File_File)
{
    if(args.Length() >= 2)
    {
        args.This()->SetPointerInInternalField(0, fopen((*v8::String::Utf8Value(args[0])), (*v8::String::Utf8Value(args[1]))));
        return v8::Undefined();
    }

    if(args.Length() >= 1)
    {
        args.This()->SetPointerInInternalField(0, fopen((*v8::String::Utf8Value(args[0])), "rw+"));
        return v8::Undefined();
    }
    V8Throw("Invalid call to Io.File");
}

V8FuncDef(global_Io_File_seekAbs)
{
    if(args.Length() >= 1)
    {
        fseek(((FILE*)args.This()->GetPointerFromInternalField(0)), args[0]->IntegerValue(), SEEK_SET);
        return v8::Undefined();
    }
    V8Throw("Invalid call to Io.File.prototype.seekAbs");
}

V8FuncDef(global_Io_File_seekRel)
{
    if(args.Length() >= 1)
    {
        fseek(((FILE*)args.This()->GetPointerFromInternalField(0)), args[0]->IntegerValue(), SEEK_CUR);
        return v8::Undefined();
    }
    V8Throw("Invalid call to Io.File.prototype.seekRel");
}

V8FuncDef(global_Io_File_tell)
{
    return v8::Integer::New(ftell(((FILE*)args.This()->GetPointerFromInternalField(0))));
}

V8FuncDef(global_Io_File_readInt)
{
    if(args.Length() >= 1)
    {
        if(args[0]->IntegerValue() == 1)
            fread_num(uint8_t, ((FILE*)args.This()->GetPointerFromInternalField(0)), return v8::Integer::New(i));
        if(args[0]->IntegerValue() == 2)
            fread_num(uint16_t, ((FILE*)args.This()->GetPointerFromInternalField(0)), return v8::Integer::New(i));
        if(args[0]->IntegerValue() == 4)
            fread_num(uint32_t, ((FILE*)args.This()->GetPointerFromInternalField(0)), return v8::Integer::New(i));
        if(args[0]->IntegerValue() == 8)
            fread_num(uint64_t, ((FILE*)args.This()->GetPointerFromInternalField(0)), return v8::Integer::New(i));
        return v8::Undefined();
    }
    V8Throw("Invalid call to Io.File.prototype.readInt");
}

V8FuncDef(global_Io_File_readFloat)
{
    if(args.Length() >= 1)
    {
        if(args[0]->IntegerValue() == 1)
            fread_num(float, ((FILE*)args.This()->GetPointerFromInternalField(0)), return v8::Number::New(i));
        if(args[0]->IntegerValue() == 2)
            fread_num(double, ((FILE*)args.This()->GetPointerFromInternalField(0)), return v8::Number::New(i));
        if(args[0]->IntegerValue() == 4)
            fread_num(long double, ((FILE*)args.This()->GetPointerFromInternalField(0)), return v8::Number::New(i));
        return v8::Undefined();
    }

    fread_num(float, ((FILE*)args.This()->GetPointerFromInternalField(0)), return v8::Number::New(i));
    return v8::Undefined();
}

V8FuncDef(global_Io_readFileContents)
{
    if(args.Length() >= 1)
    {
        ifstream file((*v8::String::Utf8Value(args[0])));
        string str;
        
        file.seekg(0, std::ios::end);   
        str.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        
        str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        return v8::String::New(str.c_str());
    }
    V8Throw("Invalid call to Io.readFileContents");
}

V8FuncDef(global_Io_readFileContentsBinary)
{
    if(args.Length() >= 1)
    {
        ifstream file((*v8::String::Utf8Value(args[0])));
        
        file.seekg(0, std::ios::end);   
        streamsize len = file.tellg();
        file.seekg(0, std::ios::beg);
        
        char *ptr = new char [len];
        file.read(ptr, len);
        
        v8::Handle<v8::Object> buffer = v8::Object::New();
        buffer->SetIndexedPropertiesToExternalArrayData(ptr, v8::kExternalUnsignedByteArray, len);
        
        return buffer;
    }
    V8Throw("Invalid call to Io.readFileContentsBinary");
}

V8FuncDef(global_Io_writeFileContents)
{
    if(args.Length() >= 2)
    {
        ofstream file((*v8::String::Utf8Value(args[0])));
        file.write((*v8::String::Utf8Value(args[1])), v8::String::Utf8Value(args[1]).length());
        return v8::Undefined();
    }
    V8Throw("Invalid call to Io.writeFileContents");
}


void SetupIo(v8::Handle<v8::Object> global)
{
    v8::Handle<v8::Object> global_Io = v8::Object::New();
    global->V8Set("Io", global_Io);
    v8::Handle<v8::FunctionTemplate> global_Io_File = V8Func(global_Io_File_File);
    global_Io_File->SetClassName(v8::String::New("File"));
    global_Io_File->InstanceTemplate()->SetInternalFieldCount(1);
    v8::Handle<v8::Function> global_Io_File_seekAbs = V8Func(global_Io_File_seekAbs)->GetFunction();
    global_Io_File_seekAbs->SetName(v8::String::New("seekAbs"));
    global_Io_File->PrototypeTemplate()->V8Set("seekAbs", global_Io_File_seekAbs);
    v8::Handle<v8::Function> global_Io_File_seekRel = V8Func(global_Io_File_seekRel)->GetFunction();
    global_Io_File_seekRel->SetName(v8::String::New("seekRel"));
    global_Io_File->PrototypeTemplate()->V8Set("seekRel", global_Io_File_seekRel);
    v8::Handle<v8::Function> global_Io_File_tell = V8Func(global_Io_File_tell)->GetFunction();
    global_Io_File_tell->SetName(v8::String::New("tell"));
    global_Io_File->PrototypeTemplate()->V8Set("tell", global_Io_File_tell);
    v8::Handle<v8::Function> global_Io_File_readInt = V8Func(global_Io_File_readInt)->GetFunction();
    global_Io_File_readInt->SetName(v8::String::New("readInt"));
    global_Io_File->PrototypeTemplate()->V8Set("readInt", global_Io_File_readInt);
    v8::Handle<v8::Function> global_Io_File_readFloat = V8Func(global_Io_File_readFloat)->GetFunction();
    global_Io_File_readFloat->SetName(v8::String::New("readFloat"));
    global_Io_File->PrototypeTemplate()->V8Set("readFloat", global_Io_File_readFloat);
    global_Io->V8Set("File", global_Io_File->GetFunction());
    v8::Handle<v8::Function> global_Io_readFileContents = V8Func(global_Io_readFileContents)->GetFunction();
    global_Io_readFileContents->SetName(v8::String::New("readFileContents"));
    global_Io->V8Set("readFileContents", global_Io_readFileContents);
    v8::Handle<v8::Function> global_Io_readFileContentsBinary = V8Func(global_Io_readFileContentsBinary)->GetFunction();
    global_Io_readFileContentsBinary->SetName(v8::String::New("readFileContentsBinary"));
    global_Io->V8Set("readFileContentsBinary", global_Io_readFileContentsBinary);
    v8::Handle<v8::Function> global_Io_writeFileContents = V8Func(global_Io_writeFileContents)->GetFunction();
    global_Io_writeFileContents->SetName(v8::String::New("writeFileContents"));
    global_Io->V8Set("writeFileContents", global_Io_writeFileContents);
}