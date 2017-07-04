#ifndef _CONFIG_H
#define _CONFIG_H

#define DOUBLE_LINKED

#ifndef NDEBUG
    #define MESSAGE(...) printf(__VA_ARGS__)
#else
    #define MESSAGE(...)
#endif


#endif /* _CONFIG_H */
