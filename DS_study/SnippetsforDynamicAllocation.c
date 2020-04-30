#include <stdio.h>
#include <stdlib.h>

#define MALLOC(p,s)  \
    if (!((p)=malloc(s))){\
        fprintf(stderr,"Insufficient Memory!"); \
        exit(EXIT_FAILURE); \
    }
#define CALLOC(p,n,s)  \
    if (!((p)=calloc(n,s))){\
        fprintf(stderr,"Insufficient Memory!"); \
        exit(EXIT_FAILURE); \
    }
#define REALLOC(p,s)  \
    if (!((p)=realloc(p,s))){\
        fprintf(stderr,"Insufficient Memory!"); \
        exit(EXIT_FAILURE); \
    }