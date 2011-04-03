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
#include "GL.h"

using namespace Gearbox;

/** \file GL.cc converted from GL.gear */

#line 1 "src/modules/GL.gear"
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <stdio.h>

#include <map>

static bool bGLIsUsed = false;
Value pGlutRedrawFunc;
Value pGlutIdleFunc;
Value pGlutKeyPressFunc;
Value pGlutKeyUpFunc;
Value pGlutMouseMoveFunc;
Value pGlutResizeFunc;

struct TimerCallback {
    TimerCallback(Value func) {
        pTimerFunc = func;
    }
    Value pTimerFunc;
    //Value pParam;
};

std::map<int, TimerCallback*> pTimers;
int nLastTimer = 0;

void GLProxyDisplayFunc() {
    TryCatch tryCatch;
    if(pGlutRedrawFunc)
        pGlutRedrawFunc();
    if(tryCatch.hasCaught())
        glutLeaveMainLoop();
}
void GLProxyIdleFunc() {
    TryCatch tryCatch;
    if(pGlutIdleFunc)
        pGlutIdleFunc();
    if(tryCatch.hasCaught())
        glutLeaveMainLoop();
}
void GLProxyKeyboardFunc(unsigned char key, int x, int y) {
    TryCatch tryCatch;
    if(pGlutKeyPressFunc)
        pGlutKeyPressFunc(Integer(key), Integer(x), Integer(y));
    if(tryCatch.hasCaught())
        glutLeaveMainLoop();
}
void GLProxyKeyboardUpFunc(unsigned char key, int x, int y) {
    TryCatch tryCatch;
    if(pGlutKeyUpFunc)
        pGlutKeyUpFunc(Integer(key), Integer(x), Integer(y));
    if(tryCatch.hasCaught())
        glutLeaveMainLoop();
}
void GLProxyMotionFunc(int x, int y) {
    TryCatch tryCatch;
    if(pGlutMouseMoveFunc)
        pGlutMouseMoveFunc(Integer(x), Integer(y));
    if(tryCatch.hasCaught())
        glutLeaveMainLoop();
}
void GLProxyReshapeFunc(int w, int h) {
    TryCatch tryCatch;
    if(pGlutResizeFunc)
        pGlutResizeFunc(Integer(w), Integer(h));
    if(tryCatch.hasCaught())
        glutLeaveMainLoop();
}
void GLProxyTimerFunc(int value) {
    TryCatch tryCatch;
    if(!pTimers.count(value))
        return;
    TimerCallback *pTimer = pTimers[value];
    pTimer->pTimerFunc();
    pTimers.erase(value);
    delete pTimer;
    if(tryCatch.hasCaught())
        glutLeaveMainLoop();
}

Value GLError() {
    String errorString = "";
    while(GLenum error = glGetError()) {
        if(errorString.length())
            errorString += " | ";
        switch(error) {
            case GL_INVALID_ENUM:
                errorString += "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                errorString += "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                errorString += "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                errorString += "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                errorString += "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                errorString += "OUT_OF_MEMORY";
                break;
        }
    }
    if(errorString.length())
        return Throw(Error(String::concat("GL_ERROR: ", errorString)));
    else
        return undefined;
}

v8::Handle<v8::Value> __global_GL_initWindow(const v8::Arguments& args) {
    if(args.Length() >= 3)
    {
        #line 88 "src/modules/GL.gear"
        Value name(args[0]), w(args[1]), h(args[2]);
        if(bGLIsUsed)
            return Throw(Error("GL is already being used"));
        int argc = 0;
        glutInit(&argc, 0);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowSize(w, h);
        glutCreateWindow(name.to<String>());
        return undefined;
    }
    return Throw(Error("Invalid call to GL.initWindow"));
}

v8::Handle<v8::Value> __global_GL_mainLoop(const v8::Arguments& args) {
    if(args.Length() >= 1)
    {
        #line 98 "src/modules/GL.gear"
        Value handlers(args[0]);
        if(handlers["redraw"]) {
            pGlutRedrawFunc = handlers["redraw"];
            glutDisplayFunc(GLProxyDisplayFunc);
        }
        
        if(handlers["idle"]) {
            pGlutIdleFunc = handlers["idle"];
            glutIdleFunc(GLProxyIdleFunc);
        }
        
        if(handlers["keyPress"]) {
            pGlutKeyPressFunc = handlers["keyPress"];
            glutKeyboardFunc(GLProxyKeyboardFunc);
        }
        
        if(handlers["keyUp"]) {
            pGlutKeyUpFunc = handlers["keyUp"];
            glutKeyboardUpFunc(GLProxyKeyboardUpFunc);
        }
        
        if(handlers["mouseMove"]) {
            pGlutMouseMoveFunc = handlers["mouseMove"];
            glutMotionFunc(GLProxyMotionFunc);
            glutPassiveMotionFunc(GLProxyMotionFunc);
        }
            
        if(handlers["resize"]) {
            pGlutResizeFunc = handlers["resize"];
            glutReshapeFunc(GLProxyReshapeFunc);
        }
        glutMainLoop();
        return undefined;
    }
    return Throw(Error("Invalid call to GL.mainLoop"));
}

v8::Handle<v8::Value> __global_GL_addTimer(const v8::Arguments& args) {
    if(args.Length() >= 2)
    {
        #line 121 "src/modules/GL.gear"
        Value ms(args[0]), func(args[1]);
        pTimers[nLastTimer] = new TimerCallback(func);
        glutTimerFunc(ms, GLProxyTimerFunc, nLastTimer);
        return Integer(nLastTimer++);
    }
    return Throw(Error("Invalid call to GL.addTimer"));
}

v8::Handle<v8::Value> __global_GL_cancelTimer(const v8::Arguments& args) {
    if(args.Length() >= 1)
    {
        #line 127 "src/modules/GL.gear"
        Value idx(args[0]);
        if(!pTimers.count(idx))
            return undefined;
        TimerCallback *pTimer = pTimers[idx];
        pTimers.erase(idx.to<int>());
        delete pTimer;
        return undefined;
    }
    return Throw(Error("Invalid call to GL.cancelTimer"));
}

v8::Handle<v8::Value> __global_GL_ignoreKeyRepeat(const v8::Arguments& args) {
    if(args.Length() >= 1)
    {
        #line 135 "src/modules/GL.gear"
        Value ignore(args[0]);
        glutIgnoreKeyRepeat(ignore);
        return undefined;
    }
    return Throw(Error("Invalid call to GL.ignoreKeyRepeat"));
}

v8::Handle<v8::Value> __global_GL_warpPointer(const v8::Arguments& args) {
    if(args.Length() >= 2)
    {
        #line 139 "src/modules/GL.gear"
        Value x(args[0]), y(args[1]);
        glutWarpPointer(x, y);
        return undefined;
    }
    return Throw(Error("Invalid call to GL.warpPointer"));
}

v8::Handle<v8::Value> __global_GL_setCursor(const v8::Arguments& args) {
    if(args.Length() >= 1)
    {
        #line 143 "src/modules/GL.gear"
        Value cursor(args[0]);
        glutSetCursor(cursor);
        return undefined;
    }
    return Error("Invalid call to GL.setCursor");
}

v8::Handle<v8::Value> __global_GL_swapBuffers(const v8::Arguments& args) {
    #line 148 "src/modules/GL.gear"
    glutSwapBuffers();
    return undefined;
}

v8::Handle<v8::Value> __global_GL_postRedisplay(const v8::Arguments& args) {
    #line 152 "src/modules/GL.gear"
    glutPostRedisplay();
    return undefined;
}

v8::Handle<v8::Value> __global_GL_bitmapCharacter(const v8::Arguments& args) {
    if(args.Length() >= 1)
    {
        #line 155 "src/modules/GL.gear"
        Value c(args[0]);
        if(c.length())
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, **c.to<String>());
        return undefined;
    }
    return Throw(Error("Invalid call to GL.bitmapCharacter"));
}

v8::Handle<v8::Value> __global_GL_perspective(const v8::Arguments& args) {
    if(args.Length() >= 4)
    {
        #line 189 "src/modules/GL.gear"
        Value fovy(args[0]), aspect(args[1]), zNear(args[2]), zFar(args[3]);
        gluPerspective(fovy, aspect, zNear, zFar);
        return undefined;
    }
    return Throw(Error("Invalid call to GL.perspective"));
}

v8::Handle<v8::Value> __global_GL_ortho2D(const v8::Arguments& args) {
    if(args.Length() >= 4)
    {
        #line 193 "src/modules/GL.gear"
        Value left(args[0]), right(args[1]), bottom(args[2]), top(args[3]);
        gluOrtho2D(left, right, bottom, top);
        return undefined;
    }
    return Throw(Error("Invalid call to GL.ortho2D"));
}

v8::Handle<v8::Value> __global_GL_lookAt(const v8::Arguments& args) {
    if(args.Length() >= 9)
    {
        #line 197 "src/modules/GL.gear"
        Value eyeX(args[0]), eyeY(args[1]), eyeZ(args[2]), centerX(args[3]), centerY(args[4]), centerZ(args[5]), upX(args[6]), upY(args[7]), upZ(args[8]);
        gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
        return undefined;
    }
    return Throw(Error("Invalid call to GL.lookAt"));
}

v8::Handle<v8::Value> __global_GL_enable(const v8::Arguments& args) {
    if(args.Length() >= 1)
    {
        #line 203 "src/modules/GL.gear"
        Value that(args[0]);
        glEnable(that);
        return GLError();
    }
    return Throw(Error("Invalid call to GL.enable"));
}

v8::Handle<v8::Value> __global_GL_disable(const v8::Arguments& args) {
    if(args.Length() >= 1)
    {
        #line 208 "src/modules/GL.gear"
        Value that(args[0]);
        glDisable(that);
        return GLError();
    }
    return Throw(Error("Invalid call to GL.disable"));
}

v8::Handle<v8::Value> __global_GL_flush(const v8::Arguments& args) {
    #line 214 "src/modules/GL.gear"
    glFlush();
    return GLError();
}

v8::Handle<v8::Value> __global_GL_loadIdentity(const v8::Arguments& args) {
    #line 219 "src/modules/GL.gear"
    glLoadIdentity();
    return GLError();
}

v8::Handle<v8::Value> __global_GL_clearColor(const v8::Arguments& args) {
    if(args.Length() >= 4)
    {
        #line 223 "src/modules/GL.gear"
        Value r(args[0]), g(args[1]), b(args[2]), a(args[3]);
        glClearColor(r, g, b, a);
        return GLError();
    }
    return Throw(Error("Invalid call to GL.clearColor"));
}

v8::Handle<v8::Value> __global_GL_clear(const v8::Arguments& args) {
    if(args.Length() >= 1)
    {
        #line 228 "src/modules/GL.gear"
        Value bits(args[0]);
        glClear(bits);
        return GLError();
    }
    return Throw(Error("Invalid call to GL.clear"));
}

v8::Handle<v8::Value> __global_GL_viewport(const v8::Arguments& args) {
    if(args.Length() >= 4)
    {
        #line 233 "src/modules/GL.gear"
        Value x1(args[0]), y1(args[1]), x2(args[2]), y2(args[3]);
        glViewport(x1, y1, x2, y2);
        return GLError();
    }
    return Throw(Error("Invalid call to GL.viewport"));
}

v8::Handle<v8::Value> __global_GL_matrixMode(const v8::Arguments& args) {
    if(args.Length() >= 1)
    {
        #line 238 "src/modules/GL.gear"
        Value mode(args[0]);
        glMatrixMode(mode);
        return GLError();
    }
    return Throw(Error("Invalid call to GL.matrixMode"));
}

v8::Handle<v8::Value> __global_GL_pushMatrix(const v8::Arguments& args) {
    #line 244 "src/modules/GL.gear"
    glPushMatrix();
    return GLError();
}

v8::Handle<v8::Value> __global_GL_popMatrix(const v8::Arguments& args) {
    #line 249 "src/modules/GL.gear"
    glPopMatrix();
    return GLError();
}

v8::Handle<v8::Value> __global_GL_translate(const v8::Arguments& args) {
    if(args.Length() >= 3)
    {
        #line 253 "src/modules/GL.gear"
        Value x(args[0]), y(args[1]), z(args[2]);
        glTranslated(x, y, z);
        return GLError();
    }
    return Throw(Error("Invalid call to GL.translate"));
}

v8::Handle<v8::Value> __global_GL_scale(const v8::Arguments& args) {
    if(args.Length() >= 3)
    {
        #line 258 "src/modules/GL.gear"
        Value x(args[0]), y(args[1]), z(args[2]);
        glScaled(x, y, z);
        return GLError();
    }
    return Throw(Error("Invalid call to GL.scale"));
}

v8::Handle<v8::Value> __global_GL_rotate(const v8::Arguments& args) {
    if(args.Length() >= 4)
    {
        #line 263 "src/modules/GL.gear"
        Value angle(args[0]), x(args[1]), y(args[2]), z(args[3]);
        glRotated(angle, x, y, z);
        return GLError();
    }
    return Throw(Error("Invalid call to GL.rotate"));
}

v8::Handle<v8::Value> __global_GL_color(const v8::Arguments& args) {
    if(args.Length() >= 4)
    {
        #line 273 "src/modules/GL.gear"
        Value r(args[0]), g(args[1]), b(args[2]), a(args[3]);
        glColor4d(r, g, b, a);
        //return GLError();
        return undefined;
    }

    if(args.Length() >= 3)
    {
        #line 268 "src/modules/GL.gear"
        Value r(args[0]), g(args[1]), b(args[2]);
        glColor3d(r, g, b);
        return GLError();
    }
    return Error("Invalid call to GL.color");
}

v8::Handle<v8::Value> __global_GL_light(const v8::Arguments& args) {
    if(args.Length() >= 6)
    {
        #line 278 "src/modules/GL.gear"
        Value which(args[0]), type(args[1]), a(args[2]), b(args[3]), c(args[4]), d(args[5]);
        float light[] = {a, b, c, d};
        glLightfv(which, type, light);
        return GLError();
    }
    return Throw(Error("Invalid call to GL.light"));
}

v8::Handle<v8::Value> __global_GL_material(const v8::Arguments& args) {
    if(args.Length() >= 6)
    {
        #line 284 "src/modules/GL.gear"
        Value which(args[0]), type(args[1]), r(args[2]), g(args[3]), b(args[4]), a(args[5]);
        /*
        // Create light components
        float ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        float diffuseLight[] = { 0.4f, 0.4f, 0.4, 1.0f };
        float specularLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        float position[] = { 0.0f, 20.0f, 0.0f, 1.0f };

        // Assign created components to GL_LIGHT0
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
        glLightfv(GL_LIGHT0, GL_POSITION, position);
        */
        
        float material[] = {r.to<float>(), g.to<float>(), b.to<float>(), a.to<float>()};
        //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);
        glMaterialfv(which, type, material);
        return GLError();
    }
    return Throw(Error("Invalid call to GL.material"));
}

v8::Handle<v8::Value> __global_GL_begin(const v8::Arguments& args) {
    if(args.Length() >= 1)
    {
        #line 305 "src/modules/GL.gear"
        Value what(args[0]);
        glBegin(what);
        //return GLError();
        return undefined;
    }
    return Throw(Error("Invalid call to GL.begin"));
}

v8::Handle<v8::Value> __global_GL_end(const v8::Arguments& args) {
    #line 311 "src/modules/GL.gear"
    glEnd();
    return GLError();
}

v8::Handle<v8::Value> __global_GL_vertex(const v8::Arguments& args) {
    if(args.Length() >= 3)
    {
        #line 315 "src/modules/GL.gear"
        Value x(args[0]), y(args[1]), z(args[2]);
        glVertex3d(x, y, z);
        //return GLError();
        return undefined;
    }
    return Throw(Error("Invalid call to GL.vertex"));
}

v8::Handle<v8::Value> __global_GL_normal(const v8::Arguments& args) {
    if(args.Length() >= 3)
    {
        #line 320 "src/modules/GL.gear"
        Value x(args[0]), y(args[1]), z(args[2]);
        glNormal3d(x, y, z);
        //return GLError();
        return undefined;
    }
    return Throw(Error("Invalid call to GL.normal"));
}

v8::Handle<v8::Value> __global_GL_rasterPos(const v8::Arguments& args) {
    if(args.Length() >= 2)
    {
        #line 325 "src/modules/GL.gear"
        Value x(args[0]), y(args[1]);
        glRasterPos2d(x, y);
        return GLError();
    }
    return Throw(Error("Invalid call to GL.rasterPos"));
}

v8::Handle<v8::Value> __global_GL_toString(const v8::Arguments& args) {
    #line 88 "src/modules/GL.gear"
    return String("[object GL]");
}


#line 497 "src/modules/GL.cc"
void SetupGL(v8::Handle<v8::Object> global) {
    v8::Handle<v8::Object> global_GL = v8::Object::New();
    global->Set(String("GL"), global_GL);
    global_GL->Set(String("initWindow"), Function(__global_GL_initWindow, "initWindow"));
    global_GL->Set(String("mainLoop"), Function(__global_GL_mainLoop, "mainLoop"));
    global_GL->Set(String("addTimer"), Function(__global_GL_addTimer, "addTimer"));
    global_GL->Set(String("cancelTimer"), Function(__global_GL_cancelTimer, "cancelTimer"));
    global_GL->Set(String("ignoreKeyRepeat"), Function(__global_GL_ignoreKeyRepeat, "ignoreKeyRepeat"));
    global_GL->Set(String("warpPointer"), Function(__global_GL_warpPointer, "warpPointer"));
    global_GL->Set(String("setCursor"), Function(__global_GL_setCursor, "setCursor"));
    global_GL->Set(String("swapBuffers"), Function(__global_GL_swapBuffers, "swapBuffers"));
    global_GL->Set(String("postRedisplay"), Function(__global_GL_postRedisplay, "postRedisplay"));
    global_GL->Set(String("bitmapCharacter"), Function(__global_GL_bitmapCharacter, "bitmapCharacter"));
    global_GL->Set(String("perspective"), Function(__global_GL_perspective, "perspective"));
    global_GL->Set(String("ortho2D"), Function(__global_GL_ortho2D, "ortho2D"));
    global_GL->Set(String("lookAt"), Function(__global_GL_lookAt, "lookAt"));
    global_GL->Set(String("enable"), Function(__global_GL_enable, "enable"));
    global_GL->Set(String("disable"), Function(__global_GL_disable, "disable"));
    global_GL->Set(String("flush"), Function(__global_GL_flush, "flush"));
    global_GL->Set(String("loadIdentity"), Function(__global_GL_loadIdentity, "loadIdentity"));
    global_GL->Set(String("clearColor"), Function(__global_GL_clearColor, "clearColor"));
    global_GL->Set(String("clear"), Function(__global_GL_clear, "clear"));
    global_GL->Set(String("viewport"), Function(__global_GL_viewport, "viewport"));
    global_GL->Set(String("matrixMode"), Function(__global_GL_matrixMode, "matrixMode"));
    global_GL->Set(String("pushMatrix"), Function(__global_GL_pushMatrix, "pushMatrix"));
    global_GL->Set(String("popMatrix"), Function(__global_GL_popMatrix, "popMatrix"));
    global_GL->Set(String("translate"), Function(__global_GL_translate, "translate"));
    global_GL->Set(String("scale"), Function(__global_GL_scale, "scale"));
    global_GL->Set(String("rotate"), Function(__global_GL_rotate, "rotate"));
    global_GL->Set(String("color"), Function(__global_GL_color, "color"));
    global_GL->Set(String("light"), Function(__global_GL_light, "light"));
    global_GL->Set(String("material"), Function(__global_GL_material, "material"));
    global_GL->Set(String("begin"), Function(__global_GL_begin, "begin"));
    global_GL->Set(String("end"), Function(__global_GL_end, "end"));
    global_GL->Set(String("vertex"), Function(__global_GL_vertex, "vertex"));
    global_GL->Set(String("normal"), Function(__global_GL_normal, "normal"));
    global_GL->Set(String("rasterPos"), Function(__global_GL_rasterPos, "rasterPos"));
    global_GL->Set(String("toString"), Function(__global_GL_toString, "toString"));
    global_GL->Set(String("CURSOR_RIGHT_ARROW"), Value(GLUT_CURSOR_RIGHT_ARROW));
    global_GL->Set(String("CURSOR_LEFT_ARROW"), Value(GLUT_CURSOR_LEFT_ARROW));
    global_GL->Set(String("CURSOR_INFO"), Value(GLUT_CURSOR_INFO));
    global_GL->Set(String("CURSOR_DESTROY"), Value(GLUT_CURSOR_DESTROY));
    global_GL->Set(String("CURSOR_HELP"), Value(GLUT_CURSOR_HELP));
    global_GL->Set(String("CURSOR_CYCLE"), Value(GLUT_CURSOR_CYCLE));
    global_GL->Set(String("CURSOR_SPRAY"), Value(GLUT_CURSOR_SPRAY));
    global_GL->Set(String("CURSOR_WAIT"), Value(GLUT_CURSOR_WAIT));
    global_GL->Set(String("CURSOR_TEXT"), Value(GLUT_CURSOR_TEXT));
    global_GL->Set(String("CURSOR_CROSSHAIR"), Value(GLUT_CURSOR_CROSSHAIR));
    global_GL->Set(String("CURSOR_UP_DOWN"), Value(GLUT_CURSOR_UP_DOWN));
    global_GL->Set(String("CURSOR_LEFT_RIGHT"), Value(GLUT_CURSOR_LEFT_RIGHT));
    global_GL->Set(String("CURSOR_TOP_SIDE"), Value(GLUT_CURSOR_TOP_SIDE));
    global_GL->Set(String("CURSOR_BOTTOM_SIDE"), Value(GLUT_CURSOR_BOTTOM_SIDE));
    global_GL->Set(String("CURSOR_LEFT_SIDE"), Value(GLUT_CURSOR_LEFT_SIDE));
    global_GL->Set(String("CURSOR_RIGHT_SIDE"), Value(GLUT_CURSOR_RIGHT_SIDE));
    global_GL->Set(String("CURSOR_TOP_LEFT_CORNER"), Value(GLUT_CURSOR_TOP_LEFT_CORNER));
    global_GL->Set(String("CURSOR_TOP_RIGHT_CORNER"), Value(GLUT_CURSOR_TOP_RIGHT_CORNER));
    global_GL->Set(String("CURSOR_BOTTOM_RIGHT_CORNER"), Value(GLUT_CURSOR_BOTTOM_RIGHT_CORNER));
    global_GL->Set(String("CURSOR_BOTTOM_LEFT_CORNER"), Value(GLUT_CURSOR_BOTTOM_LEFT_CORNER));
    global_GL->Set(String("CURSOR_INHERIT"), Value(GLUT_CURSOR_INHERIT));
    global_GL->Set(String("CURSOR_NONE"), Value(GLUT_CURSOR_NONE));
    global_GL->Set(String("CURSOR_FULL_CROSSHAIR"), Value(GLUT_CURSOR_FULL_CROSSHAIR));
    global_GL->Set(String("FALSE"), Value(GL_FALSE));
    global_GL->Set(String("TRUE"), Value(GL_TRUE));
    global_GL->Set(String("BYTE"), Value(GL_BYTE));
    global_GL->Set(String("UNSIGNED_BYTE"), Value(GL_UNSIGNED_BYTE));
    global_GL->Set(String("SHORT"), Value(GL_SHORT));
    global_GL->Set(String("UNSIGNED_SHORT"), Value(GL_UNSIGNED_SHORT));
    global_GL->Set(String("INT"), Value(GL_INT));
    global_GL->Set(String("UNSIGNED_INT"), Value(GL_UNSIGNED_INT));
    global_GL->Set(String("FLOAT"), Value(GL_FLOAT));
    global_GL->Set(String("DOUBLE"), Value(GL_DOUBLE));
    global_GL->Set(String("POINTS"), Value(GL_POINTS));
    global_GL->Set(String("LINES"), Value(GL_LINES));
    global_GL->Set(String("LINE_LOOP"), Value(GL_LINE_LOOP));
    global_GL->Set(String("LINE_STRIP"), Value(GL_LINE_STRIP));
    global_GL->Set(String("TRIANGLES"), Value(GL_TRIANGLES));
    global_GL->Set(String("TRIANGLE_STRIP"), Value(GL_TRIANGLE_STRIP));
    global_GL->Set(String("TRIANGLE_FAN"), Value(GL_TRIANGLE_FAN));
    global_GL->Set(String("QUADS"), Value(GL_QUADS));
    global_GL->Set(String("QUAD_STRIP"), Value(GL_QUAD_STRIP));
    global_GL->Set(String("POLYGON"), Value(GL_POLYGON));
    global_GL->Set(String("VERTEX_ARRAY"), Value(GL_VERTEX_ARRAY));
    global_GL->Set(String("NORMAL_ARRAY"), Value(GL_NORMAL_ARRAY));
    global_GL->Set(String("COLOR_ARRAY"), Value(GL_COLOR_ARRAY));
    global_GL->Set(String("INDEX_ARRAY"), Value(GL_INDEX_ARRAY));
    global_GL->Set(String("TEXTURE_COORD_ARRAY"), Value(GL_TEXTURE_COORD_ARRAY));
    global_GL->Set(String("EDGE_FLAG_ARRAY"), Value(GL_EDGE_FLAG_ARRAY));
    global_GL->Set(String("VERTEX_ARRAY_SIZE"), Value(GL_VERTEX_ARRAY_SIZE));
    global_GL->Set(String("VERTEX_ARRAY_TYPE"), Value(GL_VERTEX_ARRAY_TYPE));
    global_GL->Set(String("VERTEX_ARRAY_STRIDE"), Value(GL_VERTEX_ARRAY_STRIDE));
    global_GL->Set(String("NORMAL_ARRAY_TYPE"), Value(GL_NORMAL_ARRAY_TYPE));
    global_GL->Set(String("NORMAL_ARRAY_STRIDE"), Value(GL_NORMAL_ARRAY_STRIDE));
    global_GL->Set(String("COLOR_ARRAY_SIZE"), Value(GL_COLOR_ARRAY_SIZE));
    global_GL->Set(String("COLOR_ARRAY_TYPE"), Value(GL_COLOR_ARRAY_TYPE));
    global_GL->Set(String("COLOR_ARRAY_STRIDE"), Value(GL_COLOR_ARRAY_STRIDE));
    global_GL->Set(String("INDEX_ARRAY_TYPE"), Value(GL_INDEX_ARRAY_TYPE));
    global_GL->Set(String("INDEX_ARRAY_STRIDE"), Value(GL_INDEX_ARRAY_STRIDE));
    global_GL->Set(String("TEXTURE_COORD_ARRAY_SIZE"), Value(GL_TEXTURE_COORD_ARRAY_SIZE));
    global_GL->Set(String("TEXTURE_COORD_ARRAY_TYPE"), Value(GL_TEXTURE_COORD_ARRAY_TYPE));
    global_GL->Set(String("TEXTURE_COORD_ARRAY_STRIDE"), Value(GL_TEXTURE_COORD_ARRAY_STRIDE));
    global_GL->Set(String("EDGE_FLAG_ARRAY_STRIDE"), Value(GL_EDGE_FLAG_ARRAY_STRIDE));
    global_GL->Set(String("VERTEX_ARRAY_POINTER"), Value(GL_VERTEX_ARRAY_POINTER));
    global_GL->Set(String("NORMAL_ARRAY_POINTER"), Value(GL_NORMAL_ARRAY_POINTER));
    global_GL->Set(String("COLOR_ARRAY_POINTER"), Value(GL_COLOR_ARRAY_POINTER));
    global_GL->Set(String("INDEX_ARRAY_POINTER"), Value(GL_INDEX_ARRAY_POINTER));
    global_GL->Set(String("TEXTURE_COORD_ARRAY_POINTER"), Value(GL_TEXTURE_COORD_ARRAY_POINTER));
    global_GL->Set(String("EDGE_FLAG_ARRAY_POINTER"), Value(GL_EDGE_FLAG_ARRAY_POINTER));
    global_GL->Set(String("V2F"), Value(GL_V2F));
    global_GL->Set(String("V3F"), Value(GL_V3F));
    global_GL->Set(String("C4UB_V2F"), Value(GL_C4UB_V2F));
    global_GL->Set(String("C4UB_V3F"), Value(GL_C4UB_V3F));
    global_GL->Set(String("C3F_V3F"), Value(GL_C3F_V3F));
    global_GL->Set(String("N3F_V3F"), Value(GL_N3F_V3F));
    global_GL->Set(String("C4F_N3F_V3F"), Value(GL_C4F_N3F_V3F));
    global_GL->Set(String("T2F_V3F"), Value(GL_T2F_V3F));
    global_GL->Set(String("T4F_V4F"), Value(GL_T4F_V4F));
    global_GL->Set(String("T2F_C4UB_V3F"), Value(GL_T2F_C4UB_V3F));
    global_GL->Set(String("T2F_C3F_V3F"), Value(GL_T2F_C3F_V3F));
    global_GL->Set(String("T2F_N3F_V3F"), Value(GL_T2F_N3F_V3F));
    global_GL->Set(String("T2F_C4F_N3F_V3F"), Value(GL_T2F_C4F_N3F_V3F));
    global_GL->Set(String("T4F_C4F_N3F_V4F"), Value(GL_T4F_C4F_N3F_V4F));
    global_GL->Set(String("MATRIX_MODE"), Value(GL_MATRIX_MODE));
    global_GL->Set(String("MODELVIEW"), Value(GL_MODELVIEW));
    global_GL->Set(String("PROJECTION"), Value(GL_PROJECTION));
    global_GL->Set(String("TEXTURE"), Value(GL_TEXTURE));
    global_GL->Set(String("POINT_SMOOTH"), Value(GL_POINT_SMOOTH));
    global_GL->Set(String("POINT_SIZE"), Value(GL_POINT_SIZE));
    global_GL->Set(String("POINT_SIZE_GRANULARITY"), Value(GL_POINT_SIZE_GRANULARITY));
    global_GL->Set(String("POINT_SIZE_RANGE"), Value(GL_POINT_SIZE_RANGE));
    global_GL->Set(String("LINE_SMOOTH"), Value(GL_LINE_SMOOTH));
    global_GL->Set(String("LINE_STIPPLE"), Value(GL_LINE_STIPPLE));
    global_GL->Set(String("LINE_STIPPLE_PATTERN"), Value(GL_LINE_STIPPLE_PATTERN));
    global_GL->Set(String("LINE_STIPPLE_REPEAT"), Value(GL_LINE_STIPPLE_REPEAT));
    global_GL->Set(String("LINE_WIDTH"), Value(GL_LINE_WIDTH));
    global_GL->Set(String("LINE_WIDTH_GRANULARITY"), Value(GL_LINE_WIDTH_GRANULARITY));
    global_GL->Set(String("LINE_WIDTH_RANGE"), Value(GL_LINE_WIDTH_RANGE));
    global_GL->Set(String("POINT"), Value(GL_POINT));
    global_GL->Set(String("LINE"), Value(GL_LINE));
    global_GL->Set(String("FILL"), Value(GL_FILL));
    global_GL->Set(String("CW"), Value(GL_CW));
    global_GL->Set(String("CCW"), Value(GL_CCW));
    global_GL->Set(String("FRONT"), Value(GL_FRONT));
    global_GL->Set(String("BACK"), Value(GL_BACK));
    global_GL->Set(String("POLYGON_MODE"), Value(GL_POLYGON_MODE));
    global_GL->Set(String("POLYGON_SMOOTH"), Value(GL_POLYGON_SMOOTH));
    global_GL->Set(String("POLYGON_STIPPLE"), Value(GL_POLYGON_STIPPLE));
    global_GL->Set(String("EDGE_FLAG"), Value(GL_EDGE_FLAG));
    global_GL->Set(String("CULL_FACE"), Value(GL_CULL_FACE));
    global_GL->Set(String("CULL_FACE_MODE"), Value(GL_CULL_FACE_MODE));
    global_GL->Set(String("FRONT_FACE"), Value(GL_FRONT_FACE));
    global_GL->Set(String("POLYGON_OFFSET_FACTOR"), Value(GL_POLYGON_OFFSET_FACTOR));
    global_GL->Set(String("POLYGON_OFFSET_UNITS"), Value(GL_POLYGON_OFFSET_UNITS));
    global_GL->Set(String("POLYGON_OFFSET_POINT"), Value(GL_POLYGON_OFFSET_POINT));
    global_GL->Set(String("POLYGON_OFFSET_LINE"), Value(GL_POLYGON_OFFSET_LINE));
    global_GL->Set(String("POLYGON_OFFSET_FILL"), Value(GL_POLYGON_OFFSET_FILL));
    global_GL->Set(String("COMPILE"), Value(GL_COMPILE));
    global_GL->Set(String("COMPILE_AND_EXECUTE"), Value(GL_COMPILE_AND_EXECUTE));
    global_GL->Set(String("LIST_BASE"), Value(GL_LIST_BASE));
    global_GL->Set(String("LIST_INDEX"), Value(GL_LIST_INDEX));
    global_GL->Set(String("LIST_MODE"), Value(GL_LIST_MODE));
    global_GL->Set(String("NEVER"), Value(GL_NEVER));
    global_GL->Set(String("LESS"), Value(GL_LESS));
    global_GL->Set(String("EQUAL"), Value(GL_EQUAL));
    global_GL->Set(String("LEQUAL"), Value(GL_LEQUAL));
    global_GL->Set(String("GREATER"), Value(GL_GREATER));
    global_GL->Set(String("NOTEQUAL"), Value(GL_NOTEQUAL));
    global_GL->Set(String("GEQUAL"), Value(GL_GEQUAL));
    global_GL->Set(String("ALWAYS"), Value(GL_ALWAYS));
    global_GL->Set(String("DEPTH_TEST"), Value(GL_DEPTH_TEST));
    global_GL->Set(String("DEPTH_BITS"), Value(GL_DEPTH_BITS));
    global_GL->Set(String("DEPTH_CLEAR_VALUE"), Value(GL_DEPTH_CLEAR_VALUE));
    global_GL->Set(String("DEPTH_FUNC"), Value(GL_DEPTH_FUNC));
    global_GL->Set(String("DEPTH_RANGE"), Value(GL_DEPTH_RANGE));
    global_GL->Set(String("DEPTH_WRITEMASK"), Value(GL_DEPTH_WRITEMASK));
    global_GL->Set(String("DEPTH_COMPONENT"), Value(GL_DEPTH_COMPONENT));
    global_GL->Set(String("LIGHTING"), Value(GL_LIGHTING));
    global_GL->Set(String("LIGHT0"), Value(GL_LIGHT0));
    global_GL->Set(String("LIGHT1"), Value(GL_LIGHT1));
    global_GL->Set(String("LIGHT2"), Value(GL_LIGHT2));
    global_GL->Set(String("LIGHT3"), Value(GL_LIGHT3));
    global_GL->Set(String("LIGHT4"), Value(GL_LIGHT4));
    global_GL->Set(String("LIGHT5"), Value(GL_LIGHT5));
    global_GL->Set(String("LIGHT6"), Value(GL_LIGHT6));
    global_GL->Set(String("LIGHT7"), Value(GL_LIGHT7));
    global_GL->Set(String("SPOT_EXPONENT"), Value(GL_SPOT_EXPONENT));
    global_GL->Set(String("SPOT_CUTOFF"), Value(GL_SPOT_CUTOFF));
    global_GL->Set(String("CONSTANT_ATTENUATION"), Value(GL_CONSTANT_ATTENUATION));
    global_GL->Set(String("LINEAR_ATTENUATION"), Value(GL_LINEAR_ATTENUATION));
    global_GL->Set(String("QUADRATIC_ATTENUATION"), Value(GL_QUADRATIC_ATTENUATION));
    global_GL->Set(String("AMBIENT"), Value(GL_AMBIENT));
    global_GL->Set(String("DIFFUSE"), Value(GL_DIFFUSE));
    global_GL->Set(String("SPECULAR"), Value(GL_SPECULAR));
    global_GL->Set(String("SHININESS"), Value(GL_SHININESS));
    global_GL->Set(String("EMISSION"), Value(GL_EMISSION));
    global_GL->Set(String("POSITION"), Value(GL_POSITION));
    global_GL->Set(String("SPOT_DIRECTION"), Value(GL_SPOT_DIRECTION));
    global_GL->Set(String("AMBIENT_AND_DIFFUSE"), Value(GL_AMBIENT_AND_DIFFUSE));
    global_GL->Set(String("COLOR_INDEXES"), Value(GL_COLOR_INDEXES));
    global_GL->Set(String("LIGHT_MODEL_TWO_SIDE"), Value(GL_LIGHT_MODEL_TWO_SIDE));
    global_GL->Set(String("LIGHT_MODEL_LOCAL_VIEWER"), Value(GL_LIGHT_MODEL_LOCAL_VIEWER));
    global_GL->Set(String("LIGHT_MODEL_AMBIENT"), Value(GL_LIGHT_MODEL_AMBIENT));
    global_GL->Set(String("FRONT_AND_BACK"), Value(GL_FRONT_AND_BACK));
    global_GL->Set(String("SHADE_MODEL"), Value(GL_SHADE_MODEL));
    global_GL->Set(String("FLAT"), Value(GL_FLAT));
    global_GL->Set(String("SMOOTH"), Value(GL_SMOOTH));
    global_GL->Set(String("COLOR_MATERIAL"), Value(GL_COLOR_MATERIAL));
    global_GL->Set(String("COLOR_MATERIAL_FACE"), Value(GL_COLOR_MATERIAL_FACE));
    global_GL->Set(String("COLOR_MATERIAL_PARAMETER"), Value(GL_COLOR_MATERIAL_PARAMETER));
    global_GL->Set(String("NORMALIZE"), Value(GL_NORMALIZE));
    global_GL->Set(String("CLIP_PLANE0"), Value(GL_CLIP_PLANE0));
    global_GL->Set(String("CLIP_PLANE1"), Value(GL_CLIP_PLANE1));
    global_GL->Set(String("CLIP_PLANE2"), Value(GL_CLIP_PLANE2));
    global_GL->Set(String("CLIP_PLANE3"), Value(GL_CLIP_PLANE3));
    global_GL->Set(String("CLIP_PLANE4"), Value(GL_CLIP_PLANE4));
    global_GL->Set(String("CLIP_PLANE5"), Value(GL_CLIP_PLANE5));
    global_GL->Set(String("ACCUM_RED_BITS"), Value(GL_ACCUM_RED_BITS));
    global_GL->Set(String("ACCUM_GREEN_BITS"), Value(GL_ACCUM_GREEN_BITS));
    global_GL->Set(String("ACCUM_BLUE_BITS"), Value(GL_ACCUM_BLUE_BITS));
    global_GL->Set(String("ACCUM_ALPHA_BITS"), Value(GL_ACCUM_ALPHA_BITS));
    global_GL->Set(String("ACCUM_CLEAR_VALUE"), Value(GL_ACCUM_CLEAR_VALUE));
    global_GL->Set(String("ACCUM"), Value(GL_ACCUM));
    global_GL->Set(String("ADD"), Value(GL_ADD));
    global_GL->Set(String("LOAD"), Value(GL_LOAD));
    global_GL->Set(String("MULT"), Value(GL_MULT));
    global_GL->Set(String("RETURN"), Value(GL_RETURN));
    global_GL->Set(String("ALPHA_TEST"), Value(GL_ALPHA_TEST));
    global_GL->Set(String("ALPHA_TEST_REF"), Value(GL_ALPHA_TEST_REF));
    global_GL->Set(String("ALPHA_TEST_FUNC"), Value(GL_ALPHA_TEST_FUNC));
    global_GL->Set(String("BLEND"), Value(GL_BLEND));
    global_GL->Set(String("BLEND_SRC"), Value(GL_BLEND_SRC));
    global_GL->Set(String("BLEND_DST"), Value(GL_BLEND_DST));
    global_GL->Set(String("ZERO"), Value(GL_ZERO));
    global_GL->Set(String("ONE"), Value(GL_ONE));
    global_GL->Set(String("SRC_COLOR"), Value(GL_SRC_COLOR));
    global_GL->Set(String("ONE_MINUS_SRC_COLOR"), Value(GL_ONE_MINUS_SRC_COLOR));
    global_GL->Set(String("SRC_ALPHA"), Value(GL_SRC_ALPHA));
    global_GL->Set(String("ONE_MINUS_SRC_ALPHA"), Value(GL_ONE_MINUS_SRC_ALPHA));
    global_GL->Set(String("DST_ALPHA"), Value(GL_DST_ALPHA));
    global_GL->Set(String("ONE_MINUS_DST_ALPHA"), Value(GL_ONE_MINUS_DST_ALPHA));
    global_GL->Set(String("DST_COLOR"), Value(GL_DST_COLOR));
    global_GL->Set(String("ONE_MINUS_DST_COLOR"), Value(GL_ONE_MINUS_DST_COLOR));
    global_GL->Set(String("SRC_ALPHA_SATURATE"), Value(GL_SRC_ALPHA_SATURATE));
    global_GL->Set(String("FEEDBACK"), Value(GL_FEEDBACK));
    global_GL->Set(String("RENDER"), Value(GL_RENDER));
    global_GL->Set(String("SELECT"), Value(GL_SELECT));
    global_GL->Set(String("POINT_TOKEN"), Value(GL_POINT_TOKEN));
    global_GL->Set(String("LINE_TOKEN"), Value(GL_LINE_TOKEN));
    global_GL->Set(String("LINE_RESET_TOKEN"), Value(GL_LINE_RESET_TOKEN));
    global_GL->Set(String("POLYGON_TOKEN"), Value(GL_POLYGON_TOKEN));
    global_GL->Set(String("BITMAP_TOKEN"), Value(GL_BITMAP_TOKEN));
    global_GL->Set(String("DRAW_PIXEL_TOKEN"), Value(GL_DRAW_PIXEL_TOKEN));
    global_GL->Set(String("COPY_PIXEL_TOKEN"), Value(GL_COPY_PIXEL_TOKEN));
    global_GL->Set(String("PASS_THROUGH_TOKEN"), Value(GL_PASS_THROUGH_TOKEN));
    global_GL->Set(String("FEEDBACK_BUFFER_POINTER"), Value(GL_FEEDBACK_BUFFER_POINTER));
    global_GL->Set(String("FEEDBACK_BUFFER_SIZE"), Value(GL_FEEDBACK_BUFFER_SIZE));
    global_GL->Set(String("FEEDBACK_BUFFER_TYPE"), Value(GL_FEEDBACK_BUFFER_TYPE));
    global_GL->Set(String("SELECTION_BUFFER_POINTER"), Value(GL_SELECTION_BUFFER_POINTER));
    global_GL->Set(String("SELECTION_BUFFER_SIZE"), Value(GL_SELECTION_BUFFER_SIZE));
    global_GL->Set(String("FOG"), Value(GL_FOG));
    global_GL->Set(String("FOG_MODE"), Value(GL_FOG_MODE));
    global_GL->Set(String("FOG_DENSITY"), Value(GL_FOG_DENSITY));
    global_GL->Set(String("FOG_COLOR"), Value(GL_FOG_COLOR));
    global_GL->Set(String("FOG_INDEX"), Value(GL_FOG_INDEX));
    global_GL->Set(String("FOG_START"), Value(GL_FOG_START));
    global_GL->Set(String("FOG_END"), Value(GL_FOG_END));
    global_GL->Set(String("LINEAR"), Value(GL_LINEAR));
    global_GL->Set(String("EXP"), Value(GL_EXP));
    global_GL->Set(String("EXP2"), Value(GL_EXP2));
    global_GL->Set(String("LOGIC_OP"), Value(GL_LOGIC_OP));
    global_GL->Set(String("INDEX_LOGIC_OP"), Value(GL_INDEX_LOGIC_OP));
    global_GL->Set(String("COLOR_LOGIC_OP"), Value(GL_COLOR_LOGIC_OP));
    global_GL->Set(String("LOGIC_OP_MODE"), Value(GL_LOGIC_OP_MODE));
    global_GL->Set(String("CLEAR"), Value(GL_CLEAR));
    global_GL->Set(String("SET"), Value(GL_SET));
    global_GL->Set(String("COPY"), Value(GL_COPY));
    global_GL->Set(String("COPY_INVERTED"), Value(GL_COPY_INVERTED));
    global_GL->Set(String("NOOP"), Value(GL_NOOP));
    global_GL->Set(String("INVERT"), Value(GL_INVERT));
    global_GL->Set(String("AND"), Value(GL_AND));
    global_GL->Set(String("NAND"), Value(GL_NAND));
    global_GL->Set(String("OR"), Value(GL_OR));
    global_GL->Set(String("NOR"), Value(GL_NOR));
    global_GL->Set(String("XOR"), Value(GL_XOR));
    global_GL->Set(String("EQUIV"), Value(GL_EQUIV));
    global_GL->Set(String("AND_REVERSE"), Value(GL_AND_REVERSE));
    global_GL->Set(String("AND_INVERTED"), Value(GL_AND_INVERTED));
    global_GL->Set(String("OR_REVERSE"), Value(GL_OR_REVERSE));
    global_GL->Set(String("OR_INVERTED"), Value(GL_OR_INVERTED));
    global_GL->Set(String("STENCIL_BITS"), Value(GL_STENCIL_BITS));
    global_GL->Set(String("STENCIL_TEST"), Value(GL_STENCIL_TEST));
    global_GL->Set(String("STENCIL_CLEAR_VALUE"), Value(GL_STENCIL_CLEAR_VALUE));
    global_GL->Set(String("STENCIL_FUNC"), Value(GL_STENCIL_FUNC));
    global_GL->Set(String("STENCIL_VALUE_MASK"), Value(GL_STENCIL_VALUE_MASK));
    global_GL->Set(String("STENCIL_FAIL"), Value(GL_STENCIL_FAIL));
    global_GL->Set(String("STENCIL_PASS_DEPTH_FAIL"), Value(GL_STENCIL_PASS_DEPTH_FAIL));
    global_GL->Set(String("STENCIL_PASS_DEPTH_PASS"), Value(GL_STENCIL_PASS_DEPTH_PASS));
    global_GL->Set(String("STENCIL_REF"), Value(GL_STENCIL_REF));
    global_GL->Set(String("STENCIL_WRITEMASK"), Value(GL_STENCIL_WRITEMASK));
    global_GL->Set(String("STENCIL_INDEX"), Value(GL_STENCIL_INDEX));
    global_GL->Set(String("KEEP"), Value(GL_KEEP));
    global_GL->Set(String("REPLACE"), Value(GL_REPLACE));
    global_GL->Set(String("INCR"), Value(GL_INCR));
    global_GL->Set(String("DECR"), Value(GL_DECR));
    global_GL->Set(String("NONE"), Value(GL_NONE));
    global_GL->Set(String("LEFT"), Value(GL_LEFT));
    global_GL->Set(String("RIGHT"), Value(GL_RIGHT));
    global_GL->Set(String("FRONT_LEFT"), Value(GL_FRONT_LEFT));
    global_GL->Set(String("FRONT_RIGHT"), Value(GL_FRONT_RIGHT));
    global_GL->Set(String("BACK_LEFT"), Value(GL_BACK_LEFT));
    global_GL->Set(String("BACK_RIGHT"), Value(GL_BACK_RIGHT));
    global_GL->Set(String("AUX0"), Value(GL_AUX0));
    global_GL->Set(String("AUX1"), Value(GL_AUX1));
    global_GL->Set(String("AUX2"), Value(GL_AUX2));
    global_GL->Set(String("AUX3"), Value(GL_AUX3));
    global_GL->Set(String("COLOR_INDEX"), Value(GL_COLOR_INDEX));
    global_GL->Set(String("RED"), Value(GL_RED));
    global_GL->Set(String("GREEN"), Value(GL_GREEN));
    global_GL->Set(String("BLUE"), Value(GL_BLUE));
    global_GL->Set(String("ALPHA"), Value(GL_ALPHA));
    global_GL->Set(String("LUMINANCE"), Value(GL_LUMINANCE));
    global_GL->Set(String("LUMINANCE_ALPHA"), Value(GL_LUMINANCE_ALPHA));
    global_GL->Set(String("ALPHA_BITS"), Value(GL_ALPHA_BITS));
    global_GL->Set(String("RED_BITS"), Value(GL_RED_BITS));
    global_GL->Set(String("GREEN_BITS"), Value(GL_GREEN_BITS));
    global_GL->Set(String("BLUE_BITS"), Value(GL_BLUE_BITS));
    global_GL->Set(String("INDEX_BITS"), Value(GL_INDEX_BITS));
    global_GL->Set(String("SUBPIXEL_BITS"), Value(GL_SUBPIXEL_BITS));
    global_GL->Set(String("AUX_BUFFERS"), Value(GL_AUX_BUFFERS));
    global_GL->Set(String("READ_BUFFER"), Value(GL_READ_BUFFER));
    global_GL->Set(String("DRAW_BUFFER"), Value(GL_DRAW_BUFFER));
    global_GL->Set(String("DOUBLEBUFFER"), Value(GL_DOUBLEBUFFER));
    global_GL->Set(String("STEREO"), Value(GL_STEREO));
    global_GL->Set(String("BITMAP"), Value(GL_BITMAP));
    global_GL->Set(String("COLOR"), Value(GL_COLOR));
    global_GL->Set(String("DEPTH"), Value(GL_DEPTH));
    global_GL->Set(String("STENCIL"), Value(GL_STENCIL));
    global_GL->Set(String("DITHER"), Value(GL_DITHER));
    global_GL->Set(String("RGB"), Value(GL_RGB));
    global_GL->Set(String("RGBA"), Value(GL_RGBA));
    global_GL->Set(String("MAX_LIST_NESTING"), Value(GL_MAX_LIST_NESTING));
    global_GL->Set(String("MAX_EVAL_ORDER"), Value(GL_MAX_EVAL_ORDER));
    global_GL->Set(String("MAX_LIGHTS"), Value(GL_MAX_LIGHTS));
    global_GL->Set(String("MAX_CLIP_PLANES"), Value(GL_MAX_CLIP_PLANES));
    global_GL->Set(String("MAX_TEXTURE_SIZE"), Value(GL_MAX_TEXTURE_SIZE));
    global_GL->Set(String("MAX_PIXEL_MAP_TABLE"), Value(GL_MAX_PIXEL_MAP_TABLE));
    global_GL->Set(String("MAX_ATTRIB_STACK_DEPTH"), Value(GL_MAX_ATTRIB_STACK_DEPTH));
    global_GL->Set(String("MAX_MODELVIEW_STACK_DEPTH"), Value(GL_MAX_MODELVIEW_STACK_DEPTH));
    global_GL->Set(String("MAX_NAME_STACK_DEPTH"), Value(GL_MAX_NAME_STACK_DEPTH));
    global_GL->Set(String("MAX_PROJECTION_STACK_DEPTH"), Value(GL_MAX_PROJECTION_STACK_DEPTH));
    global_GL->Set(String("MAX_TEXTURE_STACK_DEPTH"), Value(GL_MAX_TEXTURE_STACK_DEPTH));
    global_GL->Set(String("MAX_VIEWPORT_DIMS"), Value(GL_MAX_VIEWPORT_DIMS));
    global_GL->Set(String("MAX_CLIENT_ATTRIB_STACK_DEPTH"), Value(GL_MAX_CLIENT_ATTRIB_STACK_DEPTH));
    global_GL->Set(String("ATTRIB_STACK_DEPTH"), Value(GL_ATTRIB_STACK_DEPTH));
    global_GL->Set(String("CLIENT_ATTRIB_STACK_DEPTH"), Value(GL_CLIENT_ATTRIB_STACK_DEPTH));
    global_GL->Set(String("COLOR_CLEAR_VALUE"), Value(GL_COLOR_CLEAR_VALUE));
    global_GL->Set(String("COLOR_WRITEMASK"), Value(GL_COLOR_WRITEMASK));
    global_GL->Set(String("CURRENT_INDEX"), Value(GL_CURRENT_INDEX));
    global_GL->Set(String("CURRENT_COLOR"), Value(GL_CURRENT_COLOR));
    global_GL->Set(String("CURRENT_NORMAL"), Value(GL_CURRENT_NORMAL));
    global_GL->Set(String("CURRENT_RASTER_COLOR"), Value(GL_CURRENT_RASTER_COLOR));
    global_GL->Set(String("CURRENT_RASTER_DISTANCE"), Value(GL_CURRENT_RASTER_DISTANCE));
    global_GL->Set(String("CURRENT_RASTER_INDEX"), Value(GL_CURRENT_RASTER_INDEX));
    global_GL->Set(String("CURRENT_RASTER_POSITION"), Value(GL_CURRENT_RASTER_POSITION));
    global_GL->Set(String("CURRENT_RASTER_TEXTURE_COORDS"), Value(GL_CURRENT_RASTER_TEXTURE_COORDS));
    global_GL->Set(String("CURRENT_RASTER_POSITION_VALID"), Value(GL_CURRENT_RASTER_POSITION_VALID));
    global_GL->Set(String("CURRENT_TEXTURE_COORDS"), Value(GL_CURRENT_TEXTURE_COORDS));
    global_GL->Set(String("INDEX_CLEAR_VALUE"), Value(GL_INDEX_CLEAR_VALUE));
    global_GL->Set(String("INDEX_MODE"), Value(GL_INDEX_MODE));
    global_GL->Set(String("INDEX_WRITEMASK"), Value(GL_INDEX_WRITEMASK));
    global_GL->Set(String("MODELVIEW_MATRIX"), Value(GL_MODELVIEW_MATRIX));
    global_GL->Set(String("MODELVIEW_STACK_DEPTH"), Value(GL_MODELVIEW_STACK_DEPTH));
    global_GL->Set(String("NAME_STACK_DEPTH"), Value(GL_NAME_STACK_DEPTH));
    global_GL->Set(String("PROJECTION_MATRIX"), Value(GL_PROJECTION_MATRIX));
    global_GL->Set(String("PROJECTION_STACK_DEPTH"), Value(GL_PROJECTION_STACK_DEPTH));
    global_GL->Set(String("RENDER_MODE"), Value(GL_RENDER_MODE));
    global_GL->Set(String("RGBA_MODE"), Value(GL_RGBA_MODE));
    global_GL->Set(String("TEXTURE_MATRIX"), Value(GL_TEXTURE_MATRIX));
    global_GL->Set(String("TEXTURE_STACK_DEPTH"), Value(GL_TEXTURE_STACK_DEPTH));
    global_GL->Set(String("VIEWPORT"), Value(GL_VIEWPORT));
    global_GL->Set(String("AUTO_NORMAL"), Value(GL_AUTO_NORMAL));
    global_GL->Set(String("MAP1_COLOR_4"), Value(GL_MAP1_COLOR_4));
    global_GL->Set(String("MAP1_INDEX"), Value(GL_MAP1_INDEX));
    global_GL->Set(String("MAP1_NORMAL"), Value(GL_MAP1_NORMAL));
    global_GL->Set(String("MAP1_TEXTURE_COORD_1"), Value(GL_MAP1_TEXTURE_COORD_1));
    global_GL->Set(String("MAP1_TEXTURE_COORD_2"), Value(GL_MAP1_TEXTURE_COORD_2));
    global_GL->Set(String("MAP1_TEXTURE_COORD_3"), Value(GL_MAP1_TEXTURE_COORD_3));
    global_GL->Set(String("MAP1_TEXTURE_COORD_4"), Value(GL_MAP1_TEXTURE_COORD_4));
    global_GL->Set(String("MAP1_VERTEX_3"), Value(GL_MAP1_VERTEX_3));
    global_GL->Set(String("MAP1_VERTEX_4"), Value(GL_MAP1_VERTEX_4));
    global_GL->Set(String("MAP2_COLOR_4"), Value(GL_MAP2_COLOR_4));
    global_GL->Set(String("MAP2_INDEX"), Value(GL_MAP2_INDEX));
    global_GL->Set(String("MAP2_NORMAL"), Value(GL_MAP2_NORMAL));
    global_GL->Set(String("MAP2_TEXTURE_COORD_1"), Value(GL_MAP2_TEXTURE_COORD_1));
    global_GL->Set(String("MAP2_TEXTURE_COORD_2"), Value(GL_MAP2_TEXTURE_COORD_2));
    global_GL->Set(String("MAP2_TEXTURE_COORD_3"), Value(GL_MAP2_TEXTURE_COORD_3));
    global_GL->Set(String("MAP2_TEXTURE_COORD_4"), Value(GL_MAP2_TEXTURE_COORD_4));
    global_GL->Set(String("MAP2_VERTEX_3"), Value(GL_MAP2_VERTEX_3));
    global_GL->Set(String("MAP2_VERTEX_4"), Value(GL_MAP2_VERTEX_4));
    global_GL->Set(String("MAP1_GRID_DOMAIN"), Value(GL_MAP1_GRID_DOMAIN));
    global_GL->Set(String("MAP1_GRID_SEGMENTS"), Value(GL_MAP1_GRID_SEGMENTS));
    global_GL->Set(String("MAP2_GRID_DOMAIN"), Value(GL_MAP2_GRID_DOMAIN));
    global_GL->Set(String("MAP2_GRID_SEGMENTS"), Value(GL_MAP2_GRID_SEGMENTS));
    global_GL->Set(String("COEFF"), Value(GL_COEFF));
    global_GL->Set(String("ORDER"), Value(GL_ORDER));
    global_GL->Set(String("DOMAIN"), Value(GL_DOMAIN));
    global_GL->Set(String("PERSPECTIVE_CORRECTION_HINT"), Value(GL_PERSPECTIVE_CORRECTION_HINT));
    global_GL->Set(String("POINT_SMOOTH_HINT"), Value(GL_POINT_SMOOTH_HINT));
    global_GL->Set(String("LINE_SMOOTH_HINT"), Value(GL_LINE_SMOOTH_HINT));
    global_GL->Set(String("POLYGON_SMOOTH_HINT"), Value(GL_POLYGON_SMOOTH_HINT));
    global_GL->Set(String("FOG_HINT"), Value(GL_FOG_HINT));
    global_GL->Set(String("DONT_CARE"), Value(GL_DONT_CARE));
    global_GL->Set(String("FASTEST"), Value(GL_FASTEST));
    global_GL->Set(String("NICEST"), Value(GL_NICEST));
    global_GL->Set(String("SCISSOR_BOX"), Value(GL_SCISSOR_BOX));
    global_GL->Set(String("SCISSOR_TEST"), Value(GL_SCISSOR_TEST));
    global_GL->Set(String("MAP_COLOR"), Value(GL_MAP_COLOR));
    global_GL->Set(String("MAP_STENCIL"), Value(GL_MAP_STENCIL));
    global_GL->Set(String("INDEX_SHIFT"), Value(GL_INDEX_SHIFT));
    global_GL->Set(String("INDEX_OFFSET"), Value(GL_INDEX_OFFSET));
    global_GL->Set(String("RED_SCALE"), Value(GL_RED_SCALE));
    global_GL->Set(String("RED_BIAS"), Value(GL_RED_BIAS));
    global_GL->Set(String("GREEN_SCALE"), Value(GL_GREEN_SCALE));
    global_GL->Set(String("GREEN_BIAS"), Value(GL_GREEN_BIAS));
    global_GL->Set(String("BLUE_SCALE"), Value(GL_BLUE_SCALE));
    global_GL->Set(String("BLUE_BIAS"), Value(GL_BLUE_BIAS));
    global_GL->Set(String("ALPHA_SCALE"), Value(GL_ALPHA_SCALE));
    global_GL->Set(String("ALPHA_BIAS"), Value(GL_ALPHA_BIAS));
    global_GL->Set(String("DEPTH_SCALE"), Value(GL_DEPTH_SCALE));
    global_GL->Set(String("DEPTH_BIAS"), Value(GL_DEPTH_BIAS));
    global_GL->Set(String("PIXEL_MAP_S_TO_S_SIZE"), Value(GL_PIXEL_MAP_S_TO_S_SIZE));
    global_GL->Set(String("PIXEL_MAP_I_TO_I_SIZE"), Value(GL_PIXEL_MAP_I_TO_I_SIZE));
    global_GL->Set(String("PIXEL_MAP_I_TO_R_SIZE"), Value(GL_PIXEL_MAP_I_TO_R_SIZE));
    global_GL->Set(String("PIXEL_MAP_I_TO_G_SIZE"), Value(GL_PIXEL_MAP_I_TO_G_SIZE));
    global_GL->Set(String("PIXEL_MAP_I_TO_B_SIZE"), Value(GL_PIXEL_MAP_I_TO_B_SIZE));
    global_GL->Set(String("PIXEL_MAP_I_TO_A_SIZE"), Value(GL_PIXEL_MAP_I_TO_A_SIZE));
    global_GL->Set(String("PIXEL_MAP_R_TO_R_SIZE"), Value(GL_PIXEL_MAP_R_TO_R_SIZE));
    global_GL->Set(String("PIXEL_MAP_G_TO_G_SIZE"), Value(GL_PIXEL_MAP_G_TO_G_SIZE));
    global_GL->Set(String("PIXEL_MAP_B_TO_B_SIZE"), Value(GL_PIXEL_MAP_B_TO_B_SIZE));
    global_GL->Set(String("PIXEL_MAP_A_TO_A_SIZE"), Value(GL_PIXEL_MAP_A_TO_A_SIZE));
    global_GL->Set(String("PIXEL_MAP_S_TO_S"), Value(GL_PIXEL_MAP_S_TO_S));
    global_GL->Set(String("PIXEL_MAP_I_TO_I"), Value(GL_PIXEL_MAP_I_TO_I));
    global_GL->Set(String("PIXEL_MAP_I_TO_R"), Value(GL_PIXEL_MAP_I_TO_R));
    global_GL->Set(String("PIXEL_MAP_I_TO_G"), Value(GL_PIXEL_MAP_I_TO_G));
    global_GL->Set(String("PIXEL_MAP_I_TO_B"), Value(GL_PIXEL_MAP_I_TO_B));
    global_GL->Set(String("PIXEL_MAP_I_TO_A"), Value(GL_PIXEL_MAP_I_TO_A));
    global_GL->Set(String("PIXEL_MAP_R_TO_R"), Value(GL_PIXEL_MAP_R_TO_R));
    global_GL->Set(String("PIXEL_MAP_G_TO_G"), Value(GL_PIXEL_MAP_G_TO_G));
    global_GL->Set(String("PIXEL_MAP_B_TO_B"), Value(GL_PIXEL_MAP_B_TO_B));
    global_GL->Set(String("PIXEL_MAP_A_TO_A"), Value(GL_PIXEL_MAP_A_TO_A));
    global_GL->Set(String("PACK_ALIGNMENT"), Value(GL_PACK_ALIGNMENT));
    global_GL->Set(String("PACK_LSB_FIRST"), Value(GL_PACK_LSB_FIRST));
    global_GL->Set(String("PACK_ROW_LENGTH"), Value(GL_PACK_ROW_LENGTH));
    global_GL->Set(String("PACK_SKIP_PIXELS"), Value(GL_PACK_SKIP_PIXELS));
    global_GL->Set(String("PACK_SKIP_ROWS"), Value(GL_PACK_SKIP_ROWS));
    global_GL->Set(String("PACK_SWAP_BYTES"), Value(GL_PACK_SWAP_BYTES));
    global_GL->Set(String("UNPACK_ALIGNMENT"), Value(GL_UNPACK_ALIGNMENT));
    global_GL->Set(String("UNPACK_LSB_FIRST"), Value(GL_UNPACK_LSB_FIRST));
    global_GL->Set(String("UNPACK_ROW_LENGTH"), Value(GL_UNPACK_ROW_LENGTH));
    global_GL->Set(String("UNPACK_SKIP_PIXELS"), Value(GL_UNPACK_SKIP_PIXELS));
    global_GL->Set(String("UNPACK_SKIP_ROWS"), Value(GL_UNPACK_SKIP_ROWS));
    global_GL->Set(String("UNPACK_SWAP_BYTES"), Value(GL_UNPACK_SWAP_BYTES));
    global_GL->Set(String("ZOOM_X"), Value(GL_ZOOM_X));
    global_GL->Set(String("ZOOM_Y"), Value(GL_ZOOM_Y));
    global_GL->Set(String("TEXTURE_ENV"), Value(GL_TEXTURE_ENV));
    global_GL->Set(String("TEXTURE_ENV_MODE"), Value(GL_TEXTURE_ENV_MODE));
    global_GL->Set(String("TEXTURE_1D"), Value(GL_TEXTURE_1D));
    global_GL->Set(String("TEXTURE_2D"), Value(GL_TEXTURE_2D));
    global_GL->Set(String("TEXTURE_WRAP_S"), Value(GL_TEXTURE_WRAP_S));
    global_GL->Set(String("TEXTURE_WRAP_T"), Value(GL_TEXTURE_WRAP_T));
    global_GL->Set(String("TEXTURE_MAG_FILTER"), Value(GL_TEXTURE_MAG_FILTER));
    global_GL->Set(String("TEXTURE_MIN_FILTER"), Value(GL_TEXTURE_MIN_FILTER));
    global_GL->Set(String("TEXTURE_ENV_COLOR"), Value(GL_TEXTURE_ENV_COLOR));
    global_GL->Set(String("TEXTURE_GEN_S"), Value(GL_TEXTURE_GEN_S));
    global_GL->Set(String("TEXTURE_GEN_T"), Value(GL_TEXTURE_GEN_T));
    global_GL->Set(String("TEXTURE_GEN_MODE"), Value(GL_TEXTURE_GEN_MODE));
    global_GL->Set(String("TEXTURE_BORDER_COLOR"), Value(GL_TEXTURE_BORDER_COLOR));
    global_GL->Set(String("TEXTURE_WIDTH"), Value(GL_TEXTURE_WIDTH));
    global_GL->Set(String("TEXTURE_HEIGHT"), Value(GL_TEXTURE_HEIGHT));
    global_GL->Set(String("TEXTURE_BORDER"), Value(GL_TEXTURE_BORDER));
    global_GL->Set(String("TEXTURE_COMPONENTS"), Value(GL_TEXTURE_COMPONENTS));
    global_GL->Set(String("TEXTURE_RED_SIZE"), Value(GL_TEXTURE_RED_SIZE));
    global_GL->Set(String("TEXTURE_GREEN_SIZE"), Value(GL_TEXTURE_GREEN_SIZE));
    global_GL->Set(String("TEXTURE_BLUE_SIZE"), Value(GL_TEXTURE_BLUE_SIZE));
    global_GL->Set(String("TEXTURE_ALPHA_SIZE"), Value(GL_TEXTURE_ALPHA_SIZE));
    global_GL->Set(String("TEXTURE_LUMINANCE_SIZE"), Value(GL_TEXTURE_LUMINANCE_SIZE));
    global_GL->Set(String("TEXTURE_INTENSITY_SIZE"), Value(GL_TEXTURE_INTENSITY_SIZE));
    global_GL->Set(String("NEAREST_MIPMAP_NEAREST"), Value(GL_NEAREST_MIPMAP_NEAREST));
    global_GL->Set(String("NEAREST_MIPMAP_LINEAR"), Value(GL_NEAREST_MIPMAP_LINEAR));
    global_GL->Set(String("LINEAR_MIPMAP_NEAREST"), Value(GL_LINEAR_MIPMAP_NEAREST));
    global_GL->Set(String("LINEAR_MIPMAP_LINEAR"), Value(GL_LINEAR_MIPMAP_LINEAR));
    global_GL->Set(String("OBJECT_LINEAR"), Value(GL_OBJECT_LINEAR));
    global_GL->Set(String("OBJECT_PLANE"), Value(GL_OBJECT_PLANE));
    global_GL->Set(String("EYE_LINEAR"), Value(GL_EYE_LINEAR));
    global_GL->Set(String("EYE_PLANE"), Value(GL_EYE_PLANE));
    global_GL->Set(String("SPHERE_MAP"), Value(GL_SPHERE_MAP));
    global_GL->Set(String("DECAL"), Value(GL_DECAL));
    global_GL->Set(String("MODULATE"), Value(GL_MODULATE));
    global_GL->Set(String("NEAREST"), Value(GL_NEAREST));
    global_GL->Set(String("REPEAT"), Value(GL_REPEAT));
    global_GL->Set(String("CLAMP"), Value(GL_CLAMP));
    global_GL->Set(String("S"), Value(GL_S));
    global_GL->Set(String("T"), Value(GL_T));
    global_GL->Set(String("R"), Value(GL_R));
    global_GL->Set(String("Q"), Value(GL_Q));
    global_GL->Set(String("TEXTURE_GEN_R"), Value(GL_TEXTURE_GEN_R));
    global_GL->Set(String("TEXTURE_GEN_Q"), Value(GL_TEXTURE_GEN_Q));
    global_GL->Set(String("VENDOR"), Value(GL_VENDOR));
    global_GL->Set(String("RENDERER"), Value(GL_RENDERER));
    global_GL->Set(String("VERSION"), Value(GL_VERSION));
    global_GL->Set(String("EXTENSIONS"), Value(GL_EXTENSIONS));
    global_GL->Set(String("NO_ERROR"), Value(GL_NO_ERROR));
    global_GL->Set(String("INVALID_ENUM"), Value(GL_INVALID_ENUM));
    global_GL->Set(String("INVALID_VALUE"), Value(GL_INVALID_VALUE));
    global_GL->Set(String("INVALID_OPERATION"), Value(GL_INVALID_OPERATION));
    global_GL->Set(String("STACK_OVERFLOW"), Value(GL_STACK_OVERFLOW));
    global_GL->Set(String("STACK_UNDERFLOW"), Value(GL_STACK_UNDERFLOW));
    global_GL->Set(String("OUT_OF_MEMORY"), Value(GL_OUT_OF_MEMORY));
    global_GL->Set(String("CURRENT_BIT"), Value(GL_CURRENT_BIT));
    global_GL->Set(String("POINT_BIT"), Value(GL_POINT_BIT));
    global_GL->Set(String("LINE_BIT"), Value(GL_LINE_BIT));
    global_GL->Set(String("POLYGON_BIT"), Value(GL_POLYGON_BIT));
    global_GL->Set(String("POLYGON_STIPPLE_BIT"), Value(GL_POLYGON_STIPPLE_BIT));
    global_GL->Set(String("PIXEL_MODE_BIT"), Value(GL_PIXEL_MODE_BIT));
    global_GL->Set(String("LIGHTING_BIT"), Value(GL_LIGHTING_BIT));
    global_GL->Set(String("FOG_BIT"), Value(GL_FOG_BIT));
    global_GL->Set(String("DEPTH_BUFFER_BIT"), Value(GL_DEPTH_BUFFER_BIT));
    global_GL->Set(String("ACCUM_BUFFER_BIT"), Value(GL_ACCUM_BUFFER_BIT));
    global_GL->Set(String("STENCIL_BUFFER_BIT"), Value(GL_STENCIL_BUFFER_BIT));
    global_GL->Set(String("VIEWPORT_BIT"), Value(GL_VIEWPORT_BIT));
    global_GL->Set(String("TRANSFORM_BIT"), Value(GL_TRANSFORM_BIT));
    global_GL->Set(String("ENABLE_BIT"), Value(GL_ENABLE_BIT));
    global_GL->Set(String("COLOR_BUFFER_BIT"), Value(GL_COLOR_BUFFER_BIT));
    global_GL->Set(String("HINT_BIT"), Value(GL_HINT_BIT));
    global_GL->Set(String("EVAL_BIT"), Value(GL_EVAL_BIT));
    global_GL->Set(String("LIST_BIT"), Value(GL_LIST_BIT));
    global_GL->Set(String("TEXTURE_BIT"), Value(GL_TEXTURE_BIT));
    global_GL->Set(String("SCISSOR_BIT"), Value(GL_SCISSOR_BIT));
    global_GL->Set(String("ALL_ATTRIB_BITS"), Value(GL_ALL_ATTRIB_BITS));
    global_GL->Set(String("PROXY_TEXTURE_1D"), Value(GL_PROXY_TEXTURE_1D));
    global_GL->Set(String("PROXY_TEXTURE_2D"), Value(GL_PROXY_TEXTURE_2D));
    global_GL->Set(String("TEXTURE_PRIORITY"), Value(GL_TEXTURE_PRIORITY));
    global_GL->Set(String("TEXTURE_RESIDENT"), Value(GL_TEXTURE_RESIDENT));
    global_GL->Set(String("TEXTURE_BINDING_1D"), Value(GL_TEXTURE_BINDING_1D));
    global_GL->Set(String("TEXTURE_BINDING_2D"), Value(GL_TEXTURE_BINDING_2D));
    global_GL->Set(String("TEXTURE_INTERNAL_FORMAT"), Value(GL_TEXTURE_INTERNAL_FORMAT));
    global_GL->Set(String("ALPHA4"), Value(GL_ALPHA4));
    global_GL->Set(String("ALPHA8"), Value(GL_ALPHA8));
    global_GL->Set(String("ALPHA12"), Value(GL_ALPHA12));
    global_GL->Set(String("ALPHA16"), Value(GL_ALPHA16));
    global_GL->Set(String("LUMINANCE4"), Value(GL_LUMINANCE4));
    global_GL->Set(String("LUMINANCE8"), Value(GL_LUMINANCE8));
    global_GL->Set(String("LUMINANCE12"), Value(GL_LUMINANCE12));
    global_GL->Set(String("LUMINANCE16"), Value(GL_LUMINANCE16));
    global_GL->Set(String("LUMINANCE4_ALPHA4"), Value(GL_LUMINANCE4_ALPHA4));
    global_GL->Set(String("LUMINANCE6_ALPHA2"), Value(GL_LUMINANCE6_ALPHA2));
    global_GL->Set(String("LUMINANCE8_ALPHA8"), Value(GL_LUMINANCE8_ALPHA8));
    global_GL->Set(String("LUMINANCE12_ALPHA4"), Value(GL_LUMINANCE12_ALPHA4));
    global_GL->Set(String("LUMINANCE12_ALPHA12"), Value(GL_LUMINANCE12_ALPHA12));
    global_GL->Set(String("LUMINANCE16_ALPHA16"), Value(GL_LUMINANCE16_ALPHA16));
    global_GL->Set(String("INTENSITY"), Value(GL_INTENSITY));
    global_GL->Set(String("INTENSITY4"), Value(GL_INTENSITY4));
    global_GL->Set(String("INTENSITY8"), Value(GL_INTENSITY8));
    global_GL->Set(String("INTENSITY12"), Value(GL_INTENSITY12));
    global_GL->Set(String("INTENSITY16"), Value(GL_INTENSITY16));
    global_GL->Set(String("R3_G3_B2"), Value(GL_R3_G3_B2));
    global_GL->Set(String("RGB4"), Value(GL_RGB4));
    global_GL->Set(String("RGB5"), Value(GL_RGB5));
    global_GL->Set(String("RGB8"), Value(GL_RGB8));
    global_GL->Set(String("RGB10"), Value(GL_RGB10));
    global_GL->Set(String("RGB12"), Value(GL_RGB12));
    global_GL->Set(String("RGB16"), Value(GL_RGB16));
    global_GL->Set(String("RGBA2"), Value(GL_RGBA2));
    global_GL->Set(String("RGBA4"), Value(GL_RGBA4));
    global_GL->Set(String("RGB5_A1"), Value(GL_RGB5_A1));
    global_GL->Set(String("RGBA8"), Value(GL_RGBA8));
    global_GL->Set(String("RGB10_A2"), Value(GL_RGB10_A2));
    global_GL->Set(String("RGBA12"), Value(GL_RGBA12));
    global_GL->Set(String("RGBA16"), Value(GL_RGBA16));
    global_GL->Set(String("CLIENT_PIXEL_STORE_BIT"), Value(GL_CLIENT_PIXEL_STORE_BIT));
    global_GL->Set(String("CLIENT_VERTEX_ARRAY_BIT"), Value(GL_CLIENT_VERTEX_ARRAY_BIT));
    global_GL->Set(String("CLIENT_ALL_ATTRIB_BITS"), Value(GL_CLIENT_ALL_ATTRIB_BITS));
}