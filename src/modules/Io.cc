
#include "../Gearbox.h"
#include "Io.h"

using namespace Gearbox;

/** \file Io.cc converted from Io.gear */

#line 1 "src/modules/Io.gear"
#include <fstream>

#define _THIS_FSTREAM (This["fstream"].to<std::fstream*>())

#define _FSTREAM_READ(x, i, dw) do {x i;_THIS_FSTREAM->read(reinterpret_cast<char*>(&i), sizeof(x));dw;} while(0)
//printf("Error while reading an %s: %s\n", #x, strerror(errno));

v8::Handle<v8::Value> __global_Io_Stream_Stream(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 2)
    {
        #line 18 "src/modules/Io.gear"
        Value path(args[0]), mode(args[1]);
        std::ios_base::openmode openMode = static_cast<std::ios_base::openmode>(0);
        if(mode.is<Object>()) {
            if(mode["in"])
                openMode |= std::fstream::in;
            if(mode["out"])
                openMode |= std::fstream::out;
            if(mode["binary"])
                openMode |= std::fstream::binary;
            if(mode["append"])
                openMode |= std::fstream::app;
        } else {
            String modeString = mode;
            char *modeChar = modeString;
            while(*modeChar) {
                if(*modeChar == 'r')
                    openMode |= std::fstream::in;
                if(*modeChar == 'w')
                    openMode |= std::fstream::out;
                if(*modeChar == 'b')
                    openMode |= std::fstream::binary;
                if(*modeChar == '+')
                    openMode |= std::fstream::app;
                modeChar++;
            }
        }
        This["fstream"] = new std::fstream(path.to<String>(), openMode);
        //printf("Error while openning %s: %s %i\n", *(path.to<String>()), strerror(errno), openMode);
        return undefined;
    }

    if(args.Length() >= 1)
    {
        #line 14 "src/modules/Io.gear"
        Value path(args[0]);
        This["fstream"] = new std::fstream(path.to<String>());
        return undefined;
    }
    return Throw(Error("Invalid call to Io.Stream"));
}

v8::Handle<v8::Value> __global_Io_Stream_tellg(const v8::Arguments& args) {
    Value This(args.This());
    #line 49 "src/modules/Io.gear"
    return Integer(_THIS_FSTREAM->tellg());
}

v8::Handle<v8::Value> __global_Io_Stream_seekg(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 2)
    {
        #line 57 "src/modules/Io.gear"
        Value off(args[0]), dir(args[1]);
        _THIS_FSTREAM->seekg(off, static_cast<std::ios_base::seekdir>(dir.to<int>()));
        return undefined;
    }

    if(args.Length() >= 1)
    {
        #line 52 "src/modules/Io.gear"
        Value pos(args[0]);
        _THIS_FSTREAM->seekg(pos.to<int>());
        //printf("Error while seeking to %i: %s\n", pos.to<int>(), strerror(errno));
        return undefined;
    }
    return Throw(Error("Invalid call to Io.Stream.prototype.seekg"));
}

v8::Handle<v8::Value> __global_Io_Stream_readInt(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1)
    {
        #line 61 "src/modules/Io.gear"
        Value len(args[0]);
        if(len == 1)
            _FSTREAM_READ(uint8_t, i, return Integer(i));
        if(len == 2)
            _FSTREAM_READ(uint16_t, i, return Integer(i));
        if(len == 4)
            _FSTREAM_READ(uint32_t, i, return Integer(i));
        if(len == 8)
            _FSTREAM_READ(uint64_t, i, return Integer(i));
        return undefined;
    }
    return Throw(Error("Invalid call to Io.Stream.prototype.readInt"));
}

v8::Handle<v8::Value> __global_Io_Stream_readFloat(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1)
    {
        #line 76 "src/modules/Io.gear"
        Value prec(args[0]);
        if(prec == 1)
            _FSTREAM_READ(float, f, return Number(f));
        if(prec == 2)
            _FSTREAM_READ(double, f, return Number(f));
        if(prec == 4)
            _FSTREAM_READ(long double, f, return Number(f));
        return undefined;
    }

    #line 73 "src/modules/Io.gear"
    _FSTREAM_READ(float, f, return Number(f));
    return undefined;
}

v8::Handle<v8::Value> __global_Io_Stream_tellp(const v8::Arguments& args) {
    Value This(args.This());
    #line 86 "src/modules/Io.gear"
    return Integer(_THIS_FSTREAM->tellp());
}

v8::Handle<v8::Value> __global_Io_Stream_seekp(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 2)
    {
        #line 93 "src/modules/Io.gear"
        Value off(args[0]), dir(args[1]);
        _THIS_FSTREAM->seekp(off, static_cast<std::ios_base::seekdir>(dir.to<int>()));
        return undefined;
    }

    if(args.Length() >= 1)
    {
        #line 89 "src/modules/Io.gear"
        Value pos(args[0]);
        _THIS_FSTREAM->seekp(pos.to<int>());
        return undefined;
    }
    return Throw(Error("Invalid call to Io.Stream.prototype.seekp"));
}

v8::Handle<v8::Value> __global_Io_Stream_close(const v8::Arguments& args) {
    Value This(args.This());
    #line 98 "src/modules/Io.gear"
    _THIS_FSTREAM->close();
    return undefined;
}

v8::Handle<v8::Value> __global_Io_read(const v8::Arguments& args) {
    if(args.Length() >= 1)
    {
        #line 106 "src/modules/Io.gear"
        Value path(args[0]);
        std::ifstream file(path.to<String>(), std::ifstream::in | std::ifstream::binary);
        if(!file.good())
            return undefined;
        
        file.seekg(0, std::ios::end);
        size_t length = file.tellg();
        file.seekg(0, std::ios::beg);
        
        char *pBuffer = new char [length];
        
        file.read(pBuffer, length);
        String contents(pBuffer, length);
        
        delete [] pBuffer;
        return contents;
    }
    return Throw(Error("Invalid call to Io.read"));
}

v8::Handle<v8::Value> __global_Io_write(const v8::Arguments& args) {
    if(args.Length() >= 2)
    {
        #line 124 "src/modules/Io.gear"
        Value path(args[0]), contents(args[1]);
        std::ofstream file(path.to<String>());
        file.write(contents.to<String>(), contents.length());
        return undefined;
    }
    return Throw(Error("Invalid call to Io.write"));
}

v8::Handle<v8::Value> __global_Io_toString(const v8::Arguments& args) {
    #line 11 "src/modules/Io.gear"
    return String("[object Io]");
}


#line 204 "src/modules/Io.cc"
void SetupIo(v8::Handle<v8::Object> global) {
    v8::Handle<v8::Object> global_Io = v8::Object::New();
    global->Set(String("Io"), global_Io);
    v8::Handle<v8::FunctionTemplate> global_Io_Stream = v8::FunctionTemplate::New(__global_Io_Stream_Stream);
    global_Io_Stream->SetClassName(String("Stream"));
    global_Io_Stream->PrototypeTemplate()->Set("tellg", Function(__global_Io_Stream_tellg, "tellg"));
    global_Io_Stream->PrototypeTemplate()->Set("seekg", Function(__global_Io_Stream_seekg, "seekg"));
    global_Io_Stream->PrototypeTemplate()->Set("readInt", Function(__global_Io_Stream_readInt, "readInt"));
    global_Io_Stream->PrototypeTemplate()->Set("readFloat", Function(__global_Io_Stream_readFloat, "readFloat"));
    global_Io_Stream->PrototypeTemplate()->Set("tellp", Function(__global_Io_Stream_tellp, "tellp"));
    global_Io_Stream->PrototypeTemplate()->Set("seekp", Function(__global_Io_Stream_seekp, "seekp"));
    global_Io_Stream->PrototypeTemplate()->Set("close", Function(__global_Io_Stream_close, "close"));
    global_Io_Stream->PrototypeTemplate()->Set("fstream", Value(0));
    global_Io->Set(String("Stream"), global_Io_Stream->GetFunction());
    global_Io->Set(String("read"), Function(__global_Io_read, "read"));
    global_Io->Set(String("write"), Function(__global_Io_write, "write"));
    global_Io->Set(String("toString"), Function(__global_Io_toString, "toString"));
    global_Io->Set(String("SEEK_BEG"), Value(std::ios_base::beg));
    global_Io->Set(String("SEEK_CUR"), Value(std::ios_base::cur));
    global_Io->Set(String("SEEK_END"), Value(std::ios_base::end));
}