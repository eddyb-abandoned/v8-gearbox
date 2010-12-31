
#include "../Gearbox.h"
#include "MySQL.h"
using namespace Gearbox;

/** \file MySQL.cc */

#line 1 "src/modules/MySQL.gear"
#include <my_global.h>
#include <mysql.h>

v8::Handle<v8::Value> __global_MySQL_Connection_Connection(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 4)
    {
        #line 10 "src/modules/MySQL.gear"
        Value host(args[0]), user(args[1]), password(args[2]), db(args[3]);
        MYSQL *pMYSQL = mysql_init(NULL);
        mysql_real_connect(pMYSQL, host.to<String>(), user.to<String>(), password.to<String>(), db.to<String>(), 0, NULL, 0);
        This["pMYSQL"] = pMYSQL;
        return undefined;
    }
    return Error("Invalid call to MySQL.Connection");
}

v8::Handle<v8::Value> __global_MySQL_Connection_query(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1)
    {
        #line 16 "src/modules/MySQL.gear"
        Value query(args[0]);
        MYSQL *pMYSQL = This["pMYSQL"];
        
        if(mysql_query(pMYSQL, query.to<String>()))
            return Error(mysql_error(pMYSQL));
        
        var resultArray = Array();
        MYSQL_RES *pResult = mysql_store_result(pMYSQL);
        MYSQL_ROW pRow;
        
        while((pRow = mysql_fetch_row(pResult))) {
            var rowHash = Object();
            mysql_field_seek(pResult, 0);
            for(size_t i = 0; i < mysql_num_fields(pResult); i++) {
                MYSQL_FIELD *pColumn = mysql_fetch_field(pResult);
                var value = pRow[i] ? pRow[i] : "";
                switch(pColumn->type) {
                    case MYSQL_TYPE_DECIMAL:
                    case MYSQL_TYPE_TINY:
                    case MYSQL_TYPE_SHORT:
                    case MYSQL_TYPE_LONG:
                    case MYSQL_TYPE_LONGLONG:
                        // Force an Integer value
                        rowHash[pColumn->name] = value.to<int64_t>();
                        break;
                    case MYSQL_TYPE_FLOAT:
                    case MYSQL_TYPE_DOUBLE:
                        // Force a Number value
                        rowHash[pColumn->name] = value.to<double>();
                        break;
                    default:
                        rowHash[pColumn->name] = value;
                }
            }
            resultArray[resultArray.length()] = rowHash;
        }
        mysql_free_result(pResult);
        return resultArray;
    }
    return Error("Invalid call to MySQL.Connection.prototype.query");
}

v8::Handle<v8::Value> __global_MySQL_toString(const v8::Arguments& args) {
    #line 7 "src/modules/MySQL.gear"
    return String("[object MySQL]");
}


#line 79 "src/modules/MySQL.cc"
void SetupMySQL(v8::Handle<v8::Object> global) {
    v8::Handle<v8::Object> global_MySQL = v8::Object::New();
    global->Set(String("MySQL"), global_MySQL);
    v8::Handle<v8::FunctionTemplate> global_MySQL_Connection = v8::FunctionTemplate::New(__global_MySQL_Connection_Connection);
    global_MySQL_Connection->SetClassName(String("Connection"));
    global_MySQL_Connection->PrototypeTemplate()->Set("query", Function(__global_MySQL_Connection_query, "query"));
    global_MySQL_Connection->PrototypeTemplate()->Set("pMYSQL", Value(0));
    global_MySQL->Set(String("Connection"), global_MySQL_Connection->GetFunction());
    global_MySQL->Set(String("toString"), Function(__global_MySQL_toString, "toString"));
}