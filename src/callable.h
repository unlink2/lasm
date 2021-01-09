#ifndef __CALLABLE_H__
#define __CALLABLE_H__

#include <iostream>
#include <any>
#include <vector>
#include <memory>
#include "object.h"

namespace lasm {
    class Interpreter;

    class Callable {
        public:
            Callable(unsigned short arity=0):
                arity(arity) {}
            ~Callable() {}
            virtual LasmObject call(Interpreter *interpreter, std::vector<LasmObject> arguments) {
                return LasmObject(NIL_O, nullptr);
            }

            unsigned short getArity() { return arity; }
        private:
            unsigned short arity = 0;
    };

    class NativeHi: public Callable {
        public:
            NativeHi():
                Callable::Callable(1) {}
            ~NativeHi() {}

            virtual LasmObject call(Interpreter *interpreter, std::vector<LasmObject> arguments);
    };

    class NativeLo: public Callable {
        public:
            NativeLo():
                Callable::Callable(1) {}
            ~NativeLo() {}

            virtual LasmObject call(Interpreter *interpreter, std::vector<LasmObject> arguments);
    };
}

#endif 
