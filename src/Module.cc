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

#include <v8-gearbox.h>

namespace Gearbox {
    std::map<String, Module*> *Module::m_pModules = 0;
    
    Module::Module(String moduleName, Module::SetupCallback pSetupCallback) : m_sModuleName(moduleName), m_pSetupCallback(pSetupCallback) {
        if(!Module::m_pModules)
            Module::m_pModules = new std::map<String, Module*>();
        (*m_pModules)[moduleName] = this;
    }
    
    Module::~Module() {
        if(!Module::m_pModules)
            Module::m_pModules = new std::map<String, Module*>();
        m_pModules->erase(m_sModuleName);
    }
    
    Value Module::require() {
        if(m_Exports == undefined) {
            m_Exports = Object();
            Context moduleContext;
            moduleContext.global()["exports"] = m_Exports;
            m_pSetupCallback(m_Exports);
        }
        return m_Exports;
    }
    
    Value Module::require(String moduleName) {
        if(moduleName[0] == '.' || moduleName[0] == '/')
            return Throw(Error("Can't load relative/absolute path module"));
        
        if(!Module::m_pModules)
            Module::m_pModules = new std::map<String, Module*>();
        
        if(!m_pModules->count(moduleName))
            return Throw(Error("Module isn't already loaded"));
        
        return (*m_pModules)[moduleName]->require();
    }

}
