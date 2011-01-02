#include "Gearbox.h"

namespace Gearbox {
    
    //void ValueList::push(Value that) {
    //    push(that.operator v8::Handle<v8::Value>());
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
        //INSIDE(Value::from(v8::Handle<v8::Value>));
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
    
    Value::operator v8::Handle<v8::Value>() {
        //INSIDE(Value::operator v8::Handle<v8::Value>);
        if(m_hValue.IsEmpty())
            return m_pValue;
        else
            return m_hValue;//::Local<v8::Value>::New(m_hValue);
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
        if(that->IsExternal() || that->ToObject()->HasIndexedPropertiesInExternalArrayData())
            printf("TODO: need to delete user-related stuff on disposal\n");
        that.Dispose();
    }
}