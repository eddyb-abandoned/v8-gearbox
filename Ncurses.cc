
#include <ncurses/ncurses.h>

#include "Ncurses.h"
#include "shell.h"

V8FuncDef(global_Ncurses_Window_Window)
{
    if(args.Length() >= 4)
    {
        args.This()->SetPointerInInternalField(0, newwin(args[3]->IntegerValue(), args[2]->IntegerValue(), args[1]->IntegerValue(), args[0]->IntegerValue()));
        scrollok((WINDOW*)(args.This()->GetPointerFromInternalField(0)), true);
        wtimeout((WINDOW*)(args.This()->GetPointerFromInternalField(0)), 0);
        return v8::Undefined();
    }
    V8Assert(false, "Invalid call to global.Ncurses.Window.prototype.Window")
}

V8FuncDef(global_Ncurses_Window_bold)
{
    if(args.Length() >= 1)
    {
        if(args[0]->BooleanValue())
            wattron((WINDOW*)(args.This()->GetPointerFromInternalField(0)), A_BOLD);
        else
            wattroff((WINDOW*)(args.This()->GetPointerFromInternalField(0)), A_BOLD);
        return v8::Undefined();
    }
    V8Assert(false, "Invalid call to global.Ncurses.Window.prototype.bold")
}

V8FuncDef(global_Ncurses_Window_border)
{
    if(args.Length() >= 1)
    {
        wborder((WINDOW*)(args.This()->GetPointerFromInternalField(0)), args[0]->ToObject()->V8Get("Ls")->IntegerValue(), args[0]->ToObject()->V8Get("Rs")->IntegerValue(), args[0]->ToObject()->V8Get("Ts")->IntegerValue(), args[0]->ToObject()->V8Get("Bs")->IntegerValue(), args[0]->ToObject()->V8Get("TLc")->IntegerValue(), args[0]->ToObject()->V8Get("TRc")->IntegerValue(), args[0]->ToObject()->V8Get("BLc")->IntegerValue(), args[0]->ToObject()->V8Get("BRc")->IntegerValue());
        if(args.This()->V8Get("autoRefresh")->BooleanValue())
            wrefresh((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
        return v8::Undefined();
    }

    wborder((WINDOW*)(args.This()->GetPointerFromInternalField(0)), 0, 0, 0, 0, 0, 0, 0, 0);
    if(args.This()->V8Get("autoRefresh")->BooleanValue())
        wrefresh((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_Window_setBackground)
{
    if(args.Length() >= 1)
    {
        wbkgd((WINDOW*)(args.This()->GetPointerFromInternalField(0)), (*v8::String::Utf8Value(args[0]))[0]);
        if(args.This()->V8Get("autoRefresh")->BooleanValue())
            wrefresh((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
        return v8::Undefined();
    }
    V8Assert(false, "Invalid call to global.Ncurses.Window.prototype.setBackground")
}

V8FuncDef(global_Ncurses_Window_print)
{
    if(args.Length() >= 1)
    {
        waddstr((WINDOW*)(args.This()->GetPointerFromInternalField(0)), (*v8::String::Utf8Value(args[0])));
        if(args.This()->V8Get("autoRefresh")->BooleanValue())
            wrefresh((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
        return v8::Undefined();
    }
    V8Assert(false, "Invalid call to global.Ncurses.Window.prototype.print")
}

V8FuncDef(global_Ncurses_Window_clear)
{
    werase((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
    if(args.This()->V8Get("autoRefresh")->BooleanValue())
        wrefresh((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_Window_touch)
{
    touchwin((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
    if(args.This()->V8Get("autoRefresh")->BooleanValue())
        wrefresh((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_Window_move)
{
    if(args.Length() >= 2)
    {
        wmove((WINDOW*)(args.This()->GetPointerFromInternalField(0)), args[1]->IntegerValue(), args[0]->IntegerValue());
        if(args.This()->V8Get("autoRefresh")->BooleanValue())
            wrefresh((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
        return v8::Undefined();
    }
    V8Assert(false, "Invalid call to global.Ncurses.Window.prototype.move")
}

V8FuncDef(global_Ncurses_Window_getChar)
{
    int c = wgetch((WINDOW*)(args.This()->GetPointerFromInternalField(0)));
    if(c > 0)
        return v8::Integer::New(c);
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_enter)
{
    initscr();
    scrollok(stdscr, true);
    timeout(0);
    cbreak();
    noecho();
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_exit)
{
    endwin();
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_cols)
{
    return v8::Integer::New(getmaxx(stdscr));
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_rows)
{
    return v8::Integer::New(getmaxy(stdscr));
    return v8::Undefined();
}


void SetupNcurses(v8::Handle<v8::Object> global)
{
    v8::Handle<v8::Object> global_Ncurses = v8::Object::New();
    global->V8Set("Ncurses", global_Ncurses);
    v8::Handle<v8::FunctionTemplate> global_Ncurses_Window = V8Func(global_Ncurses_Window_Window);
    global_Ncurses_Window->InstanceTemplate()->SetInternalFieldCount(1);
    global_Ncurses_Window->PrototypeTemplate()->V8Set("bold", V8Func(global_Ncurses_Window_bold)->GetFunction());
    global_Ncurses_Window->PrototypeTemplate()->V8Set("border", V8Func(global_Ncurses_Window_border)->GetFunction());
    global_Ncurses_Window->PrototypeTemplate()->V8Set("setBackground", V8Func(global_Ncurses_Window_setBackground)->GetFunction());
    global_Ncurses_Window->PrototypeTemplate()->V8Set("print", V8Func(global_Ncurses_Window_print)->GetFunction());
    global_Ncurses_Window->PrototypeTemplate()->V8Set("clear", V8Func(global_Ncurses_Window_clear)->GetFunction());
    global_Ncurses_Window->PrototypeTemplate()->V8Set("touch", V8Func(global_Ncurses_Window_touch)->GetFunction());
    global_Ncurses_Window->PrototypeTemplate()->V8Set("move", V8Func(global_Ncurses_Window_move)->GetFunction());
    global_Ncurses_Window->PrototypeTemplate()->V8Set("getChar", V8Func(global_Ncurses_Window_getChar)->GetFunction());
    global_Ncurses_Window->PrototypeTemplate()->V8Set("autoRefresh", v8::Boolean::New(true));
    global_Ncurses->V8Set("Window", global_Ncurses_Window->GetFunction());
    global_Ncurses->V8Set("enter", V8Func(global_Ncurses_enter)->GetFunction());
    global_Ncurses->V8Set("exit", V8Func(global_Ncurses_exit)->GetFunction());
    global_Ncurses->V8Set("cols", V8Func(global_Ncurses_cols)->GetFunction());
    global_Ncurses->V8Set("rows", V8Func(global_Ncurses_rows)->GetFunction());
}
