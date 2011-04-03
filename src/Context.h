#ifndef V8_GEARBOX_CONTEXT_H
#define V8_GEARBOX_CONTEXT_H

#include <v8-gearbox.h>

namespace Gearbox {
    class Context {
        public:
            Context();
            
            virtual ~Context();
            
            Value global() {
                return m_hContext->Global();
            }
            
            Value runScript(String source, String name);
            
            static Context *getCurrent() {
                return m_pCurrentContext;
            }
            
        private:
            virtual void setup();
            
            v8::Persistent<v8::Context> m_hContext;
            
            Context *m_pPreviousContext;
            
            static Context *m_pCurrentContext;
    };
}

#endif
