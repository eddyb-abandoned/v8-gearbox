
#include <stdlib.h>
#include <SDL.h>

#include "SDL.h"
#include "../shell.h"

static bool bSDLIsUsed = false;

#define CLIP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (a) : (x)))

Uint32 getPixel(SDL_Surface *surface, Uint32 x, Uint32 y)
{
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

void setPixel(SDL_Surface *surface, Uint32 x, Uint32 y, Uint32 pixel)
{
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

V8FuncDef(global_SDL_Window_Window)
{
    if(args.Length() >= 3)
    {
        if(bSDLIsUsed)
            V8Throw("SDL is already being used");
        SDL_Init(SDL_INIT_EVERYTHING);
        args.This()->SetPointerInInternalField(0, SDL_SetVideoMode(args[1]->IntegerValue(), args[2]->IntegerValue(), 32, SDL_HWSURFACE | SDL_DOUBLEBUF));
        SDL_WM_SetCaption((*v8::String::Utf8Value(args[0])), NULL);
        bSDLIsUsed = true;
        return v8::Undefined();
    }
    V8Throw("Invalid call to SDL.Window");
}

V8FuncDef(global_SDL_Window_color)
{
    if(args.Length() >= 3)
    {
        return v8::Integer::New(SDL_MapRGB((((SDL_Surface*)args.This()->GetPointerFromInternalField(0)))->format, CLIP(args[0]->IntegerValue(), 0, 255), CLIP(args[1]->IntegerValue(), 0, 255), CLIP(args[2]->IntegerValue(), 0, 255)));
    }

    if(args.Length() >= 1)
    {
        Uint8 r = 0, g = 0, b = 0;
        
        if(v8::String::Utf8Value(args[0]).length() == 7) {
            Uint32 color = strtoul(&(*v8::String::Utf8Value(args[0]))[1], 0, 16);
            r = (color >> 16) & 0xff;
            g = (color >> 8) & 0xff;
            b = color & 0xff;
        }
        else if(v8::String::Utf8Value(args[0]).length() == 4) {
            Uint32 color = strtoul(&(*v8::String::Utf8Value(args[0]))[1], 0, 16);
            r = (color >> 8) & 0xf;
            g = (color >> 4) & 0xf;
            b = color & 0xf;
            r = r | (r << 4);
            g = g | (g << 4);
            b = b | (b << 4);
        }
        return v8::Integer::New(SDL_MapRGB((((SDL_Surface*)args.This()->GetPointerFromInternalField(0)))->format, r, g, b));
    }
    V8Throw("Invalid call to SDL.Window.prototype.color");
}

V8FuncDef(global_SDL_Window_pixel)
{
    if(args.Length() >= 3)
    {
        setPixel(((SDL_Surface*)args.This()->GetPointerFromInternalField(0)), CLIP(args[0]->IntegerValue(), 0, ((SDL_Surface*)args.This()->GetPointerFromInternalField(0))->w - 1), CLIP(args[1]->IntegerValue(), 0, ((SDL_Surface*)args.This()->GetPointerFromInternalField(0))->h - 1), args[2]->IntegerValue());
        return args.This();
    }

    if(args.Length() >= 2)
    {
        return v8::Integer::New(getPixel(((SDL_Surface*)args.This()->GetPointerFromInternalField(0)), CLIP(args[0]->IntegerValue(), 0, ((SDL_Surface*)args.This()->GetPointerFromInternalField(0))->w - 1), CLIP(args[1]->IntegerValue(), 0, ((SDL_Surface*)args.This()->GetPointerFromInternalField(0))->h - 1)));
    }
    V8Throw("Invalid call to SDL.Window.prototype.pixel");
}

V8FuncDef(global_SDL_Window_update)
{
    if(args.Length() >= 4)
    {
        SDL_UpdateRect(((SDL_Surface*)args.This()->GetPointerFromInternalField(0)), CLIP(args[0]->IntegerValue(), 0, ((SDL_Surface*)args.This()->GetPointerFromInternalField(0))->w - 1), CLIP(args[1]->IntegerValue(), 0, ((SDL_Surface*)args.This()->GetPointerFromInternalField(0))->h - 1), CLIP(args[0]->IntegerValue() + args[2]->IntegerValue(), 0, ((SDL_Surface*)args.This()->GetPointerFromInternalField(0))->w - 1) - args[0]->IntegerValue(), CLIP(args[1]->IntegerValue() + args[3]->IntegerValue(), 0, ((SDL_Surface*)args.This()->GetPointerFromInternalField(0))->h - 1) - args[1]->IntegerValue());
        return args.This();
    }

    SDL_UpdateRect(((SDL_Surface*)args.This()->GetPointerFromInternalField(0)), 0, 0, 0, 0);
    return args.This();
}

V8FuncDef(global_SDL_Window_checkEvent)
{
    if(args.Length() >= 1)
    {
        SDL_Event event;
        if(!SDL_PollEvent(&event))
            return v8::Undefined();
        switch(event.type) {
            case SDL_QUIT:
                V8FuncCall0(args.This(), args[0]->ToObject()->V8Get("quit"));
                break;
        }
        return v8::Undefined();
    }
    V8Throw("Invalid call to SDL.Window.prototype.checkEvent");
}

V8FuncDef(global_SDL_Window_awaitEvent)
{
    if(args.Length() >= 1)
    {
        SDL_Event event;
        if(SDL_WaitEvent(&event) < 0)
            return v8::Undefined();
        switch(event.type) {
            case SDL_QUIT:
                V8FuncCall0(args.This(), args[0]->ToObject()->V8Get("quit"));
                break;
        }
        return v8::Undefined();
    }
    V8Throw("Invalid call to SDL.Window.prototype.awaitEvent");
}

V8FuncDef(global_SDL_Image_Image)
{
    if(args.Length() >= 1)
    {
        args.This()->SetPointerInInternalField(0, SDL_LoadBMP((*v8::String::Utf8Value(args[0]))));
        return v8::Undefined();
    }
    V8Throw("Invalid call to SDL.Image");
}


void SetupSDL(v8::Handle<v8::Object> global)
{
    v8::Handle<v8::Object> global_SDL = v8::Object::New();
    global->V8Set("SDL", global_SDL);
    v8::Handle<v8::FunctionTemplate> global_SDL_Window = V8Func(global_SDL_Window_Window);
    global_SDL_Window->SetClassName(v8::String::New("Window"));
    global_SDL_Window->InstanceTemplate()->SetInternalFieldCount(1);
    v8::Handle<v8::Function> global_SDL_Window_color = V8Func(global_SDL_Window_color)->GetFunction();
    global_SDL_Window_color->SetName(v8::String::New("color"));
    global_SDL_Window->PrototypeTemplate()->V8Set("color", global_SDL_Window_color);
    v8::Handle<v8::Function> global_SDL_Window_pixel = V8Func(global_SDL_Window_pixel)->GetFunction();
    global_SDL_Window_pixel->SetName(v8::String::New("pixel"));
    global_SDL_Window->PrototypeTemplate()->V8Set("pixel", global_SDL_Window_pixel);
    v8::Handle<v8::Function> global_SDL_Window_update = V8Func(global_SDL_Window_update)->GetFunction();
    global_SDL_Window_update->SetName(v8::String::New("update"));
    global_SDL_Window->PrototypeTemplate()->V8Set("update", global_SDL_Window_update);
    v8::Handle<v8::Function> global_SDL_Window_checkEvent = V8Func(global_SDL_Window_checkEvent)->GetFunction();
    global_SDL_Window_checkEvent->SetName(v8::String::New("checkEvent"));
    global_SDL_Window->PrototypeTemplate()->V8Set("checkEvent", global_SDL_Window_checkEvent);
    v8::Handle<v8::Function> global_SDL_Window_awaitEvent = V8Func(global_SDL_Window_awaitEvent)->GetFunction();
    global_SDL_Window_awaitEvent->SetName(v8::String::New("awaitEvent"));
    global_SDL_Window->PrototypeTemplate()->V8Set("awaitEvent", global_SDL_Window_awaitEvent);
    global_SDL->V8Set("Window", global_SDL_Window->GetFunction());
    v8::Handle<v8::FunctionTemplate> global_SDL_Image = V8Func(global_SDL_Image_Image);
    global_SDL_Image->SetClassName(v8::String::New("Image"));
    global_SDL_Image->InstanceTemplate()->SetInternalFieldCount(1);
    global_SDL->V8Set("Image", global_SDL_Image->GetFunction());
}