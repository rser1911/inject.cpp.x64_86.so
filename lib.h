#ifndef __LIB_H__
#define __LIB_H__

#define EXPORT extern "C"  __attribute__((visibility ("default")))

class Base {
    public:
        virtual int Magic() = 0;
        virtual ~Base()     = 0;
};

EXPORT Base* FactoryBase();

#endif
