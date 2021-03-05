#include <stdio.h>
#include "lib.h"

#ifdef LOADONFLY
#include <dlfcn.h>
#endif

int main(){
    
#ifdef LOADONFLY
    void * handle = dlopen("./lib.so", RTLD_NOW);
    if(!handle) return 1;
    Base* b = ((Base* (*)()) dlsym(handle, "FactoryBase"))();
#else
    Base* b = FactoryBase();
#endif
    
    printf("Magic() = %d\n", b->Magic());
    delete b;
    return 0;
}
