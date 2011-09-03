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
    bool _autodel = elm_win_autodel_get(This["eo"]);
    return Value(_autodel);
}

static void _Elm_Win_autodel(v8::Local<v8::String>, v8::Local<v8::Value> __autodel, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 188 "src/modules/Elm.gear"
    Value _autodel(__autodel);
    elm_win_autodel_set(This["eo"], _autodel);
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

static v8::Handle<v8::Value> _Elm_Background_bgOption(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 261 "src/modules/Elm.gear"
    int option;
    option = elm_bg_option_get(This["eo"]);
    return Integer(option);
}

static void _Elm_Background_bgOption(v8::Local<v8::String>, v8::Local<v8::Value> __option, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 266 "src/modules/Elm.gear"
    Value _option(__option);
    Elm_Bg_Option _opt = (Elm_Bg_Option)(_option.to<int>());
    elm_bg_option_set(This["eo"], _opt);
}

static v8::Handle<v8::Value> _Elm_Box_Box(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 273 "src/modules/Elm.gear"
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
        #line 279 "src/modules/Elm.gear"
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
    #line 284 "src/modules/Elm.gear"
    return Value(elm_box_homogeneous_get(This["eo"]));
}

static void _Elm_Box_homogenous(v8::Local<v8::String>, v8::Local<v8::Value> _homogenous, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 287 "src/modules/Elm.gear"
    Value homogenous(_homogenous);
    elm_box_homogeneous_set(This["eo"], homogenous);
}

static v8::Handle<v8::Value> _Elm_Box_horizontal(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 292 "src/modules/Elm.gear"
    return Value(elm_box_horizontal_get(This["eo"]));
}

static void _Elm_Box_horizontal(v8::Local<v8::String>, v8::Local<v8::Value> _horizontal, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 295 "src/modules/Elm.gear"
    Value horizontal(_horizontal);
    elm_box_horizontal_set(This["eo"], horizontal);
}

static v8::Handle<v8::Value> _Elm_Icon_Icon(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 302 "src/modules/Elm.gear"
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
    #line 309 "src/modules/Elm.gear"
    const char *file, *group;
    elm_icon_file_get(This["eo"], &file, &group);
    return String(file);
}

static void _Elm_Icon_file(v8::Local<v8::String>, v8::Local<v8::Value> _file, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 314 "src/modules/Elm.gear"
    Value file(_file);
    elm_icon_file_set(This["eo"], file.to<String>(), NULL);
}

static v8::Handle<v8::Value> _Elm_Icon_scaleDown(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 319 "src/modules/Elm.gear"
    Eina_Bool sd, su;
    elm_icon_scale_get(This["eo"], &su, &sd);
    return Integer(sd);
}

static void _Elm_Icon_scaleDown(v8::Local<v8::String>, v8::Local<v8::Value> __sd, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 324 "src/modules/Elm.gear"
    Value _sd(__sd);
    Eina_Bool sd, su;
    elm_icon_scale_get(This["eo"], &su, &sd);
    elm_icon_scale_set(This["eo"], su, _sd);
}

static v8::Handle<v8::Value> _Elm_Icon_scaleUp(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 331 "src/modules/Elm.gear"
    Eina_Bool sd, su;
    elm_icon_scale_get(This["eo"], &su, &sd);
    return Integer(su);
}

static void _Elm_Icon_scaleUp(v8::Local<v8::String>, v8::Local<v8::Value> __su, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 336 "src/modules/Elm.gear"
    Value _su(__su);
    Eina_Bool sd, su;
    elm_icon_scale_get(This["eo"], &su, &sd);
    elm_icon_scale_set(This["eo"], _su, sd);
}

static v8::Handle<v8::Value> _Elm_Icon_prescale(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 343 "src/modules/Elm.gear"
    return Integer(elm_icon_prescale_get(This["eo"]));
}

static void _Elm_Icon_prescale(v8::Local<v8::String>, v8::Local<v8::Value> __arit, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 346 "src/modules/Elm.gear"
    Value _arit(__arit);
    elm_icon_prescale_set(This["eo"], _arit);
}

static v8::Handle<v8::Value> _Elm_Icon_animated(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 351 "src/modules/Elm.gear"
    return Integer(elm_icon_animated_get(This["eo"]));
}

static void _Elm_Icon_animated(v8::Local<v8::String>, v8::Local<v8::Value> __anim, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 354 "src/modules/Elm.gear"
    Value _anim(__anim);
    elm_icon_animated_set(This["eo"], _anim);
}

static v8::Handle<v8::Value> _Elm_Icon_animPlay(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 359 "src/modules/Elm.gear"
    return Integer(elm_icon_animated_play_get(This["eo"]));
}

static void _Elm_Icon_animPlay(v8::Local<v8::String>, v8::Local<v8::Value> __ap, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 362 "src/modules/Elm.gear"
    Value _ap(__ap);
    elm_icon_animated_play_set(This["eo"], _ap);
}

static v8::Handle<v8::Value> _Elm_Icon_smooth(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 367 "src/modules/Elm.gear"
    return Integer(elm_icon_smooth_get(This["eo"]));
}

static void _Elm_Icon_smooth(v8::Local<v8::String>, v8::Local<v8::Value> __smooth, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 370 "src/modules/Elm.gear"
    Value _smooth(__smooth);
    elm_icon_smooth_set(This["eo"], _smooth);
}

static v8::Handle<v8::Value> _Elm_Icon_standard(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 375 "src/modules/Elm.gear"
    return String(elm_icon_standard_get(This["eo"]));
}

static void _Elm_Icon_standard(v8::Local<v8::String>, v8::Local<v8::Value> __std, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 378 "src/modules/Elm.gear"
    Value _std(__std);
    elm_icon_standard_set(This["eo"],_std);
}

static v8::Handle<v8::Value> _Elm_Icon_fillOutside(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 383 "src/modules/Elm.gear"
    return Integer(elm_icon_fill_outside_get(This["eo"]));
}

static void _Elm_Icon_fillOutside(v8::Local<v8::String>, v8::Local<v8::Value> __fo, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 386 "src/modules/Elm.gear"
    Value _fo(__fo);
    elm_icon_fill_outside_set(This["eo"], _fo);
}

static v8::Handle<v8::Value> _Elm_Button_Button(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 394 "src/modules/Elm.gear"
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

static v8::Handle<v8::Value> _Elm_Button_autorepeat(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 400 "src/modules/Elm.gear"
    bool autorepeat = elm_button_autorepeat_get(This["eo"]);
    return Integer(autorepeat);
}

static void _Elm_Button_autorepeat(v8::Local<v8::String>, v8::Local<v8::Value> _autorepeat, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 404 "src/modules/Elm.gear"
    Value autorepeat(_autorepeat);
    elm_button_autorepeat_set(This["eo"], autorepeat);
}

static v8::Handle<v8::Value> _Elm_Button_autorepeatInitTimeout(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 409 "src/modules/Elm.gear"
    double arit = elm_button_autorepeat_initial_timeout_get(This["eo"]);
    return Number(arit);
}

static void _Elm_Button_autorepeatInitTimeout(v8::Local<v8::String>, v8::Local<v8::Value> __arit, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 413 "src/modules/Elm.gear"
    Value _arit(__arit);
    elm_button_autorepeat_initial_timeout_set(This["eo"], _arit);
}

static v8::Handle<v8::Value> _Elm_Button_autorepeatGapTimeout(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 418 "src/modules/Elm.gear"
    double argt = elm_button_autorepeat_gap_timeout_get(This["eo"]);
    return Number(argt);
}

static void _Elm_Button_autorepeatGapTimeout(v8::Local<v8::String>, v8::Local<v8::Value> __argt, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 422 "src/modules/Elm.gear"
    Value _argt(__argt);
    elm_button_autorepeat_gap_timeout_set(This["eo"], _argt);
}

static v8::Handle<v8::Value> _Elm_Button_onClick(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 432 "src/modules/Elm.gear"
    return This["_onClick"];
}

static void _Elm_Button_onClick(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 426 "src/modules/Elm.gear"
    Value arg(_arg);
    This["_onClick"] = arg;
    evas_object_smart_callback_add(This["eo"], "clicked", elmOnClick, new Value(This));
}

static v8::Handle<v8::Value> _Elm_Radio_Radio(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 438 "src/modules/Elm.gear"
        Value parent(args[0]);
        This["eo"] = elm_radio_add(parent["eo"]);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to Elm.Radio");
}

static v8::Handle<v8::Value> _Elm_Radio_x(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 47 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(x);
}

static void _Elm_Radio_x(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 52 "src/modules/Elm.gear"
    Value _x(__x);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Radio_y(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 59 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(y);
}

static void _Elm_Radio_y(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 64 "src/modules/Elm.gear"
    Value _y(__y);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Radio_height(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 71 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(height);
}

static void _Elm_Radio_height(v8::Local<v8::String>, v8::Local<v8::Value> __height, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 76 "src/modules/Elm.gear"
    Value _height(__height);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], width, _height);
}

static v8::Handle<v8::Value> _Elm_Radio_width(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 83 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(width);
}

static void _Elm_Radio_width(v8::Local<v8::String>, v8::Local<v8::Value> __width, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 88 "src/modules/Elm.gear"
    Value _width(__width);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], _width, height);
}

static v8::Handle<v8::Value> _Elm_Radio_label(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 95 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Radio_label(v8::Local<v8::String>, v8::Local<v8::Value> _label, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 98 "src/modules/Elm.gear"
    Value label(_label);
    elm_object_text_set(This["eo"], label.to<String>());
}

static v8::Handle<v8::Value> _Elm_Radio_text(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 103 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Radio_text(v8::Local<v8::String>, v8::Local<v8::Value> _text, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 106 "src/modules/Elm.gear"
    Value text(_text);
    elm_object_text_set(This["eo"], text.to<String>());
}

static v8::Handle<v8::Value> _Elm_Radio_scale(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 111 "src/modules/Elm.gear"
    return Number(elm_object_scale_get(This["eo"]));
}

static void _Elm_Radio_scale(v8::Local<v8::String>, v8::Local<v8::Value> _scale, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 114 "src/modules/Elm.gear"
    Value scale(_scale);
    elm_object_scale_set(This["eo"], scale);
}

static v8::Handle<v8::Value> _Elm_Radio_xalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 119 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(x);
}

static void _Elm_Radio_xalign(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 124 "src/modules/Elm.gear"
    Value _x(__x);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Radio_yalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 131 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(y);
}

static void _Elm_Radio_yalign(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 136 "src/modules/Elm.gear"
    Value _y(__y);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Radio_style(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 143 "src/modules/Elm.gear"
    return String(elm_object_style_get(This["eo"]));
}

static void _Elm_Radio_style(v8::Local<v8::String>, v8::Local<v8::Value> _style, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 146 "src/modules/Elm.gear"
    Value style(_style);
    elm_object_style_set(This["eo"], style.to<String>());
}

static v8::Handle<v8::Value> _Elm_Radio_visible(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 151 "src/modules/Elm.gear"
    return Value(evas_object_visible_get(This["eo"]));
}

static void _Elm_Radio_visible(v8::Local<v8::String>, v8::Local<v8::Value> _visible, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 154 "src/modules/Elm.gear"
    Value visible(_visible);
    if(visible)
        evas_object_show(This["eo"]);
    else
        evas_object_hide(This["eo"]);
}

static v8::Handle<v8::Value> _Elm_Radio_group(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 444 "src/modules/Elm.gear"
    // TODO : Not implemented in EFL
    //elm_radio_group_get(This["eo"]);
    return This["eo"];
}

static void _Elm_Radio_group(v8::Local<v8::String>, v8::Local<v8::Value> _group, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 449 "src/modules/Elm.gear"
    Value group(_group);
    elm_radio_group_add(This["eo"], group["eo"]);
}

static v8::Handle<v8::Value> _Elm_Radio_state(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 454 "src/modules/Elm.gear"
    int state = elm_radio_state_value_get(This["eo"]);
    return Integer(state);
}

static void _Elm_Radio_state(v8::Local<v8::String>, v8::Local<v8::Value> __state, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 458 "src/modules/Elm.gear"
    Value _state(__state);
    elm_radio_state_value_set(This["eo"], _state);
}

static v8::Handle<v8::Value> _Elm_Radio_value(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 463 "src/modules/Elm.gear"
    int value = elm_radio_value_get(This["eo"]);
    return Integer(value);
}

static void _Elm_Radio_value(v8::Local<v8::String>, v8::Local<v8::Value> __value, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 467 "src/modules/Elm.gear"
    Value _value(__value);
    elm_radio_value_set(This["eo"], _value);
}

static v8::Handle<v8::Value> _Elm_Label_Label(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 475 "src/modules/Elm.gear"
        Value parent(args[0]);
        This["eo"] = elm_label_add(parent["eo"]);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to Elm.Label");
}

static v8::Handle<v8::Value> _Elm_Label_x(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 47 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(x);
}

static void _Elm_Label_x(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 52 "src/modules/Elm.gear"
    Value _x(__x);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Label_y(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 59 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(y);
}

static void _Elm_Label_y(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 64 "src/modules/Elm.gear"
    Value _y(__y);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Label_height(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 71 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(height);
}

static void _Elm_Label_height(v8::Local<v8::String>, v8::Local<v8::Value> __height, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 76 "src/modules/Elm.gear"
    Value _height(__height);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], width, _height);
}

static v8::Handle<v8::Value> _Elm_Label_width(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 83 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(width);
}

static void _Elm_Label_width(v8::Local<v8::String>, v8::Local<v8::Value> __width, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 88 "src/modules/Elm.gear"
    Value _width(__width);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], _width, height);
}

static v8::Handle<v8::Value> _Elm_Label_label(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 95 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Label_label(v8::Local<v8::String>, v8::Local<v8::Value> _label, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 98 "src/modules/Elm.gear"
    Value label(_label);
    elm_object_text_set(This["eo"], label.to<String>());
}

static v8::Handle<v8::Value> _Elm_Label_text(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 103 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Label_text(v8::Local<v8::String>, v8::Local<v8::Value> _text, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 106 "src/modules/Elm.gear"
    Value text(_text);
    elm_object_text_set(This["eo"], text.to<String>());
}

static v8::Handle<v8::Value> _Elm_Label_scale(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 111 "src/modules/Elm.gear"
    return Number(elm_object_scale_get(This["eo"]));
}

static void _Elm_Label_scale(v8::Local<v8::String>, v8::Local<v8::Value> _scale, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 114 "src/modules/Elm.gear"
    Value scale(_scale);
    elm_object_scale_set(This["eo"], scale);
}

static v8::Handle<v8::Value> _Elm_Label_xalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 119 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(x);
}

static void _Elm_Label_xalign(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 124 "src/modules/Elm.gear"
    Value _x(__x);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Label_yalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 131 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(y);
}

static void _Elm_Label_yalign(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 136 "src/modules/Elm.gear"
    Value _y(__y);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Label_style(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 143 "src/modules/Elm.gear"
    return String(elm_object_style_get(This["eo"]));
}

static void _Elm_Label_style(v8::Local<v8::String>, v8::Local<v8::Value> _style, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 146 "src/modules/Elm.gear"
    Value style(_style);
    elm_object_style_set(This["eo"], style.to<String>());
}

static v8::Handle<v8::Value> _Elm_Label_visible(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 151 "src/modules/Elm.gear"
    return Value(evas_object_visible_get(This["eo"]));
}

static void _Elm_Label_visible(v8::Local<v8::String>, v8::Local<v8::Value> _visible, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 154 "src/modules/Elm.gear"
    Value visible(_visible);
    if(visible)
        evas_object_show(This["eo"]);
    else
        evas_object_hide(This["eo"]);
}

static v8::Handle<v8::Value> _Elm_Label_wrap(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 481 "src/modules/Elm.gear"
    int wrap = elm_label_line_wrap_get (This["eo"]);
    return Integer(wrap);
}

static void _Elm_Label_wrap(v8::Local<v8::String>, v8::Local<v8::Value> __wrap, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 485 "src/modules/Elm.gear"
    Value _wrap(__wrap);
    Elm_Wrap_Type _wt = (Elm_Wrap_Type)(_wrap.to<int>());
    elm_label_line_wrap_set(This["eo"], _wt);
}

static v8::Handle<v8::Value> _Elm_Label_wrapWidth(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 491 "src/modules/Elm.gear"
    Evas_Coord w = elm_label_wrap_width_get (This["eo"]);
    return Integer(w);
}

static void _Elm_Label_wrapWidth(v8::Local<v8::String>, v8::Local<v8::Value> __wrap, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 495 "src/modules/Elm.gear"
    Value _wrap(__wrap);
    elm_label_wrap_width_set(This["eo"], (Evas_Coord)(_wrap));
}

static v8::Handle<v8::Value> _Elm_Label_wrapHeight(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 500 "src/modules/Elm.gear"
    Evas_Coord h = elm_label_wrap_height_get (This["eo"]);
    return Integer(h);
}

static void _Elm_Label_wrapHeight(v8::Local<v8::String>, v8::Local<v8::Value> __height, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 504 "src/modules/Elm.gear"
    Value _height(__height);
    elm_label_wrap_height_set(This["eo"], (Evas_Coord)(_height));
}

static v8::Handle<v8::Value> _Elm_Label_fontSize(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 509 "src/modules/Elm.gear"
    int fs = 0;
    // TODO : Not implemented in EFL
    //elm_label_fontsize_get(This["eo"], &fs);
    return Integer(fs);
}

static void _Elm_Label_fontSize(v8::Local<v8::String>, v8::Local<v8::Value> __fs, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 515 "src/modules/Elm.gear"
    Value _fs(__fs);
    elm_label_fontsize_set(This["eo"], _fs);
}

static v8::Handle<v8::Value> _Elm_Label_textAlign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 520 "src/modules/Elm.gear"
    // TODO : Not implemented in EFL
    return String("center");
}

static void _Elm_Label_textAlign(v8::Local<v8::String>, v8::Local<v8::Value> __align, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 524 "src/modules/Elm.gear"
    Value _align(__align);
    elm_label_text_align_set(This["eo"],_align);
}

static v8::Handle<v8::Value> _Elm_Label_ellipsis(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 529 "src/modules/Elm.gear"
    // TODO : Not implemented in EFL
    //bool ellipsis = elm_label_ellipsis_get(This["eo"]);
    return Value(false);
}

static void _Elm_Label_ellipsis(v8::Local<v8::String>, v8::Local<v8::Value> __ellipsis, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 534 "src/modules/Elm.gear"
    Value _ellipsis(__ellipsis);
    elm_label_ellipsis_set(This["eo"], _ellipsis);
}

static v8::Handle<v8::Value> _Elm_Flip_Flip(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 542 "src/modules/Elm.gear"
        Value parent(args[0]);
        This["eo"] = elm_flip_add(parent["eo"]);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to Elm.Flip");
}

static v8::Handle<v8::Value> _Elm_Flip_go(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 563 "src/modules/Elm.gear"
        Value mode(args[0]);
        elm_flip_go(This["eo"], (Elm_Flip_Mode)(mode.to<int>()));
        return undefined;
    }
    THROW_ERROR("Invalid call to Elm.Flip.prototype.go");
}

static v8::Handle<v8::Value> _Elm_Flip_x(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 47 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(x);
}

static void _Elm_Flip_x(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 52 "src/modules/Elm.gear"
    Value _x(__x);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Flip_y(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 59 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(y);
}

static void _Elm_Flip_y(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 64 "src/modules/Elm.gear"
    Value _y(__y);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Flip_height(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 71 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(height);
}

static void _Elm_Flip_height(v8::Local<v8::String>, v8::Local<v8::Value> __height, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 76 "src/modules/Elm.gear"
    Value _height(__height);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], width, _height);
}

static v8::Handle<v8::Value> _Elm_Flip_width(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 83 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(width);
}

static void _Elm_Flip_width(v8::Local<v8::String>, v8::Local<v8::Value> __width, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 88 "src/modules/Elm.gear"
    Value _width(__width);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], _width, height);
}

static v8::Handle<v8::Value> _Elm_Flip_label(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 95 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Flip_label(v8::Local<v8::String>, v8::Local<v8::Value> _label, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 98 "src/modules/Elm.gear"
    Value label(_label);
    elm_object_text_set(This["eo"], label.to<String>());
}

static v8::Handle<v8::Value> _Elm_Flip_text(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 103 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Flip_text(v8::Local<v8::String>, v8::Local<v8::Value> _text, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 106 "src/modules/Elm.gear"
    Value text(_text);
    elm_object_text_set(This["eo"], text.to<String>());
}

static v8::Handle<v8::Value> _Elm_Flip_scale(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 111 "src/modules/Elm.gear"
    return Number(elm_object_scale_get(This["eo"]));
}

static void _Elm_Flip_scale(v8::Local<v8::String>, v8::Local<v8::Value> _scale, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 114 "src/modules/Elm.gear"
    Value scale(_scale);
    elm_object_scale_set(This["eo"], scale);
}

static v8::Handle<v8::Value> _Elm_Flip_xalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 119 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(x);
}

static void _Elm_Flip_xalign(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 124 "src/modules/Elm.gear"
    Value _x(__x);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Flip_yalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 131 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(y);
}

static void _Elm_Flip_yalign(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 136 "src/modules/Elm.gear"
    Value _y(__y);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Flip_style(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 143 "src/modules/Elm.gear"
    return String(elm_object_style_get(This["eo"]));
}

static void _Elm_Flip_style(v8::Local<v8::String>, v8::Local<v8::Value> _style, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 146 "src/modules/Elm.gear"
    Value style(_style);
    elm_object_style_set(This["eo"], style.to<String>());
}

static v8::Handle<v8::Value> _Elm_Flip_visible(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 151 "src/modules/Elm.gear"
    return Value(evas_object_visible_get(This["eo"]));
}

static void _Elm_Flip_visible(v8::Local<v8::String>, v8::Local<v8::Value> _visible, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 154 "src/modules/Elm.gear"
    Value visible(_visible);
    if(visible)
        evas_object_show(This["eo"]);
    else
        evas_object_hide(This["eo"]);
}

static v8::Handle<v8::Value> _Elm_Flip_front(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 548 "src/modules/Elm.gear"
    return Value(elm_flip_content_front_get(This["eo"]));
}

static void _Elm_Flip_front(v8::Local<v8::String>, v8::Local<v8::Value> __front, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 551 "src/modules/Elm.gear"
    Value _front(__front);
    elm_flip_content_front_set(This["eo"], _front);
}

static v8::Handle<v8::Value> _Elm_Flip_back(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 556 "src/modules/Elm.gear"
    return Value(elm_flip_content_back_get(This["eo"]));
}

static void _Elm_Flip_back(v8::Local<v8::String>, v8::Local<v8::Value> __back, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 559 "src/modules/Elm.gear"
    Value _back(__back);
    elm_flip_content_back_set(This["eo"], _back);
}

static v8::Handle<v8::Value> _Elm_Flip_interaction(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 568 "src/modules/Elm.gear"
    return Integer(elm_flip_interaction_get(This["eo"]));
}

static void _Elm_Flip_interaction(v8::Local<v8::String>, v8::Local<v8::Value> __interact, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 571 "src/modules/Elm.gear"
    Value _interact(__interact);
    elm_flip_interaction_set (This["eo"], (Elm_Flip_Interaction)(_interact.to<int>()));
}

static v8::Handle<v8::Value> _Elm_ActionSlider_ActionSlider(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 579 "src/modules/Elm.gear"
        Value parent(args[0]);
        This["eo"] = elm_actionslider_add(parent["eo"]);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to Elm.ActionSlider");
}

static v8::Handle<v8::Value> _Elm_ActionSlider_x(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 47 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(x);
}

static void _Elm_ActionSlider_x(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 52 "src/modules/Elm.gear"
    Value _x(__x);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_ActionSlider_y(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 59 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(y);
}

static void _Elm_ActionSlider_y(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 64 "src/modules/Elm.gear"
    Value _y(__y);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_ActionSlider_height(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 71 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(height);
}

static void _Elm_ActionSlider_height(v8::Local<v8::String>, v8::Local<v8::Value> __height, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 76 "src/modules/Elm.gear"
    Value _height(__height);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], width, _height);
}

static v8::Handle<v8::Value> _Elm_ActionSlider_width(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 83 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(width);
}

static void _Elm_ActionSlider_width(v8::Local<v8::String>, v8::Local<v8::Value> __width, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 88 "src/modules/Elm.gear"
    Value _width(__width);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], _width, height);
}

static v8::Handle<v8::Value> _Elm_ActionSlider_label(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 95 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_ActionSlider_label(v8::Local<v8::String>, v8::Local<v8::Value> _label, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 98 "src/modules/Elm.gear"
    Value label(_label);
    elm_object_text_set(This["eo"], label.to<String>());
}

static v8::Handle<v8::Value> _Elm_ActionSlider_text(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 103 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_ActionSlider_text(v8::Local<v8::String>, v8::Local<v8::Value> _text, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 106 "src/modules/Elm.gear"
    Value text(_text);
    elm_object_text_set(This["eo"], text.to<String>());
}

static v8::Handle<v8::Value> _Elm_ActionSlider_scale(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 111 "src/modules/Elm.gear"
    return Number(elm_object_scale_get(This["eo"]));
}

static void _Elm_ActionSlider_scale(v8::Local<v8::String>, v8::Local<v8::Value> _scale, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 114 "src/modules/Elm.gear"
    Value scale(_scale);
    elm_object_scale_set(This["eo"], scale);
}

static v8::Handle<v8::Value> _Elm_ActionSlider_xalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 119 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(x);
}

static void _Elm_ActionSlider_xalign(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 124 "src/modules/Elm.gear"
    Value _x(__x);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_ActionSlider_yalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 131 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(y);
}

static void _Elm_ActionSlider_yalign(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 136 "src/modules/Elm.gear"
    Value _y(__y);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_ActionSlider_style(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 143 "src/modules/Elm.gear"
    return String(elm_object_style_get(This["eo"]));
}

static void _Elm_ActionSlider_style(v8::Local<v8::String>, v8::Local<v8::Value> _style, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 146 "src/modules/Elm.gear"
    Value style(_style);
    elm_object_style_set(This["eo"], style.to<String>());
}

static v8::Handle<v8::Value> _Elm_ActionSlider_visible(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 151 "src/modules/Elm.gear"
    return Value(evas_object_visible_get(This["eo"]));
}

static void _Elm_ActionSlider_visible(v8::Local<v8::String>, v8::Local<v8::Value> _visible, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 154 "src/modules/Elm.gear"
    Value visible(_visible);
    if(visible)
        evas_object_show(This["eo"]);
    else
        evas_object_hide(This["eo"]);
}

static v8::Handle<v8::Value> _Elm_ActionSlider_selectedLabel(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 585 "src/modules/Elm.gear"
    return String(elm_actionslider_selected_label_get(This["eo"]));
}

static void _Elm_ActionSlider_selectedLabel(v8::Local<v8::String>, v8::Local<v8::Value> __sl, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 588 "src/modules/Elm.gear"
    Value _sl(__sl);
    // TODO : Not implemented in EFL
}

static v8::Handle<v8::Value> _Elm_ActionSlider_indicatorPos(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 593 "src/modules/Elm.gear"
    return Integer(elm_actionslider_indicator_pos_get(This["eo"]));
}

static void _Elm_ActionSlider_indicatorPos(v8::Local<v8::String>, v8::Local<v8::Value> __ip, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 596 "src/modules/Elm.gear"
    Value _ip(__ip);
    elm_actionslider_indicator_pos_set(This["eo"], (Elm_Actionslider_Pos)(_ip.to<int>()));
}

static v8::Handle<v8::Value> _Elm_ActionSlider_enabledPos(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 601 "src/modules/Elm.gear"
    return Integer(elm_actionslider_enabled_pos_get(This["eo"]));
}

static void _Elm_ActionSlider_enabledPos(v8::Local<v8::String>, v8::Local<v8::Value> __ip, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 604 "src/modules/Elm.gear"
    Value _ip(__ip);
    elm_actionslider_enabled_pos_set(This["eo"], (Elm_Actionslider_Pos)(_ip.to<int>()));
}

static v8::Handle<v8::Value> _Elm_ActionSlider_magnetPos(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 609 "src/modules/Elm.gear"
    return Integer(elm_actionslider_magnet_pos_get(This["eo"]));
}

static void _Elm_ActionSlider_magnetPos(v8::Local<v8::String>, v8::Local<v8::Value> __ip, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 612 "src/modules/Elm.gear"
    Value _ip(__ip);
    elm_actionslider_magnet_pos_set(This["eo"], (Elm_Actionslider_Pos)(_ip.to<int>()));
}

static v8::Handle<v8::Value> _Elm_Entry_Entry(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 620 "src/modules/Elm.gear"
        Value parent(args[0]);
        This["eo"] = elm_entry_add(parent["eo"]);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to Elm.Entry");
}

static v8::Handle<v8::Value> _Elm_Entry_x(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 47 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(x);
}

static void _Elm_Entry_x(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 52 "src/modules/Elm.gear"
    Value _x(__x);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Entry_y(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 59 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(y);
}

static void _Elm_Entry_y(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 64 "src/modules/Elm.gear"
    Value _y(__y);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_move(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Entry_height(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 71 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(height);
}

static void _Elm_Entry_height(v8::Local<v8::String>, v8::Local<v8::Value> __height, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 76 "src/modules/Elm.gear"
    Value _height(__height);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], width, _height);
}

static v8::Handle<v8::Value> _Elm_Entry_width(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 83 "src/modules/Elm.gear"
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    return Number(width);
}

static void _Elm_Entry_width(v8::Local<v8::String>, v8::Local<v8::Value> __width, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 88 "src/modules/Elm.gear"
    Value _width(__width);
    Evas_Coord x, y, width, height;
    evas_object_geometry_get(This["eo"], &x, &y, &width, &height);
    evas_object_resize(This["eo"], _width, height);
}

static v8::Handle<v8::Value> _Elm_Entry_label(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 95 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Entry_label(v8::Local<v8::String>, v8::Local<v8::Value> _label, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 98 "src/modules/Elm.gear"
    Value label(_label);
    elm_object_text_set(This["eo"], label.to<String>());
}

static v8::Handle<v8::Value> _Elm_Entry_text(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 103 "src/modules/Elm.gear"
    return String(elm_object_text_get(This["eo"]));
}

static void _Elm_Entry_text(v8::Local<v8::String>, v8::Local<v8::Value> _text, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 106 "src/modules/Elm.gear"
    Value text(_text);
    elm_object_text_set(This["eo"], text.to<String>());
}

static v8::Handle<v8::Value> _Elm_Entry_scale(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 111 "src/modules/Elm.gear"
    return Number(elm_object_scale_get(This["eo"]));
}

static void _Elm_Entry_scale(v8::Local<v8::String>, v8::Local<v8::Value> _scale, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 114 "src/modules/Elm.gear"
    Value scale(_scale);
    elm_object_scale_set(This["eo"], scale);
}

static v8::Handle<v8::Value> _Elm_Entry_xalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 119 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(x);
}

static void _Elm_Entry_xalign(v8::Local<v8::String>, v8::Local<v8::Value> __x, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 124 "src/modules/Elm.gear"
    Value _x(__x);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], _x, y);
}

static v8::Handle<v8::Value> _Elm_Entry_yalign(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 131 "src/modules/Elm.gear"
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    return Number(y);
}

static void _Elm_Entry_yalign(v8::Local<v8::String>, v8::Local<v8::Value> __y, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 136 "src/modules/Elm.gear"
    Value _y(__y);
    double x, y;
    evas_object_size_hint_align_get(This["eo"], &x, &y);
    evas_object_size_hint_align_set(This["eo"], x, _y);
}

static v8::Handle<v8::Value> _Elm_Entry_style(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 143 "src/modules/Elm.gear"
    return String(elm_object_style_get(This["eo"]));
}

static void _Elm_Entry_style(v8::Local<v8::String>, v8::Local<v8::Value> _style, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 146 "src/modules/Elm.gear"
    Value style(_style);
    elm_object_style_set(This["eo"], style.to<String>());
}

static v8::Handle<v8::Value> _Elm_Entry_visible(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 151 "src/modules/Elm.gear"
    return Value(evas_object_visible_get(This["eo"]));
}

static void _Elm_Entry_visible(v8::Local<v8::String>, v8::Local<v8::Value> _visible, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 154 "src/modules/Elm.gear"
    Value visible(_visible);
    if(visible)
        evas_object_show(This["eo"]);
    else
        evas_object_hide(This["eo"]);
}

static v8::Handle<v8::Value> _Elm_Entry_entry(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 631 "src/modules/Elm.gear"
    const char *entry;
    entry = elm_entry_entry_get(This["eo"]);
    return String(entry);
}

static void _Elm_Entry_entry(v8::Local<v8::String>, v8::Local<v8::Value> _text, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 625 "src/modules/Elm.gear"
    Value text(_text);
    This["entry"] = text;
    elm_entry_entry_set(This["eo"], text);
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


#line 2654 "src/modules/Elm.cc"
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
    _Elm_Background->PrototypeTemplate()->SetAccessor(String("bgOption"), _Elm_Background_bgOption, _Elm_Background_bgOption);
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
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("scaleDown"), _Elm_Icon_scaleDown, _Elm_Icon_scaleDown);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("scaleUp"), _Elm_Icon_scaleUp, _Elm_Icon_scaleUp);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("prescale"), _Elm_Icon_prescale, _Elm_Icon_prescale);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("animated"), _Elm_Icon_animated, _Elm_Icon_animated);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("animPlay"), _Elm_Icon_animPlay, _Elm_Icon_animPlay);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("smooth"), _Elm_Icon_smooth, _Elm_Icon_smooth);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("standard"), _Elm_Icon_standard, _Elm_Icon_standard);
    _Elm_Icon->PrototypeTemplate()->SetAccessor(String("fillOutside"), _Elm_Icon_fillOutside, _Elm_Icon_fillOutside);
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
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("autorepeat"), _Elm_Button_autorepeat, _Elm_Button_autorepeat);
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("autorepeatInitTimeout"), _Elm_Button_autorepeatInitTimeout, _Elm_Button_autorepeatInitTimeout);
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("autorepeatGapTimeout"), _Elm_Button_autorepeatGapTimeout, _Elm_Button_autorepeatGapTimeout);
    _Elm_Button->PrototypeTemplate()->SetAccessor(String("onClick"), _Elm_Button_onClick, _Elm_Button_onClick);
    _exports["Button"] = _Elm_Button->GetFunction();
    v8::Handle<v8::FunctionTemplate> _Elm_Radio = v8::FunctionTemplate::New(_Elm_Radio_Radio);
    _Elm_Radio->SetClassName(String("Radio"));
    _Elm_Radio->PrototypeTemplate()->SetAccessor(String("x"), _Elm_Radio_x, _Elm_Radio_x);
    _Elm_Radio->PrototypeTemplate()->SetAccessor(String("y"), _Elm_Radio_y, _Elm_Radio_y);
    _Elm_Radio->PrototypeTemplate()->SetAccessor(String("height"), _Elm_Radio_height, _Elm_Radio_height);
    _Elm_Radio->PrototypeTemplate()->SetAccessor(String("width"), _Elm_Radio_width, _Elm_Radio_width);
    _Elm_Radio->PrototypeTemplate()->SetAccessor(String("label"), _Elm_Radio_label, _Elm_Radio_label);
    _Elm_Radio->PrototypeTemplate()->SetAccessor(String("text"), _Elm_Radio_text, _Elm_Radio_text);
    _Elm_Radio->PrototypeTemplate()->SetAccessor(String("scale"), _Elm_Radio_scale, _Elm_Radio_scale);
    _Elm_Radio->PrototypeTemplate()->SetAccessor(String("xalign"), _Elm_Radio_xalign, _Elm_Radio_xalign);
    _Elm_Radio->PrototypeTemplate()->SetAccessor(String("yalign"), _Elm_Radio_yalign, _Elm_Radio_yalign);
    _Elm_Radio->PrototypeTemplate()->SetAccessor(String("style"), _Elm_Radio_style, _Elm_Radio_style);
    _Elm_Radio->PrototypeTemplate()->SetAccessor(String("visible"), _Elm_Radio_visible, _Elm_Radio_visible);
    _Elm_Radio->PrototypeTemplate()->SetAccessor(String("group"), _Elm_Radio_group, _Elm_Radio_group);
    _Elm_Radio->PrototypeTemplate()->SetAccessor(String("state"), _Elm_Radio_state, _Elm_Radio_state);
    _Elm_Radio->PrototypeTemplate()->SetAccessor(String("value"), _Elm_Radio_value, _Elm_Radio_value);
    _exports["Radio"] = _Elm_Radio->GetFunction();
    v8::Handle<v8::FunctionTemplate> _Elm_Label = v8::FunctionTemplate::New(_Elm_Label_Label);
    _Elm_Label->SetClassName(String("Label"));
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("x"), _Elm_Label_x, _Elm_Label_x);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("y"), _Elm_Label_y, _Elm_Label_y);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("height"), _Elm_Label_height, _Elm_Label_height);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("width"), _Elm_Label_width, _Elm_Label_width);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("label"), _Elm_Label_label, _Elm_Label_label);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("text"), _Elm_Label_text, _Elm_Label_text);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("scale"), _Elm_Label_scale, _Elm_Label_scale);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("xalign"), _Elm_Label_xalign, _Elm_Label_xalign);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("yalign"), _Elm_Label_yalign, _Elm_Label_yalign);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("style"), _Elm_Label_style, _Elm_Label_style);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("visible"), _Elm_Label_visible, _Elm_Label_visible);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("wrap"), _Elm_Label_wrap, _Elm_Label_wrap);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("wrapWidth"), _Elm_Label_wrapWidth, _Elm_Label_wrapWidth);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("wrapHeight"), _Elm_Label_wrapHeight, _Elm_Label_wrapHeight);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("fontSize"), _Elm_Label_fontSize, _Elm_Label_fontSize);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("textAlign"), _Elm_Label_textAlign, _Elm_Label_textAlign);
    _Elm_Label->PrototypeTemplate()->SetAccessor(String("ellipsis"), _Elm_Label_ellipsis, _Elm_Label_ellipsis);
    _exports["Label"] = _Elm_Label->GetFunction();
    v8::Handle<v8::FunctionTemplate> _Elm_Flip = v8::FunctionTemplate::New(_Elm_Flip_Flip);
    _Elm_Flip->SetClassName(String("Flip"));
    _Elm_Flip->PrototypeTemplate()->Set("go", Function(_Elm_Flip_go, "go"));
    _Elm_Flip->PrototypeTemplate()->SetAccessor(String("x"), _Elm_Flip_x, _Elm_Flip_x);
    _Elm_Flip->PrototypeTemplate()->SetAccessor(String("y"), _Elm_Flip_y, _Elm_Flip_y);
    _Elm_Flip->PrototypeTemplate()->SetAccessor(String("height"), _Elm_Flip_height, _Elm_Flip_height);
    _Elm_Flip->PrototypeTemplate()->SetAccessor(String("width"), _Elm_Flip_width, _Elm_Flip_width);
    _Elm_Flip->PrototypeTemplate()->SetAccessor(String("label"), _Elm_Flip_label, _Elm_Flip_label);
    _Elm_Flip->PrototypeTemplate()->SetAccessor(String("text"), _Elm_Flip_text, _Elm_Flip_text);
    _Elm_Flip->PrototypeTemplate()->SetAccessor(String("scale"), _Elm_Flip_scale, _Elm_Flip_scale);
    _Elm_Flip->PrototypeTemplate()->SetAccessor(String("xalign"), _Elm_Flip_xalign, _Elm_Flip_xalign);
    _Elm_Flip->PrototypeTemplate()->SetAccessor(String("yalign"), _Elm_Flip_yalign, _Elm_Flip_yalign);
    _Elm_Flip->PrototypeTemplate()->SetAccessor(String("style"), _Elm_Flip_style, _Elm_Flip_style);
    _Elm_Flip->PrototypeTemplate()->SetAccessor(String("visible"), _Elm_Flip_visible, _Elm_Flip_visible);
    _Elm_Flip->PrototypeTemplate()->SetAccessor(String("front"), _Elm_Flip_front, _Elm_Flip_front);
    _Elm_Flip->PrototypeTemplate()->SetAccessor(String("back"), _Elm_Flip_back, _Elm_Flip_back);
    _Elm_Flip->PrototypeTemplate()->SetAccessor(String("interaction"), _Elm_Flip_interaction, _Elm_Flip_interaction);
    _exports["Flip"] = _Elm_Flip->GetFunction();
    v8::Handle<v8::FunctionTemplate> _Elm_ActionSlider = v8::FunctionTemplate::New(_Elm_ActionSlider_ActionSlider);
    _Elm_ActionSlider->SetClassName(String("ActionSlider"));
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("x"), _Elm_ActionSlider_x, _Elm_ActionSlider_x);
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("y"), _Elm_ActionSlider_y, _Elm_ActionSlider_y);
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("height"), _Elm_ActionSlider_height, _Elm_ActionSlider_height);
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("width"), _Elm_ActionSlider_width, _Elm_ActionSlider_width);
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("label"), _Elm_ActionSlider_label, _Elm_ActionSlider_label);
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("text"), _Elm_ActionSlider_text, _Elm_ActionSlider_text);
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("scale"), _Elm_ActionSlider_scale, _Elm_ActionSlider_scale);
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("xalign"), _Elm_ActionSlider_xalign, _Elm_ActionSlider_xalign);
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("yalign"), _Elm_ActionSlider_yalign, _Elm_ActionSlider_yalign);
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("style"), _Elm_ActionSlider_style, _Elm_ActionSlider_style);
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("visible"), _Elm_ActionSlider_visible, _Elm_ActionSlider_visible);
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("selectedLabel"), _Elm_ActionSlider_selectedLabel, _Elm_ActionSlider_selectedLabel);
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("indicatorPos"), _Elm_ActionSlider_indicatorPos, _Elm_ActionSlider_indicatorPos);
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("enabledPos"), _Elm_ActionSlider_enabledPos, _Elm_ActionSlider_enabledPos);
    _Elm_ActionSlider->PrototypeTemplate()->SetAccessor(String("magnetPos"), _Elm_ActionSlider_magnetPos, _Elm_ActionSlider_magnetPos);
    _exports["ActionSlider"] = _Elm_ActionSlider->GetFunction();
    v8::Handle<v8::FunctionTemplate> _Elm_Entry = v8::FunctionTemplate::New(_Elm_Entry_Entry);
    _Elm_Entry->SetClassName(String("Entry"));
    _Elm_Entry->PrototypeTemplate()->SetAccessor(String("x"), _Elm_Entry_x, _Elm_Entry_x);
    _Elm_Entry->PrototypeTemplate()->SetAccessor(String("y"), _Elm_Entry_y, _Elm_Entry_y);
    _Elm_Entry->PrototypeTemplate()->SetAccessor(String("height"), _Elm_Entry_height, _Elm_Entry_height);
    _Elm_Entry->PrototypeTemplate()->SetAccessor(String("width"), _Elm_Entry_width, _Elm_Entry_width);
    _Elm_Entry->PrototypeTemplate()->SetAccessor(String("label"), _Elm_Entry_label, _Elm_Entry_label);
    _Elm_Entry->PrototypeTemplate()->SetAccessor(String("text"), _Elm_Entry_text, _Elm_Entry_text);
    _Elm_Entry->PrototypeTemplate()->SetAccessor(String("scale"), _Elm_Entry_scale, _Elm_Entry_scale);
    _Elm_Entry->PrototypeTemplate()->SetAccessor(String("xalign"), _Elm_Entry_xalign, _Elm_Entry_xalign);
    _Elm_Entry->PrototypeTemplate()->SetAccessor(String("yalign"), _Elm_Entry_yalign, _Elm_Entry_yalign);
    _Elm_Entry->PrototypeTemplate()->SetAccessor(String("style"), _Elm_Entry_style, _Elm_Entry_style);
    _Elm_Entry->PrototypeTemplate()->SetAccessor(String("visible"), _Elm_Entry_visible, _Elm_Entry_visible);
    _Elm_Entry->PrototypeTemplate()->SetAccessor(String("entry"), _Elm_Entry_entry, _Elm_Entry_entry);
    _exports["Entry"] = _Elm_Entry->GetFunction();
    _exports["mainLoop"] = Function(_Elm_mainLoop, "mainLoop");
    _exports["toString"] = Function(_Elm_toString, "toString");

        eina_init();
        ecore_init();
        elm_init(0, NULL);
    
}
static Module _module_Elm("Elm", _setup_Elm);