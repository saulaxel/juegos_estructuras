#ifndef _CONFIG_H
#define _CONFIG_H

#define NDEBUG

#ifdef __unix__
    #define DIAG "/"
#else
    #define DIAG "\\"
#endif

#define DOUBLE_LINKED

#endif /* _CONFIG_H */
