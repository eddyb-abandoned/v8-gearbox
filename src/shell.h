#ifndef SHELL_H
#define SHELL_H

#ifndef GEARBOX_APACHE_MOD
    #include <cstdio>
    #define errprintf(...) std::fprintf(stderr, __VA_ARGS__)
    #define _STR_NEWLINE "\n"
    #define _STR_SPACE " "
#else
    #include "apache2/mod_gearbox.h"
    
    extern ApacheRequestRec *g_pRequest;
    bool RunScript(const char *sScript);
    
    #define printf if(g_pRequest)g_pRequest->rprintf
    #define errprintf if(g_pRequest)g_pRequest->rprintf
    #define _STR_NEWLINE "<br>"
    #define _STR_SPACE "&nbsp;"
#endif

#endif
