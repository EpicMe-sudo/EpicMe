#include <stdio.h>
#include <stdlib.h>
#define MALLOC(p,s) \
     if (!((p)=malloc(s))) {\
        fprintf(stderr,"Insufficient memory");\
        exit(EXIT_FAILURE); \
    }

int main() {

    //그냥 int 선언
    int x[3][5]={{0x00,},{0x00,},{0x00,}};

    printf("When int x[3][5] \n"); 
    printf("The value of x: %p \n",x);
    printf("The value of x[0]: %p \n",x[0]);
    printf("The value of x[0][0]: %d \n",x[0][0]);
    printf("The value of &x: %p \n",&x);
    printf("The value of &x[0]: %p \n",&x[0]);
    printf("The value of &x[0][0]: %p \n",&x[0][0]);
    printf("The value of *x: %p \n",*x);
    printf("The value of **x: %d \n",**x);
    printf("The size of x: %ld \n",sizeof(x));
    printf("The size of *x: %ld \n",sizeof(*x));
    printf("The size of **x: %ld \n",sizeof(**x));

    //int* 선언
    int *y[3];
    for(int i=0;i<3;i++){
        MALLOC(y[i],5*sizeof(**x));
    }
    printf("When int y[3][5] \n"); 
    printf("The value of y: %p \n",y);
    printf("The value of y[0]: %p \n",y[0]);
    printf("The value of y[0][0]: %d \n",y[0][0]);
    printf("The value of &y: %p \n",&y);
    printf("The value of &y[0]: %p \n",&y[0]);
    printf("The value of &y[0][0]: %p \n",&y[0][0]);
    printf("The value of *y: %p \n",*y);
    printf("The value of **y: %d \n",**y);
    printf("The size of y:: %ld \n",sizeof(y));
    printf("The size of *y:: %ld \n",sizeof(*y));
    printf("The size of **y:: %ld \n",sizeof(**y));

    printf("The size of int, int*, int**: %ld, %ld, %ld",sizeof(int),sizeof(int*),sizeof(int**));

    return 0;
}