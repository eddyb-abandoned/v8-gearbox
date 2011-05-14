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

#ifndef V8_GEARBOX_TRYCATCH_H
#define V8_GEARBOX_TRYCATCH_H

#include <v8-gearbox.h>

#define GEARBOX_TRY_CATCH_REPORT_STACKTRACE 1

namespace Gearbox {
    class TryCatch {
        public:
            TryCatch() : m_pPreviousTryCatch(m_pCurrentTryCatch), m_bHasLocalException(false) {
                m_pCurrentTryCatch = this;
            }
            
            virtual ~TryCatch() {
                if(hasCaught()) {
                    if(m_pPreviousTryCatch) {
                        if(!m_TryCatch.HasCaught()) {
                            m_pPreviousTryCatch->m_bHasLocalException = true;
                            #if GEARBOX_TRY_CATCH_REPORT_STACKTRACE
                                m_pPreviousTryCatch->m_LocalStackTrace = m_LocalStackTrace;
                            #else
                                m_pPreviousTryCatch->m_LocalException = m_LocalException;
                                m_pPreviousTryCatch->m_LocalMessage = m_LocalMessage;
                            #endif
                        }
                        else {
                            m_pPreviousTryCatch->m_bHasLocalException = true;
                            #if GEARBOX_TRY_CATCH_REPORT_STACKTRACE
                                m_pPreviousTryCatch->m_LocalStackTrace = m_TryCatch.StackTrace();
                            #else
                                m_pPreviousTryCatch->m_LocalException = m_TryCatch.Exception();
                                m_pPreviousTryCatch->m_LocalMessage = v8::Persistent<v8::Message>::New(m_TryCatch.Message());
                            #endif
                            if(canThrow())
                                m_TryCatch.ReThrow();
                        }
                    }
                    else
                        reportException();
                }
                m_pCurrentTryCatch = m_pPreviousTryCatch;
            }
            
            bool hasCaught() {
                return m_bHasLocalException || m_TryCatch.HasCaught();
            }
            
            void reportException();
            
            static bool canThrow() {
                return m_bCanThrow;
            }
            
            static bool canThrow(bool bCanThrow) {
                bool bCanThrowBefore = m_bCanThrow;
                m_bCanThrow = bCanThrow;
                return bCanThrowBefore;
            }
            
            static Value _throw(Value exception) {
                if(m_pCurrentTryCatch) {
                    m_pCurrentTryCatch->m_TryCatch.Reset();
                    m_pCurrentTryCatch->m_bHasLocalException = true;
                    #if GEARBOX_TRY_CATCH_REPORT_STACKTRACE
                        m_pCurrentTryCatch->m_LocalStackTrace = exception;
                    #else
                        m_pCurrentTryCatch->m_LocalException = exception;
                    #endif
                }
                if(canThrow())
                    return v8::ThrowException(exception);
                return undefined;
            }
            
        private:
            v8::TryCatch m_TryCatch;
            bool m_bHasLocalException;
            #if GEARBOX_TRY_CATCH_REPORT_STACKTRACE
                Value m_LocalStackTrace;
            #else
                Value m_LocalException;
                v8::Persistent<v8::Message> m_LocalMessage;
            #endif
            
            TryCatch *m_pPreviousTryCatch;
            
            static bool m_bCanThrow;
            static TryCatch *m_pCurrentTryCatch;
    };
    
    static Value Throw(Value exception) {
        return TryCatch::_throw(exception);
    }
}

#endif
