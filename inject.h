/* THIS FILE IS AUTOGENERATED */

HOOK(FactoryBase, "EXPORT Base* FactoryBase();");

static const char * Base_name = "Base";

static const char * Base_calls_desc[] = { 
    "int Base::Magic()",
    "Base::~Base() [complete]",
    "Base::~Base() [deleting]",
};

static const char * Base_calls_names[] = { 
    "Magic",
    "~Base",
    "~Base",
};

static struct obj_desc Base_desc (Base_name, Base_calls_names, Base_calls_desc, 3);

enum __i_Base { 
    Magic0,
    DESTRUCTOR0,
    DESTRUCTOR1,
};

