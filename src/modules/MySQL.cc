
#include <my_global.h>
#include <mysql.h>

#include "MySQL.h"
#include "../shell.h"

V8FuncDef(global_MySQL_Connection_Connection)
{
    if(args.Length() >= 4)
    {
        args.This()->SetPointerInInternalField(0, mysql_init(NULL));
        mysql_real_connect(((MYSQL*)args.This()->GetPointerFromInternalField(0)), (*v8::String::Utf8Value(args[0])), (*v8::String::Utf8Value(args[1])), (*v8::String::Utf8Value(args[2])), (*v8::String::Utf8Value(args[3])), 0, NULL, 0);
        return v8::Undefined();
    }
    V8Throw("Invalid call to MySQL.Connection");
}

V8FuncDef(global_MySQL_Connection_query)
{
    if(args.Length() >= 1)
    {
        v8::Handle<v8::Object> pQuery = v8::Array::New();
        
        if(mysql_query(((MYSQL*)args.This()->GetPointerFromInternalField(0)), (*v8::String::Utf8Value(args[0]))))
            V8Throw(mysql_error(((MYSQL*)args.This()->GetPointerFromInternalField(0))));
        MYSQL_RES *pResult = mysql_store_result(((MYSQL*)args.This()->GetPointerFromInternalField(0)));
        MYSQL_ROW pRow;
        
        while((pRow = mysql_fetch_row(pResult))) {
            v8::Handle<v8::Object> pHash = v8::Object::New();
            mysql_field_seek(pResult, 0);
            for(size_t i = 0; i < mysql_num_fields(pResult); i++) {
                MYSQL_FIELD *pColumn = mysql_fetch_field(pResult);
                const char *pValue = pRow[i] ? pRow[i] : "";
                switch(pColumn->type) {
                    case MYSQL_TYPE_DECIMAL:
                    case MYSQL_TYPE_TINY:
                    case MYSQL_TYPE_SHORT:
                    case MYSQL_TYPE_LONG:
                    case MYSQL_TYPE_FLOAT:
                    case MYSQL_TYPE_DOUBLE:
                    case MYSQL_TYPE_LONGLONG:
                        pHash->V8Set(pColumn->name, v8::Local<v8::Number>::Cast(v8::String::New(pValue)));
                        break;
                    default:
                        pHash->V8Set(pColumn->name, v8::String::New(pValue));
                }
            }
            V8FuncCall(pQuery, pQuery->V8Get("push"), pHash);
        }
        mysql_free_result(pResult);
        return pQuery;
    }
    V8Throw("Invalid call to MySQL.Connection.prototype.query");
}


void SetupMySQL(v8::Handle<v8::Object> global)
{
    v8::Handle<v8::Object> global_MySQL = v8::Object::New();
    global->V8Set("MySQL", global_MySQL);
    v8::Handle<v8::FunctionTemplate> global_MySQL_Connection = V8Func(global_MySQL_Connection_Connection);
    global_MySQL_Connection->SetClassName(v8::String::New("Connection"));
    global_MySQL_Connection->InstanceTemplate()->SetInternalFieldCount(1);
    v8::Handle<v8::Function> global_MySQL_Connection_query = V8Func(global_MySQL_Connection_query)->GetFunction();
    global_MySQL_Connection_query->SetName(v8::String::New("query"));
    global_MySQL_Connection->PrototypeTemplate()->V8Set("query", global_MySQL_Connection_query);
    global_MySQL->V8Set("Connection", global_MySQL_Connection->GetFunction());
}