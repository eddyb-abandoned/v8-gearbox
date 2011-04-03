#ifndef V8_GEARBOX_VALUE_H
#define V8_GEARBOX_VALUE_H

#include <v8-gearbox.h>

namespace Gearbox {
    
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
                if(m_Kind <= Null)
                    return m_Kind == that.m_Kind;
                return operator v8::Handle<v8::Value>()->Equals(that);
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
                // Avoid exposing an External to JavaScript
                m_Flags |= Internal;
            }
            
            /** Conversion tools, used to get primitive values */
            template <class T>
            T to() {
                return to(Type<T>());
            }
            
            v8::Handle<v8::Value> to(Type<v8::Handle<v8::Value>>);
            v8::Handle<v8::Data> to(Type<v8::Handle<v8::Data>>) {
                return to<v8::Handle<v8::Value>>();
            }
            template <class T>
            v8::Handle<T> to(Type<v8::Handle<T>>) {
                return v8::Handle<T>::Cast(to<v8::Handle<v8::Value>>());
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
            uint64_t to(Type<uint64_t>) {
                return to<int64_t>();
            }
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
                    errprintf("WARNING: empty/NULL External!" _STR_NEWLINE);
                    return 0;
                }
                return reinterpret_cast<T*>(v8::External::Unwrap(m_hValue));
            }
            
            /** Compare operators */
#define DECLARE_OP(OP) \
bool operator OP(Primitive that) { \
    if(m_hValue.IsEmpty()) \
        return m_pValue OP that; \
    else \
        return m_hValue->Equals(that);\
}
            bool operator==(Value that) {
                if(m_hValue.IsEmpty() && that.m_hValue.IsEmpty())
                    return m_pValue == that.m_pValue;
                else
                    return to<v8::Handle<v8::Value>>()->Equals(that);
            }
            bool operator!=(Value that) {
                return !operator==(that);
            }
            //DECLARE_OP(==)
            //DECLARE_OP(!=)
            //DECLARE_OP(>)
            //DECLARE_OP(<)
            //DECLARE_OP(>=)
            //DECLARE_OP(<=)
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
            
            /** Returns true if this Value is an instance of class T */
            template <class T>
            bool is() {
                return T::is(*this);
            }
            
            /** Convert operator */
            template <class T>
            operator T() {
                return to<T>();
            }
            
            /** Call operator for Functions */
            template <class... Args>
            Value operator()(Args... _args) {
                if(m_hValue.IsEmpty() || !m_hValue->IsFunction())
                    return undefined;
                
                ValueList args;
                args.add(_args...);
                
                return v8::Handle<v8::Function>::Cast(m_hValue)->Call(v8::Object::New(), args.numValues(), args.values());
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
}

#endif
