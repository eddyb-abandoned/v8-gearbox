#include <v8.h>
#include <ncurses/ncurses.h>

#include "Ncurses.h"
#include "shell.h"

V8FuncDef(global_Ncurses_Window)
{
    V8AssertArgs(4);
    args.This()->SetPointerInInternalField(0, newwin((int)(args[3]->IntegerValue()), (int)(args[2]->IntegerValue()), (int)(args[1]->IntegerValue()), (int)(args[0]->IntegerValue())));
    scrollok((WINDOW*)(args.This()->GetPointerFromInternalField(0)), true);
    wtimeout((WINDOW*)(args.This()->GetPointerFromInternalField(0)), 0);
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_Window_bold)
{
    V8AssertArgs(1);
    if(args[0]->BooleanValue())
        wattron((WINDOW*)(args.This()->GetPointerFromInternalField(0)),A_BOLD);
    else
        wattroff((WINDOW*)(args.This()->GetPointerFromInternalField(0)),A_BOLD);
    return v8::Undefined();
}


V8FuncDef(global_Ncurses_Window_border)
{
    //V8AssertArgs(0);
    wborder((WINDOW*)(args.This()->GetPointerFromInternalField(0)),0,0,0,0,(args.Length()>0?args[0]->IntegerValue():0),(args.Length()>1?args[1]->IntegerValue():0),(args.Length()>2?args[2]->IntegerValue():0),(args.Length()>3?args[3]->IntegerValue():0));
    if(args.This()->V8Get("autoRefresh")->BooleanValue())
        wrefresh((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_Window_print)
{
    V8AssertArgs(1);
    waddstr((WINDOW*)(args.This()->GetPointerFromInternalField(0)),(*v8::String::Utf8Value(args[0])));
    if(args.This()->V8Get("autoRefresh")->BooleanValue())
        wrefresh((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_Window_clear)
{
    V8AssertArgs(0);
    werase((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
    if(args.This()->V8Get("autoRefresh")->BooleanValue())
        wrefresh((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_Window_touch)
{
    V8AssertArgs(0);
    touchwin((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
    if(args.This()->V8Get("autoRefresh")->BooleanValue())
        wrefresh((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_Window_move)
{
    V8AssertArgs(2);
    wmove((WINDOW*)(args.This()->GetPointerFromInternalField(0)), args[1]->IntegerValue(), args[0]->IntegerValue());
    if(args.This()->V8Get("autoRefresh")->BooleanValue())
        wrefresh((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_Window_getChar)
{
    V8AssertArgs(0);
    int c = wgetch((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
    if(c<=0) return v8::Undefined();
    return V8Str((char*)&c, 1);
}

V8FuncDef(global_Ncurses_enter)
{
    V8AssertArgs(0);
    initscr();
    scrollok(stdscr, true);
    timeout(0);
    cbreak();
    noecho();
    
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_exit)
{
    V8AssertArgs(0);
    endwin();
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_bold)
{
    V8AssertArgs(1);
    if(args[0]->BooleanValue())
        attron(A_BOLD);
    else
        attroff(A_BOLD);
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_print)
{
    V8AssertArgs(1);
    printw((*v8::String::Utf8Value(args[0])));refresh();
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_clear)
{
    V8AssertArgs(0);
    erase();refresh();
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_setBackground)
{
    V8AssertArgs(1);
    bkgd((*v8::String::Utf8Value(args[0]))[0]);refresh();
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_getSize)
{
    V8AssertArgs(0);
    v8::Handle<v8::Object> obj = v8::Object::New();
    int x, y;
    getmaxyx(stdscr, y, x);
    obj->V8Set("cols", v8::Integer::New(x));
    obj->V8Set("rows", v8::Integer::New(y));
    return obj;
}

V8FuncDef(global_Ncurses_getChar)
{
    V8AssertArgs(0);
    int c = getch();
    if(c<=0)
        return v8::Undefined();
    return V8Str((char*)&c, 1);
}

void SetupNcurses(v8::Handle<v8::Object> global)
{
    v8::Handle<v8::Object> global_Ncurses = v8::Object::New();
    global->V8Set("Ncurses", global_Ncurses);
    v8::Handle<v8::FunctionTemplate> global_Ncurses_Window = V8Func(global_Ncurses_Window);
    global_Ncurses_Window->InstanceTemplate()->SetInternalFieldCount(1);
    global_Ncurses_Window->PrototypeTemplate()->V8Set("bold", V8Func(global_Ncurses_Window_bold));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("border", V8Func(global_Ncurses_Window_border));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("print", V8Func(global_Ncurses_Window_print));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("clear", V8Func(global_Ncurses_Window_clear));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("touch", V8Func(global_Ncurses_Window_touch));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("move", V8Func(global_Ncurses_Window_move));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("getChar", V8Func(global_Ncurses_Window_getChar));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("autoRefresh", v8::Boolean::New(true));
    global_Ncurses->V8Set("Window", global_Ncurses_Window->GetFunction());
    global_Ncurses->V8Set("enter", V8Func(global_Ncurses_enter)->GetFunction());
    global_Ncurses->V8Set("exit", V8Func(global_Ncurses_exit)->GetFunction());
    global_Ncurses->V8Set("bold", V8Func(global_Ncurses_bold)->GetFunction());
    global_Ncurses->V8Set("print", V8Func(global_Ncurses_print)->GetFunction());
    global_Ncurses->V8Set("clear", V8Func(global_Ncurses_clear)->GetFunction());
    global_Ncurses->V8Set("setBackground", V8Func(global_Ncurses_setBackground)->GetFunction());
    global_Ncurses->V8Set("getSize", V8Func(global_Ncurses_getSize)->GetFunction());
    global_Ncurses->V8Set("getChar", V8Func(global_Ncurses_getChar)->GetFunction());
}