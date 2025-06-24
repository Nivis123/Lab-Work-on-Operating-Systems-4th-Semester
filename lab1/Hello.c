#include<stdio.h>



int Global_data=123;

int main(void){
    int* ptr = &Global_data;
    int data;
    printf("Hello,world!\n");
    printf("PTR=%p!\n",ptr);
    scanf("%d",&data);
    

    return 0;
}

//nm -g -n hello