//TowerOfHanoi Algorithem Iterative and Recursive
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#define Max_Size 1000
typedef unsigned long long int Big;

/*==============================================================================================================================*/

void Hanoi_Recursive(int Stacks, char Init_Rod, char Dest_Rod, char Assis_Rod, int* NumOfMoves, int* NumOfFunctionCall) {
    (*NumOfFunctionCall)++;
    if (Stacks == 1)
    {
        (*NumOfMoves)++;
        printf("The upmost disk in rod %c is moved to %c\n", Init_Rod, Dest_Rod);
    }
    else {
        Hanoi_Recursive(Stacks - 1, Init_Rod, Assis_Rod, Dest_Rod, NumOfMoves, NumOfFunctionCall);
        Hanoi_Recursive(1, Init_Rod, Dest_Rod, Assis_Rod, NumOfMoves, NumOfFunctionCall);
        Hanoi_Recursive(Stacks - 1, Assis_Rod, Dest_Rod, Init_Rod, NumOfMoves, NumOfFunctionCall);
    }

}

/*==============================================================================================================================*/

/*==============================================================================================================================*/

void push(int* Rod, int* Rod_Index, int Value_Poped) {

    (*Rod_Index)++;

    Rod[*Rod_Index] = Value_Poped;

}

int pop(int* Rod, int* Rod_Index) {

    int Value_Poped = Rod[*Rod_Index];
    Rod[*Rod_Index] = Max_Size;
    (*Rod_Index)--;

    return Value_Poped;
}

void NumOfStacksIsOdd(int NumOfStacks, int* NumOfMoves) {

    int* Rod_A = malloc(sizeof(int) * (NumOfStacks + 1));
    int Rod_A_index = NumOfStacks; //Stack Number labeling: 0 1 2 3 ... NumOfStacks => NumOfStack+1
    Rod_A[0] = Max_Size;
    for (int i = 1;i <= NumOfStacks;i++) {
        Rod_A[i] = NumOfStacks - i + 1;
    }
   

    int* Rod_B = malloc(sizeof(int) * (NumOfStacks + 1));
    int Rod_B_index = 0;
    for (int i = 0;i <= NumOfStacks;i++) {
        Rod_B[i] = Max_Size;
    }


    int* Rod_C = malloc(sizeof(int) * (NumOfStacks + 1));
    int Rod_C_index = 0;
    for (int i = 0;i <= NumOfStacks;i++) {
        Rod_C[i] = Max_Size;
    }

    Big A = 0x01;

    for (int i = 0;i < ((A << NumOfStacks) - 1);i++) {
        if (i % 3 == 0) {
            if (Rod_C[Rod_C_index] < Rod_A[Rod_A_index]) {
                push(Rod_A, &Rod_A_index, pop(Rod_C, &Rod_C_index));
                printf("The upmost disk in rod 'C' is moved to 'A'\n");
                (*NumOfMoves)++;
            }
            else {
                push(Rod_C, &Rod_C_index, pop(Rod_A, &Rod_A_index));
                printf("The upmost disk in rod 'A' is moved to 'C'\n");
                (*NumOfMoves)++;
            }
        }
        else if (i % 3 == 1) {
            if (Rod_B[Rod_B_index] < Rod_A[Rod_A_index]) {
                push(Rod_A, &Rod_A_index, pop(Rod_B, &Rod_B_index));
                printf("The upmost disk in rod 'B' is moved to 'A'\n");
                (*NumOfMoves)++;
            }
            else {
                push(Rod_B, &Rod_B_index, pop(Rod_A, &Rod_A_index));
                printf("The upmost disk in rod 'A' is moved to 'B'\n");
                (*NumOfMoves)++;
            }
        }
        else {
            if (Rod_C[Rod_C_index] < Rod_B[Rod_B_index]) {
                push(Rod_B, &Rod_B_index, pop(Rod_C, &Rod_C_index));
                printf("The upmost disk in rod 'C' is moved to 'B'\n");
                (*NumOfMoves)++;
            }
            else {
                push(Rod_C, &Rod_C_index, pop(Rod_B, &Rod_B_index));
                printf("The upmost disk in rod 'B' is moved to 'C'\n");
                (*NumOfMoves)++;
            }
        }
    }
}

void NumOfStacksIsEven(int NumOfStacks, int* NumOfMoves) {

    int* Rod_A = malloc(sizeof(int) * (NumOfStacks + 1));
    int Rod_A_index = NumOfStacks; //Stack Number labeling: 0 1 2 3 ... NumOfStacks => NumOfStack+1
    for (int i = 0;i <= NumOfStacks;i++) {
        Rod_A[i] = NumOfStacks - i;
    }

    int* Rod_B = malloc(sizeof(int) * (NumOfStacks + 1));
    int Rod_B_index = 0;
    for (int i = 0;i <= NumOfStacks;i++) {
        Rod_B[i] = Max_Size;
    }


    int* Rod_C = malloc(sizeof(int) * (NumOfStacks + 1));
    int Rod_C_index = 0;
    for (int i = 0;i <= NumOfStacks;i++) {
        Rod_C[i] = Max_Size;
    }

    Big A = 0x01;

    for (int i = 0;i < ((A << NumOfStacks) - 1);i++) {
        if (i % 3 == 1) {
            if (Rod_C[Rod_C_index] < Rod_A[Rod_A_index]) {
                push(Rod_A, &Rod_A_index, pop(Rod_C, &Rod_C_index));
                printf("The upmost disk in rod 'C' is moved to 'A'\n");
                (*NumOfMoves)++;
            }
            else {
                push(Rod_C, &Rod_C_index, pop(Rod_A, &Rod_A_index));
                printf("The upmost disk in rod 'A' is moved to 'C'\n");
                (*NumOfMoves)++;
            }
        }
        else if (i % 3 == 0) {
            if (Rod_B[Rod_B_index] < Rod_A[Rod_A_index]) {
                push(Rod_A, &Rod_A_index, pop(Rod_B, &Rod_B_index));
                printf("The upmost disk in rod 'B' is moved to 'A'\n");
                (*NumOfMoves)++;
            }
            else {
                push(Rod_B, &Rod_B_index, pop(Rod_A, &Rod_A_index));
                printf("The upmost disk in rod 'A' is moved to 'B'\n");
                (*NumOfMoves)++;
            }
        }
        else {
            if (Rod_C[Rod_C_index] < Rod_B[Rod_B_index]) {
                push(Rod_B, &Rod_B_index, pop(Rod_C, &Rod_C_index));
                printf("The upmost disk in rod 'C' is moved to 'B'\n");
                (*NumOfMoves)++;
            }
            else {
                push(Rod_C, &Rod_C_index, pop(Rod_B, &Rod_B_index));
                printf("The upmost disk in rod 'B' is moved to 'C'\n");
                (*NumOfMoves)++;
            }
        }
    }
}

void Hanoi_Iterative(int NumOfStacks, int* NumOfMoves) {

    if (NumOfStacks & 0x01) {

        NumOfStacksIsOdd(NumOfStacks, NumOfMoves);

    }
    else {

        NumOfStacksIsEven(NumOfStacks, NumOfMoves);

    }

}

/*==============================================================================================================================*/

int main() {
    int NumOfStacks = 0, NumOfMoves = 0, NumOfFunctionCall = 0;

    printf("input the number of stacks: ");
    scanf("%d", &NumOfStacks);

    printf("==============================================================\n");
    printf("======================Hanoi_Recursive=========================\n");
    Hanoi_Recursive(NumOfStacks, 'A', 'C', 'B', &NumOfMoves, &NumOfFunctionCall);
    printf("The number of Moves is %d\n", NumOfMoves);
    printf("The number of function calls is %d\n", NumOfFunctionCall);
    printf("==============================================================\n");
    printf("==============================================================\n");

    NumOfMoves = 0;
    NumOfFunctionCall = 0;
    printf("\n");

    printf("==============================================================\n");
    printf("======================Hanoi_Iterative=========================\n");
    Hanoi_Iterative(NumOfStacks, &NumOfMoves);
    printf("The number of Moves is %d\n", NumOfMoves);
    printf("==============================================================\n");
    printf("==============================================================\n");

    return 0;
}

