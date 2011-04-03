
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

v8::Handle<v8::Value> __global_Network_Socket_Socket(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 2)
    {
        #line 20 "src/modules/Network.gear"
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

v8::Handle<v8::Value> __global_Network_Socket_connect(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 2)
    {
        #line 30 "src/modules/Network.gear"
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

v8::Handle<v8::Value> __global_Network_Socket_receive(const v8::Arguments& args) {
    Value This(args.This());
    #line 48 "src/modules/Network.gear"
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

v8::Handle<v8::Value> __global_Network_Socket_send(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1)
    {
        #line 59 "src/modules/Network.gear"
        Value data(args[0]);
        send(This["socket"], data.to<String>(), data.length(), 0);
        return undefined;
    }
    return Throw(Error("Invalid call to Network.Socket.prototype.send"));
}

v8::Handle<v8::Value> __global_Network_Socket_close(const v8::Arguments& args) {
    Value This(args.This());
    #line 64 "src/modules/Network.gear"
    #ifdef _WIN32
    closesocket(This["socket"]);
#else
    close(This["socket"]);
#endif
    return undefined;
}

v8::Handle<v8::Value> __global_Network_Socket_block(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1)
    {
        #line 71 "src/modules/Network.gear"
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

v8::Handle<v8::Value> __global_Network_toString(const v8::Arguments& args) {
    #line 14 "src/modules/Network.gear"
    return String("[object Network]");
}


#line 129 "src/modules/Network.cc"
void SetupNetwork(v8::Handle<v8::Object> global) {
    v8::Handle<v8::Object> global_Network = v8::Object::New();
    global->Set(String("Network"), global_Network);
    v8::Handle<v8::FunctionTemplate> global_Network_Socket = v8::FunctionTemplate::New(__global_Network_Socket_Socket);
    global_Network_Socket->SetClassName(String("Socket"));
    global_Network_Socket->PrototypeTemplate()->Set("connect", Function(__global_Network_Socket_connect, "connect"));
    global_Network_Socket->PrototypeTemplate()->Set("receive", Function(__global_Network_Socket_receive, "receive"));
    global_Network_Socket->PrototypeTemplate()->Set("send", Function(__global_Network_Socket_send, "send"));
    global_Network_Socket->PrototypeTemplate()->Set("close", Function(__global_Network_Socket_close, "close"));
    global_Network_Socket->PrototypeTemplate()->Set("block", Function(__global_Network_Socket_block, "block"));
    global_Network_Socket->PrototypeTemplate()->Set("socket", Value(-1));
    global_Network_Socket->PrototypeTemplate()->Set("family", Value(-1));
    global_Network_Socket->PrototypeTemplate()->Set("type", Value(-1));
    global_Network_Socket->PrototypeTemplate()->Set("isConnected", Value(false));
    global_Network_Socket->GetFunction()->Set(String("INET"), Value(AF_INET));
    global_Network_Socket->GetFunction()->Set(String("UNIX"), Value(AF_UNIX));
    global_Network_Socket->GetFunction()->Set(String("TCP"), Value(SOCK_STREAM));
    global_Network_Socket->GetFunction()->Set(String("UDP"), Value(SOCK_DGRAM));
    global_Network->Set(String("Socket"), global_Network_Socket->GetFunction());
    global_Network->Set(String("toString"), Function(__global_Network_toString, "toString"));
}