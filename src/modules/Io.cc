/*
 * Copyright (c) 2011 Eduard Burtescu
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITRTLSS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, RTLGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONRTLCTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <v8-gearbox.h>
#include "Io.h"

using namespace Gearbox;

/** \file Io.cc converted from Io.gear */

#line 1 "src/modules/Io.gear"
#include <fstream>

using namespace Modules;

static var CError(String prefix = "") {
    return Error(prefix + strerror(errno));
}
#define THROW_CERROR(...) THROW(CError(__VA_ARGS__))

var Io::read(String filePath) {
    std::ifstream file(filePath, std::ifstream::in | std::ifstream::binary);
    if(!file.good())
        THROW_CERROR(filePath + ": ");
    
    file.seekg(0, std::ios::end);
    size_t length = file.tellg();
    file.seekg(0, std::ios::beg);
    
    char *pBuffer = new char [length];
    
    file.read(pBuffer, length);
    String contents(pBuffer, length);
    
    delete [] pBuffer;
    return contents;
}

var Io::write(String filePath, String contents) {
    std::ofstream file(filePath);
    if(!file.good())
        THROW_CERROR(filePath + ": ");
    
    file.write(contents, contents.length());
    return undefined;
}

#define _THIS_FSTREAM (This["fstream"].to<std::fstream*>())

#define _FSTREAM_READ(x, i, dw) do {x i;_THIS_FSTREAM->read(reinterpret_cast<char*>(&i), sizeof(x));dw;} while(0)
//printf("Error while reading an %s: %s\n", #x, strerror(errno));

static v8::Handle<v8::Value> _Io_Stream_Stream(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 81 "src/modules/Io.gear"
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
        if(!_THIS_FSTREAM->good())
            THROW_CERROR(path.to<String>() + ": ");
        return undefined;
    }

    if(args.Length() >= 1) {
        #line 77 "src/modules/Io.gear"
        Value path(args[0]);
        This["fstream"] = new std::fstream(path.to<String>());
        return undefined;
    }
    THROW_ERROR("Invalid call to Io.Stream");
}

static v8::Handle<v8::Value> _Io_Stream_tellg(const v8::Arguments &args) {
    Value This(args.This());
    #line 113 "src/modules/Io.gear"
    return Integer(_THIS_FSTREAM->tellg());
}

static v8::Handle<v8::Value> _Io_Stream_seekg(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 120 "src/modules/Io.gear"
        Value off(args[0]), dir(args[1]);
        _THIS_FSTREAM->seekg(off, static_cast<std::ios_base::seekdir>(dir.to<int>()));
        return undefined;
    }

    if(args.Length() >= 1) {
        #line 116 "src/modules/Io.gear"
        Value pos(args[0]);
        _THIS_FSTREAM->seekg(pos.to<int>());
        return undefined;
    }
    THROW_ERROR("Invalid call to Io.Stream.prototype.seekg");
}

static v8::Handle<v8::Value> _Io_Stream_readBinary(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 124 "src/modules/Io.gear"
        Value _f(args[0]);
        String f = _f;
        
        #define _HANDLE_FMT(fmt, type, retType)if(f == #fmt)_FSTREAM_READ(type, x, return retType(x))
        #define _HANDLE_INT_FMT(n) _HANDLE_FMT(u##n, uint##n##_t, Integer);_HANDLE_FMT(i##n, int##n##_t, Integer);_HANDLE_FMT(s##n, int##n##_t, Integer)
        
        _HANDLE_INT_FMT(8);
        _HANDLE_INT_FMT(16);
        _HANDLE_INT_FMT(32);
        _HANDLE_INT_FMT(64);
        
        _HANDLE_FMT(f, float, Number);
        _HANDLE_FMT(f32, float, Number);
        _HANDLE_FMT(d, double, Number);
        _HANDLE_FMT(f64, double, Number);
        _HANDLE_FMT(ld, long double, Number);
        _HANDLE_FMT(f128, long double, Number);
        
        #undef _HANDLE_INT_FMT
        #undef _HANDLE_FMT
        return undefined;
    }
    THROW_ERROR("Invalid call to Io.Stream.prototype.readBinary");
}

static v8::Handle<v8::Value> _Io_Stream_tellp(const v8::Arguments &args) {
    Value This(args.This());
    #line 147 "src/modules/Io.gear"
    return Integer(_THIS_FSTREAM->tellp());
}

static v8::Handle<v8::Value> _Io_Stream_seekp(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 154 "src/modules/Io.gear"
        Value off(args[0]), dir(args[1]);
        _THIS_FSTREAM->seekp(off, static_cast<std::ios_base::seekdir>(dir.to<int>()));
        return undefined;
    }

    if(args.Length() >= 1) {
        #line 150 "src/modules/Io.gear"
        Value pos(args[0]);
        _THIS_FSTREAM->seekp(pos.to<int>());
        return undefined;
    }
    THROW_ERROR("Invalid call to Io.Stream.prototype.seekp");
}

static v8::Handle<v8::Value> _Io_Stream_close(const v8::Arguments &args) {
    Value This(args.This());
    #line 159 "src/modules/Io.gear"
    _THIS_FSTREAM->close();
    return undefined;
}

static v8::Handle<v8::Value> _Io_read(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 166 "src/modules/Io.gear"
        Value filePath(args[0]);
        return Io::read(filePath);
    }
    THROW_ERROR("Invalid call to Io.read");
}

static v8::Handle<v8::Value> _Io_write(const v8::Arguments &args) {
    if(args.Length() >= 2) {
        #line 170 "src/modules/Io.gear"
        Value filePath(args[0]), contents(args[1]);
        return Io::write(filePath, contents);
    }
    THROW_ERROR("Invalid call to Io.write");
}

static v8::Handle<v8::Value> _Io_toString(const v8::Arguments &args) {
    #line 74 "src/modules/Io.gear"
    return String("[module Io]");
}


#line 219 "src/modules/Io.cc"
static void _setup_Io(Value _exports) {
    v8::Handle<v8::FunctionTemplate> _Io_Stream = v8::FunctionTemplate::New(_Io_Stream_Stream);
    _Io_Stream->SetClassName(String("Stream"));
    _Io_Stream->PrototypeTemplate()->Set("tellg", Function(_Io_Stream_tellg, "tellg"));
    _Io_Stream->PrototypeTemplate()->Set("seekg", Function(_Io_Stream_seekg, "seekg"));
    _Io_Stream->PrototypeTemplate()->Set("readBinary", Function(_Io_Stream_readBinary, "readBinary"));
    _Io_Stream->PrototypeTemplate()->Set("tellp", Function(_Io_Stream_tellp, "tellp"));
    _Io_Stream->PrototypeTemplate()->Set("seekp", Function(_Io_Stream_seekp, "seekp"));
    _Io_Stream->PrototypeTemplate()->Set("close", Function(_Io_Stream_close, "close"));
    _Io_Stream->PrototypeTemplate()->Set("fstream", Value(0));
    _exports["Stream"] = _Io_Stream->GetFunction();
    _exports["read"] = Function(_Io_read, "read");
    _exports["write"] = Function(_Io_write, "write");
    _exports["toString"] = Function(_Io_toString, "toString");
    _exports["SeekBeg"] = Value(std::ios_base::beg);
    _exports["SeekEnd"] = Value(std::ios_base::end);
}
static Module _module_Io("Io", _setup_Io);