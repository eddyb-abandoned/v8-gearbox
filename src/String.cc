
#include <v8-gearbox.h>

namespace Gearbox {
    
    String String::concat(String left, String right) {
        return Value(v8::String::Concat(left, right));
    }
}