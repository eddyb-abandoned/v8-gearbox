#ifndef SHELL_H
#define SHELL_H

#define ArrayCount(arr) (sizeof(arr) / sizeof(*arr))

#define V8Str(str...) v8::String::New(str)
#define V8Set(str, obj) Set(V8Str(str), obj)
#define V8Get(str) Get(V8Str(str))
#define V8Func(name) v8::FunctionTemplate::New(V8Func##_##name)
#define V8FuncDef(name) v8::Handle<v8::Value> V8Func##_##name(const v8::Arguments& args)
#define V8FuncSet(name) V8Set(#name, v8::FunctionTemplate::New(V8Func##_##name));
#define V8FuncCall(this, func, args...) do {\
                                            v8::Handle<v8::Value> _args[] = {args};\
                                            v8::Local<v8::Function>::Cast(func)->Call(this, ArrayCount(_args), _args);\
                                        } while(0)
#define V8FuncCall0(this, func) v8::Local<v8::Function>::Cast(func)->Call(this, 0, 0);
#define V8Assert(cond, str) if (!(cond)) return v8::ThrowException(V8Str(str));
#define V8AssertArgs(x) V8Assert(args.Length() == x, "Expected " #x " parameters");

#endif
