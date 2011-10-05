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
        THROW_ERROR(String::concat("GL_ERROR: ", errorString));
    else
        return undefined;
}

template <class T>
T *ArrayToVector(Value array) {
    if(!array.is<Array>())
        return 0;
    int length = array.length();
    T *vector = new T [length];
    for(int i = 0; i < length; i++)
        vector[i] = array[i];
    return vector;
}

static v8::Handle<v8::Value> _GL_initWindow(const v8::Arguments &args) {
    if(args.Length() >= 3) {
        #line 144 "src/modules/GL.gear"
        Value name(args[0]), w(args[1]), h(args[2]);
        if(bGLIsUsed)
            THROW_ERROR("GL is already being used");
        int argc = 0;
        glutInit(&argc, 0);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowSize(w, h);
        glutCreateWindow(name.to<String>());
        return undefined;
    }
    THROW_ERROR("Invalid call to GL.initWindow");
}

static v8::Handle<v8::Value> _GL_mainLoop(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 154 "src/modules/GL.gear"
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
    THROW_ERROR("Invalid call to GL.mainLoop");
}

static v8::Handle<v8::Value> _GL_addTimer(const v8::Arguments &args) {
    if(args.Length() >= 2) {
        #line 188 "src/modules/GL.gear"
        Value ms(args[0]), func(args[1]);
        pTimers[nLastTimer] = new TimerCallback(func);
        glutTimerFunc(ms, GLProxyTimerFunc, nLastTimer);
        return Integer(nLastTimer++);
    }
    THROW_ERROR("Invalid call to GL.addTimer");
}

static v8::Handle<v8::Value> _GL_cancelTimer(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 194 "src/modules/GL.gear"
        Value idx(args[0]);
        if(!pTimers.count(idx))
            return undefined;
        TimerCallback *pTimer = pTimers[idx];
        pTimers.erase(idx.to<int>());
        delete pTimer;
        return undefined;
    }
    THROW_ERROR("Invalid call to GL.cancelTimer");
}

static v8::Handle<v8::Value> _GL_ignoreKeyRepeat(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 202 "src/modules/GL.gear"
        Value ignore(args[0]);
        glutIgnoreKeyRepeat(ignore);
        return undefined;
    }
    THROW_ERROR("Invalid call to GL.ignoreKeyRepeat");
}

static v8::Handle<v8::Value> _GL_warpPointer(const v8::Arguments &args) {
    if(args.Length() >= 2) {
        #line 206 "src/modules/GL.gear"
        Value x(args[0]), y(args[1]);
        glutWarpPointer(x, y);
        return undefined;
    }
    THROW_ERROR("Invalid call to GL.warpPointer");
}

static v8::Handle<v8::Value> _GL_setCursor(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 210 "src/modules/GL.gear"
        Value cursor(args[0]);
        glutSetCursor(cursor);
        return undefined;
    }
    THROW_ERROR("Invalid call to GL.setCursor");
}

static v8::Handle<v8::Value> _GL_swapBuffers(const v8::Arguments &args) {
    #line 215 "src/modules/GL.gear"
    glutSwapBuffers();
    return undefined;
}

static v8::Handle<v8::Value> _GL_postRedisplay(const v8::Arguments &args) {
    #line 219 "src/modules/GL.gear"
    glutPostRedisplay();
    return undefined;
}

static v8::Handle<v8::Value> _GL_bitmapCharacter(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 222 "src/modules/GL.gear"
        Value c(args[0]);
        if(c.length())
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, **c.to<String>());
        return undefined;
    }
    THROW_ERROR("Invalid call to GL.bitmapCharacter");
}

static v8::Handle<v8::Value> _GL_perspective(const v8::Arguments &args) {
    if(args.Length() >= 4) {
        #line 256 "src/modules/GL.gear"
        Value fovy(args[0]), aspect(args[1]), zNear(args[2]), zFar(args[3]);
        gluPerspective(fovy, aspect, zNear, zFar);
        return undefined;
    }
    THROW_ERROR("Invalid call to GL.perspective");
}

static v8::Handle<v8::Value> _GL_ortho2D(const v8::Arguments &args) {
    if(args.Length() >= 4) {
        #line 260 "src/modules/GL.gear"
        Value left(args[0]), right(args[1]), bottom(args[2]), top(args[3]);
        gluOrtho2D(left, right, bottom, top);
        return undefined;
    }
    THROW_ERROR("Invalid call to GL.ortho2D");
}

static v8::Handle<v8::Value> _GL_lookAt(const v8::Arguments &args) {
    if(args.Length() >= 9) {
        #line 264 "src/modules/GL.gear"
        Value eyeX(args[0]), eyeY(args[1]), eyeZ(args[2]), centerX(args[3]), centerY(args[4]), centerZ(args[5]), upX(args[6]), upY(args[7]), upZ(args[8]);
        gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
        return undefined;
    }
    THROW_ERROR("Invalid call to GL.lookAt");
}

static v8::Handle<v8::Value> _GL_makeFloatArray(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 269 "src/modules/GL.gear"
        Value size(args[0]);
        float *array = new float [size.to<size_t>()];
        var obj = Object();
        obj.to<v8::Handle<v8::Object>>()->SetIndexedPropertiesToExternalArrayData(array, v8::kExternalFloatArray, size);
        return obj;
    }
    THROW_ERROR("Invalid call to GL.makeFloatArray");
}

static v8::Handle<v8::Value> _GL_makeUInt32Array(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 275 "src/modules/GL.gear"
        Value size(args[0]);
        uint32_t *array = new uint32_t [size.to<size_t>()];
        var obj = Object();
        obj.to<v8::Handle<v8::Object>>()->SetIndexedPropertiesToExternalArrayData(array, v8::kExternalUnsignedIntArray, size);
        return obj;
    }
    THROW_ERROR("Invalid call to GL.makeUInt32Array");
}

static v8::Handle<v8::Value> _GL_drawElements(const v8::Arguments &args) {
    if(args.Length() >= 3) {
        #line 281 "src/modules/GL.gear"
        Value mode(args[0]), count(args[1]), indices(args[2]);
        /*uint32_t *_indices = ArrayToVector<uint32_t>(indices);
        double *_vertices = ArrayToVector<double>(vertices);
        double *_normals = ArrayToVector<double>(normals);
        
        if(texCoords) {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            float *_texCoords = reinterpret_cast<float*>(texCoords.to<v8::Handle<v8::Object>>()->GetIndexedPropertiesExternalArrayData());
            glTexCoordPointer(2, GL_FLOAT, 0, _texCoords);
        }
        if(normals) {
            glEnableClientState(GL_NORMAL_ARRAY);
            float *_normals = reinterpret_cast<float*>(normals.to<v8::Handle<v8::Object>>()->GetIndexedPropertiesExternalArrayData());
            glNormalPointer(GL_FLOAT, 0, _normals);
        }
        glEnableClientState(GL_VERTEX_ARRAY);
        float *_vertices = reinterpret_cast<float*>(vertices.to<v8::Handle<v8::Object>>()->GetIndexedPropertiesExternalArrayData());
        glVertexPointer(3, GL_FLOAT, 0, _vertices);*/
        
        glDrawElements(mode, count, GL_UNSIGNED_INT, reinterpret_cast<uint32_t*>(indices.to<v8::Handle<v8::Object>>()->GetIndexedPropertiesExternalArrayData()));
        /*delete [] _indices;
        delete [] _vertices;
        delete [] _normals;*/
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.drawElements");
}

static v8::Handle<v8::Value> _GL_vertexPointer(const v8::Arguments &args) {
    if(args.Length() >= 2) {
        #line 308 "src/modules/GL.gear"
        Value size(args[0]), vertices(args[1]);
        glVertexPointer(size, GL_FLOAT, 0, reinterpret_cast<float*>(vertices.to<v8::Handle<v8::Object>>()->GetIndexedPropertiesExternalArrayData()));
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.vertexPointer");
}

static v8::Handle<v8::Value> _GL_normalPointer(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 313 "src/modules/GL.gear"
        Value normals(args[0]);
        glNormalPointer(GL_FLOAT, 0, reinterpret_cast<float*>(normals.to<v8::Handle<v8::Object>>()->GetIndexedPropertiesExternalArrayData()));
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.normalPointer");
}

static v8::Handle<v8::Value> _GL_texCoordPointer(const v8::Arguments &args) {
    if(args.Length() >= 2) {
        #line 318 "src/modules/GL.gear"
        Value size(args[0]), texCoords(args[1]);
        glTexCoordPointer(size, GL_FLOAT, 0, reinterpret_cast<float*>(texCoords.to<v8::Handle<v8::Object>>()->GetIndexedPropertiesExternalArrayData()));
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.texCoordPointer");
}

static v8::Handle<v8::Value> _GL_enableClientState(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 323 "src/modules/GL.gear"
        Value that(args[0]);
        glEnableClientState(that);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.enableClientState");
}

static v8::Handle<v8::Value> _GL_enable(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 328 "src/modules/GL.gear"
        Value that(args[0]);
        glEnable(that);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.enable");
}

static v8::Handle<v8::Value> _GL_disable(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 333 "src/modules/GL.gear"
        Value that(args[0]);
        glDisable(that);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.disable");
}

static v8::Handle<v8::Value> _GL_hint(const v8::Arguments &args) {
    if(args.Length() >= 2) {
        #line 338 "src/modules/GL.gear"
        Value target(args[0]), mode(args[1]);
        glHint(target, mode);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.hint");
}

static v8::Handle<v8::Value> _GL_shadeModel(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 343 "src/modules/GL.gear"
        Value mode(args[0]);
        glShadeModel(mode);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.shadeModel");
}

static v8::Handle<v8::Value> _GL_flush(const v8::Arguments &args) {
    #line 349 "src/modules/GL.gear"
    glFlush();
    return GLError();
}

static v8::Handle<v8::Value> _GL_loadIdentity(const v8::Arguments &args) {
    #line 354 "src/modules/GL.gear"
    glLoadIdentity();
    return GLError();
}

static v8::Handle<v8::Value> _GL_clearColor(const v8::Arguments &args) {
    if(args.Length() >= 4) {
        #line 358 "src/modules/GL.gear"
        Value r(args[0]), g(args[1]), b(args[2]), a(args[3]);
        glClearColor(r, g, b, a);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.clearColor");
}

static v8::Handle<v8::Value> _GL_clear(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 363 "src/modules/GL.gear"
        Value bits(args[0]);
        glClear(bits);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.clear");
}

static v8::Handle<v8::Value> _GL_viewport(const v8::Arguments &args) {
    if(args.Length() >= 4) {
        #line 368 "src/modules/GL.gear"
        Value x1(args[0]), y1(args[1]), x2(args[2]), y2(args[3]);
        glViewport(x1, y1, x2, y2);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.viewport");
}

static v8::Handle<v8::Value> _GL_matrixMode(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 373 "src/modules/GL.gear"
        Value mode(args[0]);
        glMatrixMode(mode);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.matrixMode");
}

static v8::Handle<v8::Value> _GL_pushMatrix(const v8::Arguments &args) {
    #line 379 "src/modules/GL.gear"
    glPushMatrix();
    return GLError();
}

static v8::Handle<v8::Value> _GL_popMatrix(const v8::Arguments &args) {
    #line 384 "src/modules/GL.gear"
    glPopMatrix();
    return GLError();
}

static v8::Handle<v8::Value> _GL_translate(const v8::Arguments &args) {
    if(args.Length() >= 3) {
        #line 388 "src/modules/GL.gear"
        Value x(args[0]), y(args[1]), z(args[2]);
        glTranslated(x, y, z);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.translate");
}

static v8::Handle<v8::Value> _GL_scale(const v8::Arguments &args) {
    if(args.Length() >= 3) {
        #line 393 "src/modules/GL.gear"
        Value x(args[0]), y(args[1]), z(args[2]);
        glScaled(x, y, z);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.scale");
}

static v8::Handle<v8::Value> _GL_rotate(const v8::Arguments &args) {
    if(args.Length() >= 4) {
        #line 398 "src/modules/GL.gear"
        Value angle(args[0]), x(args[1]), y(args[2]), z(args[3]);
        glRotated(angle, x, y, z);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.rotate");
}

static v8::Handle<v8::Value> _GL_color(const v8::Arguments &args) {
    if(args.Length() >= 4) {
        #line 408 "src/modules/GL.gear"
        Value r(args[0]), g(args[1]), b(args[2]), a(args[3]);
        glColor4d(r, g, b, a);
        //return GLError();
        return undefined;
    }

    if(args.Length() >= 3) {
        #line 403 "src/modules/GL.gear"
        Value r(args[0]), g(args[1]), b(args[2]);
        glColor3d(r, g, b);
        //return GLError();
        return undefined;
    }
    THROW_ERROR("Invalid call to GL.color");
}

static v8::Handle<v8::Value> _GL_fog(const v8::Arguments &args) {
    if(args.Length() >= 2) {
        #line 413 "src/modules/GL.gear"
        Value what(args[0]), val(args[1]);
        if(what == GL_FOG_COLOR && val.is<Object>()) {
            float fog[] = {val["r"], val["g"], val["b"], val["a"]};
            glFogfv(what, fog);
            return GLError();
        }
        if(what == GL_FOG_COLOR && val.is<Array>()) {
            float fog[] = {val[0], val[1], val[2], val[3]};
            glFogfv(what, fog);
            return GLError();
        }
        if(what == GL_FOG_MODE)
            glFogi(what, val);
        else
            glFogf(what, val);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.fog");
}

static v8::Handle<v8::Value> _GL_light(const v8::Arguments &args) {
    if(args.Length() >= 6) {
        #line 431 "src/modules/GL.gear"
        Value which(args[0]), type(args[1]), a(args[2]), b(args[3]), c(args[4]), d(args[5]);
        float light[] = {a, b, c, d};
        glLightfv(which, type, light);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.light");
}

static v8::Handle<v8::Value> _GL_material(const v8::Arguments &args) {
    if(args.Length() >= 6) {
        #line 442 "src/modules/GL.gear"
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
        
        float material[] = {r, g, b, a};
        //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);
        glMaterialfv(which, type, material);
        return GLError();
    }

    if(args.Length() >= 3) {
        #line 437 "src/modules/GL.gear"
        Value which(args[0]), type(args[1]), val(args[2]);
        glMaterialf(which, type, val);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.material");
}

static v8::Handle<v8::Value> _GL_begin(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 463 "src/modules/GL.gear"
        Value what(args[0]);
        glBegin(what);
        //return GLError();
        return undefined;
    }
    THROW_ERROR("Invalid call to GL.begin");
}

static v8::Handle<v8::Value> _GL_end(const v8::Arguments &args) {
    #line 469 "src/modules/GL.gear"
    glEnd();
    return GLError();
}

static v8::Handle<v8::Value> _GL_vertex(const v8::Arguments &args) {
    if(args.Length() >= 3) {
        #line 473 "src/modules/GL.gear"
        Value x(args[0]), y(args[1]), z(args[2]);
        glVertex3d(x, y, z);
        //return GLError();
        return undefined;
    }
    THROW_ERROR("Invalid call to GL.vertex");
}

static v8::Handle<v8::Value> _GL_normal(const v8::Arguments &args) {
    if(args.Length() >= 3) {
        #line 478 "src/modules/GL.gear"
        Value x(args[0]), y(args[1]), z(args[2]);
        glNormal3d(x, y, z);
        //return GLError();
        return undefined;
    }
    THROW_ERROR("Invalid call to GL.normal");
}

static v8::Handle<v8::Value> _GL_rasterPos(const v8::Arguments &args) {
    if(args.Length() >= 2) {
        #line 483 "src/modules/GL.gear"
        Value x(args[0]), y(args[1]);
        glRasterPos2d(x, y);
        return GLError();
    }
    THROW_ERROR("Invalid call to GL.rasterPos");
}

static v8::Handle<v8::Value> _GL_toString(const v8::Arguments &args) {
    #line 144 "src/modules/GL.gear"
    return String("[module GL]");
}


#line 681 "src/modules/GL.cc"
static void _setup_GL(Value _exports) {
    _exports["initWindow"] = Function(_GL_initWindow, "initWindow");
    _exports["mainLoop"] = Function(_GL_mainLoop, "mainLoop");
    _exports["addTimer"] = Function(_GL_addTimer, "addTimer");
    _exports["cancelTimer"] = Function(_GL_cancelTimer, "cancelTimer");
    _exports["ignoreKeyRepeat"] = Function(_GL_ignoreKeyRepeat, "ignoreKeyRepeat");
    _exports["warpPointer"] = Function(_GL_warpPointer, "warpPointer");
    _exports["setCursor"] = Function(_GL_setCursor, "setCursor");
    _exports["swapBuffers"] = Function(_GL_swapBuffers, "swapBuffers");
    _exports["postRedisplay"] = Function(_GL_postRedisplay, "postRedisplay");
    _exports["bitmapCharacter"] = Function(_GL_bitmapCharacter, "bitmapCharacter");
    _exports["perspective"] = Function(_GL_perspective, "perspective");
    _exports["ortho2D"] = Function(_GL_ortho2D, "ortho2D");
    _exports["lookAt"] = Function(_GL_lookAt, "lookAt");
    _exports["makeFloatArray"] = Function(_GL_makeFloatArray, "makeFloatArray");
    _exports["makeUInt32Array"] = Function(_GL_makeUInt32Array, "makeUInt32Array");
    _exports["drawElements"] = Function(_GL_drawElements, "drawElements");
    _exports["vertexPointer"] = Function(_GL_vertexPointer, "vertexPointer");
    _exports["normalPointer"] = Function(_GL_normalPointer, "normalPointer");
    _exports["texCoordPointer"] = Function(_GL_texCoordPointer, "texCoordPointer");
    _exports["enableClientState"] = Function(_GL_enableClientState, "enableClientState");
    _exports["enable"] = Function(_GL_enable, "enable");
    _exports["disable"] = Function(_GL_disable, "disable");
    _exports["hint"] = Function(_GL_hint, "hint");
    _exports["shadeModel"] = Function(_GL_shadeModel, "shadeModel");
    _exports["flush"] = Function(_GL_flush, "flush");
    _exports["loadIdentity"] = Function(_GL_loadIdentity, "loadIdentity");
    _exports["clearColor"] = Function(_GL_clearColor, "clearColor");
    _exports["clear"] = Function(_GL_clear, "clear");
    _exports["viewport"] = Function(_GL_viewport, "viewport");
    _exports["matrixMode"] = Function(_GL_matrixMode, "matrixMode");
    _exports["pushMatrix"] = Function(_GL_pushMatrix, "pushMatrix");
    _exports["popMatrix"] = Function(_GL_popMatrix, "popMatrix");
    _exports["translate"] = Function(_GL_translate, "translate");
    _exports["scale"] = Function(_GL_scale, "scale");
    _exports["rotate"] = Function(_GL_rotate, "rotate");
    _exports["color"] = Function(_GL_color, "color");
    _exports["fog"] = Function(_GL_fog, "fog");
    _exports["light"] = Function(_GL_light, "light");
    _exports["material"] = Function(_GL_material, "material");
    _exports["begin"] = Function(_GL_begin, "begin");
    _exports["end"] = Function(_GL_end, "end");
    _exports["vertex"] = Function(_GL_vertex, "vertex");
    _exports["normal"] = Function(_GL_normal, "normal");
    _exports["rasterPos"] = Function(_GL_rasterPos, "rasterPos");
    _exports["toString"] = Function(_GL_toString, "toString");
    _exports["CURSOR_RIGHT_ARROW"] = Value(GLUT_CURSOR_RIGHT_ARROW);
    _exports["CURSOR_LEFT_ARROW"] = Value(GLUT_CURSOR_LEFT_ARROW);
    _exports["CURSOR_INFO"] = Value(GLUT_CURSOR_INFO);
    _exports["CURSOR_DESTROY"] = Value(GLUT_CURSOR_DESTROY);
    _exports["CURSOR_HELP"] = Value(GLUT_CURSOR_HELP);
    _exports["CURSOR_CYCLE"] = Value(GLUT_CURSOR_CYCLE);
    _exports["CURSOR_SPRAY"] = Value(GLUT_CURSOR_SPRAY);
    _exports["CURSOR_WAIT"] = Value(GLUT_CURSOR_WAIT);
    _exports["CURSOR_TEXT"] = Value(GLUT_CURSOR_TEXT);
    _exports["CURSOR_CROSSHAIR"] = Value(GLUT_CURSOR_CROSSHAIR);
    _exports["CURSOR_UP_DOWN"] = Value(GLUT_CURSOR_UP_DOWN);
    _exports["CURSOR_LEFT_RIGHT"] = Value(GLUT_CURSOR_LEFT_RIGHT);
    _exports["CURSOR_TOP_SIDE"] = Value(GLUT_CURSOR_TOP_SIDE);
    _exports["CURSOR_BOTTOM_SIDE"] = Value(GLUT_CURSOR_BOTTOM_SIDE);
    _exports["CURSOR_LEFT_SIDE"] = Value(GLUT_CURSOR_LEFT_SIDE);
    _exports["CURSOR_RIGHT_SIDE"] = Value(GLUT_CURSOR_RIGHT_SIDE);
    _exports["CURSOR_TOP_LEFT_CORNER"] = Value(GLUT_CURSOR_TOP_LEFT_CORNER);
    _exports["CURSOR_TOP_RIGHT_CORNER"] = Value(GLUT_CURSOR_TOP_RIGHT_CORNER);
    _exports["CURSOR_BOTTOM_RIGHT_CORNER"] = Value(GLUT_CURSOR_BOTTOM_RIGHT_CORNER);
    _exports["CURSOR_BOTTOM_LEFT_CORNER"] = Value(GLUT_CURSOR_BOTTOM_LEFT_CORNER);
    _exports["CURSOR_INHERIT"] = Value(GLUT_CURSOR_INHERIT);
    _exports["CURSOR_NONE"] = Value(GLUT_CURSOR_NONE);
    _exports["CURSOR_FULL_CROSSHAIR"] = Value(GLUT_CURSOR_FULL_CROSSHAIR);
    _exports["FALSE"] = Value(GL_FALSE);
    _exports["TRUE"] = Value(GL_TRUE);
    _exports["BYTE"] = Value(GL_BYTE);
    _exports["UNSIGNED_BYTE"] = Value(GL_UNSIGNED_BYTE);
    _exports["SHORT"] = Value(GL_SHORT);
    _exports["UNSIGNED_SHORT"] = Value(GL_UNSIGNED_SHORT);
    _exports["INT"] = Value(GL_INT);
    _exports["UNSIGNED_INT"] = Value(GL_UNSIGNED_INT);
    _exports["FLOAT"] = Value(GL_FLOAT);
    _exports["DOUBLE"] = Value(GL_DOUBLE);
    _exports["POINTS"] = Value(GL_POINTS);
    _exports["LINES"] = Value(GL_LINES);
    _exports["LINE_LOOP"] = Value(GL_LINE_LOOP);
    _exports["LINE_STRIP"] = Value(GL_LINE_STRIP);
    _exports["TRIANGLES"] = Value(GL_TRIANGLES);
    _exports["TRIANGLE_STRIP"] = Value(GL_TRIANGLE_STRIP);
    _exports["TRIANGLE_FAN"] = Value(GL_TRIANGLE_FAN);
    _exports["QUADS"] = Value(GL_QUADS);
    _exports["QUAD_STRIP"] = Value(GL_QUAD_STRIP);
    _exports["POLYGON"] = Value(GL_POLYGON);
    _exports["VERTEX_ARRAY"] = Value(GL_VERTEX_ARRAY);
    _exports["NORMAL_ARRAY"] = Value(GL_NORMAL_ARRAY);
    _exports["COLOR_ARRAY"] = Value(GL_COLOR_ARRAY);
    _exports["INDEX_ARRAY"] = Value(GL_INDEX_ARRAY);
    _exports["TEXTURE_COORD_ARRAY"] = Value(GL_TEXTURE_COORD_ARRAY);
    _exports["EDGE_FLAG_ARRAY"] = Value(GL_EDGE_FLAG_ARRAY);
    _exports["VERTEX_ARRAY_SIZE"] = Value(GL_VERTEX_ARRAY_SIZE);
    _exports["VERTEX_ARRAY_TYPE"] = Value(GL_VERTEX_ARRAY_TYPE);
    _exports["VERTEX_ARRAY_STRIDE"] = Value(GL_VERTEX_ARRAY_STRIDE);
    _exports["NORMAL_ARRAY_TYPE"] = Value(GL_NORMAL_ARRAY_TYPE);
    _exports["NORMAL_ARRAY_STRIDE"] = Value(GL_NORMAL_ARRAY_STRIDE);
    _exports["COLOR_ARRAY_SIZE"] = Value(GL_COLOR_ARRAY_SIZE);
    _exports["COLOR_ARRAY_TYPE"] = Value(GL_COLOR_ARRAY_TYPE);
    _exports["COLOR_ARRAY_STRIDE"] = Value(GL_COLOR_ARRAY_STRIDE);
    _exports["INDEX_ARRAY_TYPE"] = Value(GL_INDEX_ARRAY_TYPE);
    _exports["INDEX_ARRAY_STRIDE"] = Value(GL_INDEX_ARRAY_STRIDE);
    _exports["TEXTURE_COORD_ARRAY_SIZE"] = Value(GL_TEXTURE_COORD_ARRAY_SIZE);
    _exports["TEXTURE_COORD_ARRAY_TYPE"] = Value(GL_TEXTURE_COORD_ARRAY_TYPE);
    _exports["TEXTURE_COORD_ARRAY_STRIDE"] = Value(GL_TEXTURE_COORD_ARRAY_STRIDE);
    _exports["EDGE_FLAG_ARRAY_STRIDE"] = Value(GL_EDGE_FLAG_ARRAY_STRIDE);
    _exports["VERTEX_ARRAY_POINTER"] = Value(GL_VERTEX_ARRAY_POINTER);
    _exports["NORMAL_ARRAY_POINTER"] = Value(GL_NORMAL_ARRAY_POINTER);
    _exports["COLOR_ARRAY_POINTER"] = Value(GL_COLOR_ARRAY_POINTER);
    _exports["INDEX_ARRAY_POINTER"] = Value(GL_INDEX_ARRAY_POINTER);
    _exports["TEXTURE_COORD_ARRAY_POINTER"] = Value(GL_TEXTURE_COORD_ARRAY_POINTER);
    _exports["EDGE_FLAG_ARRAY_POINTER"] = Value(GL_EDGE_FLAG_ARRAY_POINTER);
    _exports["V2F"] = Value(GL_V2F);
    _exports["V3F"] = Value(GL_V3F);
    _exports["C4UB_V2F"] = Value(GL_C4UB_V2F);
    _exports["C4UB_V3F"] = Value(GL_C4UB_V3F);
    _exports["C3F_V3F"] = Value(GL_C3F_V3F);
    _exports["N3F_V3F"] = Value(GL_N3F_V3F);
    _exports["C4F_N3F_V3F"] = Value(GL_C4F_N3F_V3F);
    _exports["T2F_V3F"] = Value(GL_T2F_V3F);
    _exports["T4F_V4F"] = Value(GL_T4F_V4F);
    _exports["T2F_C4UB_V3F"] = Value(GL_T2F_C4UB_V3F);
    _exports["T2F_C3F_V3F"] = Value(GL_T2F_C3F_V3F);
    _exports["T2F_N3F_V3F"] = Value(GL_T2F_N3F_V3F);
    _exports["T2F_C4F_N3F_V3F"] = Value(GL_T2F_C4F_N3F_V3F);
    _exports["T4F_C4F_N3F_V4F"] = Value(GL_T4F_C4F_N3F_V4F);
    _exports["MATRIX_MODE"] = Value(GL_MATRIX_MODE);
    _exports["MODELVIEW"] = Value(GL_MODELVIEW);
    _exports["PROJECTION"] = Value(GL_PROJECTION);
    _exports["TEXTURE"] = Value(GL_TEXTURE);
    _exports["POINT_SMOOTH"] = Value(GL_POINT_SMOOTH);
    _exports["POINT_SIZE"] = Value(GL_POINT_SIZE);
    _exports["POINT_SIZE_GRANULARITY"] = Value(GL_POINT_SIZE_GRANULARITY);
    _exports["POINT_SIZE_RANGE"] = Value(GL_POINT_SIZE_RANGE);
    _exports["LINE_SMOOTH"] = Value(GL_LINE_SMOOTH);
    _exports["LINE_STIPPLE"] = Value(GL_LINE_STIPPLE);
    _exports["LINE_STIPPLE_PATTERN"] = Value(GL_LINE_STIPPLE_PATTERN);
    _exports["LINE_STIPPLE_REPEAT"] = Value(GL_LINE_STIPPLE_REPEAT);
    _exports["LINE_WIDTH"] = Value(GL_LINE_WIDTH);
    _exports["LINE_WIDTH_GRANULARITY"] = Value(GL_LINE_WIDTH_GRANULARITY);
    _exports["LINE_WIDTH_RANGE"] = Value(GL_LINE_WIDTH_RANGE);
    _exports["POINT"] = Value(GL_POINT);
    _exports["LINE"] = Value(GL_LINE);
    _exports["FILL"] = Value(GL_FILL);
    _exports["CW"] = Value(GL_CW);
    _exports["CCW"] = Value(GL_CCW);
    _exports["FRONT"] = Value(GL_FRONT);
    _exports["BACK"] = Value(GL_BACK);
    _exports["POLYGON_MODE"] = Value(GL_POLYGON_MODE);
    _exports["POLYGON_SMOOTH"] = Value(GL_POLYGON_SMOOTH);
    _exports["POLYGON_STIPPLE"] = Value(GL_POLYGON_STIPPLE);
    _exports["EDGE_FLAG"] = Value(GL_EDGE_FLAG);
    _exports["CULL_FACE"] = Value(GL_CULL_FACE);
    _exports["CULL_FACE_MODE"] = Value(GL_CULL_FACE_MODE);
    _exports["FRONT_FACE"] = Value(GL_FRONT_FACE);
    _exports["POLYGON_OFFSET_FACTOR"] = Value(GL_POLYGON_OFFSET_FACTOR);
    _exports["POLYGON_OFFSET_UNITS"] = Value(GL_POLYGON_OFFSET_UNITS);
    _exports["POLYGON_OFFSET_POINT"] = Value(GL_POLYGON_OFFSET_POINT);
    _exports["POLYGON_OFFSET_LINE"] = Value(GL_POLYGON_OFFSET_LINE);
    _exports["POLYGON_OFFSET_FILL"] = Value(GL_POLYGON_OFFSET_FILL);
    _exports["COMPILE"] = Value(GL_COMPILE);
    _exports["COMPILE_AND_EXECUTE"] = Value(GL_COMPILE_AND_EXECUTE);
    _exports["LIST_BASE"] = Value(GL_LIST_BASE);
    _exports["LIST_INDEX"] = Value(GL_LIST_INDEX);
    _exports["LIST_MODE"] = Value(GL_LIST_MODE);
    _exports["NEVER"] = Value(GL_NEVER);
    _exports["LESS"] = Value(GL_LESS);
    _exports["EQUAL"] = Value(GL_EQUAL);
    _exports["LEQUAL"] = Value(GL_LEQUAL);
    _exports["GREATER"] = Value(GL_GREATER);
    _exports["NOTEQUAL"] = Value(GL_NOTEQUAL);
    _exports["GEQUAL"] = Value(GL_GEQUAL);
    _exports["ALWAYS"] = Value(GL_ALWAYS);
    _exports["DEPTH_TEST"] = Value(GL_DEPTH_TEST);
    _exports["DEPTH_BITS"] = Value(GL_DEPTH_BITS);
    _exports["DEPTH_CLEAR_VALUE"] = Value(GL_DEPTH_CLEAR_VALUE);
    _exports["DEPTH_FUNC"] = Value(GL_DEPTH_FUNC);
    _exports["DEPTH_RANGE"] = Value(GL_DEPTH_RANGE);
    _exports["DEPTH_WRITEMASK"] = Value(GL_DEPTH_WRITEMASK);
    _exports["DEPTH_COMPONENT"] = Value(GL_DEPTH_COMPONENT);
    _exports["LIGHTING"] = Value(GL_LIGHTING);
    _exports["LIGHT0"] = Value(GL_LIGHT0);
    _exports["LIGHT1"] = Value(GL_LIGHT1);
    _exports["LIGHT2"] = Value(GL_LIGHT2);
    _exports["LIGHT3"] = Value(GL_LIGHT3);
    _exports["LIGHT4"] = Value(GL_LIGHT4);
    _exports["LIGHT5"] = Value(GL_LIGHT5);
    _exports["LIGHT6"] = Value(GL_LIGHT6);
    _exports["LIGHT7"] = Value(GL_LIGHT7);
    _exports["SPOT_EXPONENT"] = Value(GL_SPOT_EXPONENT);
    _exports["SPOT_CUTOFF"] = Value(GL_SPOT_CUTOFF);
    _exports["CONSTANT_ATTENUATION"] = Value(GL_CONSTANT_ATTENUATION);
    _exports["LINEAR_ATTENUATION"] = Value(GL_LINEAR_ATTENUATION);
    _exports["QUADRATIC_ATTENUATION"] = Value(GL_QUADRATIC_ATTENUATION);
    _exports["AMBIENT"] = Value(GL_AMBIENT);
    _exports["DIFFUSE"] = Value(GL_DIFFUSE);
    _exports["SPECULAR"] = Value(GL_SPECULAR);
    _exports["SHININESS"] = Value(GL_SHININESS);
    _exports["EMISSION"] = Value(GL_EMISSION);
    _exports["POSITION"] = Value(GL_POSITION);
    _exports["SPOT_DIRECTION"] = Value(GL_SPOT_DIRECTION);
    _exports["AMBIENT_AND_DIFFUSE"] = Value(GL_AMBIENT_AND_DIFFUSE);
    _exports["COLOR_INDEXES"] = Value(GL_COLOR_INDEXES);
    _exports["LIGHT_MODEL_TWO_SIDE"] = Value(GL_LIGHT_MODEL_TWO_SIDE);
    _exports["LIGHT_MODEL_LOCAL_VIEWER"] = Value(GL_LIGHT_MODEL_LOCAL_VIEWER);
    _exports["LIGHT_MODEL_AMBIENT"] = Value(GL_LIGHT_MODEL_AMBIENT);
    _exports["FRONT_AND_BACK"] = Value(GL_FRONT_AND_BACK);
    _exports["SHADE_MODEL"] = Value(GL_SHADE_MODEL);
    _exports["FLAT"] = Value(GL_FLAT);
    _exports["SMOOTH"] = Value(GL_SMOOTH);
    _exports["COLOR_MATERIAL"] = Value(GL_COLOR_MATERIAL);
    _exports["COLOR_MATERIAL_FACE"] = Value(GL_COLOR_MATERIAL_FACE);
    _exports["COLOR_MATERIAL_PARAMETER"] = Value(GL_COLOR_MATERIAL_PARAMETER);
    _exports["NORMALIZE"] = Value(GL_NORMALIZE);
    _exports["CLIP_PLANE0"] = Value(GL_CLIP_PLANE0);
    _exports["CLIP_PLANE1"] = Value(GL_CLIP_PLANE1);
    _exports["CLIP_PLANE2"] = Value(GL_CLIP_PLANE2);
    _exports["CLIP_PLANE3"] = Value(GL_CLIP_PLANE3);
    _exports["CLIP_PLANE4"] = Value(GL_CLIP_PLANE4);
    _exports["CLIP_PLANE5"] = Value(GL_CLIP_PLANE5);
    _exports["ACCUM_RED_BITS"] = Value(GL_ACCUM_RED_BITS);
    _exports["ACCUM_GREEN_BITS"] = Value(GL_ACCUM_GREEN_BITS);
    _exports["ACCUM_BLUE_BITS"] = Value(GL_ACCUM_BLUE_BITS);
    _exports["ACCUM_ALPHA_BITS"] = Value(GL_ACCUM_ALPHA_BITS);
    _exports["ACCUM_CLEAR_VALUE"] = Value(GL_ACCUM_CLEAR_VALUE);
    _exports["ACCUM"] = Value(GL_ACCUM);
    _exports["ADD"] = Value(GL_ADD);
    _exports["LOAD"] = Value(GL_LOAD);
    _exports["MULT"] = Value(GL_MULT);
    _exports["RETURN"] = Value(GL_RETURN);
    _exports["ALPHA_TEST"] = Value(GL_ALPHA_TEST);
    _exports["ALPHA_TEST_REF"] = Value(GL_ALPHA_TEST_REF);
    _exports["ALPHA_TEST_FUNC"] = Value(GL_ALPHA_TEST_FUNC);
    _exports["BLEND"] = Value(GL_BLEND);
    _exports["BLEND_SRC"] = Value(GL_BLEND_SRC);
    _exports["BLEND_DST"] = Value(GL_BLEND_DST);
    _exports["ZERO"] = Value(GL_ZERO);
    _exports["ONE"] = Value(GL_ONE);
    _exports["SRC_COLOR"] = Value(GL_SRC_COLOR);
    _exports["ONE_MINUS_SRC_COLOR"] = Value(GL_ONE_MINUS_SRC_COLOR);
    _exports["SRC_ALPHA"] = Value(GL_SRC_ALPHA);
    _exports["ONE_MINUS_SRC_ALPHA"] = Value(GL_ONE_MINUS_SRC_ALPHA);
    _exports["DST_ALPHA"] = Value(GL_DST_ALPHA);
    _exports["ONE_MINUS_DST_ALPHA"] = Value(GL_ONE_MINUS_DST_ALPHA);
    _exports["DST_COLOR"] = Value(GL_DST_COLOR);
    _exports["ONE_MINUS_DST_COLOR"] = Value(GL_ONE_MINUS_DST_COLOR);
    _exports["SRC_ALPHA_SATURATE"] = Value(GL_SRC_ALPHA_SATURATE);
    _exports["FEEDBACK"] = Value(GL_FEEDBACK);
    _exports["RENDER"] = Value(GL_RENDER);
    _exports["SELECT"] = Value(GL_SELECT);
    _exports["POINT_TOKEN"] = Value(GL_POINT_TOKEN);
    _exports["LINE_TOKEN"] = Value(GL_LINE_TOKEN);
    _exports["LINE_RESET_TOKEN"] = Value(GL_LINE_RESET_TOKEN);
    _exports["POLYGON_TOKEN"] = Value(GL_POLYGON_TOKEN);
    _exports["BITMAP_TOKEN"] = Value(GL_BITMAP_TOKEN);
    _exports["DRAW_PIXEL_TOKEN"] = Value(GL_DRAW_PIXEL_TOKEN);
    _exports["COPY_PIXEL_TOKEN"] = Value(GL_COPY_PIXEL_TOKEN);
    _exports["PASS_THROUGH_TOKEN"] = Value(GL_PASS_THROUGH_TOKEN);
    _exports["FEEDBACK_BUFFER_POINTER"] = Value(GL_FEEDBACK_BUFFER_POINTER);
    _exports["FEEDBACK_BUFFER_SIZE"] = Value(GL_FEEDBACK_BUFFER_SIZE);
    _exports["FEEDBACK_BUFFER_TYPE"] = Value(GL_FEEDBACK_BUFFER_TYPE);
    _exports["SELECTION_BUFFER_POINTER"] = Value(GL_SELECTION_BUFFER_POINTER);
    _exports["SELECTION_BUFFER_SIZE"] = Value(GL_SELECTION_BUFFER_SIZE);
    _exports["FOG"] = Value(GL_FOG);
    _exports["FOG_MODE"] = Value(GL_FOG_MODE);
    _exports["FOG_DENSITY"] = Value(GL_FOG_DENSITY);
    _exports["FOG_COLOR"] = Value(GL_FOG_COLOR);
    _exports["FOG_INDEX"] = Value(GL_FOG_INDEX);
    _exports["FOG_START"] = Value(GL_FOG_START);
    _exports["FOG_END"] = Value(GL_FOG_END);
    _exports["LINEAR"] = Value(GL_LINEAR);
    _exports["EXP"] = Value(GL_EXP);
    _exports["EXP2"] = Value(GL_EXP2);
    _exports["LOGIC_OP"] = Value(GL_LOGIC_OP);
    _exports["INDEX_LOGIC_OP"] = Value(GL_INDEX_LOGIC_OP);
    _exports["COLOR_LOGIC_OP"] = Value(GL_COLOR_LOGIC_OP);
    _exports["LOGIC_OP_MODE"] = Value(GL_LOGIC_OP_MODE);
    _exports["CLEAR"] = Value(GL_CLEAR);
    _exports["SET"] = Value(GL_SET);
    _exports["COPY"] = Value(GL_COPY);
    _exports["COPY_INVERTED"] = Value(GL_COPY_INVERTED);
    _exports["NOOP"] = Value(GL_NOOP);
    _exports["INVERT"] = Value(GL_INVERT);
    _exports["AND"] = Value(GL_AND);
    _exports["NAND"] = Value(GL_NAND);
    _exports["OR"] = Value(GL_OR);
    _exports["NOR"] = Value(GL_NOR);
    _exports["XOR"] = Value(GL_XOR);
    _exports["EQUIV"] = Value(GL_EQUIV);
    _exports["AND_REVERSE"] = Value(GL_AND_REVERSE);
    _exports["AND_INVERTED"] = Value(GL_AND_INVERTED);
    _exports["OR_REVERSE"] = Value(GL_OR_REVERSE);
    _exports["OR_INVERTED"] = Value(GL_OR_INVERTED);
    _exports["STENCIL_BITS"] = Value(GL_STENCIL_BITS);
    _exports["STENCIL_TEST"] = Value(GL_STENCIL_TEST);
    _exports["STENCIL_CLEAR_VALUE"] = Value(GL_STENCIL_CLEAR_VALUE);
    _exports["STENCIL_FUNC"] = Value(GL_STENCIL_FUNC);
    _exports["STENCIL_VALUE_MASK"] = Value(GL_STENCIL_VALUE_MASK);
    _exports["STENCIL_FAIL"] = Value(GL_STENCIL_FAIL);
    _exports["STENCIL_PASS_DEPTH_FAIL"] = Value(GL_STENCIL_PASS_DEPTH_FAIL);
    _exports["STENCIL_PASS_DEPTH_PASS"] = Value(GL_STENCIL_PASS_DEPTH_PASS);
    _exports["STENCIL_REF"] = Value(GL_STENCIL_REF);
    _exports["STENCIL_WRITEMASK"] = Value(GL_STENCIL_WRITEMASK);
    _exports["STENCIL_INDEX"] = Value(GL_STENCIL_INDEX);
    _exports["KEEP"] = Value(GL_KEEP);
    _exports["REPLACE"] = Value(GL_REPLACE);
    _exports["INCR"] = Value(GL_INCR);
    _exports["DECR"] = Value(GL_DECR);
    _exports["NONE"] = Value(GL_NONE);
    _exports["LEFT"] = Value(GL_LEFT);
    _exports["RIGHT"] = Value(GL_RIGHT);
    _exports["FRONT_LEFT"] = Value(GL_FRONT_LEFT);
    _exports["FRONT_RIGHT"] = Value(GL_FRONT_RIGHT);
    _exports["BACK_LEFT"] = Value(GL_BACK_LEFT);
    _exports["BACK_RIGHT"] = Value(GL_BACK_RIGHT);
    _exports["AUX0"] = Value(GL_AUX0);
    _exports["AUX1"] = Value(GL_AUX1);
    _exports["AUX2"] = Value(GL_AUX2);
    _exports["AUX3"] = Value(GL_AUX3);
    _exports["COLOR_INDEX"] = Value(GL_COLOR_INDEX);
    _exports["RED"] = Value(GL_RED);
    _exports["GREEN"] = Value(GL_GREEN);
    _exports["BLUE"] = Value(GL_BLUE);
    _exports["ALPHA"] = Value(GL_ALPHA);
    _exports["LUMINANCE"] = Value(GL_LUMINANCE);
    _exports["LUMINANCE_ALPHA"] = Value(GL_LUMINANCE_ALPHA);
    _exports["ALPHA_BITS"] = Value(GL_ALPHA_BITS);
    _exports["RED_BITS"] = Value(GL_RED_BITS);
    _exports["GREEN_BITS"] = Value(GL_GREEN_BITS);
    _exports["BLUE_BITS"] = Value(GL_BLUE_BITS);
    _exports["INDEX_BITS"] = Value(GL_INDEX_BITS);
    _exports["SUBPIXEL_BITS"] = Value(GL_SUBPIXEL_BITS);
    _exports["AUX_BUFFERS"] = Value(GL_AUX_BUFFERS);
    _exports["READ_BUFFER"] = Value(GL_READ_BUFFER);
    _exports["DRAW_BUFFER"] = Value(GL_DRAW_BUFFER);
    _exports["DOUBLEBUFFER"] = Value(GL_DOUBLEBUFFER);
    _exports["STEREO"] = Value(GL_STEREO);
    _exports["BITMAP"] = Value(GL_BITMAP);
    _exports["COLOR"] = Value(GL_COLOR);
    _exports["DEPTH"] = Value(GL_DEPTH);
    _exports["STENCIL"] = Value(GL_STENCIL);
    _exports["DITHER"] = Value(GL_DITHER);
    _exports["RGB"] = Value(GL_RGB);
    _exports["RGBA"] = Value(GL_RGBA);
    _exports["MAX_LIST_NESTING"] = Value(GL_MAX_LIST_NESTING);
    _exports["MAX_EVAL_ORDER"] = Value(GL_MAX_EVAL_ORDER);
    _exports["MAX_LIGHTS"] = Value(GL_MAX_LIGHTS);
    _exports["MAX_CLIP_PLANES"] = Value(GL_MAX_CLIP_PLANES);
    _exports["MAX_TEXTURE_SIZE"] = Value(GL_MAX_TEXTURE_SIZE);
    _exports["MAX_PIXEL_MAP_TABLE"] = Value(GL_MAX_PIXEL_MAP_TABLE);
    _exports["MAX_ATTRIB_STACK_DEPTH"] = Value(GL_MAX_ATTRIB_STACK_DEPTH);
    _exports["MAX_MODELVIEW_STACK_DEPTH"] = Value(GL_MAX_MODELVIEW_STACK_DEPTH);
    _exports["MAX_NAME_STACK_DEPTH"] = Value(GL_MAX_NAME_STACK_DEPTH);
    _exports["MAX_PROJECTION_STACK_DEPTH"] = Value(GL_MAX_PROJECTION_STACK_DEPTH);
    _exports["MAX_TEXTURE_STACK_DEPTH"] = Value(GL_MAX_TEXTURE_STACK_DEPTH);
    _exports["MAX_VIEWPORT_DIMS"] = Value(GL_MAX_VIEWPORT_DIMS);
    _exports["MAX_CLIENT_ATTRIB_STACK_DEPTH"] = Value(GL_MAX_CLIENT_ATTRIB_STACK_DEPTH);
    _exports["ATTRIB_STACK_DEPTH"] = Value(GL_ATTRIB_STACK_DEPTH);
    _exports["CLIENT_ATTRIB_STACK_DEPTH"] = Value(GL_CLIENT_ATTRIB_STACK_DEPTH);
    _exports["COLOR_CLEAR_VALUE"] = Value(GL_COLOR_CLEAR_VALUE);
    _exports["COLOR_WRITEMASK"] = Value(GL_COLOR_WRITEMASK);
    _exports["CURRENT_INDEX"] = Value(GL_CURRENT_INDEX);
    _exports["CURRENT_COLOR"] = Value(GL_CURRENT_COLOR);
    _exports["CURRENT_NORMAL"] = Value(GL_CURRENT_NORMAL);
    _exports["CURRENT_RASTER_COLOR"] = Value(GL_CURRENT_RASTER_COLOR);
    _exports["CURRENT_RASTER_DISTANCE"] = Value(GL_CURRENT_RASTER_DISTANCE);
    _exports["CURRENT_RASTER_INDEX"] = Value(GL_CURRENT_RASTER_INDEX);
    _exports["CURRENT_RASTER_POSITION"] = Value(GL_CURRENT_RASTER_POSITION);
    _exports["CURRENT_RASTER_TEXTURE_COORDS"] = Value(GL_CURRENT_RASTER_TEXTURE_COORDS);
    _exports["CURRENT_RASTER_POSITION_VALID"] = Value(GL_CURRENT_RASTER_POSITION_VALID);
    _exports["CURRENT_TEXTURE_COORDS"] = Value(GL_CURRENT_TEXTURE_COORDS);
    _exports["INDEX_CLEAR_VALUE"] = Value(GL_INDEX_CLEAR_VALUE);
    _exports["INDEX_MODE"] = Value(GL_INDEX_MODE);
    _exports["INDEX_WRITEMASK"] = Value(GL_INDEX_WRITEMASK);
    _exports["MODELVIEW_MATRIX"] = Value(GL_MODELVIEW_MATRIX);
    _exports["MODELVIEW_STACK_DEPTH"] = Value(GL_MODELVIEW_STACK_DEPTH);
    _exports["NAME_STACK_DEPTH"] = Value(GL_NAME_STACK_DEPTH);
    _exports["PROJECTION_MATRIX"] = Value(GL_PROJECTION_MATRIX);
    _exports["PROJECTION_STACK_DEPTH"] = Value(GL_PROJECTION_STACK_DEPTH);
    _exports["RENDER_MODE"] = Value(GL_RENDER_MODE);
    _exports["RGBA_MODE"] = Value(GL_RGBA_MODE);
    _exports["TEXTURE_MATRIX"] = Value(GL_TEXTURE_MATRIX);
    _exports["TEXTURE_STACK_DEPTH"] = Value(GL_TEXTURE_STACK_DEPTH);
    _exports["VIEWPORT"] = Value(GL_VIEWPORT);
    _exports["AUTO_NORMAL"] = Value(GL_AUTO_NORMAL);
    _exports["MAP1_COLOR_4"] = Value(GL_MAP1_COLOR_4);
    _exports["MAP1_INDEX"] = Value(GL_MAP1_INDEX);
    _exports["MAP1_NORMAL"] = Value(GL_MAP1_NORMAL);
    _exports["MAP1_TEXTURE_COORD_1"] = Value(GL_MAP1_TEXTURE_COORD_1);
    _exports["MAP1_TEXTURE_COORD_2"] = Value(GL_MAP1_TEXTURE_COORD_2);
    _exports["MAP1_TEXTURE_COORD_3"] = Value(GL_MAP1_TEXTURE_COORD_3);
    _exports["MAP1_TEXTURE_COORD_4"] = Value(GL_MAP1_TEXTURE_COORD_4);
    _exports["MAP1_VERTEX_3"] = Value(GL_MAP1_VERTEX_3);
    _exports["MAP1_VERTEX_4"] = Value(GL_MAP1_VERTEX_4);
    _exports["MAP2_COLOR_4"] = Value(GL_MAP2_COLOR_4);
    _exports["MAP2_INDEX"] = Value(GL_MAP2_INDEX);
    _exports["MAP2_NORMAL"] = Value(GL_MAP2_NORMAL);
    _exports["MAP2_TEXTURE_COORD_1"] = Value(GL_MAP2_TEXTURE_COORD_1);
    _exports["MAP2_TEXTURE_COORD_2"] = Value(GL_MAP2_TEXTURE_COORD_2);
    _exports["MAP2_TEXTURE_COORD_3"] = Value(GL_MAP2_TEXTURE_COORD_3);
    _exports["MAP2_TEXTURE_COORD_4"] = Value(GL_MAP2_TEXTURE_COORD_4);
    _exports["MAP2_VERTEX_3"] = Value(GL_MAP2_VERTEX_3);
    _exports["MAP2_VERTEX_4"] = Value(GL_MAP2_VERTEX_4);
    _exports["MAP1_GRID_DOMAIN"] = Value(GL_MAP1_GRID_DOMAIN);
    _exports["MAP1_GRID_SEGMENTS"] = Value(GL_MAP1_GRID_SEGMENTS);
    _exports["MAP2_GRID_DOMAIN"] = Value(GL_MAP2_GRID_DOMAIN);
    _exports["MAP2_GRID_SEGMENTS"] = Value(GL_MAP2_GRID_SEGMENTS);
    _exports["COEFF"] = Value(GL_COEFF);
    _exports["ORDER"] = Value(GL_ORDER);
    _exports["DOMAIN"] = Value(GL_DOMAIN);
    _exports["PERSPECTIVE_CORRECTION_HINT"] = Value(GL_PERSPECTIVE_CORRECTION_HINT);
    _exports["POINT_SMOOTH_HINT"] = Value(GL_POINT_SMOOTH_HINT);
    _exports["LINE_SMOOTH_HINT"] = Value(GL_LINE_SMOOTH_HINT);
    _exports["POLYGON_SMOOTH_HINT"] = Value(GL_POLYGON_SMOOTH_HINT);
    _exports["FOG_HINT"] = Value(GL_FOG_HINT);
    _exports["DONT_CARE"] = Value(GL_DONT_CARE);
    _exports["FASTEST"] = Value(GL_FASTEST);
    _exports["NICEST"] = Value(GL_NICEST);
    _exports["SCISSOR_BOX"] = Value(GL_SCISSOR_BOX);
    _exports["SCISSOR_TEST"] = Value(GL_SCISSOR_TEST);
    _exports["MAP_COLOR"] = Value(GL_MAP_COLOR);
    _exports["MAP_STENCIL"] = Value(GL_MAP_STENCIL);
    _exports["INDEX_SHIFT"] = Value(GL_INDEX_SHIFT);
    _exports["INDEX_OFFSET"] = Value(GL_INDEX_OFFSET);
    _exports["RED_SCALE"] = Value(GL_RED_SCALE);
    _exports["RED_BIAS"] = Value(GL_RED_BIAS);
    _exports["GREEN_SCALE"] = Value(GL_GREEN_SCALE);
    _exports["GREEN_BIAS"] = Value(GL_GREEN_BIAS);
    _exports["BLUE_SCALE"] = Value(GL_BLUE_SCALE);
    _exports["BLUE_BIAS"] = Value(GL_BLUE_BIAS);
    _exports["ALPHA_SCALE"] = Value(GL_ALPHA_SCALE);
    _exports["ALPHA_BIAS"] = Value(GL_ALPHA_BIAS);
    _exports["DEPTH_SCALE"] = Value(GL_DEPTH_SCALE);
    _exports["DEPTH_BIAS"] = Value(GL_DEPTH_BIAS);
    _exports["PIXEL_MAP_S_TO_S_SIZE"] = Value(GL_PIXEL_MAP_S_TO_S_SIZE);
    _exports["PIXEL_MAP_I_TO_I_SIZE"] = Value(GL_PIXEL_MAP_I_TO_I_SIZE);
    _exports["PIXEL_MAP_I_TO_R_SIZE"] = Value(GL_PIXEL_MAP_I_TO_R_SIZE);
    _exports["PIXEL_MAP_I_TO_G_SIZE"] = Value(GL_PIXEL_MAP_I_TO_G_SIZE);
    _exports["PIXEL_MAP_I_TO_B_SIZE"] = Value(GL_PIXEL_MAP_I_TO_B_SIZE);
    _exports["PIXEL_MAP_I_TO_A_SIZE"] = Value(GL_PIXEL_MAP_I_TO_A_SIZE);
    _exports["PIXEL_MAP_R_TO_R_SIZE"] = Value(GL_PIXEL_MAP_R_TO_R_SIZE);
    _exports["PIXEL_MAP_G_TO_G_SIZE"] = Value(GL_PIXEL_MAP_G_TO_G_SIZE);
    _exports["PIXEL_MAP_B_TO_B_SIZE"] = Value(GL_PIXEL_MAP_B_TO_B_SIZE);
    _exports["PIXEL_MAP_A_TO_A_SIZE"] = Value(GL_PIXEL_MAP_A_TO_A_SIZE);
    _exports["PIXEL_MAP_S_TO_S"] = Value(GL_PIXEL_MAP_S_TO_S);
    _exports["PIXEL_MAP_I_TO_I"] = Value(GL_PIXEL_MAP_I_TO_I);
    _exports["PIXEL_MAP_I_TO_R"] = Value(GL_PIXEL_MAP_I_TO_R);
    _exports["PIXEL_MAP_I_TO_G"] = Value(GL_PIXEL_MAP_I_TO_G);
    _exports["PIXEL_MAP_I_TO_B"] = Value(GL_PIXEL_MAP_I_TO_B);
    _exports["PIXEL_MAP_I_TO_A"] = Value(GL_PIXEL_MAP_I_TO_A);
    _exports["PIXEL_MAP_R_TO_R"] = Value(GL_PIXEL_MAP_R_TO_R);
    _exports["PIXEL_MAP_G_TO_G"] = Value(GL_PIXEL_MAP_G_TO_G);
    _exports["PIXEL_MAP_B_TO_B"] = Value(GL_PIXEL_MAP_B_TO_B);
    _exports["PIXEL_MAP_A_TO_A"] = Value(GL_PIXEL_MAP_A_TO_A);
    _exports["PACK_ALIGNMENT"] = Value(GL_PACK_ALIGNMENT);
    _exports["PACK_LSB_FIRST"] = Value(GL_PACK_LSB_FIRST);
    _exports["PACK_ROW_LENGTH"] = Value(GL_PACK_ROW_LENGTH);
    _exports["PACK_SKIP_PIXELS"] = Value(GL_PACK_SKIP_PIXELS);
    _exports["PACK_SKIP_ROWS"] = Value(GL_PACK_SKIP_ROWS);
    _exports["PACK_SWAP_BYTES"] = Value(GL_PACK_SWAP_BYTES);
    _exports["UNPACK_ALIGNMENT"] = Value(GL_UNPACK_ALIGNMENT);
    _exports["UNPACK_LSB_FIRST"] = Value(GL_UNPACK_LSB_FIRST);
    _exports["UNPACK_ROW_LENGTH"] = Value(GL_UNPACK_ROW_LENGTH);
    _exports["UNPACK_SKIP_PIXELS"] = Value(GL_UNPACK_SKIP_PIXELS);
    _exports["UNPACK_SKIP_ROWS"] = Value(GL_UNPACK_SKIP_ROWS);
    _exports["UNPACK_SWAP_BYTES"] = Value(GL_UNPACK_SWAP_BYTES);
    _exports["ZOOM_X"] = Value(GL_ZOOM_X);
    _exports["ZOOM_Y"] = Value(GL_ZOOM_Y);
    _exports["TEXTURE_ENV"] = Value(GL_TEXTURE_ENV);
    _exports["TEXTURE_ENV_MODE"] = Value(GL_TEXTURE_ENV_MODE);
    _exports["TEXTURE_1D"] = Value(GL_TEXTURE_1D);
    _exports["TEXTURE_2D"] = Value(GL_TEXTURE_2D);
    _exports["TEXTURE_WRAP_S"] = Value(GL_TEXTURE_WRAP_S);
    _exports["TEXTURE_WRAP_T"] = Value(GL_TEXTURE_WRAP_T);
    _exports["TEXTURE_MAG_FILTER"] = Value(GL_TEXTURE_MAG_FILTER);
    _exports["TEXTURE_MIN_FILTER"] = Value(GL_TEXTURE_MIN_FILTER);
    _exports["TEXTURE_ENV_COLOR"] = Value(GL_TEXTURE_ENV_COLOR);
    _exports["TEXTURE_GEN_S"] = Value(GL_TEXTURE_GEN_S);
    _exports["TEXTURE_GEN_T"] = Value(GL_TEXTURE_GEN_T);
    _exports["TEXTURE_GEN_MODE"] = Value(GL_TEXTURE_GEN_MODE);
    _exports["TEXTURE_BORDER_COLOR"] = Value(GL_TEXTURE_BORDER_COLOR);
    _exports["TEXTURE_WIDTH"] = Value(GL_TEXTURE_WIDTH);
    _exports["TEXTURE_HEIGHT"] = Value(GL_TEXTURE_HEIGHT);
    _exports["TEXTURE_BORDER"] = Value(GL_TEXTURE_BORDER);
    _exports["TEXTURE_COMPONENTS"] = Value(GL_TEXTURE_COMPONENTS);
    _exports["TEXTURE_RED_SIZE"] = Value(GL_TEXTURE_RED_SIZE);
    _exports["TEXTURE_GREEN_SIZE"] = Value(GL_TEXTURE_GREEN_SIZE);
    _exports["TEXTURE_BLUE_SIZE"] = Value(GL_TEXTURE_BLUE_SIZE);
    _exports["TEXTURE_ALPHA_SIZE"] = Value(GL_TEXTURE_ALPHA_SIZE);
    _exports["TEXTURE_LUMINANCE_SIZE"] = Value(GL_TEXTURE_LUMINANCE_SIZE);
    _exports["TEXTURE_INTENSITY_SIZE"] = Value(GL_TEXTURE_INTENSITY_SIZE);
    _exports["NEAREST_MIPMAP_NEAREST"] = Value(GL_NEAREST_MIPMAP_NEAREST);
    _exports["NEAREST_MIPMAP_LINEAR"] = Value(GL_NEAREST_MIPMAP_LINEAR);
    _exports["LINEAR_MIPMAP_NEAREST"] = Value(GL_LINEAR_MIPMAP_NEAREST);
    _exports["LINEAR_MIPMAP_LINEAR"] = Value(GL_LINEAR_MIPMAP_LINEAR);
    _exports["OBJECT_LINEAR"] = Value(GL_OBJECT_LINEAR);
    _exports["OBJECT_PLANE"] = Value(GL_OBJECT_PLANE);
    _exports["EYE_LINEAR"] = Value(GL_EYE_LINEAR);
    _exports["EYE_PLANE"] = Value(GL_EYE_PLANE);
    _exports["SPHERE_MAP"] = Value(GL_SPHERE_MAP);
    _exports["DECAL"] = Value(GL_DECAL);
    _exports["MODULATE"] = Value(GL_MODULATE);
    _exports["NEAREST"] = Value(GL_NEAREST);
    _exports["REPEAT"] = Value(GL_REPEAT);
    _exports["CLAMP"] = Value(GL_CLAMP);
    _exports["S"] = Value(GL_S);
    _exports["T"] = Value(GL_T);
    _exports["R"] = Value(GL_R);
    _exports["Q"] = Value(GL_Q);
    _exports["TEXTURE_GEN_R"] = Value(GL_TEXTURE_GEN_R);
    _exports["TEXTURE_GEN_Q"] = Value(GL_TEXTURE_GEN_Q);
    _exports["VENDOR"] = Value(GL_VENDOR);
    _exports["RENDERER"] = Value(GL_RENDERER);
    _exports["VERSION"] = Value(GL_VERSION);
    _exports["EXTENSIONS"] = Value(GL_EXTENSIONS);
    _exports["NO_ERROR"] = Value(GL_NO_ERROR);
    _exports["INVALID_ENUM"] = Value(GL_INVALID_ENUM);
    _exports["INVALID_VALUE"] = Value(GL_INVALID_VALUE);
    _exports["INVALID_OPERATION"] = Value(GL_INVALID_OPERATION);
    _exports["STACK_OVERFLOW"] = Value(GL_STACK_OVERFLOW);
    _exports["STACK_UNDERFLOW"] = Value(GL_STACK_UNDERFLOW);
    _exports["OUT_OF_MEMORY"] = Value(GL_OUT_OF_MEMORY);
    _exports["CURRENT_BIT"] = Value(GL_CURRENT_BIT);
    _exports["POINT_BIT"] = Value(GL_POINT_BIT);
    _exports["LINE_BIT"] = Value(GL_LINE_BIT);
    _exports["POLYGON_BIT"] = Value(GL_POLYGON_BIT);
    _exports["POLYGON_STIPPLE_BIT"] = Value(GL_POLYGON_STIPPLE_BIT);
    _exports["PIXEL_MODE_BIT"] = Value(GL_PIXEL_MODE_BIT);
    _exports["LIGHTING_BIT"] = Value(GL_LIGHTING_BIT);
    _exports["FOG_BIT"] = Value(GL_FOG_BIT);
    _exports["DEPTH_BUFFER_BIT"] = Value(GL_DEPTH_BUFFER_BIT);
    _exports["ACCUM_BUFFER_BIT"] = Value(GL_ACCUM_BUFFER_BIT);
    _exports["STENCIL_BUFFER_BIT"] = Value(GL_STENCIL_BUFFER_BIT);
    _exports["VIEWPORT_BIT"] = Value(GL_VIEWPORT_BIT);
    _exports["TRANSFORM_BIT"] = Value(GL_TRANSFORM_BIT);
    _exports["ENABLE_BIT"] = Value(GL_ENABLE_BIT);
    _exports["COLOR_BUFFER_BIT"] = Value(GL_COLOR_BUFFER_BIT);
    _exports["HINT_BIT"] = Value(GL_HINT_BIT);
    _exports["EVAL_BIT"] = Value(GL_EVAL_BIT);
    _exports["LIST_BIT"] = Value(GL_LIST_BIT);
    _exports["TEXTURE_BIT"] = Value(GL_TEXTURE_BIT);
    _exports["SCISSOR_BIT"] = Value(GL_SCISSOR_BIT);
    _exports["ALL_ATTRIB_BITS"] = Value(GL_ALL_ATTRIB_BITS);
    _exports["PROXY_TEXTURE_1D"] = Value(GL_PROXY_TEXTURE_1D);
    _exports["PROXY_TEXTURE_2D"] = Value(GL_PROXY_TEXTURE_2D);
    _exports["TEXTURE_PRIORITY"] = Value(GL_TEXTURE_PRIORITY);
    _exports["TEXTURE_RESIDENT"] = Value(GL_TEXTURE_RESIDENT);
    _exports["TEXTURE_BINDING_1D"] = Value(GL_TEXTURE_BINDING_1D);
    _exports["TEXTURE_BINDING_2D"] = Value(GL_TEXTURE_BINDING_2D);
    _exports["TEXTURE_INTERNAL_FORMAT"] = Value(GL_TEXTURE_INTERNAL_FORMAT);
    _exports["ALPHA4"] = Value(GL_ALPHA4);
    _exports["ALPHA8"] = Value(GL_ALPHA8);
    _exports["ALPHA12"] = Value(GL_ALPHA12);
    _exports["ALPHA16"] = Value(GL_ALPHA16);
    _exports["LUMINANCE4"] = Value(GL_LUMINANCE4);
    _exports["LUMINANCE8"] = Value(GL_LUMINANCE8);
    _exports["LUMINANCE12"] = Value(GL_LUMINANCE12);
    _exports["LUMINANCE16"] = Value(GL_LUMINANCE16);
    _exports["LUMINANCE4_ALPHA4"] = Value(GL_LUMINANCE4_ALPHA4);
    _exports["LUMINANCE6_ALPHA2"] = Value(GL_LUMINANCE6_ALPHA2);
    _exports["LUMINANCE8_ALPHA8"] = Value(GL_LUMINANCE8_ALPHA8);
    _exports["LUMINANCE12_ALPHA4"] = Value(GL_LUMINANCE12_ALPHA4);
    _exports["LUMINANCE12_ALPHA12"] = Value(GL_LUMINANCE12_ALPHA12);
    _exports["LUMINANCE16_ALPHA16"] = Value(GL_LUMINANCE16_ALPHA16);
    _exports["INTENSITY"] = Value(GL_INTENSITY);
    _exports["INTENSITY4"] = Value(GL_INTENSITY4);
    _exports["INTENSITY8"] = Value(GL_INTENSITY8);
    _exports["INTENSITY12"] = Value(GL_INTENSITY12);
    _exports["INTENSITY16"] = Value(GL_INTENSITY16);
    _exports["R3_G3_B2"] = Value(GL_R3_G3_B2);
    _exports["RGB4"] = Value(GL_RGB4);
    _exports["RGB5"] = Value(GL_RGB5);
    _exports["RGB8"] = Value(GL_RGB8);
    _exports["RGB10"] = Value(GL_RGB10);
    _exports["RGB12"] = Value(GL_RGB12);
    _exports["RGB16"] = Value(GL_RGB16);
    _exports["RGBA2"] = Value(GL_RGBA2);
    _exports["RGBA4"] = Value(GL_RGBA4);
    _exports["RGB5_A1"] = Value(GL_RGB5_A1);
    _exports["RGBA8"] = Value(GL_RGBA8);
    _exports["RGB10_A2"] = Value(GL_RGB10_A2);
    _exports["RGBA12"] = Value(GL_RGBA12);
    _exports["RGBA16"] = Value(GL_RGBA16);
    _exports["CLIENT_PIXEL_STORE_BIT"] = Value(GL_CLIENT_PIXEL_STORE_BIT);
    _exports["CLIENT_VERTEX_ARRAY_BIT"] = Value(GL_CLIENT_VERTEX_ARRAY_BIT);
    _exports["CLIENT_ALL_ATTRIB_BITS"] = Value(GL_CLIENT_ALL_ATTRIB_BITS);
}
static Module _module_GL("GL", _setup_GL);