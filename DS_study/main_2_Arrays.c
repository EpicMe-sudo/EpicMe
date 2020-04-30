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



//////////////////// Problem 1 /////////////////////////////////
/*모든 원소가 0으로 설정된 2차원배열을 생성하는 함수를 만들기 위하여
프로그램 2.3을 최소한의 변경을 해보라. 새로운 함수를 검사해보라.

Program 2.3_
int ** make2dArray(int rows, int cols){
    // Create a two dimensional rows X cols array 
    int **x,i;

    // get memory for row pointers
    MALLOC(x,rows*sizeof(*x))

    // get memory for each row
    for(i=0;i<rows;i++){
        MALLOC(x[i],cols*sizeof(**x))
    }
    return x;
} */

int ** make2dArray(int rows, int cols){
    // Create a two dimensional rows X cols array 
    int **x,i;

    // get memory for row pointers
    MALLOC(x,rows*sizeof(*x));

    // get memory for each row
    for(i=0;i<rows;i++){
        CALLOC(x[i],cols,cols*sizeof(**x));
    }
    return x;
}

/////////////////////// Problem 2 /////////////////////////////////
/* 2차원 배열의 행 i의 원하는 길이(크기 또는 원소 수)를 length[i]라고 하자.
행 i가 length[i], 0<=i<row, 원소를 갖는 2차원 배열을 생성하는 함수를
프로그램 2.3과 비슷하게 작성하라. 코드를 검사해보라.*/

int ** make2dArray2(int rows, int* cols){
    // Create a two dimensional rows X cols array 
    int **x,i;

    // get memory for row pointers
    MALLOC(x,rows*sizeof(*x));

    // get memory for each row
    for(i=0;i<rows;i++){
        CALLOC(x[i],cols[i],cols[i]*sizeof(**x));
    }
    return x;
}

/////////////////////// Problem 3 /////////////////////////////////
/*동적 할당 배열을 이용하여 프로그램 1.16의 행렬 덧셈 함수를 다시 작성하라.
함수의 헤더는 다음과 같아야 한다.

void add(int **a, int **b, int **c, int rows, int cols)

프로그램 1.16
void add(int a[][MAX_SIZE], int b[][MAX_SIZE], int c[][MAX_SIZE], int rows, int cols)
{
    int i, j;
    for(i=0;i<rows;i++)
        for(j=0;j<cols;j++)
            c[i][j] = a[i][j] + b[i][j]    
} */

void add(int **a, int **b, int **c, int rows, int cols){

    for(int i=0;i<rows;i++)
        for(int j=0;j<cols;j++)
            c[i][j] = a[i][j] + b[i][j];
}

/////////////////////// Problem 4 /////////////////////////////////
/*동적 할당 배열을 이용하여 프로그램 1.20의 행렬 곱셈 함수를 다시 작성하라.
함수의 헤더는 다음과 같아야한다.

void mult(int **a, int **b, int **c,int rows)
여기서 각 행렬은 rows * rows 행렬이다. */

void mult(int **a, int **b, int **c,int rows){



}

/////////////////////// Problem 5 /////////////////////////////////
/*동적 할당 배열을 이용하여 프로그램 1.22의 행렬 전치 함수를 다시 작성하라.
함수의 헤더는 다음과 같아야 한다.

void transpose(int **a, int rows) */
void transpose(int **a, int rows){

}

/////////////////////// Problem 6 /////////////////////////////////
/*정방형이 아닐 수도 있는 행렬에 대한 행렬 전치 함수를 작성하라.
동적 할당 배열을 이용하라. 함수의 헤더는 다음과 같아야 한다.

void transpose(int **a, int **b, int rows, int cols)

여기서 a는 전치시킬 rows * cols 행렬이고 b는 함수에 의해 전치된 행렬이다.
전치된 행렬은 cols * rows라는 것을 명심하라. */

void transpose(int **a, int **b, int rows, int cols){



}

int main(){
    
    int Problem;
    printf("Enter Problem Number(1~5): ");
    scanf("%d",&Problem);


    if(Problem==1)    
    {            ////////// Problem 1 ////////////////
        int** x=0;
        x=make2dArray(5,5);

        printf("print Array: \n");
        for(int i=0;i<5;i++){
            for(int j=0;j<5;j++)
                printf("%d ",x[i][j]);
            printf("\n");
        }
    }
    else if(Problem==2)
    {
        ////////// Problem 2 ////////////////
        
        int** x=0;
        int rows=0;
        int *cols;
        printf("input the number of rows: ");
        scanf("%d",&rows);
        
        MALLOC(cols,sizeof(int));

        for(int i=0;i<rows;i++){
            printf("input a number of %dst row: ",i+1);
            scanf("%d",&cols[i]);
        }

        x=make2dArray2(rows,cols);
        
        printf("print Array: \n");
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols[i];j++)
                printf("%d ",x[i][j]);
            printf("\n");
        }
    }
    else if(Problem==3)
    {    ////////// Problem 3 ////////////////
        //생략      
  
    }
    else if(Problem==4)
    {    ////////// Problem 4 ////////////////
           

    }
    else if(Problem==5)
    {    ////////// Problem 5 ////////////////

    }
    return 0;
}