#ifndef V8_GEARBOX_TRYCATCH_H
#define V8_GEARBOX_TRYCATCH_H

#include <v8-gearbox.h>

#define GEARBOX_TRY_CATCH_REPORT_STACKTRACE 0

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
