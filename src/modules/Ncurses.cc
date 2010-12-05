
#include <ncurses/ncurses.h>

#include "Ncurses.h"
#include "../shell.h"

V8FuncDef(global_Ncurses_Window_Window)
{
    if(args.Length() >= 4)
    {
        args.This()->SetPointerInInternalField(0, newwin(args[3]->IntegerValue(), args[2]->IntegerValue(), args[1]->IntegerValue(), args[0]->IntegerValue()));
        scrollok(((WINDOW*)args.This()->GetPointerFromInternalField(0)), true);
        wtimeout(((WINDOW*)args.This()->GetPointerFromInternalField(0)), 0);
        return v8::Undefined();
    }
    V8Throw("Invalid call to Ncurses.Window");
}

V8FuncDef(global_Ncurses_Window_bold)
{
    if(args.Length() >= 1)
    {
        if(args[0]->BooleanValue())
            wattron(((WINDOW*)args.This()->GetPointerFromInternalField(0)), A_BOLD);
        else
            wattroff(((WINDOW*)args.This()->GetPointerFromInternalField(0)), A_BOLD);
        return v8::Undefined();
    }
    V8Throw("Invalid call to Ncurses.Window.prototype.bold");
}

V8FuncDef(global_Ncurses_Window_border)
{
    if(args.Length() >= 1)
    {
        wborder(((WINDOW*)args.This()->GetPointerFromInternalField(0)), args[0]->ToObject()->V8Get("Ls")->IntegerValue(), args[0]->ToObject()->V8Get("Rs")->IntegerValue(), args[0]->ToObject()->V8Get("Ts")->IntegerValue(), args[0]->ToObject()->V8Get("Bs")->IntegerValue(), args[0]->ToObject()->V8Get("TLc")->IntegerValue(), args[0]->ToObject()->V8Get("TRc")->IntegerValue(), args[0]->ToObject()->V8Get("BLc")->IntegerValue(), args[0]->ToObject()->V8Get("BRc")->IntegerValue());
        if(args.This()->V8Get("autoRefresh")->BooleanValue())
            wrefresh(((WINDOW*)args.This()->GetPointerFromInternalField(0)));
        return v8::Undefined();
    }

    wborder(((WINDOW*)args.This()->GetPointerFromInternalField(0)), 0, 0, 0, 0, 0, 0, 0, 0);
    if(args.This()->V8Get("autoRefresh")->BooleanValue())
        wrefresh(((WINDOW*)args.This()->GetPointerFromInternalField(0)));
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_Window_setBackground)
{
    if(args.Length() >= 1)
    {
        wbkgd(((WINDOW*)args.This()->GetPointerFromInternalField(0)), (*v8::String::Utf8Value(args[0]))[0]);
        if(args.This()->V8Get("autoRefresh")->BooleanValue())
            wrefresh(((WINDOW*)args.This()->GetPointerFromInternalField(0)));
        return v8::Undefined();
    }
    V8Throw("Invalid call to Ncurses.Window.prototype.setBackground");
}

V8FuncDef(global_Ncurses_Window_print)
{
    if(args.Length() >= 1)
    {
        waddstr(((WINDOW*)args.This()->GetPointerFromInternalField(0)), (*v8::String::Utf8Value(args[0])));
        if(args.This()->V8Get("autoRefresh")->BooleanValue())
            wrefresh(((WINDOW*)args.This()->GetPointerFromInternalField(0)));
        return v8::Undefined();
    }
    V8Throw("Invalid call to Ncurses.Window.prototype.print");
}

V8FuncDef(global_Ncurses_Window_clear)
{
    werase(((WINDOW*)args.This()->GetPointerFromInternalField(0)));
    if(args.This()->V8Get("autoRefresh")->BooleanValue())
        wrefresh(((WINDOW*)args.This()->GetPointerFromInternalField(0)));
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_Window_touch)
{
    touchwin(((WINDOW*)args.This()->GetPointerFromInternalField(0)));
    if(args.This()->V8Get("autoRefresh")->BooleanValue())
        wrefresh(((WINDOW*)args.This()->GetPointerFromInternalField(0)));
    return v8::Undefined();
}

V8FuncDef(global_Ncurses_Window_move)
{
    if(args.Length() >= 2)
    {
        wmove(((WINDOW*)args.This()->GetPointerFromInternalField(0)), args[1]->IntegerValue(), args[0]->IntegerValue());
        if(args.This()->V8Get("autoRefresh")->BooleanValue())
            wrefresh(((WINDOW*)args.This()->GetPointerFromInternalField(0)));
        return v8::Undefined();
    }
    V8Throw("Invalid call to Ncurses.Window.prototype.move");
}

V8FuncDef(global_Ncurses_Window_getChar)
{
    int c = wgetch(((WINDOW*)args.This()->GetPointerFromInternalField(0)));
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
}

V8FuncDef(global_Ncurses_rows)
{
    return v8::Integer::New(getmaxy(stdscr));
}


void SetupNcurses(v8::Handle<v8::Object> global)
{
    v8::Handle<v8::Object> global_Ncurses = v8::Object::New();
    global->V8Set("Ncurses", global_Ncurses);
    v8::Handle<v8::FunctionTemplate> global_Ncurses_Window = V8Func(global_Ncurses_Window_Window);
    global_Ncurses_Window->SetClassName(v8::String::New("Window"));
    global_Ncurses_Window->InstanceTemplate()->SetInternalFieldCount(1);
    v8::Handle<v8::Function> global_Ncurses_Window_bold = V8Func(global_Ncurses_Window_bold)->GetFunction();
    global_Ncurses_Window_bold->SetName(v8::String::New("bold"));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("bold", global_Ncurses_Window_bold);
    v8::Handle<v8::Function> global_Ncurses_Window_border = V8Func(global_Ncurses_Window_border)->GetFunction();
    global_Ncurses_Window_border->SetName(v8::String::New("border"));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("border", global_Ncurses_Window_border);
    v8::Handle<v8::Function> global_Ncurses_Window_setBackground = V8Func(global_Ncurses_Window_setBackground)->GetFunction();
    global_Ncurses_Window_setBackground->SetName(v8::String::New("setBackground"));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("setBackground", global_Ncurses_Window_setBackground);
    v8::Handle<v8::Function> global_Ncurses_Window_print = V8Func(global_Ncurses_Window_print)->GetFunction();
    global_Ncurses_Window_print->SetName(v8::String::New("print"));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("print", global_Ncurses_Window_print);
    v8::Handle<v8::Function> global_Ncurses_Window_clear = V8Func(global_Ncurses_Window_clear)->GetFunction();
    global_Ncurses_Window_clear->SetName(v8::String::New("clear"));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("clear", global_Ncurses_Window_clear);
    v8::Handle<v8::Function> global_Ncurses_Window_touch = V8Func(global_Ncurses_Window_touch)->GetFunction();
    global_Ncurses_Window_touch->SetName(v8::String::New("touch"));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("touch", global_Ncurses_Window_touch);
    v8::Handle<v8::Function> global_Ncurses_Window_move = V8Func(global_Ncurses_Window_move)->GetFunction();
    global_Ncurses_Window_move->SetName(v8::String::New("move"));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("move", global_Ncurses_Window_move);
    v8::Handle<v8::Function> global_Ncurses_Window_getChar = V8Func(global_Ncurses_Window_getChar)->GetFunction();
    global_Ncurses_Window_getChar->SetName(v8::String::New("getChar"));
    global_Ncurses_Window->PrototypeTemplate()->V8Set("getChar", global_Ncurses_Window_getChar);
    global_Ncurses_Window->PrototypeTemplate()->V8Set("autoRefresh", v8::Boolean::New(true));
    global_Ncurses->V8Set("Window", global_Ncurses_Window->GetFunction());
    v8::Handle<v8::Function> global_Ncurses_enter = V8Func(global_Ncurses_enter)->GetFunction();
    global_Ncurses_enter->SetName(v8::String::New("enter"));
    global_Ncurses->V8Set("enter", global_Ncurses_enter);
    v8::Handle<v8::Function> global_Ncurses_exit = V8Func(global_Ncurses_exit)->GetFunction();
    global_Ncurses_exit->SetName(v8::String::New("exit"));
    global_Ncurses->V8Set("exit", global_Ncurses_exit);
    v8::Handle<v8::Function> global_Ncurses_cols = V8Func(global_Ncurses_cols)->GetFunction();
    global_Ncurses_cols->SetName(v8::String::New("cols"));
    global_Ncurses->V8Set("cols", global_Ncurses_cols);
    v8::Handle<v8::Function> global_Ncurses_rows = V8Func(global_Ncurses_rows)->GetFunction();
    global_Ncurses_rows->SetName(v8::String::New("rows"));
    global_Ncurses->V8Set("rows", global_Ncurses_rows);
}