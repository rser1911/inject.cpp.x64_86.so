#include <stdio.h>
#include "lib.h"

int main(){
    Base* b = FactoryBase();
    printf("Magic() = %d\n", b->Magic());
    delete b;
    return 0;
}
