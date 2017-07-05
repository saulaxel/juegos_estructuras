#ifndef _CONFIG_H
#define _CONFIG_H

#define NDEBUG

#define DOUBLE_LINKED
#define STORE_ELEMENTS_NUM

#ifdef _WIN32
    #define DIAG "\\"
#else
    #define DIAG "/"
#endif

#ifndef NDEBUG
    #define MESSAGE(...) printf(__VA_ARGS__)
#else
    #define MESSAGE(...)
#endif


#endif /* _CONFIG_H */
