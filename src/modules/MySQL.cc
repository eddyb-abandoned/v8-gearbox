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
#include "MySQL.h"

using namespace Gearbox;

/** \file MySQL.cc converted from MySQL.gear */

#line 1 "src/modules/MySQL.gear"
#ifdef _WIN32
    #define HAVE_RINT
    #define __CYGWIN__
#endif
#include <my_global.h>
#include <mysql.h>

static v8::Handle<v8::Value> _MySQL_Connection_Connection(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 4) {
        #line 32 "src/modules/MySQL.gear"
        Value host(args[0]), user(args[1]), password(args[2]), db(args[3]);
        MYSQL *pMYSQL = mysql_init(NULL);
        mysql_real_connect(pMYSQL, host.to<String>(), user.to<String>(), password.to<String>(), db.to<String>(), 0, NULL, 0);
        This["pMYSQL"] = pMYSQL;
        return undefined;
    }
    THROW_ERROR("Invalid call to MySQL.Connection");
}

static v8::Handle<v8::Value> _MySQL_Connection_query(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 38 "src/modules/MySQL.gear"
        Value query(args[0]);
        MYSQL *pMYSQL = This["pMYSQL"];
        
        if(mysql_query(pMYSQL, query.to<String>()))
            return Throw(Error(mysql_error(pMYSQL)));
        
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
    THROW_ERROR("Invalid call to MySQL.Connection.prototype.query");
}

static v8::Handle<v8::Value> _MySQL_toString(const v8::Arguments &args) {
    #line 29 "src/modules/MySQL.gear"
    return String("[module MySQL]");
}


#line 97 "src/modules/MySQL.cc"
static void _setup_MySQL(Value _exports) {
    v8::Handle<v8::FunctionTemplate> _MySQL_Connection = v8::FunctionTemplate::New(_MySQL_Connection_Connection);
    _MySQL_Connection->SetClassName(String("Connection"));
    _MySQL_Connection->PrototypeTemplate()->Set("query", Function(_MySQL_Connection_query, "query"));
    _MySQL_Connection->PrototypeTemplate()->Set("pMYSQL", Value(0));
    _exports["Connection"] = _MySQL_Connection->GetFunction();
    _exports["toString"] = Function(_MySQL_toString, "toString");
}
static Module _module_MySQL("MySQL", _setup_MySQL);