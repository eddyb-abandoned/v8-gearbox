#ifndef GEARBOX_STRING_H
#define GEARBOX_STRING_H

namespace Gearbox {
    class String {
        public:
            String(char *pString=0, int iLength=-1) {
                clone(pString, iLength);
            }
            String(const char *pString, int iLength=-1) {
                clone(const_cast<char*>(pString), iLength);
            }
            String(const String &that) {
                clone(that.m_pString, that.m_iLength);
            }
            ~String() {
                if(m_pString)
                    delete m_pString;
            }
            
            String &operator =(const String &that) {
                if(m_pString)
                    delete m_pString;
                clone(that.m_pString, that.m_iLength);
                return *this;
            }
            
            /** empty: returns true if the string is null, false otherwise */
            bool empty() {
                return !m_pString;
            }
            /** length: return 0 if the string is null, the actual length of the string otherwise */
            int length() {
                if(empty())
                    return 0;
                return m_iLength;
            }
            
            /** Concatenate operators */
            String operator+(const String &that) {
                return concat(*this, that);
            }
            String &operator+=(const String &that) {
                return operator=(concat(*this, that));
            }
            
            /** Convert operators */
            operator char*() {
                if(!m_pString)
                    return const_cast<char*>("");
                return m_pString;
            }
            char *operator*() {
                return operator char*();
            }
            operator v8::Handle<v8::String>() {
                return v8::String::New(m_pString, m_iLength);
            }
            operator v8::Handle<v8::Value>() {
                return operator v8::Handle<v8::String>();
            }
            static String concat(String left, String right);
        private:
            void clone(char *pString, int iLength) {
                if(!pString) {
                    pString = 0;
                    iLength = 0;
                    return;
                }
                
                // Use strlen to get the length if not provided
                if(iLength == -1)
                    iLength = strlen(pString);
                
                // End the string with \0 to make C stuff happy
                m_pString = new char [iLength + 1];
                m_pString[iLength] = '\0';
                
                // Copy the original string over
                memcpy(m_pString, pString, iLength);
                m_iLength = iLength;
            }
            
            char *m_pString;
            int m_iLength;
    };
}

#endif
