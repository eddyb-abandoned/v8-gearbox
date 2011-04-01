#ifndef V8_GEARBOX_TRYCATCH_H
#define V8_GEARBOX_TRYCATCH_H

#include <v8-gearbox.h>

#define GEARBOX_TRY_CATCH_REPORT_STACKTRACE 0

namespace Gearbox {
    class TryCatch {
        public:
            TryCatch() : m_PreviousTryCatch(m_ActualTryCatch), m_bHasLocalException(false) {
                m_ActualTryCatch = this;
            }
            
            virtual ~TryCatch() {
                if(hasCaught()) {
                    if(m_PreviousTryCatch) {
                        if(!m_TryCatch.HasCaught()) {
                            m_PreviousTryCatch->m_bHasLocalException = true;
                            #if GEARBOX_TRY_CATCH_REPORT_STACKTRACE
                                m_PreviousTryCatch->m_LocalStackTrace = m_LocalStackTrace;
                            #else
                                m_PreviousTryCatch->m_LocalException = m_LocalException;
                                m_PreviousTryCatch->m_LocalMessage = m_LocalMessage;
                            #endif
                        }
                        else {
                            m_PreviousTryCatch->m_bHasLocalException = true;
                            #if GEARBOX_TRY_CATCH_REPORT_STACKTRACE
                                m_PreviousTryCatch->m_LocalStackTrace = m_TryCatch.StackTrace();
                            #else
                                m_PreviousTryCatch->m_LocalException = m_TryCatch.Exception();
                                m_PreviousTryCatch->m_LocalMessage = v8::Persistent<v8::Message>::New(m_TryCatch.Message());
                            #endif
                            if(canThrow())
                                m_TryCatch.ReThrow();
                        }
                    }
                    else
                        reportException();
                }
                m_ActualTryCatch = m_PreviousTryCatch;
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
                if(m_ActualTryCatch) {
                    m_ActualTryCatch->m_TryCatch.Reset();
                    m_ActualTryCatch->m_bHasLocalException = true;
                    #if GEARBOX_TRY_CATCH_REPORT_STACKTRACE
                        m_ActualTryCatch->m_LocalStackTrace = exception;
                    #else
                        m_ActualTryCatch->m_LocalException = exception;
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
            
            TryCatch *m_PreviousTryCatch;
            
            static bool m_bCanThrow;
            static TryCatch *m_ActualTryCatch;
    };
    
    static Value Throw(Value exception) {
        return TryCatch::_throw(exception);
    }
}

#endif
