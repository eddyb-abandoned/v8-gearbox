
#include "../Gearbox.h"
#include "Io.h"
using namespace Gearbox;

/** \file Io.cc */

#line 1 "src/modules/Io.gear"
#include <string>
#include <fstream>
#include <streambuf>
#include <stdio.h>

using namespace std;

#define fread_num(x, fp, dw) do {x i;fread(&i, sizeof(x), 1, fp);dw;} while(0)

v8::Handle<v8::Value> __global_Io_File_File(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 2)
    {
        #line 20 "src/modules/Io.gear"
        Value path(args[0]), mode(args[1]);
        This["f"] = fopen(path.to<String>(), mode.to<String>());
        return undefined;
    }

    if(args.Length() >= 1)
    {
        #line 16 "src/modules/Io.gear"
        Value path(args[0]);
        This["f"] = fopen(path.to<String>(), "rw+");
        return undefined;
    }
    return Error("Invalid call to Io.File");
}

v8::Handle<v8::Value> __global_Io_File_seekAbs(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1)
    {
        #line 24 "src/modules/Io.gear"
        Value pos(args[0]);
        fseek(This["f"], pos.to<int>(), SEEK_SET);
        return undefined;
    }
    return Error("Invalid call to Io.File.prototype.seekAbs");
}

v8::Handle<v8::Value> __global_Io_File_seekRel(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1)
    {
        #line 28 "src/modules/Io.gear"
        Value pos(args[0]);
        fseek(This["f"], pos.to<int>(), SEEK_CUR);
        return undefined;
    }
    return Error("Invalid call to Io.File.prototype.seekRel");
}

v8::Handle<v8::Value> __global_Io_File_tell(const v8::Arguments& args) {
    Value This(args.This());
    #line 33 "src/modules/Io.gear"
    return Integer(ftell(This["f"]));
}

v8::Handle<v8::Value> __global_Io_File_readInt(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1)
    {
        #line 36 "src/modules/Io.gear"
        Value len(args[0]);
        if(len == 1)
            fread_num(uint8_t, This["f"], return Integer(i));
        if(len == 2)
            fread_num(uint16_t, This["f"], return Integer(i));
        if(len == 4)
            fread_num(uint32_t, This["f"], return Integer(i));
        if(len == 8)
            fread_num(uint64_t, This["f"], return Integer(i));
        return undefined;
    }
    return Error("Invalid call to Io.File.prototype.readInt");
}

v8::Handle<v8::Value> __global_Io_File_readFloat(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1)
    {
        #line 51 "src/modules/Io.gear"
        Value prec(args[0]);
        if(prec == 1)
            fread_num(float, This["f"], return Number(i));
        if(prec == 2)
            fread_num(double, This["f"], return Number(i));
        if(prec == 4)
            fread_num(long double, This["f"], return Number(i));
        return undefined;
    }

    #line 48 "src/modules/Io.gear"
    fread_num(float, This["f"], return Number(i));
    return undefined;
}

v8::Handle<v8::Value> __global_Io_readFileContents(const v8::Arguments& args) {
    if(args.Length() >= 1)
    {
        #line 61 "src/modules/Io.gear"
        Value path(args[0]);
        ifstream file(path.to<String>());
        string str;
        
        file.seekg(0, std::ios::end);   
        str.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        
        str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        return String(str.c_str());
    }
    return Error("Invalid call to Io.readFileContents");
}

v8::Handle<v8::Value> __global_Io_readFileContentsBinary(const v8::Arguments& args) {
    if(args.Length() >= 1)
    {
        #line 74 "src/modules/Io.gear"
        Value path(args[0]);
        ifstream file(path.to<String>());
        
        file.seekg(0, std::ios::end);   
        streamsize len = file.tellg();
        file.seekg(0, std::ios::beg);
        
        char *ptr = new char [len];
        file.read(ptr, len);
        
        v8::Handle<v8::Object> buffer = v8::Object::New();
        buffer->SetIndexedPropertiesToExternalArrayData(ptr, v8::kExternalUnsignedByteArray, len);
        
        return buffer;
    }
    return Error("Invalid call to Io.readFileContentsBinary");
}

v8::Handle<v8::Value> __global_Io_writeFileContents(const v8::Arguments& args) {
    if(args.Length() >= 2)
    {
        #line 90 "src/modules/Io.gear"
        Value path(args[0]), contents(args[1]);
        ofstream file(path.to<String>());
        file.write(contents.to<String>(), contents.length());
        return undefined;
    }
    return Error("Invalid call to Io.writeFileContents");
}

v8::Handle<v8::Value> __global_Io_toString(const v8::Arguments& args) {
    #line 13 "src/modules/Io.gear"
    return String("[object Io]");
}


#line 166 "src/modules/Io.cc"
void SetupIo(v8::Handle<v8::Object> global) {
    v8::Handle<v8::Object> global_Io = v8::Object::New();
    global->Set(String("Io"), global_Io);
    v8::Handle<v8::FunctionTemplate> global_Io_File = v8::FunctionTemplate::New(__global_Io_File_File);
    global_Io_File->SetClassName(String("File"));
    global_Io_File->PrototypeTemplate()->Set("seekAbs", Function(__global_Io_File_seekAbs, "seekAbs"));
    global_Io_File->PrototypeTemplate()->Set("seekRel", Function(__global_Io_File_seekRel, "seekRel"));
    global_Io_File->PrototypeTemplate()->Set("tell", Function(__global_Io_File_tell, "tell"));
    global_Io_File->PrototypeTemplate()->Set("readInt", Function(__global_Io_File_readInt, "readInt"));
    global_Io_File->PrototypeTemplate()->Set("readFloat", Function(__global_Io_File_readFloat, "readFloat"));
    global_Io_File->PrototypeTemplate()->Set("f", Value(0));
    global_Io->Set(String("File"), global_Io_File->GetFunction());
    global_Io->Set(String("readFileContents"), Function(__global_Io_readFileContents, "readFileContents"));
    global_Io->Set(String("readFileContentsBinary"), Function(__global_Io_readFileContentsBinary, "readFileContentsBinary"));
    global_Io->Set(String("writeFileContents"), Function(__global_Io_writeFileContents, "writeFileContents"));
    global_Io->Set(String("toString"), Function(__global_Io_toString, "toString"));
}