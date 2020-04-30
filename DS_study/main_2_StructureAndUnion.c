#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char data;
    list *link; // 구조체 포인터는 앞에 정의가 안되도 먼저 선언만 가능
} list;

int main(){
    list item1, item2, item3;
    item1.data = 'a';
    item2.data = 'b';
    item3.data = 'c';
    item1.link = &item2;
    item2.link = &item3;
    item3.link = NULL;
    return 0;
}