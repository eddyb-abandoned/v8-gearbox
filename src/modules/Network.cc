
#include "Network.h"
#include "../shell.h"

#ifdef WINDOWS
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include <fcntl.h>
#include <v8.h>

V8FuncDef(global_Network_TcpConnection_TcpConnection)
{
    if(args.Length() >= 2)
    {
        struct hostent *host_s = gethostbyname((*v8::String::Utf8Value(args[0])));
        if(!host_s)
            V8Throw("Unrecoverable name resolution failure");
            
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons((uint32_t)args[1]->IntegerValue());
        server_addr.sin_addr = *((struct in_addr *)host_s->h_addr);
        
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
        fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, NULL) | O_NONBLOCK);
        
        args.This()->SetInternalField(0, v8::Integer::New(sock));
        return v8::Undefined();
    }
    V8Throw("Invalid call to Network.TcpConnection");
}

V8FuncDef(global_Network_TcpConnection_receive)
{
    char buffer[1024];
    int len = recv(args.This()->GetInternalField(0)->IntegerValue(), buffer, 1024, 0);
    if(len > 0)
        return v8::String::New(buffer, len);
    return v8::Undefined();
}

V8FuncDef(global_Network_TcpConnection_send)
{
    if(args.Length() >= 1)
    {
        send(args.This()->GetInternalField(0)->IntegerValue(), (*v8::String::Utf8Value(args[0])), v8::String::Utf8Value(args[0]).length(), 0);
        return v8::Undefined();
    }
    V8Throw("Invalid call to Network.TcpConnection.prototype.send");
}


void SetupNetwork(v8::Handle<v8::Object> global)
{
    v8::Handle<v8::Object> global_Network = v8::Object::New();
    global->V8Set("Network", global_Network);
    v8::Handle<v8::FunctionTemplate> global_Network_TcpConnection = V8Func(global_Network_TcpConnection_TcpConnection);
    global_Network_TcpConnection->SetClassName(v8::String::New("TcpConnection"));
    global_Network_TcpConnection->InstanceTemplate()->SetInternalFieldCount(1);
    v8::Handle<v8::Function> global_Network_TcpConnection_receive = V8Func(global_Network_TcpConnection_receive)->GetFunction();
    global_Network_TcpConnection_receive->SetName(v8::String::New("receive"));
    global_Network_TcpConnection->PrototypeTemplate()->V8Set("receive", global_Network_TcpConnection_receive);
    v8::Handle<v8::Function> global_Network_TcpConnection_send = V8Func(global_Network_TcpConnection_send)->GetFunction();
    global_Network_TcpConnection_send->SetName(v8::String::New("send"));
    global_Network_TcpConnection->PrototypeTemplate()->V8Set("send", global_Network_TcpConnection_send);
    global_Network->V8Set("TcpConnection", global_Network_TcpConnection->GetFunction());
}