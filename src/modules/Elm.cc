/*
 * Copyright (c) 2011 Sanjeev B.A.
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
#include "Elm.h"

using namespace Gearbox;

/** \file Elm.cc converted from Elm.gear */

#line 1 "src/modules/Elm.gear"
#include <Elementary.h>

void close(void *data, Evas_Object *obj, void *event) {
    elm_exit();
}

void elmOnClick(void *_this, Evas_Object *eo, void *event_info) {
    (*reinterpret_cast<Value*>(_this))["_onClick"]();
}

static v8::Handle<v8::Value> _Elm_EvasObject_EvasObject(const v8::Arguments &args) {
    Value This(args.This());
    #line 45 "src/modules/Elm.gear"
    
    return undefined;
}

static v8::Handle<v8::Value> _Elm_EvasObject_x(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 47 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(x);
}

static void _Elm_EvasObject_x(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 52 "src/modules/Elm.gear"
    Value _x(__x);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_EvasObject_y(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 59 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(y);
}

static void _Elm_EvasObject_y(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 64 "src/modules/Elm.gear"
    Value _y(__y);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_EvasObject_height(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 71 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(height);
}

static void _Elm_EvasObject_height(v8::Local<v8::String>, v8::Local<v8::Value> __height, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 76 "src/modules/Elm.gear"
    Value _height(__height);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], width, _height);
}

static v8::Handle<v8::Value> _Elm_EvasObject_width(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 83 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(width);
}

static void _Elm_EvasObject_width(v8::Local<v8::String>, v8::Local<v8::Value> __width, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 88 "src/modules/Elm.gear"
    Value _width(__width);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], _width, height);
}

static v8::Handle<v8::Value> _Elm_EvasObject_label(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 95 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_EvasObject_label(v8::Local<v8::String>, v8::Local<v8::Value> _label, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 98 "src/modules/Elm.gear"
    Value label(_label);
    elm_object_text_set(This["eo"], label.to<String>());
}

static v8::Handle<v8::Value> _Elm_EvasObject_text(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 103 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_EvasObject_text(v8::Local<v8::String>, v8::Local<v8::Value> _text, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 106 "src/modules/Elm.gear"
    Value text(_text);
    elm_object_text_set(This["eo"], text.to<String>());
}

static v8::Handle<v8::Value> _Elm_EvasObject_scale(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 111 "src/modules/Elm.gear"
    return Number(elm_object_scale_get(This["eo"]));
}

static void _Elm_EvasObject_scale(v8::Local<v8::String>, v8::Local<v8::Value> _scale, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 114 "src/modules/Elm.gear"
    Value scale(_scale);
    elm_object_scale_set(This["eo"], scale);
}

static v8::Handle<v8::Value> _Elm_EvasObject_xalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 119 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(x);
}

static void _Elm_EvasObject_xalign(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 124 "src/modules/Elm.gear"
    Value _x(__x);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_EvasObject_yalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 131 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(y);
}

static void _Elm_EvasObject_yalign(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 136 "src/modules/Elm.gear"
    Value _y(__y);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_EvasObject_style(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 143 "src/modules/Elm.gear"
    return String(elm_object_style_get(This["eo"]));
}

static void _Elm_EvasObject_style(v8::Local<v8::String>, v8::Local<v8::Value> _style, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 146 "src/modules/Elm.gear"
    Value style(_style);
    elm_object_style_set(This["eo"], style.to<String>());
}

static v8::Handle<v8::Value> _Elm_EvasObject_visible(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 151 "src/modules/Elm.gear"
    return Value(evas_object_visible_get(This["eo"]));
}

static void _Elm_EvasObject_visible(v8::Local<v8::String>, v8::Local<v8::Value> _visible, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 154 "src/modules/Elm.gear"
    Value visible(_visible);
    if(visible)
        evas_object_show(This["eo"]);
    else
        evas_object_hide(This["eo"]);
}

static v8::Handle<v8::Value> _Elm_Win_Win(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 165 "src/modules/Elm.gear"
        Value parent(args[0]), title(args[1]);
        This["eo"] = elm_win_add(NULL, title.to<String>(), ELM_WIN_BASIC);
        evas_object_smart_callback_add(This["eo"], "delete,request", close, NULL);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to Elm.Win");
}

static v8::Handle<v8::Value> _Elm_Win_resize(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 171 "src/modules/Elm.gear"
        Value width(args[0]), height(args[1]);
        evas_object_resize(This["eo"], width, height);
        return undefined;
    }
    THROW_ERROR("Invalid call to Elm.Win.prototype.resize");
}

static v8::Handle<v8::Value> _Elm_Win_x(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 47 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(x);
}

static void _Elm_Win_x(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 52 "src/modules/Elm.gear"
    Value _x(__x);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Win_y(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 59 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(y);
}

static void _Elm_Win_y(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 64 "src/modules/Elm.gear"
    Value _y(__y);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Win_height(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 71 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(height);
}

static void _Elm_Win_height(v8::Local<v8::String>, v8::Local<v8::Value> __height, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 76 "src/modules/Elm.gear"
    Value _height(__height);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], width, _height);
}

static v8::Handle<v8::Value> _Elm_Win_width(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 83 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(width);
}

static void _Elm_Win_width(v8::Local<v8::String>, v8::Local<v8::Value> __width, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 88 "src/modules/Elm.gear"
    Value _width(__width);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], _width, height);
}

static v8::Handle<v8::Value> _Elm_Win_label(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 95 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Win_label(v8::Local<v8::String>, v8::Local<v8::Value> _label, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 98 "src/modules/Elm.gear"
    Value label(_label);
    elm_object_text_set(This["eo"], label.to<String>());
}

static v8::Handle<v8::Value> _Elm_Win_text(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 103 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Win_text(v8::Local<v8::String>, v8::Local<v8::Value> _text, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 106 "src/modules/Elm.gear"
    Value text(_text);
    elm_object_text_set(This["eo"], text.to<String>());
}

static v8::Handle<v8::Value> _Elm_Win_scale(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 111 "src/modules/Elm.gear"
    return Number(elm_object_scale_get(This["eo"]));
}

static void _Elm_Win_scale(v8::Local<v8::String>, v8::Local<v8::Value> _scale, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 114 "src/modules/Elm.gear"
    Value scale(_scale);
    elm_object_scale_set(This["eo"], scale);
}

static v8::Handle<v8::Value> _Elm_Win_xalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 119 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(x);
}

static void _Elm_Win_xalign(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 124 "src/modules/Elm.gear"
    Value _x(__x);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Win_yalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 131 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(y);
}

static void _Elm_Win_yalign(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 136 "src/modules/Elm.gear"
    Value _y(__y);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Win_style(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 143 "src/modules/Elm.gear"
    return String(elm_object_style_get(This["eo"]));
}

static void _Elm_Win_style(v8::Local<v8::String>, v8::Local<v8::Value> _style, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 146 "src/modules/Elm.gear"
    Value style(_style);
    elm_object_style_set(This["eo"], style.to<String>());
}

static v8::Handle<v8::Value> _Elm_Win_visible(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 151 "src/modules/Elm.gear"
    return Value(evas_object_visible_get(This["eo"]));
}

static void _Elm_Win_visible(v8::Local<v8::String>, v8::Local<v8::Value> _visible, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 154 "src/modules/Elm.gear"
    Value visible(_visible);
    if(visible)
        evas_object_show(This["eo"]);
    else
        evas_object_hide(This["eo"]);
}

static v8::Handle<v8::Value> _Elm_Win_title(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 176 "src/modules/Elm.gear"
    return String(elm_win_title_get(This["eo"]));
}

static void _Elm_Win_title(v8::Local<v8::String>, v8::Local<v8::Value> _title, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 179 "src/modules/Elm.gear"
    Value title(_title);
    elm_win_title_set(This["eo"], title.to<String>());
}

static v8::Handle<v8::Value> _Elm_Win_autodel(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 184 "src/modules/Elm.gear"
    bool autodel = elm_win_autodel_get(This["eo"]);
    return Value(autodel);
}

static void _Elm_Win_autodel(v8::Local<v8::String>, v8::Local<v8::Value> _autodel, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 188 "src/modules/Elm.gear"
    Value autodel(_autodel);
    elm_win_autodel_set(This["eo"], autodel);
}

static v8::Handle<v8::Value> _Elm_Background_Background(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 3) {
        #line 196 "src/modules/Elm.gear"
        Value parent(args[0]), file(args[1]), group(args[2]);
        This["eo"] = elm_bg_add(parent["eo"]);
        elm_bg_file_set(This["eo"], file.to<String>(), group.to<String>());
        evas_object_size_hint_weight_set(This["eo"], EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_win_resize_object_add(parent["eo"], This["eo"]);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to Elm.Background");
}

static v8::Handle<v8::Value> _Elm_Background_x(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 47 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(x);
}

static void _Elm_Background_x(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 52 "src/modules/Elm.gear"
    Value _x(__x);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Background_y(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 59 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(y);
}

static void _Elm_Background_y(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 64 "src/modules/Elm.gear"
    Value _y(__y);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Background_height(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 71 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(height);
}

static void _Elm_Background_height(v8::Local<v8::String>, v8::Local<v8::Value> __height, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 76 "src/modules/Elm.gear"
    Value _height(__height);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], width, _height);
}

static v8::Handle<v8::Value> _Elm_Background_width(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 83 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(width);
}

static void _Elm_Background_width(v8::Local<v8::String>, v8::Local<v8::Value> __width, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 88 "src/modules/Elm.gear"
    Value _width(__width);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], _width, height);
}

static v8::Handle<v8::Value> _Elm_Background_label(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 95 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Background_label(v8::Local<v8::String>, v8::Local<v8::Value> _label, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 98 "src/modules/Elm.gear"
    Value label(_label);
    elm_object_text_set(This["eo"], label.to<String>());
}

static v8::Handle<v8::Value> _Elm_Background_text(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 103 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Background_text(v8::Local<v8::String>, v8::Local<v8::Value> _text, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 106 "src/modules/Elm.gear"
    Value text(_text);
    elm_object_text_set(This["eo"], text.to<String>());
}

static v8::Handle<v8::Value> _Elm_Background_scale(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 111 "src/modules/Elm.gear"
    return Number(elm_object_scale_get(This["eo"]));
}

static void _Elm_Background_scale(v8::Local<v8::String>, v8::Local<v8::Value> _scale, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 114 "src/modules/Elm.gear"
    Value scale(_scale);
    elm_object_scale_set(This["eo"], scale);
}

static v8::Handle<v8::Value> _Elm_Background_xalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 119 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(x);
}

static void _Elm_Background_xalign(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 124 "src/modules/Elm.gear"
    Value _x(__x);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Background_yalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 131 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(y);
}

static void _Elm_Background_yalign(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 136 "src/modules/Elm.gear"
    Value _y(__y);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Background_style(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 143 "src/modules/Elm.gear"
    return String(elm_object_style_get(This["eo"]));
}

static void _Elm_Background_style(v8::Local<v8::String>, v8::Local<v8::Value> _style, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 146 "src/modules/Elm.gear"
    Value style(_style);
    elm_object_style_set(This["eo"], style.to<String>());
}

static v8::Handle<v8::Value> _Elm_Background_visible(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 151 "src/modules/Elm.gear"
    return Value(evas_object_visible_get(This["eo"]));
}

static void _Elm_Background_visible(v8::Local<v8::String>, v8::Local<v8::Value> _visible, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 154 "src/modules/Elm.gear"
    Value visible(_visible);
    if(visible)
        evas_object_show(This["eo"]);
    else
        evas_object_hide(This["eo"]);
}

static v8::Handle<v8::Value> _Elm_Background_red(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 205 "src/modules/Elm.gear"
    int r;
    elm_bg_color_get(This["eo"], &r, NULL, NULL);
    return Integer(r);
}

static void _Elm_Background_red(v8::Local<v8::String>, v8::Local<v8::Value> __r, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 210 "src/modules/Elm.gear"
    Value _r(__r);
    int r, g, b;
    elm_bg_color_get(This["eo"], &r, &g, &b);
    elm_bg_color_set(This["eo"], _r, g, b);
}

static v8::Handle<v8::Value> _Elm_Background_green(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 217 "src/modules/Elm.gear"
    int g;
    elm_bg_color_get(This["eo"], NULL, NULL, &g);
    return Integer(g);
}

static void _Elm_Background_green(v8::Local<v8::String>, v8::Local<v8::Value> __g, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 222 "src/modules/Elm.gear"
    Value _g(__g);
    int r, g, b;
    elm_bg_color_get(This["eo"], &r, &g, &b);
    elm_bg_color_set(This["eo"], r, _g, b);
}

static v8::Handle<v8::Value> _Elm_Background_blue(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 229 "src/modules/Elm.gear"
    int b;
    elm_bg_color_get(This["eo"], NULL, &b, NULL);
    return Integer(b);
}

static void _Elm_Background_blue(v8::Local<v8::String>, v8::Local<v8::Value> __b, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 234 "src/modules/Elm.gear"
    Value _b(__b);
    int r, g, b;
    elm_bg_color_get(This["eo"], &r, &g, &b);
    elm_bg_color_set(This["eo"], r, g, _b);
}

static v8::Handle<v8::Value> _Elm_Background_file(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 241 "src/modules/Elm.gear"
    const char *file, *group;
    elm_bg_file_get(This["eo"], &file, &group);
    return String(file);
}

static void _Elm_Background_file(v8::Local<v8::String>, v8::Local<v8::Value> _file, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 246 "src/modules/Elm.gear"
    Value file(_file);
    elm_bg_file_set(This["eo"], file.to<String>(), NULL);
}

static v8::Handle<v8::Value> _Elm_Background_group(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 251 "src/modules/Elm.gear"
    const char *file, *group;
    elm_bg_file_get(This["eo"], &file, &group);
    return String(group);
}

static void _Elm_Background_group(v8::Local<v8::String>, v8::Local<v8::Value> _group, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 256 "src/modules/Elm.gear"
    Value group(_group);
    elm_bg_file_set(This["eo"], NULL, group.to<String>());
}

static v8::Handle<v8::Value> _Elm_Box_Box(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 263 "src/modules/Elm.gear"
        Value parent(args[0]);
        This["eo"] = elm_box_add(parent["eo"]);
        evas_object_size_hint_weight_set(This["eo"], EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to Elm.Box");
}

static v8::Handle<v8::Value> _Elm_Box_add(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 269 "src/modules/Elm.gear"
        Value child(args[0]);
        elm_box_pack_end(This["eo"], child["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to Elm.Box.prototype.add");
}

static v8::Handle<v8::Value> _Elm_Box_x(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 47 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(x);
}

static void _Elm_Box_x(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 52 "src/modules/Elm.gear"
    Value _x(__x);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Box_y(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 59 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(y);
}

static void _Elm_Box_y(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 64 "src/modules/Elm.gear"
    Value _y(__y);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Box_height(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 71 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(height);
}

static void _Elm_Box_height(v8::Local<v8::String>, v8::Local<v8::Value> __height, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 76 "src/modules/Elm.gear"
    Value _height(__height);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], width, _height);
}

static v8::Handle<v8::Value> _Elm_Box_width(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 83 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(width);
}

static void _Elm_Box_width(v8::Local<v8::String>, v8::Local<v8::Value> __width, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 88 "src/modules/Elm.gear"
    Value _width(__width);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], _width, height);
}

static v8::Handle<v8::Value> _Elm_Box_label(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 95 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Box_label(v8::Local<v8::String>, v8::Local<v8::Value> _label, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 98 "src/modules/Elm.gear"
    Value label(_label);
    elm_object_text_set(This["eo"], label.to<String>());
}

static v8::Handle<v8::Value> _Elm_Box_text(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 103 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Box_text(v8::Local<v8::String>, v8::Local<v8::Value> _text, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 106 "src/modules/Elm.gear"
    Value text(_text);
    elm_object_text_set(This["eo"], text.to<String>());
}

static v8::Handle<v8::Value> _Elm_Box_scale(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 111 "src/modules/Elm.gear"
    return Number(elm_object_scale_get(This["eo"]));
}

static void _Elm_Box_scale(v8::Local<v8::String>, v8::Local<v8::Value> _scale, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 114 "src/modules/Elm.gear"
    Value scale(_scale);
    elm_object_scale_set(This["eo"], scale);
}

static v8::Handle<v8::Value> _Elm_Box_xalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 119 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(x);
}

static void _Elm_Box_xalign(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 124 "src/modules/Elm.gear"
    Value _x(__x);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Box_yalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 131 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(y);
}

static void _Elm_Box_yalign(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 136 "src/modules/Elm.gear"
    Value _y(__y);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Box_style(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 143 "src/modules/Elm.gear"
    return String(elm_object_style_get(This["eo"]));
}

static void _Elm_Box_style(v8::Local<v8::String>, v8::Local<v8::Value> _style, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 146 "src/modules/Elm.gear"
    Value style(_style);
    elm_object_style_set(This["eo"], style.to<String>());
}

static v8::Handle<v8::Value> _Elm_Box_visible(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 151 "src/modules/Elm.gear"
    return Value(evas_object_visible_get(This["eo"]));
}

static void _Elm_Box_visible(v8::Local<v8::String>, v8::Local<v8::Value> _visible, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 154 "src/modules/Elm.gear"
    Value visible(_visible);
    if(visible)
        evas_object_show(This["eo"]);
    else
        evas_object_hide(This["eo"]);
}

static v8::Handle<v8::Value> _Elm_Box_homogenous(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 274 "src/modules/Elm.gear"
    return Value(elm_box_homogeneous_get(This["eo"]));
}

static void _Elm_Box_homogenous(v8::Local<v8::String>, v8::Local<v8::Value> _homogenous, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 277 "src/modules/Elm.gear"
    Value homogenous(_homogenous);
    elm_box_homogeneous_set(This["eo"], homogenous);
}

static v8::Handle<v8::Value> _Elm_Box_horizontal(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 282 "src/modules/Elm.gear"
    return Value(elm_box_horizontal_get(This["eo"]));
}

static void _Elm_Box_horizontal(v8::Local<v8::String>, v8::Local<v8::Value> _horizontal, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 285 "src/modules/Elm.gear"
    Value horizontal(_horizontal);
    elm_box_horizontal_set(This["eo"], horizontal);
}

static v8::Handle<v8::Value> _Elm_Icon_Icon(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 292 "src/modules/Elm.gear"
        Value parent(args[0]), file(args[1]);
        This["eo"] = elm_icon_add(parent["eo"]);
        elm_icon_file_set(This["eo"], file.to<String>(), NULL);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to Elm.Icon");
}

static v8::Handle<v8::Value> _Elm_Icon_x(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 47 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(x);
}

static void _Elm_Icon_x(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 52 "src/modules/Elm.gear"
    Value _x(__x);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Icon_y(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 59 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(y);
}

static void _Elm_Icon_y(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 64 "src/modules/Elm.gear"
    Value _y(__y);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Icon_height(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 71 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(height);
}

static void _Elm_Icon_height(v8::Local<v8::String>, v8::Local<v8::Value> __height, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 76 "src/modules/Elm.gear"
    Value _height(__height);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], width, _height);
}

static v8::Handle<v8::Value> _Elm_Icon_width(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 83 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(width);
}

static void _Elm_Icon_width(v8::Local<v8::String>, v8::Local<v8::Value> __width, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 88 "src/modules/Elm.gear"
    Value _width(__width);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], _width, height);
}

static v8::Handle<v8::Value> _Elm_Icon_label(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 95 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Icon_label(v8::Local<v8::String>, v8::Local<v8::Value> _label, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 98 "src/modules/Elm.gear"
    Value label(_label);
    elm_object_text_set(This["eo"], label.to<String>());
}

static v8::Handle<v8::Value> _Elm_Icon_text(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 103 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Icon_text(v8::Local<v8::String>, v8::Local<v8::Value> _text, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 106 "src/modules/Elm.gear"
    Value text(_text);
    elm_object_text_set(This["eo"], text.to<String>());
}

static v8::Handle<v8::Value> _Elm_Icon_scale(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 111 "src/modules/Elm.gear"
    return Number(elm_object_scale_get(This["eo"]));
}

static void _Elm_Icon_scale(v8::Local<v8::String>, v8::Local<v8::Value> _scale, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 114 "src/modules/Elm.gear"
    Value scale(_scale);
    elm_object_scale_set(This["eo"], scale);
}

static v8::Handle<v8::Value> _Elm_Icon_xalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 119 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(x);
}

static void _Elm_Icon_xalign(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 124 "src/modules/Elm.gear"
    Value _x(__x);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Icon_yalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 131 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(y);
}

static void _Elm_Icon_yalign(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 136 "src/modules/Elm.gear"
    Value _y(__y);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Icon_style(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 143 "src/modules/Elm.gear"
    return String(elm_object_style_get(This["eo"]));
}

static void _Elm_Icon_style(v8::Local<v8::String>, v8::Local<v8::Value> _style, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 146 "src/modules/Elm.gear"
    Value style(_style);
    elm_object_style_set(This["eo"], style.to<String>());
}

static v8::Handle<v8::Value> _Elm_Icon_visible(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 151 "src/modules/Elm.gear"
    return Value(evas_object_visible_get(This["eo"]));
}

static void _Elm_Icon_visible(v8::Local<v8::String>, v8::Local<v8::Value> _visible, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 154 "src/modules/Elm.gear"
    Value visible(_visible);
    if(visible)
        evas_object_show(This["eo"]);
    else
        evas_object_hide(This["eo"]);
}

static v8::Handle<v8::Value> _Elm_Icon_file(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 299 "src/modules/Elm.gear"
    const char *file, *group;
    elm_icon_file_get(This["eo"], &file, &group);
    return String(file);
}

static void _Elm_Icon_file(v8::Local<v8::String>, v8::Local<v8::Value> _file, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 304 "src/modules/Elm.gear"
    Value file(_file);
    elm_icon_file_set(This["eo"], file.to<String>(), NULL);
}

static v8::Handle<v8::Value> _Elm_Button_Button(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 312 "src/modules/Elm.gear"
        Value parent(args[0]);
        This["eo"] = elm_button_add(parent["eo"]);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to Elm.Button");
}

static v8::Handle<v8::Value> _Elm_Button_x(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 47 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(x);
}

static void _Elm_Button_x(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 52 "src/modules/Elm.gear"
    Value _x(__x);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Button_y(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 59 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(y);
}

static void _Elm_Button_y(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 64 "src/modules/Elm.gear"
    Value _y(__y);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Button_height(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 71 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(height);
}

static void _Elm_Button_height(v8::Local<v8::String>, v8::Local<v8::Value> __height, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 76 "src/modules/Elm.gear"
    Value _height(__height);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], width, _height);
}

static v8::Handle<v8::Value> _Elm_Button_width(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 83 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(width);
}

static void _Elm_Button_width(v8::Local<v8::String>, v8::Local<v8::Value> __width, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 88 "src/modules/Elm.gear"
    Value _width(__width);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], _width, height);
}

static v8::Handle<v8::Value> _Elm_Button_label(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 95 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Button_label(v8::Local<v8::String>, v8::Local<v8::Value> _label, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 98 "src/modules/Elm.gear"
    Value label(_label);
    elm_object_text_set(This["eo"], label.to<String>());
}

static v8::Handle<v8::Value> _Elm_Button_text(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 103 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Button_text(v8::Local<v8::String>, v8::Local<v8::Value> _text, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 106 "src/modules/Elm.gear"
    Value text(_text);
    elm_object_text_set(This["eo"], text.to<String>());
}

static v8::Handle<v8::Value> _Elm_Button_scale(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 111 "src/modules/Elm.gear"
    return Number(elm_object_scale_get(This["eo"]));
}

static void _Elm_Button_scale(v8::Local<v8::String>, v8::Local<v8::Value> _scale, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 114 "src/modules/Elm.gear"
    Value scale(_scale);
    elm_object_scale_set(This["eo"], scale);
}

static v8::Handle<v8::Value> _Elm_Button_xalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 119 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(x);
}

static void _Elm_Button_xalign(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 124 "src/modules/Elm.gear"
    Value _x(__x);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Button_yalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 131 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(y);
}

static void _Elm_Button_yalign(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 136 "src/modules/Elm.gear"
    Value _y(__y);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Button_style(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 143 "src/modules/Elm.gear"
    return String(elm_object_style_get(This["eo"]));
}

static void _Elm_Button_style(v8::Local<v8::String>, v8::Local<v8::Value> _style, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 146 "src/modules/Elm.gear"
    Value style(_style);
    elm_object_style_set(This["eo"], style.to<String>());
}

static v8::Handle<v8::Value> _Elm_Button_visible(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 151 "src/modules/Elm.gear"
    return Value(evas_object_visible_get(This["eo"]));
}

static void _Elm_Button_visible(v8::Local<v8::String>, v8::Local<v8::Value> _visible, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 154 "src/modules/Elm.gear"
    Value visible(_visible);
    if(visible)
        evas_object_show(This["eo"]);
    else
        evas_object_hide(This["eo"]);
}

static v8::Handle<v8::Value> _Elm_Button_onClick(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 323 "src/modules/Elm.gear"
    return This["_onClick"];
}

static void _Elm_Button_onClick(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 317 "src/modules/Elm.gear"
    Value arg(_arg);
    This["_onClick"] = arg;
    evas_object_smart_callback_add(This["eo"], "clicked", elmOnClick, new Value(This));
}

static v8::Handle<v8::Value> _Elm_mainLoop(const v8::Arguments &args) {
    #line 34 "src/modules/Elm.gear"
    ecore_main_loop_begin();
    return undefined;
}

static v8::Handle<v8::Value> _Elm_toString(const v8::Arguments &args) {
    #line 32 "src/modules/Elm.gear"
    return String("[module Elm]");
}


#line 1325 "src/modules/Elm.cc"
static void _setup_Elm(Value _exports) {
    v8::Handle<v8::FunctionTemplate> _Elm_EvasObject = v8::FunctionTemplate::New(_Elm_EvasObject_EvasObject);
    _Elm_EvasObject->SetClassName(String("EvasObject"));
    _Elm_EvasObject->PrototypeTemplate()->SetAccessor(String("x"), _Elm_EvasObject_x, _Elm_EvasObject_x);
    _Elm_EvasObject->PrototypeTemplate()->SetAccessor(String("y"), _Elm_EvasObject_y, _Elm_EvasObject_y);
    _Elm_EvasObject->PrototypeTemplate()->SetAccessor(String("height"), _Elm_EvasObject_height, _Elm_EvasObject_height);
    _Elm_EvasObject->PrototypeTemplate()->SetAccessor(String("width"), _Elm_EvasObject_width, _Elm_EvasObject_width);
    _Elm_EvasObject->PrototypeTemplate()->SetAccessor(String("label"), _Elm_EvasObject_label, _Elm_EvasObject_label);
    _Elm_EvasObject->PrototypeTemplate()->SetAccessor(String("text"), _Elm_EvasObject_text, _Elm_EvasObject_text);
    _Elm_EvasObject->PrototypeTemplate()->SetAccessor(String("scale"), _Elm_EvasObject_scale, _Elm_EvasObject_scale);
    _Elm_EvasObject->PrototypeTemplate()->SetAccessor(String("xalign"), _Elm_EvasObject_xalign, _Elm_EvasObject_xalign);
    _Elm_EvasObject->PrototypeTemplate()->SetAccessor(String("yalign"), _Elm_EvasObject_yalign, _Elm_EvasObject_yalign);
    _Elm_EvasObject->PrototypeTemplate()->SetAccessor(String("style"), _Elm_EvasObject_style, _Elm_EvasObject_style);
    _Elm_EvasObject->PrototypeTemplate()->SetAccessor(String("visible"), _Elm_EvasObject_visible, _Elm_EvasObject_visible);
    _exports["EvasObject"] = _Elm_EvasObject->GetFunction();
    v8::Handle<v8::FunctionTemplate> _Elm_Win = v8::FunctionTemplate::New(_Elm_Win_Win);
    _Elm_Win->SetClassName(String("Win"));
    _Elm_Win->PrototypeTemplate()->Set("resize", Function(_Elm_Win_resize, "resize"));
    _Elm_Win->PrototypeTemplate()->SetAccessor(String("x"), _Elm_Win_x, _Elm_Win_x);
    _Elm_Win->PrototypeTemplate()->SetAccessor(String("y"), _Elm_Win_y, _Elm_Win_y);
    _Elm_Win->PrototypeTemplate()->SetAccessor(String("height"), _Elm_Win_height, _Elm_Win_height);
    _Elm_Win->PrototypeTemplate()->SetAccessor(String("width"), _Elm_Win_width, _Elm_Win_width);
    _Elm_Win->PrototypeTemplate()->SetAccessor(String("label"), _Elm_Win_label, _Elm_Win_label);
    _Elm_Win->PrototypeTemplate()->SetAccessor(String("text"), _Elm_Win_text, _Elm_Win_text);
    _Elm_Win->PrototypeTemplate()->SetAccessor(String("scale"), _Elm_Win_scale, _Elm_Win_scale);
    _Elm_Win->PrototypeTemplate()->SetAccessor(String("xalign"), _Elm_Win_xalign, _Elm_Win_xalign);
    _Elm_Win->PrototypeTemplate()->SetAccessor(String("yalign"), _Elm_Win_yalign, _Elm_Win_yalign);
    _Elm_Win->PrototypeTemplate()->SetAccessor(String("style"), _Elm_Win_style, _Elm_Win_style);
    _Elm_Win->PrototypeTemplate()->SetAccessor(String("visible"), _Elm_Win_visible, _Elm_Win_visible);
    _Elm_Win->PrototypeTemplate()->SetAccessor(String("title"), _Elm_Win_title, _Elm_Win_title);
    _Elm_Win->PrototypeTemplate()->SetAccessor(String("autodel"), _Elm_Win_autodel, _Elm_Win_autodel);
    _exports["Win"] = _Elm_Win->GetFunction();
    v8::Handle<v8::FunctionTemplate> _Elm_Background = v8::FunctionTemplate::New(_Elm_Background_Background);
    _Elm_Background->SetClassName(String("Background"));
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("x"), _Elm_Background_x, _Elm_Background_x);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("y"), _Elm_Background_y, _Elm_Background_y);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("height"), _Elm_Background_height, _Elm_Background_height);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("width"), _Elm_Background_width, _Elm_Background_width);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("label"), _Elm_Background_label, _Elm_Background_label);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("text"), _Elm_Background_text, _Elm_Background_text);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("scale"), _Elm_Background_scale, _Elm_Background_scale);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("xalign"), _Elm_Background_xalign, _Elm_Background_xalign);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("yalign"), _Elm_Background_yalign, _Elm_Background_yalign);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("style"), _Elm_Background_style, _Elm_Background_style);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("visible"), _Elm_Background_visible, _Elm_Background_visible);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("red"), _Elm_Background_red, _Elm_Background_red);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("green"), _Elm_Background_green, _Elm_Background_green);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("blue"), _Elm_Background_blue, _Elm_Background_blue);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("file"), _Elm_Background_file, _Elm_Background_file);
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("group"), _Elm_Background_group, _Elm_Background_group);
    _exports["Background"] = _Elm_Background->GetFunction();
    v8::Handle<v8::FunctionTemplate> _Elm_Box = v8::FunctionTemplate::New(_Elm_Box_Box);
    _Elm_Box->SetClassName(String("Box"));
    _Elm_Box->PrototypeTemplate()->Set("add", Function(_Elm_Box_add, "add"));
    _Elm_Box->PrototypeTemplate()->SetAccessor(String("x"), _Elm_Box_x, _Elm_Box_x);
    _Elm_Box->PrototypeTemplate()->SetAccessor(String("y"), _Elm_Box_y, _Elm_Box_y);
    _Elm_Box->PrototypeTemplate()->SetAccessor(String("height"), _Elm_Box_height, _Elm_Box_height);
    _Elm_Box->PrototypeTemplate()->SetAccessor(String("width"), _Elm_Box_width, _Elm_Box_width);
    _Elm_Box->PrototypeTemplate()->SetAccessor(String("label"), _Elm_Box_label, _Elm_Box_label);
    _Elm_Box->PrototypeTemplate()->SetAccessor(String("text"), _Elm_Box_text, _Elm_Box_text);
    _Elm_Box->PrototypeTemplate()->SetAccessor(String("scale"), _Elm_Box_scale, _Elm_Box_scale);
    _Elm_Box->PrototypeTemplate()->SetAccessor(String("xalign"), _Elm_Box_xalign, _Elm_Box_xalign);
    _Elm_Box->PrototypeTemplate()->SetAccessor(String("yalign"), _Elm_Box_yalign, _Elm_Box_yalign);
    _Elm_Box->PrototypeTemplate()->SetAccessor(String("style"), _Elm_Box_style, _Elm_Box_style);
    _Elm_Box->PrototypeTemplate()->SetAccessor(String("visible"), _Elm_Box_visible, _Elm_Box_visible);
    _Elm_Box->PrototypeTemplate()->SetAccessor(String("homogenous"), _Elm_Box_homogenous, _Elm_Box_homogenous);
    _Elm_Box->PrototypeTemplate()->SetAccessor(String("horizontal"), _Elm_Box_horizontal, _Elm_Box_horizontal);
    _exports["Box"] = _Elm_Box->GetFunction();
    v8::Handle<v8::FunctionTemplate> _Elm_Icon = v8::FunctionTemplate::New(_Elm_Icon_Icon);
    _Elm_Icon->SetClassName(String("Icon"));
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("x"), _Elm_Icon_x, _Elm_Icon_x);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("y"), _Elm_Icon_y, _Elm_Icon_y);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("height"), _Elm_Icon_height, _Elm_Icon_height);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("width"), _Elm_Icon_width, _Elm_Icon_width);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("label"), _Elm_Icon_label, _Elm_Icon_label);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("text"), _Elm_Icon_text, _Elm_Icon_text);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("scale"), _Elm_Icon_scale, _Elm_Icon_scale);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("xalign"), _Elm_Icon_xalign, _Elm_Icon_xalign);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("yalign"), _Elm_Icon_yalign, _Elm_Icon_yalign);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("style"), _Elm_Icon_style, _Elm_Icon_style);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("visible"), _Elm_Icon_visible, _Elm_Icon_visible);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("file"), _Elm_Icon_file, _Elm_Icon_file);
    _exports["Icon"] = _Elm_Icon->GetFunction();
    v8::Handle<v8::FunctionTemplate> _Elm_Button = v8::FunctionTemplate::New(_Elm_Button_Button);
    _Elm_Button->SetClassName(String("Button"));
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("x"), _Elm_Button_x, _Elm_Button_x);
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("y"), _Elm_Button_y, _Elm_Button_y);
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("height"), _Elm_Button_height, _Elm_Button_height);
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("width"), _Elm_Button_width, _Elm_Button_width);
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("label"), _Elm_Button_label, _Elm_Button_label);
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("text"), _Elm_Button_text, _Elm_Button_text);
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("scale"), _Elm_Button_scale, _Elm_Button_scale);
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("xalign"), _Elm_Button_xalign, _Elm_Button_xalign);
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("yalign"), _Elm_Button_yalign, _Elm_Button_yalign);
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("style"), _Elm_Button_style, _Elm_Button_style);
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("visible"), _Elm_Button_visible, _Elm_Button_visible);
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("onClick"), _Elm_Button_onClick, _Elm_Button_onClick);
    _exports["Button"] = _Elm_Button->GetFunction();
    _exports["mainLoop"] = Function(_Elm_mainLoop, "mainLoop");
    _exports["toString"] = Function(_Elm_toString, "toString");

        eina_init();
        ecore_init();
        elm_init(0, NULL);
    
}
static Module _module_Elm("Elm", _setup_Elm);