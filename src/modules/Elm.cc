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
#include "Elm.h"

using namespace Gearbox;

/** \file Elm.cc converted from Elm.gear */

#line 1 "src/modules/Elm.gear"
#include <Elementary.h>
void close(void *data, Evas_Object *obj, void *event) {
    elm_exit();
}
int init() {
    eina_init();
    ecore_init();
    elm_init(0, NULL);
}
void mainLoop() {
    ecore_main_loop_begin();
}

static v8::Handle<v8::Value> _elm_Evas_Object_Evas_Object(const v8::Arguments &args) {
    Value This(args.This());
    #line 37 "src/modules/Elm.gear"

    return undefined;
}

static v8::Handle<v8::Value> _elm_Evas_Object_x(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 39 "src/modules/Elm.gear"
    return This["x"];
}

static void _elm_Evas_Object_x(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 42 "src/modules/Elm.gear"
    Value arg(_arg);
    This["x"] = arg;
}

static v8::Handle<v8::Value> _elm_Evas_Object_y(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 47 "src/modules/Elm.gear"
    return This["y"];
}

static void _elm_Evas_Object_y(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 50 "src/modules/Elm.gear"
    Value arg(_arg);
    This["y"] = arg;
}

static v8::Handle<v8::Value> _elm_Evas_Object_visible(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 55 "src/modules/Elm.gear"
    return This["visible"];
}

static void _elm_Evas_Object_visible(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 58 "src/modules/Elm.gear"
    Value arg(_arg);
    This["visible"] = arg;
}

static v8::Handle<v8::Value> _elm_Evas_Object_style(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 63 "src/modules/Elm.gear"
    return This["style"];
}

static void _elm_Evas_Object_style(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 66 "src/modules/Elm.gear"
    Value arg(_arg);
    This["style"] = arg;
}

static v8::Handle<v8::Value> _elm_Evas_Object_image(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 71 "src/modules/Elm.gear"
    return This["image"];
}

static void _elm_Evas_Object_image(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 74 "src/modules/Elm.gear"
    Value arg(_arg);
    This["image"] = arg;
}

static v8::Handle<v8::Value> _elm_Evas_Object_type(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 79 "src/modules/Elm.gear"
    return This["type"];
}

static void _elm_Evas_Object_type(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 82 "src/modules/Elm.gear"
    Value arg(_arg);
    This["type"] = arg;
}

static v8::Handle<v8::Value> _elm_Evas_Object_text(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 87 "src/modules/Elm.gear"
    return This["text"];
}

static void _elm_Evas_Object_text(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 90 "src/modules/Elm.gear"
    Value arg(_arg);
    This["text"] = arg;
}

static v8::Handle<v8::Value> _elm_Evas_Object_width(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 95 "src/modules/Elm.gear"
    return This["width"];
}

static void _elm_Evas_Object_width(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 98 "src/modules/Elm.gear"
    Value arg(_arg);
    This["width"] = arg;
}

static v8::Handle<v8::Value> _elm_Evas_Object_height(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 103 "src/modules/Elm.gear"
    return This["height"];
}

static void _elm_Evas_Object_height(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 106 "src/modules/Elm.gear"
    Value arg(_arg);
    This["height"] = arg;
}

static v8::Handle<v8::Value> _elm_Evas_Object_align(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 111 "src/modules/Elm.gear"
    return This["align"];
}

static void _elm_Evas_Object_align(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 114 "src/modules/Elm.gear"
    Value arg(_arg);
    This["align"] = arg;
}

static v8::Handle<v8::Value> _elm_Evas_Object_weight(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 119 "src/modules/Elm.gear"
    return This["weight"];
}

static void _elm_Evas_Object_weight(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 122 "src/modules/Elm.gear"
    Value arg(_arg);
    This["weight"] = arg;
}

static v8::Handle<v8::Value> _elm_Evas_Object_scale(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 127 "src/modules/Elm.gear"
    return This["scale"];
}

static void _elm_Evas_Object_scale(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 130 "src/modules/Elm.gear"
    Value arg(_arg);
    This["scale"] = arg;
}

static v8::Handle<v8::Value> _elm_Evas_Object_pointer(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 135 "src/modules/Elm.gear"
    return This["pointer"];
}

static void _elm_Evas_Object_pointer(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 138 "src/modules/Elm.gear"
    Value arg(_arg);
    This["pointer"] = arg;
}

static v8::Handle<v8::Value> _elm_Evas_Object_resize(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 143 "src/modules/Elm.gear"
    return This["resize"];
}

static void _elm_Evas_Object_resize(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 146 "src/modules/Elm.gear"
    Value arg(_arg);
    This["resize"] = arg;
}

static v8::Handle<v8::Value> _elm_Win_Win(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 154 "src/modules/Elm.gear"
        Value parent(args[0]), title(args[1]);
        This["eo"] = elm_win_add(NULL, title.to<String>(), ELM_WIN_BASIC);
        evas_object_smart_callback_add(This["eo"], "delete,request", close, NULL);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to elm.Win");
}

static v8::Handle<v8::Value> _elm_Win_resize(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 160 "src/modules/Elm.gear"
        Value width(args[0]), height(args[1]);
        evas_object_resize(This["eo"], width.to<int>(), height.to<int>());
        return undefined;
    }
    THROW_ERROR("Invalid call to elm.Win.prototype.resize");
}

static v8::Handle<v8::Value> _elm_Win_obj(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 165 "src/modules/Elm.gear"
    return This["eo"];
}

static void _elm_Win_obj(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 168 "src/modules/Elm.gear"
    Value arg(_arg);
    This["eo"] = arg;
}

static v8::Handle<v8::Value> _elm_Win_title(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 173 "src/modules/Elm.gear"
    const char *title = elm_win_title_get(This["eo"]);
    String str(title);
    return str;
}

static void _elm_Win_title(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 178 "src/modules/Elm.gear"
    Value arg(_arg);
    elm_win_title_set(This["eo"], arg.to<String>());
}

static v8::Handle<v8::Value> _elm_Win_autodel(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 183 "src/modules/Elm.gear"
    bool autodel = elm_win_autodel_get(This["eo"]);
    return v8::Boolean::New(autodel);
}

static void _elm_Win_autodel(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 187 "src/modules/Elm.gear"
    Value arg(_arg);
    elm_win_autodel_set(This["eo"], arg.to<bool>());
}

static v8::Handle<v8::Value> _elm_Bg_Bg(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 3) {
        #line 195 "src/modules/Elm.gear"
        Value parent(args[0]), file(args[1]), group(args[2]);
        This["eo"] = elm_bg_add(parent);
        elm_bg_file_set(This["eo"], file.to<String>(), group.to<String>());
        evas_object_size_hint_weight_set(This["eo"], EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_win_resize_object_add(parent, This["eo"]);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to elm.Bg");
}

static v8::Handle<v8::Value> _elm_Bg_red(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 204 "src/modules/Elm.gear"
    int val;
    elm_bg_color_get(This["eo"], &val, NULL, NULL);
    return v8::Integer::New(val);
}

static void _elm_Bg_red(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 209 "src/modules/Elm.gear"
    Value arg(_arg);
    int r, g, b;
    elm_bg_color_get(This["eo"], &r, &g, &b);
    r = arg.to<int>();
    elm_bg_color_set(This["eo"], r,g,b);
    evas_object_show(This["eo"]);
}

static v8::Handle<v8::Value> _elm_Bg_blue(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 219 "src/modules/Elm.gear"
    int val;
    elm_bg_color_get(This["eo"], NULL, &val, NULL);
    return v8::Integer::New(val);
}

static void _elm_Bg_blue(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 224 "src/modules/Elm.gear"
    Value arg(_arg);
    int r, g, b;
    elm_bg_color_get(This["eo"], &r, &g, &b);
    b = arg.to<int>();
    elm_bg_color_set(This["eo"], r,g,b);
    evas_object_show(This["eo"]);
}

static v8::Handle<v8::Value> _elm_Bg_green(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 234 "src/modules/Elm.gear"
    int val;
    elm_bg_color_get(This["eo"], NULL, NULL, &val);
    return v8::Integer::New(val);
}

static void _elm_Bg_green(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 239 "src/modules/Elm.gear"
    Value arg(_arg);
    int r, g, b;
    elm_bg_color_get(This["eo"], &r, &g, &b);
    g = arg.to<int>();
    elm_bg_color_set(This["eo"], r,g,b);
    evas_object_show(This["eo"]);
}

static v8::Handle<v8::Value> _elm_Bg_file(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 248 "src/modules/Elm.gear"
    const char *file, *group;
    elm_bg_file_get(This["eo"], &file, &group);
    return v8::String::New(file);
}

static void _elm_Bg_file(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 253 "src/modules/Elm.gear"
    Value arg(_arg);
    elm_bg_file_set(This["eo"], arg.to<String>(), NULL );
    evas_object_show(This["eo"]);
}

static v8::Handle<v8::Value> _elm_Box_Box(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 261 "src/modules/Elm.gear"
        Value parent(args[0]);
        This["eo"] = elm_box_add(parent);
        evas_object_size_hint_weight_set(This["eo"],EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to elm.Box");
}

static v8::Handle<v8::Value> _elm_Box_add(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 267 "src/modules/Elm.gear"
        Value child(args[0]);
        elm_box_pack_end(This["eo"], child);
        return undefined;
    }
    THROW_ERROR("Invalid call to elm.Box.prototype.add");
}

static v8::Handle<v8::Value> _elm_Image_Image(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 2) {
        #line 273 "src/modules/Elm.gear"
        Value parent(args[0]), file(args[1]);
        Evas *evas = evas_object_evas_get(parent);
        This["eo"] = evas_object_image_filled_add(evas);
        evas_object_size_hint_weight_set(This["eo"],EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to elm.Image");
}

static v8::Handle<v8::Value> _elm_Image_file(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 281 "src/modules/Elm.gear"
    const char *file, *key;
    evas_object_image_file_get(This["eo"], &file, &key);
    return v8::String::New(file);
}

static void _elm_Image_file(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 286 "src/modules/Elm.gear"
    Value arg(_arg);
    evas_object_image_file_set(This["eo"], arg.to<String>(), NULL );
    evas_object_show(This["eo"]);
}

static v8::Handle<v8::Value> _elm_Icon_icon(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 294 "src/modules/Elm.gear"
        Value parent(args[0]);
        This["eo"] = elm_icon_add(parent);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to elm.Icon.prototype.icon");
}

static v8::Handle<v8::Value> _elm_Icon_Icon(const v8::Arguments &args) {
    Value This(args.This());
    #line 294 "src/modules/Elm.gear"

    return undefined;
}

static v8::Handle<v8::Value> _elm_Icon_file(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 300 "src/modules/Elm.gear"
    const char *file, *group;
    elm_icon_file_get(This["eo"], &file, &group);
    return v8::String::New(file);
}

static void _elm_Icon_file(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 305 "src/modules/Elm.gear"
    Value arg(_arg);
    elm_icon_file_set(This["eo"], arg.to<String>(), NULL);
}

static v8::Handle<v8::Value> _elm_Button_Button(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 313 "src/modules/Elm.gear"
        Value parent(args[0]);
        This["eo"] = elm_button_add(parent);
        evas_object_show(This["eo"]);
        return undefined;
    }
    THROW_ERROR("Invalid call to elm.Button");
}

static v8::Handle<v8::Value> _elm_Button_label(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 319 "src/modules/Elm.gear"
    return v8::String::New(elm_object_text_get(This["eo"]));
}

static void _elm_Button_label(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 322 "src/modules/Elm.gear"
    Value arg(_arg);
    elm_object_text_set(This["eo"], arg.to<String>());
}

static v8::Handle<v8::Value> _elm_Button_obj(v8::Local<v8::String>, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 327 "src/modules/Elm.gear"
    return This["eo"];
}

static void _elm_Button_obj(v8::Local<v8::String>, v8::Local<v8::Value> _arg, const v8::AccessorInfo &args) {
    Value This(args.This());
    #line 330 "src/modules/Elm.gear"
    Value arg(_arg);
    This["eo"] = arg;
}

static v8::Handle<v8::Value> _elm_toString(const v8::Arguments &args) {
    #line 35 "src/modules/Elm.gear"
    return String("[module elm]");
}


#line 513 "src/modules/Elm.cc"
static void _setup_elm(Value _exports) {
    v8::Handle<v8::FunctionTemplate> _elm_Evas_Object = v8::FunctionTemplate::New(_elm_Evas_Object_Evas_Object);
    _elm_Evas_Object->SetClassName(String("Evas_Object"));
    _elm_Evas_Object->PrototypeTemplate()->SetAccessor(String("x"), _elm_Evas_Object_x, _elm_Evas_Object_x);
    _elm_Evas_Object->PrototypeTemplate()->SetAccessor(String("y"), _elm_Evas_Object_y, _elm_Evas_Object_y);
    _elm_Evas_Object->PrototypeTemplate()->SetAccessor(String("visible"), _elm_Evas_Object_visible, _elm_Evas_Object_visible);
    _elm_Evas_Object->PrototypeTemplate()->SetAccessor(String("style"), _elm_Evas_Object_style, _elm_Evas_Object_style);
    _elm_Evas_Object->PrototypeTemplate()->SetAccessor(String("image"), _elm_Evas_Object_image, _elm_Evas_Object_image);
    _elm_Evas_Object->PrototypeTemplate()->SetAccessor(String("type"), _elm_Evas_Object_type, _elm_Evas_Object_type);
    _elm_Evas_Object->PrototypeTemplate()->SetAccessor(String("text"), _elm_Evas_Object_text, _elm_Evas_Object_text);
    _elm_Evas_Object->PrototypeTemplate()->SetAccessor(String("width"), _elm_Evas_Object_width, _elm_Evas_Object_width);
    _elm_Evas_Object->PrototypeTemplate()->SetAccessor(String("height"), _elm_Evas_Object_height, _elm_Evas_Object_height);
    _elm_Evas_Object->PrototypeTemplate()->SetAccessor(String("align"), _elm_Evas_Object_align, _elm_Evas_Object_align);
    _elm_Evas_Object->PrototypeTemplate()->SetAccessor(String("weight"), _elm_Evas_Object_weight, _elm_Evas_Object_weight);
    _elm_Evas_Object->PrototypeTemplate()->SetAccessor(String("scale"), _elm_Evas_Object_scale, _elm_Evas_Object_scale);
    _elm_Evas_Object->PrototypeTemplate()->SetAccessor(String("pointer"), _elm_Evas_Object_pointer, _elm_Evas_Object_pointer);
    _elm_Evas_Object->PrototypeTemplate()->SetAccessor(String("resize"), _elm_Evas_Object_resize, _elm_Evas_Object_resize);
    _exports["Evas_Object"] = _elm_Evas_Object->GetFunction();
    v8::Handle<v8::FunctionTemplate> _elm_Win = v8::FunctionTemplate::New(_elm_Win_Win);
    _elm_Win->SetClassName(String("Win"));
    _elm_Win->PrototypeTemplate()->Set("resize", Function(_elm_Win_resize, "resize"));
    _elm_Win->PrototypeTemplate()->SetAccessor(String("obj"), _elm_Win_obj, _elm_Win_obj);
    _elm_Win->PrototypeTemplate()->SetAccessor(String("title"), _elm_Win_title, _elm_Win_title);
    _elm_Win->PrototypeTemplate()->SetAccessor(String("autodel"), _elm_Win_autodel, _elm_Win_autodel);
    _exports["Win"] = _elm_Win->GetFunction();
    v8::Handle<v8::FunctionTemplate> _elm_Bg = v8::FunctionTemplate::New(_elm_Bg_Bg);
    _elm_Bg->SetClassName(String("Bg"));
    _elm_Bg->PrototypeTemplate()->SetAccessor(String("red"), _elm_Bg_red, _elm_Bg_red);
    _elm_Bg->PrototypeTemplate()->SetAccessor(String("blue"), _elm_Bg_blue, _elm_Bg_blue);
    _elm_Bg->PrototypeTemplate()->SetAccessor(String("green"), _elm_Bg_green, _elm_Bg_green);
    _elm_Bg->PrototypeTemplate()->SetAccessor(String("file"), _elm_Bg_file, _elm_Bg_file);
    _exports["Bg"] = _elm_Bg->GetFunction();
    v8::Handle<v8::FunctionTemplate> _elm_Box = v8::FunctionTemplate::New(_elm_Box_Box);
    _elm_Box->SetClassName(String("Box"));
    _elm_Box->PrototypeTemplate()->Set("add", Function(_elm_Box_add, "add"));
    _exports["Box"] = _elm_Box->GetFunction();
    v8::Handle<v8::FunctionTemplate> _elm_Image = v8::FunctionTemplate::New(_elm_Image_Image);
    _elm_Image->SetClassName(String("Image"));
    _elm_Image->PrototypeTemplate()->SetAccessor(String("file"), _elm_Image_file, _elm_Image_file);
    _exports["Image"] = _elm_Image->GetFunction();
    v8::Handle<v8::FunctionTemplate> _elm_Icon = v8::FunctionTemplate::New(_elm_Icon_Icon);
    _elm_Icon->SetClassName(String("Icon"));
    _elm_Icon->PrototypeTemplate()->Set("icon", Function(_elm_Icon_icon, "icon"));
    _elm_Icon->PrototypeTemplate()->SetAccessor(String("file"), _elm_Icon_file, _elm_Icon_file);
    _exports["Icon"] = _elm_Icon->GetFunction();
    v8::Handle<v8::FunctionTemplate> _elm_Button = v8::FunctionTemplate::New(_elm_Button_Button);
    _elm_Button->SetClassName(String("Button"));
    _elm_Button->PrototypeTemplate()->SetAccessor(String("label"), _elm_Button_label, _elm_Button_label);
    _elm_Button->PrototypeTemplate()->SetAccessor(String("obj"), _elm_Button_obj, _elm_Button_obj);
    _exports["Button"] = _elm_Button->GetFunction();
    _exports["toString"] = Function(_elm_toString, "toString");
}
static Module _module_elm("elm", _setup_elm);