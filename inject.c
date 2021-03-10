#define EXPORT extern "C"  __attribute__((visibility ("default")))
#define HIDDEN __attribute__ ((visibility ("hidden")))
#include <dlfcn.h>
#include <stdio.h>
#pragma GCC visibility push(hidden)
#include <inttypes.h>
#include <map>
#include <vector>
#include <stack>

extern "C" void vtable();
extern "C" void after_ret();
extern "C" void * retaddr_swap(void *);

asm (R"(
    .type after_ret, @function
    after_ret:
    
    push   %r12
    push   %rdx
    subq   $16, %rsp
    movdqu %xmm0, (%rsp)
    subq   $16, %rsp
    movdqu %xmm1, (%rsp)
    
    mov     %rax, %r12 # result
    call    retaddr_load
    mov     %r12,  %rdi
    mov     %rax,  %r12 # ret
    call    after_ret_do # fix ret
    mov     %r12, %rdi

    movdqu (%rsp),%xmm1
    addq   $16, %rsp
    movdqu (%rsp),%xmm0
    addq   $16, %rsp
    pop    %rdx
    pop    %r12
    
    push    %rdi # ret
    ret
)");

asm (R"(
    .type vtable_do, @function
    vtable_do:
    pop    %r11 # pos
    
    push   %rdx
    push   %rsi
    push   %rdi
    push   %rcx
    push   %r8
    push   %r9
    
    push   %r11
    call   vtable_find_addr
    pop    %r11 # clear
    
    cmpq   $0, %rax
    jne    not_fixret
        mov    0x30(%rsp), %rdi # ret
        lea    after_ret(%rip), %rax
        mov    %rax, 0x30(%rsp)
        call   retaddr_swap
        
    not_fixret:
    
    pop    %r9
    pop    %r8
    pop    %rcx
    pop    %rdi
    pop    %rsi
    pop    %rdx
    jmp    *%rax
    )");


#define F(n) "push $" #n " \n jmp vtable_do \n" 
#define FSIZE 7

struct obj_desc {
    const char * name;
    const char ** calls_names;
    const char ** calls_desc;
    int calls_num;
    
    obj_desc (const char * iname, const char ** icalls_names, 
              const char ** icalls_desc, int icalls_num){
        name = iname;
        calls_names = icalls_names;
        calls_desc = icalls_desc;
        calls_num = icalls_num;
    }
};

struct obj{
  void ** vtable;
  void * object;
  struct obj_desc * desc;
  
};

static std::map <void *, struct obj *> p2obj;
static std::stack <void *> retaddr;
static void ** vtable_arr = NULL;

extern "C" void * retaddr_load(){
    void *p = retaddr.top();
    retaddr.pop();
    return p;
}

extern "C" void retaddr_save(void * p){
    retaddr.push(p);
}

extern "C" void *  retaddr_swap(void * p){
    void * ret = retaddr.top();
    retaddr.pop();
    retaddr.push(p);
    return ret;
}

asm (R"(
   .type vtable, @function
   vtable:)" \
   F(  0) F(  1) F(  2) F(  3) F(  4) F(  5) F(  6) F(  7) F(  8) F(  9)
   F( 10) F( 11) F( 12) F( 13) F( 14) F( 15) F( 16) F( 17) F( 18) F( 19)
   F( 20) F( 21) F( 22) F( 23) F( 24) F( 25) F( 26) F( 27) F( 28) F( 29)
   F( 30) F( 31) F( 32) F( 33) F( 34) F( 35) F( 36) F( 37) F( 38) F( 39)
   F( 40) F( 41) F( 42) F( 43) F( 44) F( 45) F( 46) F( 47) F( 48) F( 49)
   F( 50) F( 51) F( 52) F( 53) F( 54) F( 55) F( 56) F( 57) F( 58) F( 59)
   F( 60) F( 61) F( 62) F( 63) F( 64) F( 65) F( 66) F( 67) F( 68) F( 69)
   F( 70) F( 71) F( 72) F( 73) F( 74) F( 75) F( 76) F( 77) F( 78) F( 79)
   F( 80) F( 81) F( 82) F( 83) F( 84) F( 85) F( 86) F( 87) F( 88) F( 89) 
   F( 90) F( 91) F( 92) F( 93) F( 94) F( 95) F( 96) F( 97) F( 98) F( 99)
   F(100) F(101) F(102) F(103) F(104) F(105) F(106) F(107) F(108) F(109)
   F(110) F(111) F(112) F(113) F(114) F(115) F(116) F(117) F(118) F(119)
   F(120) F(121) F(122) F(123) F(124) F(125) F(126) F(127) );

static int ZERO_COUNTER = __COUNTER__;
void fix_ZERO_COUNTER(){}; 
 
#define STR(x) #x
#define HOOK(name, desc) HOOK_DO(name, desc, __COUNTER__ ) 
#define HOOK_DO(name, desc, pos)                                      \
    asm volatile (                                                    \
    ".globl " #name " \n"                                             \
    ".type " #name ", @function \n"                                   \
    "" #name ": \n"                                                   \
    "push $-" STR(pos) "\n"                                           \
    "jmp vtable_do");                                                 \
    static int __i_##name = pos - ZERO_COUNTER;                       \
    void __attribute__((constructor))                                 \
    __init_##name(void) {                                             \
        func_names->push_back(#name);                                 \
        func_desc->push_back(desc);                                   \
        func_calls->push_back(NULL);                                  \
    }                                                                 \
    void fix_##name(){};  /* FIXME */          


static std::vector <const char *> * func_names;
static std::vector <const char *> * func_desc;
static std::vector <void *>       * func_calls;

void __attribute__((constructor)) __init(void) {
    vtable_arr = new void * [128];
    for (int i = 0; i < 128; ++i)
        vtable_arr[i] = (void *)((int64_t) vtable + FSIZE * i);
        
    void * p = (void *) after_ret; /* FIXME */ 
    func_names = new std::vector <const char *> ();
    func_names->push_back(NULL);
    func_desc = new std::vector <const char *> ();
    func_desc->push_back(NULL);
    func_calls = new std::vector <void *> ();
    func_calls->push_back(NULL);
}

struct obj * hook_object(void * p, struct obj_desc * desc){
    struct obj * o = new struct obj;
    o->vtable = * (void ***) p;
    o->desc = desc;
    o->object = p;
    p2obj[p] = o;
    * (void ***) p = vtable_arr;
    return o;
}

void unhook_object(struct obj * o){
    p2obj.erase(o->object);
    delete o;
}

#include "inject.h"
#include "inject_user.cpp"

extern "C"  void * after_ret_do(void * a){
    int64_t i = (int64_t) retaddr_load();
    if (i < 0){
        return after_call_func(a, -i);
    }else{
        struct obj * o = (struct obj *) retaddr_load();
        return after_call_method(a, o, i);
    }
}

extern "C" void * vtable_find_addr(void * a, void * b, void * c, 
                                   void * d, void * e, void * f, 
                                   int64_t i){
    void * res, * jmp = NULL;
    
    if (i < 0){ // functions
        i = i + ZERO_COUNTER;
        jmp = func_calls->at(-i);
        
        if (jmp == NULL){
           jmp = (void *) dlsym(RTLD_NEXT, func_names->at(-i)); 
           func_calls->at(-i) = jmp;
        }
        
        res = before_call_func(a, b, c, d, e, f, -i);
        if (res != NULL) return res;
    } else {    // methods
        struct obj * o = p2obj[a];
        jmp = o->vtable[i];
        res = before_call_method (a, b, c, d, e, f, o, i);
        
        if (res != NULL) return res;
        retaddr_save((void *) o); 
    }
    
    retaddr_save((void *) i);
    retaddr_save(jmp);
    return NULL;
}

#pragma GCC visibility pop
