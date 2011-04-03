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

namespace Gearbox {
    
    //void ValueList::push(Value that) {
    //    push(that.to<v8::Handle<v8::Value>>());
    //}
    
    Value::~Value() {
        if(!m_hValue.IsEmpty())
            m_hValue.MakeWeak(0, weakCallback);
    }
    
    void Value::from(const Value &that) {
        if(that.m_hValue.IsEmpty())
            from(that.m_pValue);
        else
            from(that.m_hValue);
    }
    
    void Value::from(v8::Handle<v8::Value> that) {
        if(that.IsEmpty())
            from(undefined);
        else if(that->IsNumber() || that->IsUint32() || that->IsInt32() ||  that->IsUndefined() || that->IsNull() || that->IsBoolean()) {
            m_hValue.Clear();
            m_pValue.from(that);
        }
        else
            m_hValue = v8::Persistent<v8::Value>::New(that);
    }
    
    void Value::from(Primitive that) {
        m_hValue.Clear();
        m_pValue = that;
    }
    
    String Value::to(Type<String>) {
        v8::String::Utf8Value stringValue(m_hValue.IsEmpty() ? m_pValue.operator v8::Handle<v8::Value>() : m_hValue);
        return String(*stringValue /*? *stringValue : "<string conversion failed>"*/);
    }
    
    int64_t Value::to(Type<int64_t>) {
        if(m_hValue.IsEmpty()) {
            if(m_pValue.m_Kind == Primitive::Number)return m_pValue.m_dValue;
            else if(m_pValue.m_Kind == Primitive::True)return 1;
            else if(m_pValue.m_Kind == Primitive::Integer)return m_pValue.m_iValue;
            return 0;
        }
        return m_hValue->IntegerValue();
    }
    
    double Value::to(Type<double>) {
        if(m_hValue.IsEmpty()) {
            if(m_pValue.m_Kind == Primitive::Number)return m_pValue.m_dValue;
            else if(m_pValue.m_Kind == Primitive::True)return 1;
            else if(m_pValue.m_Kind == Primitive::Integer)return m_pValue.m_iValue;
            return 0;
        }
        return m_hValue->NumberValue();
    }
    
    bool Value::to(Type<bool>) {
        if(m_hValue.IsEmpty()) {
            if(m_pValue.m_Kind == Primitive::Number)return m_pValue.m_dValue;
            else if(m_pValue.m_Kind == Primitive::True)return true;
            else if(m_pValue.m_Kind == Primitive::Integer)return m_pValue.m_iValue;
            return false;
        }
        return m_hValue->BooleanValue();
    }
    
    v8::Handle<v8::Value> Value::to(Type<v8::Handle<v8::Value>>){
        if(m_hValue.IsEmpty())
            return m_pValue;
        else
            return m_hValue;
    }
    
    int Value::length() {
        if(m_hValue.IsEmpty())
            return 0;
        if(m_hValue->IsArray())
            return v8::Handle<v8::Array>::Cast(m_hValue)->Length();
        if(m_hValue->IsString())
            return m_hValue->ToString()->Length();
        return 0;
    }
    
    void Value::weakCallback(v8::Persistent<v8::Value> that, void*) {
        //if(that->IsExternal() || that->ToObject()->HasIndexedPropertiesInExternalArrayData())
         //   printf("TODO: need to delete user-related stuff on disposal\n");
        that.Dispose();
    }
}