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
#include "Network.h"

using namespace Gearbox;

/** \file Network.cc converted from Network.gear */

#line 1 "src/modules/Network.gear"
#ifdef _WIN32
#include <winsock.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

static v8::Handle<v8::Value> _Network_Socket_Socket(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 38 "src/modules/Network.gear"
        Value family(args[0]), type(args[1]);
        int sock = socket(family, type, 0);
        if(sock == -1)
            return Throw(Error("Unable to create socket"));
        
        This["socket"] = Internal(sock);
        This["family"] = Internal(family);
        This["type"] = Internal(type);
        return undefined;
    }
    return Throw(Error("Invalid call to Network.Socket"));
}

static v8::Handle<v8::Value> _Network_Socket_connect(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 48 "src/modules/Network.gear"
        Value host(args[0]), port(args[1]);
        struct hostent *host_s = gethostbyname(host.to<String>());
        if(!host_s)
            return Throw(Error("Unable to resolve host"));
            
        struct sockaddr_in server_addr;
        server_addr.sin_family = This["family"].to<uint32_t>();
        server_addr.sin_port = htons(port.to<uint32_t>());
        server_addr.sin_addr = *((struct in_addr *)host_s->h_addr);
        
        int result = connect(This["socket"], (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
        if(result == -1)
            return Throw(Error("Unable to connect"));
        
        This["isConnected"] = Internal(true);
        return undefined;
    }
    return Throw(Error("Invalid call to Network.Socket.prototype.connect"));
}

static v8::Handle<v8::Value> _Network_Socket_receive(const v8::Arguments& args) {
    Value This(args.This());
    #line 66 "src/modules/Network.gear"
    int maxLen = Value(args[0]) == undefined ? 1024 : Value(args[0]).to<int>();
    char *buffer = new char [maxLen];
    int len = recv(This["socket"], buffer, maxLen, 0);
    if(len > 0) {
        String str(buffer, len);
        delete [] buffer;
        return str;
    }
    delete [] buffer;
    return undefined;
}

static v8::Handle<v8::Value> _Network_Socket_send(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 77 "src/modules/Network.gear"
        Value data(args[0]);
        send(This["socket"], data.to<String>(), data.length(), 0);
        return undefined;
    }
    return Throw(Error("Invalid call to Network.Socket.prototype.send"));
}

static v8::Handle<v8::Value> _Network_Socket_close(const v8::Arguments& args) {
    Value This(args.This());
    #line 82 "src/modules/Network.gear"
    #ifdef _WIN32
    closesocket(This["socket"]);
#else
    close(This["socket"]);
#endif
    return undefined;
}

static v8::Handle<v8::Value> _Network_Socket_block(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 89 "src/modules/Network.gear"
        Value blocking(args[0]);
        #ifdef _WIN32
        u_long mode = blocking ? 1 : 0;
        ioctlsocket(This["socket"], FIONBIO, &mode);
#else
        int mode = fcntl(This["socket"], F_GETFL, 0);
        if(blocking)
            mode &= ~O_NONBLOCK;
        else
            mode |= O_NONBLOCK;
        fcntl(This["socket"], F_SETFL, mode);
#endif
        return undefined;
    }
    return Throw(Error("Invalid call to Network.Socket.prototype.block"));
}

static v8::Handle<v8::Value> _Network_toString(const v8::Arguments& args) {
    #line 32 "src/modules/Network.gear"
    return String("[module Network]");
}


#line 140 "src/modules/Network.cc"
static void _setup_Network(Value _exports) {
    v8::Handle<v8::FunctionTemplate> _Network_Socket = v8::FunctionTemplate::New(_Network_Socket_Socket);
    _Network_Socket->SetClassName(String("Socket"));
    _Network_Socket->PrototypeTemplate()->Set("connect", Function(_Network_Socket_connect, "connect"));
    _Network_Socket->PrototypeTemplate()->Set("receive", Function(_Network_Socket_receive, "receive"));
    _Network_Socket->PrototypeTemplate()->Set("send", Function(_Network_Socket_send, "send"));
    _Network_Socket->PrototypeTemplate()->Set("close", Function(_Network_Socket_close, "close"));
    _Network_Socket->PrototypeTemplate()->Set("block", Function(_Network_Socket_block, "block"));
    _Network_Socket->PrototypeTemplate()->Set("socket", Value(-1));
    _Network_Socket->PrototypeTemplate()->Set("family", Value(-1));
    _Network_Socket->PrototypeTemplate()->Set("type", Value(-1));
    _Network_Socket->PrototypeTemplate()->Set("isConnected", Value(false));
    _exports["Socket"] = _Network_Socket->GetFunction();
    _exports["Socket"]["INET"] = Value(AF_INET);
    _exports["Socket"]["UNIX"] = Value(AF_UNIX);
    _exports["Socket"]["TCP"] = Value(SOCK_STREAM);
    _exports["Socket"]["UDP"] = Value(SOCK_DGRAM);
    _exports["toString"] = Function(_Network_toString, "toString");
}
static Module _module_Network("Network", _setup_Network);