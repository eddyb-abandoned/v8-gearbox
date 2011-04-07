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

#ifndef V8_GEARBOX_STRING_H
#define V8_GEARBOX_STRING_H

#include <v8-gearbox.h>

#include <cstring>

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
            
            /** Compare operators */
            bool operator==(const String &that) {
                return compare(that);
            }
            bool operator==(char *that) {
                return compare(that);
            }
            bool operator==(const char *that) {
                return compare(that);
            }
            bool operator<(const String &that) const {
                return std::strcmp(m_pString, that.m_pString) < 0;
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
            
            bool compare(const String &that, int len=0) {
                int minLen = m_iLength > that.m_iLength ? that.m_iLength : m_iLength;
                if(len) {
                    if(len > minLen)
                        return false;
                }
                else {
                    if(minLen != m_iLength || minLen != that.m_iLength)
                        return false;
                    len = minLen;
                }
                for(int i = 0; i < len; i++)
                    if(m_pString[i] != that.m_pString[i])
                        return false;
                return true;
            }
            static String concat(String left, String right);
        private:
            void clone(char *pString, int iLength) {
                if(!pString) {
                    m_pString = 0;
                    m_iLength = 0;
                    return;
                }
                
                // Use strlen to get the length if not provided
                if(iLength == -1)
                    iLength = std::strlen(pString);
                
                // End the string with \0 to make C stuff happy
                m_pString = new char [iLength + 1];
                m_pString[iLength] = '\0';
                
                // Copy the original string over
                std::memcpy(m_pString, pString, iLength);
                m_iLength = iLength;
            }
            
            char *m_pString;
            int m_iLength;
    };
}

#endif
