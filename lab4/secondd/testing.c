#include<stdio.h>

int sum(int a,int b){
    int sum =0;
    char buffer[8]={1,2,3,4,5,6,7};
    return a+b;
}

int main(void){
    int a = 10;
    int b=10;
    printf("%d\n",sum(a,b));

    return 0;
}
