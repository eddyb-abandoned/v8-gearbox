#ifndef V8_GEARBOX_H
#define V8_GEARBOX_H

#include <v8.h>
#include "shell.h"

namespace Gearbox {
    
    /** Structure that wraps a type as a template parameter */
    template <class T>
    class Type {};
    
}

#include "String.h"
#include "Value.h"
#include "TryCatch.h"
#include "Context.h"

namespace Gearbox {
    
    static void PrintTrace() { 
        v8::Message::PrintCurrentStackTrace(stdout);
    }

    static Primitive Integer(int64_t val) {
        return Primitive(Primitive::Integer, val);
    }
    
    static Primitive Number(double val) {
        return Primitive(Primitive::Number, val);
    }
    
    class Object : public Value {
        public:
            Object() : Value(v8::Object::New()) {}
            static bool is(Value &that) {
                return that.to<v8::Handle<v8::Value>>()->IsObject();
            }
    };
    
    class Array : public Value {
        public:
            Array(int length=0) : Value(v8::Array::New(length)) {}
            static bool is(Value &that) {
                return that.to<v8::Handle<v8::Value>>()->IsArray();
            }
    };
    
    static Value Function(v8::InvocationCallback _function, String name) {
        v8::Handle<v8::Function> function = v8::FunctionTemplate::New(_function)->GetFunction();
        function->SetName(name);
        return function;
    }
    
    static Value Error(String message) {
        return v8::Exception::Error(message);
    }
    
    template <class T>
    static Value Internal(T that) {
        return Value(that, Value::Internal);
    }
    
    String ReadFile(String name);
    
    typedef Value var;
    
}

#endif
