#pragma GCC visibility push(hidden)

#include <stdio.h>
#include "lib.h"

class Derived: public Base {
    public:
        virtual int Magic();
        virtual ~Derived();
    private:
        virtual int One();
        int state = 5;
};

Base* FactoryBase(){
   return new Derived(); 
}

// *** //

int Derived::Magic(){ return One() * state * 20100; }
int Derived::One() { return 1; }

Derived::~Derived(){ printf("~ Derived\n"); }
Base::~Base(){ printf("~ Base\n"); }

#pragma GCC visibility pop
