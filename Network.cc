#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#include <v8.h>

#include "Network.h"
#include "shell.h"

V8FuncDef(global_Network_Connection)
{
    V8AssertArgs(2);
    struct hostent *host_s = gethostbyname(*v8::String::Utf8Value(args[0]));
    V8Assert(host_s, "Unrecoverable name resolution failure");
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(args[1]->Uint32Value());
    server_addr.sin_addr = *((struct in_addr *)host_s->h_addr);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, NULL)|O_NONBLOCK);
    args.This()->SetInternalField(0, v8::Integer::New(sock));
    return v8::Undefined();
}

V8FuncDef(global_Network_Connection_receive)
{
    V8AssertArgs(0);
    char buffer[1024];
    int len = recv(args.This()->GetInternalField(0)->IntegerValue(), buffer, 1024, 0);
    if(len > 0)
        return V8Str(buffer, len);
    return v8::Undefined();
}

V8FuncDef(global_Network_Connection_send)
{
    V8AssertArgs(1);
    send(args.This()->GetInternalField(0)->IntegerValue(), *v8::String::Utf8Value(args[0]), v8::String::Utf8Value(args[0]).length(), 0);
    return v8::Undefined();
}

void SetupNetwork(v8::Handle<v8::Object> global)
{
    v8::Handle<v8::Object> global_Network = v8::Object::New();
    global->V8Set("Network", global_Network);
    v8::Handle<v8::FunctionTemplate> global_Network_Connection = V8Func(global_Network_Connection);
    global_Network_Connection->InstanceTemplate()->SetInternalFieldCount(1);
    global_Network_Connection->PrototypeTemplate()->V8Set("receive", V8Func(global_Network_Connection_receive));
    global_Network_Connection->PrototypeTemplate()->V8Set("send", V8Func(global_Network_Connection_send));
    global_Network->V8Set("Connection", global_Network_Connection->GetFunction());
}