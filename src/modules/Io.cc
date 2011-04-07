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
    return Throw(Error(prefix + strerror(errno)));
}

var Io::read(String filePath) {
    std::ifstream file(filePath, std::ifstream::in | std::ifstream::binary);
    if(!file.good())
        return CError(filePath + ": ");
    
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
        return CError(filePath + ": ");
    
    file.write(contents, contents.length());
    return undefined;
}

#define _THIS_FSTREAM (This["fstream"].to<std::fstream*>())

#define _FSTREAM_READ(x, i, dw) do {x i;_THIS_FSTREAM->read(reinterpret_cast<char*>(&i), sizeof(x));dw;} while(0)
//printf("Error while reading an %s: %s\n", #x, strerror(errno));

static v8::Handle<v8::Value> _Io_Stream_Stream(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 80 "src/modules/Io.gear"
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

    if(args.Length() >= 1) {
        #line 76 "src/modules/Io.gear"
        Value path(args[0]);
        This["fstream"] = new std::fstream(path.to<String>());
        return undefined;
    }
    return Throw(Error("Invalid call to Io.Stream"));
}

static v8::Handle<v8::Value> _Io_Stream_tellg(const v8::Arguments& args) {
    Value This(args.This());
    #line 111 "src/modules/Io.gear"
    return Integer(_THIS_FSTREAM->tellg());
}

static v8::Handle<v8::Value> _Io_Stream_seekg(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 119 "src/modules/Io.gear"
        Value off(args[0]), dir(args[1]);
        _THIS_FSTREAM->seekg(off, static_cast<std::ios_base::seekdir>(dir.to<int>()));
        return undefined;
    }

    if(args.Length() >= 1) {
        #line 114 "src/modules/Io.gear"
        Value pos(args[0]);
        _THIS_FSTREAM->seekg(pos.to<int>());
        //printf("Error while seeking to %i: %s\n", pos.to<int>(), strerror(errno));
        return undefined;
    }
    return Throw(Error("Invalid call to Io.Stream.prototype.seekg"));
}

static v8::Handle<v8::Value> _Io_Stream_readInt(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 123 "src/modules/Io.gear"
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

static v8::Handle<v8::Value> _Io_Stream_readFloat(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 138 "src/modules/Io.gear"
        Value prec(args[0]);
        if(prec == 1)
            _FSTREAM_READ(float, f, return Number(f));
        if(prec == 2)
            _FSTREAM_READ(double, f, return Number(f));
        if(prec == 4)
            _FSTREAM_READ(long double, f, return Number(f));
        return undefined;
    }

    #line 135 "src/modules/Io.gear"
    _FSTREAM_READ(float, f, return Number(f));
    return undefined;
}

static v8::Handle<v8::Value> _Io_Stream_tellp(const v8::Arguments& args) {
    Value This(args.This());
    #line 148 "src/modules/Io.gear"
    return Integer(_THIS_FSTREAM->tellp());
}

static v8::Handle<v8::Value> _Io_Stream_seekp(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 155 "src/modules/Io.gear"
        Value off(args[0]), dir(args[1]);
        _THIS_FSTREAM->seekp(off, static_cast<std::ios_base::seekdir>(dir.to<int>()));
        return undefined;
    }

    if(args.Length() >= 1) {
        #line 151 "src/modules/Io.gear"
        Value pos(args[0]);
        _THIS_FSTREAM->seekp(pos.to<int>());
        return undefined;
    }
    return Throw(Error("Invalid call to Io.Stream.prototype.seekp"));
}

static v8::Handle<v8::Value> _Io_Stream_close(const v8::Arguments& args) {
    Value This(args.This());
    #line 160 "src/modules/Io.gear"
    _THIS_FSTREAM->close();
    return undefined;
}

static v8::Handle<v8::Value> _Io_read(const v8::Arguments& args) {
    if(args.Length() >= 1) {
        #line 168 "src/modules/Io.gear"
        Value filePath(args[0]);
        return Io::read(filePath);
    }
    return Throw(Error("Invalid call to Io.read"));
}

static v8::Handle<v8::Value> _Io_write(const v8::Arguments& args) {
    if(args.Length() >= 2) {
        #line 172 "src/modules/Io.gear"
        Value filePath(args[0]), contents(args[1]);
        return Io::write(filePath, contents);
    }
    return Throw(Error("Invalid call to Io.write"));
}

static v8::Handle<v8::Value> _Io_toString(const v8::Arguments& args) {
    #line 73 "src/modules/Io.gear"
    return String("[module Io]");
}


#line 226 "src/modules/Io.cc"
static void _setup_Io(Value _exports) {
    v8::Handle<v8::FunctionTemplate> _Io_Stream = v8::FunctionTemplate::New(_Io_Stream_Stream);
    _Io_Stream->SetClassName(String("Stream"));
    _Io_Stream->PrototypeTemplate()->Set("tellg", Function(_Io_Stream_tellg, "tellg"));
    _Io_Stream->PrototypeTemplate()->Set("seekg", Function(_Io_Stream_seekg, "seekg"));
    _Io_Stream->PrototypeTemplate()->Set("readInt", Function(_Io_Stream_readInt, "readInt"));
    _Io_Stream->PrototypeTemplate()->Set("readFloat", Function(_Io_Stream_readFloat, "readFloat"));
    _Io_Stream->PrototypeTemplate()->Set("tellp", Function(_Io_Stream_tellp, "tellp"));
    _Io_Stream->PrototypeTemplate()->Set("seekp", Function(_Io_Stream_seekp, "seekp"));
    _Io_Stream->PrototypeTemplate()->Set("close", Function(_Io_Stream_close, "close"));
    _Io_Stream->PrototypeTemplate()->Set("fstream", Value(0));
    _exports["Stream"] = _Io_Stream->GetFunction();
    _exports["read"] = Function(_Io_read, "read");
    _exports["write"] = Function(_Io_write, "write");
    _exports["toString"] = Function(_Io_toString, "toString");
    _exports["SEEK_BEG"] = Value(std::ios_base::beg);
    _exports["SEEK_CUR"] = Value(std::ios_base::cur);
    _exports["SEEK_END"] = Value(std::ios_base::end);
}
static Module _module_Io("Io", _setup_Io);