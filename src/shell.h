#ifndef SHELL_H
#define SHELL_H

#define ArrayCount(arr) (sizeof(arr) / sizeof(*arr))

#define V8Str(...) v8::String::New(__VA_ARGS__)
#define V8Set(str, obj) Set(V8Str(str), obj)
#define V8Get(str) Get(V8Str(str))
#define V8Func(name) v8::FunctionTemplate::New(V8Func##_##name)
#define V8FuncDef(name) v8::Handle<v8::Value> V8Func##_##name(const v8::Arguments& args)
#define V8FuncSet(name) V8Set(#name, V8Func(name)->GetFunction());
#define V8FuncCall(this, func, ...) if(func->IsFunction()) \
                                            do { \
                                                v8::Handle<v8::Value> _args[] = {__VA_ARGS__}; \
                                                v8::Local<v8::Function>::Cast(func)->Call(this, ArrayCount(_args), _args); \
                                            } while(0)
#define V8FuncCall0(this, func) if(func->IsFunction()) \
                                    v8::Local<v8::Function>::Cast(func)->Call(this, 0, 0);
#define V8Throw(str) return v8::ThrowException(V8Str(str))
#define V8Assert(cond, str) if(!(cond)) V8Throw(str)
#define V8AssertArgs(x) V8Assert(args.Length() == x, "Expected " #x " parameters")

#ifdef APACHE_MODULE_GB_
bool RunScript(const char *sScript);
#endif

#endif
