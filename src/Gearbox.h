#ifndef GEARBOX_H
#define GEARBOX_H

#include <stdio.h>
#include <string.h>
#include <v8.h>
#include <stdexcept>

#include "shell.h"

#define INSIDE(x) printf("INSIDE " #x "\n");

namespace Gearbox {
    /** Structure that wraps a type as a template parameter */
    template <class T>
    class Type {};
    
    class Value;
    
    class Primitive {
        public:
            enum Kind {
                Undefined=0,
                Null,
                False,
                True,
                Integer,
                Number
            };
            Primitive(Kind kind=Undefined, int64_t iValue=0) : m_Kind(kind) {
                if(kind == Integer)
                    m_iValue = iValue;
                else if(kind == Number)
                    m_dValue = iValue;
            }
            Primitive(Kind kind, double dValue) : m_Kind(kind) {
                if(kind == Integer)
                    m_iValue = dValue;
                else if(kind == Number)
                    m_dValue = dValue;
            }
            
            operator v8::Handle<v8::Value>() {
                if(m_Kind == Undefined)
                    return v8::Undefined();
                else if(m_Kind == Null)
                    return v8::Null();
                else if(m_Kind == False)
                    return v8::False();
                else if(m_Kind == True)
                    return v8::True();
                else if(m_Kind == Integer) {
                    if(m_iValue >= 0 && m_iValue <= 0xffffffffL)
                        return v8::Integer::NewFromUnsigned(m_iValue);
                    else if(m_iValue >= -0x80000000L && m_iValue <= 0x7fffffffL)
                        return v8::Integer::New(m_iValue);
                    else
                        return v8::Number::New(m_iValue);
                }
                else if(m_Kind == Number)
                    return v8::Number::New(m_dValue);
                return v8::Undefined();
            }
            
            void from(v8::Handle<v8::Value> hValue) {
                if(hValue->IsNumber()) {
                    m_Kind = Number;
                    m_dValue = hValue->NumberValue();
                }
                else if(hValue->IsUint32() || hValue->IsInt32()) {
                    m_Kind = Integer;
                    m_iValue = hValue->IntegerValue();
                }
                else if(hValue->IsNull())
                    m_Kind = Null;
                else if(hValue->IsFalse())
                    m_Kind = False;
                else if(hValue->IsTrue())
                    m_Kind = True;
                else 
                    m_Kind = Undefined;
            }
            
            bool operator ==(Primitive that) {
                if(m_Kind <= Null && that.m_Kind <= Null)
                    return m_Kind == that.m_Kind;
                return false;
            }
            
            bool operator !=(Primitive that) {
                return !operator==(that);
            }
            
        private:
            Kind m_Kind;
            union {
                int64_t m_iValue;
                double m_dValue;
            };
            friend class Value;
    };
    
    static Primitive undefined;
    static Primitive null(Primitive::Null);
    
    class String {
        public:
            String() : m_pString(0), m_iLength(-1), m_bCloneOnUse(false) {}
            String(char *pString, int iLength=-1)
                : m_pString(clone(pString, iLength)), m_iLength(iLength), m_bCloneOnUse(false) {}
            String(const char *pString, int iLength=-1)
                : m_pString(const_cast<char*>(pString)/*clone(const_cast<char*>(pString), iLength)*/), m_iLength(iLength), m_bCloneOnUse(true) {}
            ~String() {
                if(m_pString && !m_bCloneOnUse) {
                    delete m_pString;
                }
            }
            String(const String &that) : m_pString(clone(that.m_pString, that.m_iLength)), m_iLength(that.m_iLength), m_bCloneOnUse(false)  {
            }
            String &operator =(const String &that) {
                if(m_pString && !m_bCloneOnUse)
                    delete m_pString;
                if(that.m_bCloneOnUse) {
                    m_pString = that.m_pString;
                    m_bCloneOnUse = true;
                }
                else {
                    m_pString = clone(that.m_pString, that.m_iLength);
                    m_iLength = that.m_iLength;
                    m_bCloneOnUse = false;
                }
                return *this;
            }
            bool empty() {
                return !m_pString;
            }
            int length() {
                if(empty())
                    return 0;
                else if(m_iLength > -1)
                    return m_iLength;
                else
                    return strlen(m_pString);
            }
            char *operator*() {
                return operator char*();
            }
            String &operator+=(const String &that) {
                return operator=(concat(*this, that));
            }
            operator char*() {
                if(!m_pString)
                    return const_cast<char*>("");
                if(m_bCloneOnUse) {
                    m_pString = clone(m_pString, m_iLength);
                    m_bCloneOnUse = false;
                }
                return m_pString;
            }
            operator v8::Handle<v8::Value>() {
                return operator v8::Handle<v8::String>();
            }
            operator v8::Handle<v8::String>() {
                return v8::String::New(m_pString, m_iLength);
            }
            static String concat(String left, String right);
        private:
            static char *clone(char *pString, int iLength) {
                if(!pString)
                    return 0;
                if(iLength > -1)
                    return strndup(pString, iLength);
                else
                    return strdup(pString);
            }
            
            char *m_pString;
            int m_iLength;
            bool m_bCloneOnUse;
    };
    
    template <class Node, class Index>
    class Assignable : public Node {
        public:
            Assignable(Node parent, Index index)/* : m_Parent(parent), m_Index(index)*/ {
                m_Parent = parent;
                m_Index = index;
                from(m_Parent.get(m_Index));
            }
            template <class T>
            Node &operator=(T _that) {
                Node that(_that);
                m_Parent.set(m_Index, that, that.m_Flags);
                return *this;
            }
            Node &operator=(Node that) {
                m_Parent.set(m_Index, that, that.m_Flags);
                return *this;
            }
        private:
            Node m_Parent;
            Index m_Index;
    };
    
    class ValueList {
        public:
            ValueList() : m_nValues(0) {}
            ~ValueList() {
                if(m_nValues)
                    delete [] m_phValues;
            }
            
            template <class First, class... Last>
            void add(First first, Last... last) {
                push(first);
                add(last...);
            }
            
            void add() {}
            
            v8::Handle<v8::Value> *values() {
                return m_phValues;
            }
            
            size_t numValues() {
                return m_nValues;
            }
            
        private:
            void push(v8::Handle<v8::Value> value) {
                v8::Handle<v8::Value> *phOldValues = m_phValues;
                m_phValues = new v8::Handle<v8::Value> [m_nValues + 1];
                
                for(size_t i = 0; i < m_nValues; i++)
                    m_phValues[i] = phOldValues[i];
                
               m_phValues[m_nValues++] = value;
            }
            //void push(Value);
            
            v8::Handle<v8::Value> *m_phValues;
            size_t m_nValues;
    };
    void ReportException(v8::TryCatch* handler);
    
    /** A class for every kind of JavaScript value (Objects, Arrays, Functions, Numbers, Strings) */
    class Value {
        public:
            /** Various control flags */
            enum Flags {
                Default = 0,
                Internal  = 1
            };
            
            /** Default constructor */
            Value() : m_Flags(Default) {}
            /** Constructors */
            template <class T>
            Value(T that, uint8_t flags=Default) : m_Flags(flags) {
                from(that);
            }
            /** Default destructor */
            virtual ~Value();
            
            /** Copy operators */
            template <class T>
            Value &operator=(T that) {
                if(!m_hValue.IsEmpty()) {
                    m_hValue.MakeWeak(0, weakCallback);
                    m_hValue.Clear();
                }
                from(that);
                return *this;
            }
            
            /** Instantiation tools */
            void from(const Value&);
            void from(v8::Handle<v8::Value>);
            template <class T>
            void from(v8::Handle<T> that) {
                from(v8::Handle<v8::Value>(that));
            }
            void from(String that) {
                from(that.operator v8::Handle<v8::Value>());
            }
            void from(Primitive);
            void from(const char *that) {
                from(v8::String::New(that));
            }
            void from(char *that) {
                from(v8::String::New(that));
            }
            void from(int64_t that) {
                from(Primitive(Primitive::Integer, that));
            }
            void from(int that) {
                from(static_cast<int64_t>(that));
            }
            void from(unsigned int that) {
                from(static_cast<int64_t>(that));
            }
            void from(double that) {
                from(Primitive(Primitive::Number, that));
            }
            void from(bool that) {
                from(Primitive(that ? Primitive::True : Primitive::False));
            }
            void from(void *that) {
                from(v8::External::New(that));
                printf("wrapping an External (%p) now\n", that/*, to<intptr_t>()*/);
                // Avoid exposing an External to JavaScript
                m_Flags |= Internal;
            }
            
            /** Conversion tools, used to get primitive values */
            template <class T>
            T to() {
                return to(Type<T>());
            }
            Primitive to(Type<Primitive>) {
                if(m_hValue.IsEmpty())
                    return m_pValue;
                Primitive value;
                value.from(m_hValue);
                return value;
            }
            String to(Type<String>);
            int64_t to(Type<int64_t>);
            uint32_t to(Type<uint32_t>) {
                return to<int64_t>();
            }
            uint16_t to(Type<uint16_t>) {
                return to<int64_t>();
            }
            int to(Type<int>) {
                return to<int64_t>();
            }
            double to(Type<double>);
            float to(Type<float>) {
                return to<double>();
            }
            bool to(Type<bool>);
            
            template <class T>
            T *to(Type<T*>) {
                if(m_hValue.IsEmpty() || !m_hValue->IsExternal() || !v8::External::Unwrap(m_hValue)) {
                    printf("WARNING: empty/NULL External!\n");
                    //throw std::logic_error("Gearbox::Value: attempting to use NULL External pointer");
                    return 0;
                }
                return reinterpret_cast<T*>(v8::External::Unwrap(m_hValue));
            }
            
            /** Compare operators */
#define DECLARE_OP(OP) \
template <class T> \
bool operator OP(T that) { \
    return to<T>() OP that; \
}
            DECLARE_OP(==)
            DECLARE_OP(!=)
            DECLARE_OP(>)
            DECLARE_OP(<)
            DECLARE_OP(>=)
            DECLARE_OP(<=)
#undef DECLARE_OP
            
            /** Length, for Arrays and Strings */
            int length();
            
            /** Access to Object or Array elements */
            Assignable<Value, uint32_t> operator[](uint32_t idx) {
                //INSIDE(Value::operator[](uint32_t idx));
                return Assignable<Value, uint32_t>(*this, idx);
            }
            Assignable<Value, uint32_t> operator[](int32_t idx) {
                //INSIDE(Value::operator[](int32_t idx));
                return Assignable<Value, uint32_t>(*this, idx);
            }
            Assignable<Value, String> operator[](String idx) {
                return Assignable<Value, String>(*this, idx);
            }
            Value get(uint32_t idx) {
                if(m_hValue.IsEmpty() || !(m_hValue->IsObject()))
                    return undefined;
                return v8::Handle<v8::Object>::Cast(m_hValue)->Get(idx);
            }
            Value get(String idx) {
                if(m_hValue.IsEmpty() || !(m_hValue->IsObject()))
                    return undefined;
                return v8::Handle<v8::Object>::Cast(m_hValue)->Get(idx.operator v8::Handle<v8::Value>());
            }
            void set(uint32_t idx, Value val, uint8_t flags) {
                if(m_hValue.IsEmpty() || !(m_hValue->IsObject()))
                    return;
                if(flags & Internal)
                    throw std::logic_error("Gearbox::Value: attempting to set an indexed element as Internal");
                    //v8::Handle<v8::Object>::Cast(m_hValue)->Set(Value(idx).operator v8::Handle<v8::Value>(), val, v8PropertyInternal);
                else
                    v8::Handle<v8::Object>::Cast(m_hValue)->Set(idx, val);
            }
            void set(String idx, Value &val, uint8_t flags) {
                if(m_hValue.IsEmpty() || !(m_hValue->IsObject()))
                    return;
                if(flags & Internal)
                    v8::Handle<v8::Object>::Cast(m_hValue)->Set(idx.operator v8::Handle<v8::Value>(), val, v8PropertyInternal);
                else
                    v8::Handle<v8::Object>::Cast(m_hValue)->Set(idx.operator v8::Handle<v8::Value>(), val);
            }
            
            /** Convert operator */
            operator v8::Handle<v8::Value>();
            template <class T>
            operator v8::Handle<T>() {
                return operator v8::Handle<v8::Value>();
            }
            template <class T>
            operator T() {
                return to<T>();
            }
            
            /** Call operator for Functions */
            template <class... Args>
            Value operator()(Args... _args) {
                if(m_hValue.IsEmpty() || !m_hValue->IsFunction())
                    return undefined;
                
                v8::TryCatch try_catch;
                v8::HandleScope handle_scope;
                ValueList args;
                args.add(_args...);
                
                v8::Handle<v8::Value> result = v8::Handle<v8::Function>::Cast(m_hValue)->Call(v8::Object::New(), args.numValues(), args.values());
                if(result.IsEmpty()) {
                    ReportException(&try_catch);
                    return null;
                }
                return result;
            }
            
        private:
            
            static void weakCallback(v8::Persistent<v8::Value>, void*);
            static const v8::PropertyAttribute v8PropertyInternal = static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontEnum | v8::DontDelete);
            
            uint8_t m_Flags;
            Primitive m_pValue;
            v8::Persistent<v8::Value> m_hValue;
            
            friend class Assignable<Value, uint32_t>;
            friend class Assignable<Value, String>;
    };
    
    static void PrintTrace() { 
        v8::Message::PrintCurrentStackTrace(stdout);
    }
    static void *DebugChild(uintptr_t father, const char *kid) {
        return Value(v8::Handle<v8::Value>(reinterpret_cast<v8::Value*>(father)));
    }

    static Primitive Integer(int64_t val) {
        return Primitive(Primitive::Integer, val);
    }
    
    static Primitive Number(double val) {
        return Primitive(Primitive::Number, val);
    }
    
    static Value Object() {
        return v8::Object::New();
    }
    
    static Value Array(int length=0) {
        return v8::Array::New(length);
    }
    
    static Value Function(v8::InvocationCallback __function, String name) {
        v8::Handle<v8::Function> function = v8::FunctionTemplate::New(__function)->GetFunction();
        function->SetName(name);
        return function;
    }
    
    static Value Error(String message) {
        return v8::ThrowException(v8::Exception::Error(message));
    }
    
    template <class T>
    static Value Internal(T that) {
        return Value(that, Value::Internal);
    }
    
    Value ExecuteString(String source, String name);
    String ReadFile(String name);
    
    /*static Value String(const char *val) {
        return Value(val);
    }
    
    static Value String(char *val) {
        return Value(val);
    }
    
    static Value String(char *val, int len) {
        return Value(v8::String::New(val, len));
    }*/
    
    typedef Value var;
}

#endif
