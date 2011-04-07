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
#include "Ncurses.h"

using namespace Gearbox;

/** \file Ncurses.cc converted from Ncurses.gear */

#line 1 "src/modules/Ncurses.gear"
#include <curses.h>

static v8::Handle<v8::Value> _Ncurses_Window_Window(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 4) {
        #line 28 "src/modules/Ncurses.gear"
        Value x(args[0]), y(args[1]), cols(args[2]), rows(args[3]);
        This["win"] = newwin(rows.to<int>(), cols.to<int>(), y.to<int>(), x.to<int>());
        scrollok(This["win"].to<WINDOW*>(), true);
        wtimeout(This["win"], 0);
        return undefined;
    }
    return Throw(Error("Invalid call to Ncurses.Window"));
}

static v8::Handle<v8::Value> _Ncurses_Window_bold(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 34 "src/modules/Ncurses.gear"
        Value on(args[0]);
        if(on.to<bool>())
            wattron(This["win"], A_BOLD);
        else
            wattroff(This["win"], A_BOLD);
        return undefined;
    }
    return Throw(Error("Invalid call to Ncurses.Window.prototype.bold"));
}

static v8::Handle<v8::Value> _Ncurses_Window_border(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 47 "src/modules/Ncurses.gear"
        Value obj(args[0]);
        wborder(This["win"], obj["Ls"].to<int>(), obj["Rs"].to<int>(), obj["Ts"].to<int>(), obj["Bs"].to<int>(), obj["TLc"].to<int>(), obj["TRc"].to<int>(), obj["BLc"].to<int>(), obj["BRc"].to<int>());
        if(This["autoRefresh"])
            wrefresh(This["win"]);
        return undefined;
    }

    #line 42 "src/modules/Ncurses.gear"
    wborder(This["win"], 0, 0, 0, 0, 0, 0, 0, 0);
    if(This["autoRefresh"])
        wrefresh(This["win"]);
    return undefined;
}

static v8::Handle<v8::Value> _Ncurses_Window_setBackground(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 53 "src/modules/Ncurses.gear"
        Value _char(args[0]);
        wbkgd(This["win"], _char.to<String>()[0]);
        if(This["autoRefresh"])
            wrefresh(This["win"]);
        return undefined;
    }
    return Throw(Error("Invalid call to Ncurses.Window.prototype.setBackground"));
}

static v8::Handle<v8::Value> _Ncurses_Window_print(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 59 "src/modules/Ncurses.gear"
        Value text(args[0]);
        waddstr(This["win"], text.to<String>());
        if(This["autoRefresh"])
            wrefresh(This["win"]);
        return undefined;
    }
    return Throw(Error("Invalid call to Ncurses.Window.prototype.print"));
}

static v8::Handle<v8::Value> _Ncurses_Window_clear(const v8::Arguments& args) {
    Value This(args.This());
    #line 66 "src/modules/Ncurses.gear"
    werase(This["win"]);
    if(This["autoRefresh"])
        wrefresh(This["win"]);
    return undefined;
}

static v8::Handle<v8::Value> _Ncurses_Window_touch(const v8::Arguments& args) {
    Value This(args.This());
    #line 72 "src/modules/Ncurses.gear"
    touchwin(This["win"].to<WINDOW*>());
    if(This["autoRefresh"])
        wrefresh(This["win"]);
    return undefined;
}

static v8::Handle<v8::Value> _Ncurses_Window_move(const v8::Arguments& args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 77 "src/modules/Ncurses.gear"
        Value x(args[0]), y(args[1]);
        wmove(This["win"], y.to<int>(), x.to<int>());
        if(This["autoRefresh"])
            wrefresh(This["win"]);
        return undefined;
    }
    return Throw(Error("Invalid call to Ncurses.Window.prototype.move"));
}

static v8::Handle<v8::Value> _Ncurses_Window_getChar(const v8::Arguments& args) {
    Value This(args.This());
    #line 84 "src/modules/Ncurses.gear"
    int c = wgetch(This["win"]);
    if(c > 0)
        return Integer(c);
    return undefined;
}

static v8::Handle<v8::Value> _Ncurses_enter(const v8::Arguments& args) {
    #line 91 "src/modules/Ncurses.gear"
    initscr();
    scrollok(stdscr, true);
    timeout(0);
    cbreak();
    noecho();
    return undefined;
}

static v8::Handle<v8::Value> _Ncurses_exit(const v8::Arguments& args) {
    #line 99 "src/modules/Ncurses.gear"
    endwin();
    return undefined;
}

static v8::Handle<v8::Value> _Ncurses_cols(const v8::Arguments& args) {
    #line 103 "src/modules/Ncurses.gear"
    return Integer(getmaxx(stdscr));
}

static v8::Handle<v8::Value> _Ncurses_rows(const v8::Arguments& args) {
    #line 107 "src/modules/Ncurses.gear"
    return Integer(getmaxy(stdscr));
}

static v8::Handle<v8::Value> _Ncurses_toString(const v8::Arguments& args) {
    #line 24 "src/modules/Ncurses.gear"
    return String("[module Ncurses]");
}


#line 170 "src/modules/Ncurses.cc"
static void _setup_Ncurses(Value _exports) {
    v8::Handle<v8::FunctionTemplate> _Ncurses_Window = v8::FunctionTemplate::New(_Ncurses_Window_Window);
    _Ncurses_Window->SetClassName(String("Window"));
    _Ncurses_Window->PrototypeTemplate()->Set("bold", Function(_Ncurses_Window_bold, "bold"));
    _Ncurses_Window->PrototypeTemplate()->Set("border", Function(_Ncurses_Window_border, "border"));
    _Ncurses_Window->PrototypeTemplate()->Set("setBackground", Function(_Ncurses_Window_setBackground, "setBackground"));
    _Ncurses_Window->PrototypeTemplate()->Set("print", Function(_Ncurses_Window_print, "print"));
    _Ncurses_Window->PrototypeTemplate()->Set("clear", Function(_Ncurses_Window_clear, "clear"));
    _Ncurses_Window->PrototypeTemplate()->Set("touch", Function(_Ncurses_Window_touch, "touch"));
    _Ncurses_Window->PrototypeTemplate()->Set("move", Function(_Ncurses_Window_move, "move"));
    _Ncurses_Window->PrototypeTemplate()->Set("getChar", Function(_Ncurses_Window_getChar, "getChar"));
    _Ncurses_Window->PrototypeTemplate()->Set("win", Value(0));
    _Ncurses_Window->PrototypeTemplate()->Set("autoRefresh", Value(true));
    _exports["Window"] = _Ncurses_Window->GetFunction();
    _exports["enter"] = Function(_Ncurses_enter, "enter");
    _exports["exit"] = Function(_Ncurses_exit, "exit");
    _exports["cols"] = Function(_Ncurses_cols, "cols");
    _exports["rows"] = Function(_Ncurses_rows, "rows");
    _exports["toString"] = Function(_Ncurses_toString, "toString");
}
static Module _module_Ncurses("Ncurses", _setup_Ncurses);