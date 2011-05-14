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
#include "SDL.h"

using namespace Gearbox;

/** \file SDL.cc converted from SDL.gear */

#line 1 "src/modules/SDL.gear"
#include <SDL/SDL.h>

static bool bSDLIsUsed = false;

#define CLIP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (a) : (x)))

Uint32 getPixel(SDL_Surface *surface, Uint32 x, Uint32 y) {
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    
    switch(bpp) {
        case 1:
            return *p;
            break;
            
        case 2:
            return *(Uint16 *)p;
            break;
            
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;
            
        case 4:
            return *(Uint32 *)p;
            break;
            
        default:
            return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void setPixel(SDL_Surface *surface, Uint32 x, Uint32 y, Uint32 pixel) {
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    
    switch(bpp) {
        case 1:
            *p = pixel;
            break;
            
        case 2:
            *(Uint16 *)p = pixel;
            break;
            
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
            
        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}

static v8::Handle<v8::Value> _SDL_Window_Window(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 3) {
        #line 93 "src/modules/SDL.gear"
        Value name(args[0]), w(args[1]), h(args[2]);
        if(bSDLIsUsed)
            THROW_ERROR("SDL is already being used");
        SDL_Init(SDL_INIT_EVERYTHING);
        This["surface"] = SDL_SetVideoMode(w.to<int>(), h.to<int>(), 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
        SDL_WM_SetCaption(name.to<String>(), NULL);
        bSDLIsUsed = true;
        return undefined;
    }
    THROW_ERROR("Invalid call to SDL.Window");
}

static v8::Handle<v8::Value> _SDL_Window_color(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 3) {
        #line 123 "src/modules/SDL.gear"
        Value r(args[0]), g(args[1]), b(args[2]);
        return Integer(SDL_MapRGB(This["surface"].to<SDL_Surface*>()->format, CLIP(r.to<int>(), 0, 255), CLIP(g.to<int>(), 0, 255), CLIP(b.to<int>(), 0, 255)));
    }

    if(args.Length() >= 1) {
        #line 102 "src/modules/SDL.gear"
        Value htmlColor(args[0]);
        Uint8 r = 0, g = 0, b = 0;
        
        if(htmlColor.length() == 7) {
            Uint32 color = strtoul(htmlColor.to<String>()+1, 0, 16);
            r = (color >> 16) & 0xff;
            g = (color >> 8) & 0xff;
            b = color & 0xff;
        }
        else if(htmlColor.length() == 4) {
            Uint32 color = strtoul(htmlColor.to<String>()+1, 0, 16);
            r = (color >> 8) & 0xf;
            g = (color >> 4) & 0xf;
            b = color & 0xf;
            r = r | (r << 4);
            g = g | (g << 4);
            b = b | (b << 4);
        }
        return Integer(SDL_MapRGB(This["surface"].to<SDL_Surface*>()->format, r, g, b));
    }
    THROW_ERROR("Invalid call to SDL.Window.prototype.color");
}

static v8::Handle<v8::Value> _SDL_Window_pixel(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 3) {
        #line 127 "src/modules/SDL.gear"
        Value x(args[0]), y(args[1]), color(args[2]);
        setPixel(This["surface"], CLIP(x.to<int>(), 0, This["surface"].to<SDL_Surface*>()->w - 1), CLIP(y.to<int>(), 0, This["surface"].to<SDL_Surface*>()->h - 1), color.to<int>());
        return This;
    }

    if(args.Length() >= 2) {
        #line 132 "src/modules/SDL.gear"
        Value x(args[0]), y(args[1]);
        return Integer(getPixel(This["surface"], CLIP(x.to<int>(), 0, This["surface"].to<SDL_Surface*>()->w - 1), CLIP(y.to<int>(), 0, This["surface"].to<SDL_Surface*>()->h - 1)));
    }
    THROW_ERROR("Invalid call to SDL.Window.prototype.pixel");
}

static v8::Handle<v8::Value> _SDL_Window_update(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 4) {
        #line 136 "src/modules/SDL.gear"
        Value x(args[0]), y(args[1]), width(args[2]), height(args[3]);
        SDL_UpdateRect(This["surface"], CLIP(x.to<int>(), 0, This["surface"].to<SDL_Surface*>()->w - 1), CLIP(y.to<int>(), 0, This["surface"].to<SDL_Surface*>()->h - 1),
                    CLIP(x.to<int>() + width.to<int>(), 0, This["surface"].to<SDL_Surface*>()->w - 1) - x.to<int>(), CLIP(y.to<int>() + height.to<int>(), 0, This["surface"].to<SDL_Surface*>()->h - 1) - y.to<int>());
        return This;
    }

    #line 143 "src/modules/SDL.gear"
    SDL_UpdateRect(This["surface"], 0, 0, 0, 0);
    return This;
}

static v8::Handle<v8::Value> _SDL_Window_checkEvent(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 147 "src/modules/SDL.gear"
        Value handlers(args[0]);
        SDL_Event event;
        if(!SDL_PollEvent(&event))
            return undefined;
        switch(event.type) {
            case SDL_QUIT:
                handlers["quit"]();
                break;
        }
        return undefined;
    }
    THROW_ERROR("Invalid call to SDL.Window.prototype.checkEvent");
}

static v8::Handle<v8::Value> _SDL_Window_awaitEvent(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 158 "src/modules/SDL.gear"
        Value handlers(args[0]);
        SDL_Event event;
        if(SDL_WaitEvent(&event) < 0)
            return undefined;
        switch(event.type) {
            case SDL_QUIT:
                handlers["quit"]();
                break;
        }
        return undefined;
    }
    THROW_ERROR("Invalid call to SDL.Window.prototype.awaitEvent");
}

static v8::Handle<v8::Value> _SDL_Image_Image(const v8::Arguments &args) {
    Value This(args.This());
    if(args.Length() >= 1) {
        #line 173 "src/modules/SDL.gear"
        Value path(args[0]);
        This["surface"] = SDL_LoadBMP(path.to<String>());
        return undefined;
    }
    THROW_ERROR("Invalid call to SDL.Image");
}

static v8::Handle<v8::Value> _SDL_toString(const v8::Arguments &args) {
    #line 90 "src/modules/SDL.gear"
    return String("[module SDL]");
}


#line 227 "src/modules/SDL.cc"
static void _setup_SDL(Value _exports) {
    v8::Handle<v8::FunctionTemplate> _SDL_Window = v8::FunctionTemplate::New(_SDL_Window_Window);
    _SDL_Window->SetClassName(String("Window"));
    _SDL_Window->PrototypeTemplate()->Set("color", Function(_SDL_Window_color, "color"));
    _SDL_Window->PrototypeTemplate()->Set("pixel", Function(_SDL_Window_pixel, "pixel"));
    _SDL_Window->PrototypeTemplate()->Set("update", Function(_SDL_Window_update, "update"));
    _SDL_Window->PrototypeTemplate()->Set("checkEvent", Function(_SDL_Window_checkEvent, "checkEvent"));
    _SDL_Window->PrototypeTemplate()->Set("awaitEvent", Function(_SDL_Window_awaitEvent, "awaitEvent"));
    _SDL_Window->PrototypeTemplate()->Set("surface", Value(0));
    _exports["Window"] = _SDL_Window->GetFunction();
    v8::Handle<v8::FunctionTemplate> _SDL_Image = v8::FunctionTemplate::New(_SDL_Image_Image);
    _SDL_Image->SetClassName(String("Image"));
    _SDL_Image->PrototypeTemplate()->Set("surface", Value(0));
    _exports["Image"] = _SDL_Image->GetFunction();
    _exports["toString"] = Function(_SDL_toString, "toString");
}
static Module _module_SDL("SDL", _setup_SDL);