
#include <v8-gearbox.h>
#include "Ncurses.h"

using namespace Gearbox;

/** \file Ncurses.cc converted from Ncurses.gear */

#line 1 "src/modules/Ncurses.gear"
#include <curses.h>

v8::Handle<v8::Value> __global_Ncurses_Window_Window(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 4)
    {
        #line 10 "src/modules/Ncurses.gear"
        Value x(args[0]), y(args[1]), cols(args[2]), rows(args[3]);
        This["win"] = newwin(rows.to<int>(), cols.to<int>(), y.to<int>(), x.to<int>());
        scrollok(This["win"].to<WINDOW*>(), true);
        wtimeout(This["win"], 0);
        return undefined;
    }
    return Throw(Error("Invalid call to Ncurses.Window"));
}

v8::Handle<v8::Value> __global_Ncurses_Window_bold(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1)
    {
        #line 16 "src/modules/Ncurses.gear"
        Value on(args[0]);
        if(on.to<bool>())
            wattron(This["win"], A_BOLD);
        else
            wattroff(This["win"], A_BOLD);
        return undefined;
    }
    return Throw(Error("Invalid call to Ncurses.Window.prototype.bold"));
}

v8::Handle<v8::Value> __global_Ncurses_Window_border(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1)
    {
        #line 29 "src/modules/Ncurses.gear"
        Value obj(args[0]);
        wborder(This["win"], obj["Ls"].to<int>(), obj["Rs"].to<int>(), obj["Ts"].to<int>(), obj["Bs"].to<int>(), obj["TLc"].to<int>(), obj["TRc"].to<int>(), obj["BLc"].to<int>(), obj["BRc"].to<int>());
        if(This["autoRefresh"])
            wrefresh(This["win"]);
        return undefined;
    }

    #line 24 "src/modules/Ncurses.gear"
    wborder(This["win"], 0, 0, 0, 0, 0, 0, 0, 0);
    if(This["autoRefresh"])
        wrefresh(This["win"]);
    return undefined;
}

v8::Handle<v8::Value> __global_Ncurses_Window_setBackground(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1)
    {
        #line 35 "src/modules/Ncurses.gear"
        Value _char(args[0]);
        wbkgd(This["win"], _char.to<String>()[0]);
        if(This["autoRefresh"])
            wrefresh(This["win"]);
        return undefined;
    }
    return Throw(Error("Invalid call to Ncurses.Window.prototype.setBackground"));
}

v8::Handle<v8::Value> __global_Ncurses_Window_print(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1)
    {
        #line 41 "src/modules/Ncurses.gear"
        Value text(args[0]);
        waddstr(This["win"], text.to<String>());
        if(This["autoRefresh"])
            wrefresh(This["win"]);
        return undefined;
    }
    return Throw(Error("Invalid call to Ncurses.Window.prototype.print"));
}

v8::Handle<v8::Value> __global_Ncurses_Window_clear(const v8::Arguments& args) {
    Value This(args.This());
    #line 48 "src/modules/Ncurses.gear"
    werase(This["win"]);
    if(This["autoRefresh"])
        wrefresh(This["win"]);
    return undefined;
}

v8::Handle<v8::Value> __global_Ncurses_Window_touch(const v8::Arguments& args) {
    Value This(args.This());
    #line 54 "src/modules/Ncurses.gear"
    touchwin(This["win"].to<WINDOW*>());
    if(This["autoRefresh"])
        wrefresh(This["win"]);
    return undefined;
}

v8::Handle<v8::Value> __global_Ncurses_Window_move(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 2)
    {
        #line 59 "src/modules/Ncurses.gear"
        Value x(args[0]), y(args[1]);
        wmove(This["win"], y.to<int>(), x.to<int>());
        if(This["autoRefresh"])
            wrefresh(This["win"]);
        return undefined;
    }
    return Throw(Error("Invalid call to Ncurses.Window.prototype.move"));
}

v8::Handle<v8::Value> __global_Ncurses_Window_getChar(const v8::Arguments& args) {
    Value This(args.This());
    #line 66 "src/modules/Ncurses.gear"
    int c = wgetch(This["win"]);
    if(c > 0)
        return Integer(c);
    return undefined;
}

v8::Handle<v8::Value> __global_Ncurses_enter(const v8::Arguments& args) {
    #line 73 "src/modules/Ncurses.gear"
    initscr();
    scrollok(stdscr, true);
    timeout(0);
    cbreak();
    noecho();
    return undefined;
}

v8::Handle<v8::Value> __global_Ncurses_exit(const v8::Arguments& args) {
    #line 81 "src/modules/Ncurses.gear"
    endwin();
    return undefined;
}

v8::Handle<v8::Value> __global_Ncurses_cols(const v8::Arguments& args) {
    #line 85 "src/modules/Ncurses.gear"
    return Integer(getmaxx(stdscr));
}

v8::Handle<v8::Value> __global_Ncurses_rows(const v8::Arguments& args) {
    #line 89 "src/modules/Ncurses.gear"
    return Integer(getmaxy(stdscr));
}

v8::Handle<v8::Value> __global_Ncurses_toString(const v8::Arguments& args) {
    #line 6 "src/modules/Ncurses.gear"
    return String("[object Ncurses]");
}


#line 161 "src/modules/Ncurses.cc"
void SetupNcurses(v8::Handle<v8::Object> global) {
    v8::Handle<v8::Object> global_Ncurses = v8::Object::New();
    global->Set(String("Ncurses"), global_Ncurses);
    v8::Handle<v8::FunctionTemplate> global_Ncurses_Window = v8::FunctionTemplate::New(__global_Ncurses_Window_Window);
    global_Ncurses_Window->SetClassName(String("Window"));
    global_Ncurses_Window->PrototypeTemplate()->Set("bold", Function(__global_Ncurses_Window_bold, "bold"));
    global_Ncurses_Window->PrototypeTemplate()->Set("border", Function(__global_Ncurses_Window_border, "border"));
    global_Ncurses_Window->PrototypeTemplate()->Set("setBackground", Function(__global_Ncurses_Window_setBackground, "setBackground"));
    global_Ncurses_Window->PrototypeTemplate()->Set("print", Function(__global_Ncurses_Window_print, "print"));
    global_Ncurses_Window->PrototypeTemplate()->Set("clear", Function(__global_Ncurses_Window_clear, "clear"));
    global_Ncurses_Window->PrototypeTemplate()->Set("touch", Function(__global_Ncurses_Window_touch, "touch"));
    global_Ncurses_Window->PrototypeTemplate()->Set("move", Function(__global_Ncurses_Window_move, "move"));
    global_Ncurses_Window->PrototypeTemplate()->Set("getChar", Function(__global_Ncurses_Window_getChar, "getChar"));
    global_Ncurses_Window->PrototypeTemplate()->Set("win", Value(0));
    global_Ncurses_Window->PrototypeTemplate()->Set("autoRefresh", Value(true));
    global_Ncurses->Set(String("Window"), global_Ncurses_Window->GetFunction());
    global_Ncurses->Set(String("enter"), Function(__global_Ncurses_enter, "enter"));
    global_Ncurses->Set(String("exit"), Function(__global_Ncurses_exit, "exit"));
    global_Ncurses->Set(String("cols"), Function(__global_Ncurses_cols, "cols"));
    global_Ncurses->Set(String("rows"), Function(__global_Ncurses_rows, "rows"));
    global_Ncurses->Set(String("toString"), Function(__global_Ncurses_toString, "toString"));
}