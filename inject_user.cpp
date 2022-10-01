void * before_call_func(void * a, void * b, void * c, 
                        void * d, void * e, void * f, 
                        int64_t i){
         
    fprintf(stderr, "@ > %s [%ld]", func_names->at(i), i);
    fprintf(stderr, " --- %s", func_desc->at(i));
    fprintf(stderr, "\n");
    
    /*  */
    
    return NULL; // for hook ret
}

void * after_call_func(void * a, 
                       int64_t i){
                           
    fprintf(stderr, "@ < %s [%ld], ret = %p\n", func_names->at(i), i, a);
    
    /*  */
    
    if (i == __i_FactoryBase){
        fprintf(stderr, "@ << HOOKED %s object\n",  
            (hook_object(a, &Base_desc))->desc->name);
    }
    return a;
}

/*  */

int Base_One(void *p){
    int64_t i = (int64_t) retaddr_load();
    struct obj * o = (struct obj *) retaddr_load();
    
    int ret = 2;
    int state = 8; // position in object
    ret *= *((int *) ((char * ) p + state)); // private state = 5
    ret *= ((int (*)(void *)) o->vtable[i])(p); // private One() = 1
    fprintf(stderr, "@ < Base::UNKNOWN [%ld = One], ret = %d\n", i, ret);
    // 2*5*1 = 10
    return ret;
}

void * before_call_method(void * a, void * b, void * c, 
                            void * d, void * e, void * f, 
                            struct obj * o, int64_t i){
    
    if (i < o->desc->calls_num){
        fprintf(stderr, "@ > %s::%s [%ld]", 
            o->desc->name, o->desc->calls_names[i], i);
        fprintf(stderr, " --- %s", o->desc->calls_desc[i]);
        fprintf(stderr, ", obj = %p\n", o->object);
    }else{
        fprintf(stderr, "@ > %s::UNKNOWN [%ld]", o->desc->name, i);
        fprintf(stderr, " --- ???, obj = %p\n", o->object);
    }

    /*  */
    
    if (o->desc->name == Base_name){
        if (i == 3){ // find from UNKNOWN print
            retaddr_save((void *) o); // you can save something for
            retaddr_save((void *) i); // usage in function
            return (void * ) Base_One; // jump for this
        }
    }
        
    return NULL; // for hook ret
}


void * after_call_method(void * a,
                         struct obj * o, int64_t i){
    
    if (i < o->desc->calls_num){
        fprintf(stderr, "@ < %s::%s [%ld], ret = %ld, obj = %p\n",
            o->desc->name, o->desc->calls_names[i], i, (int64_t)a, o->object);
    }else{
        fprintf(stderr, "@ < %s::UNKNOWN [%ld], ret = %ld, obj = %p\n",
            o->desc->name, i, (int64_t)a, o->object);
    }
    
    /*  */
    
    if (o->desc->name == Base_name){
        if (i == __i_Base::Magic0){
            a = (void *) ((int64_t) a / 10);
            fprintf(stderr, "@ << %s::%s, changed ret = %ld\n",
                 o->desc->name, o->desc->calls_names[i], (int64_t)a);
        }else
        if (i == __i_Base::DESTRUCTOR1){
            fprintf(stderr, "@ << UNHOOKED %s object\n", o->desc->name);
            unhook_object(o);
        }
    }
    return a;
}

