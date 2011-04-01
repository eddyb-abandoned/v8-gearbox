
#include <v8-gearbox.h>

namespace Gearbox {
    bool TryCatch::m_bCanThrow = false;
    TryCatch *TryCatch::m_ActualTryCatch = 0;
    
    void TryCatch::reportException() {
        #if GEARBOX_TRY_CATCH_REPORT_STACKTRACE
            String stackTrace;
            if(!m_TryCatch.HasCaught())
                stackTrace = m_LocalStackTrace;
            else
                stackTrace = Value(m_TryCatch.StackTrace());
            
            errprintf("%s" _STR_NEWLINE, *stackTrace);
            
            m_TryCatch.Reset();
            m_bHasLocalException = false;
            m_LocalStackTrace = undefined;
        #else
            String exception;
            if(!m_TryCatch.HasCaught())
                exception = m_LocalException;
            else
                exception = Value(m_TryCatch.Exception());
            
            v8::Handle<v8::Message> message;
            if(!m_TryCatch.HasCaught())
                message = m_LocalMessage;
            else
                message = m_TryCatch.Message();
            
            if(message.IsEmpty())
                errprintf("%s" _STR_NEWLINE, *exception);
            else {
                // Print (filename):(line number): (message).
                String filename = Value(message->GetScriptResourceName());
                errprintf("%s:%i: %s" _STR_NEWLINE, *filename, message->GetLineNumber(), *exception);
                
                // Print line of source code.
                String sourceline = Value(message->GetSourceLine());
                errprintf("%s" _STR_NEWLINE, *sourceline);
                
                // Print wavy underline
                int start = message->GetStartColumn(), end = message->GetEndColumn();
                for(int i = 0; i < start; i++)
                    errprintf(_STR_SPACE);
                for(int i = start; i < end; i++)
                    errprintf("^");
                errprintf(_STR_NEWLINE);
            }
            
            m_TryCatch.Reset();
            m_bHasLocalException = false;
            m_LocalException = undefined;
            if(!m_LocalMessage.IsEmpty()) {
                m_LocalMessage.Dispose();
                m_LocalMessage.Clear();
            }
        #endif
    }
}