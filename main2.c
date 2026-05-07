#include <stdio.h>
#include <unistd.h>

int main() {

    int a = 4;
    int b = 9;
    
    int const *p1 = &a;
    const int *p2 = &a;

    int *const p3 = &a;

    const int *const p4 = &a;
    int const *const p5 = &a;

    int arr[10] = {0}; 

    p1 = &b;
    *p1 = 5; 
    printf("p1: %d\n", *p1);

    p2 = &b;
    *p2 = 5;
    printf("p2: %d\n", *p2);

    p3 = &b;
    *p3 = 5;
    printf("p3: %d\n", *p3);

    p4 = &b;
    *p4 = 5;
    printf("p4: %d\n", *p4);

    p5 = &b;
    *p5 = 5;
    printf("p5: %d\n", *p5);

    char str_buffer[10] = "Hello";
    printf("str_buffer: %s\n", str_buffer);
    
    char str_buffer2[10] = "Hello, World!";
    printf("str_buffer2: %s\n", str_buffer2);

    return 0;
}