#include <stdio.h>
#include <unistd.h>

int main() {
    
    int arr[11] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    for (int i = 0; i < 11; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    int arr2[11] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    for (int i = 0; i < 11; i++) {
        printf("%d ", arr2[0]+i);
    }
    printf("\n");

    printf("is error? : %d\n", arr[15]);
    printf("\n");

    int arr3[4][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", arr3[i][j]);
        }
        printf("\n");
    }
    for (int i = 0; i < 16; i++) {
        printf("%d ", arr3[0][0]+i);
    }
    printf("\n");

    char *c;
    c = NULL;
    write(1, c, 10); // This will cause a segmentation fault because c is uninitialized
    printf("%s\n", c); // This will also cause a segmentation fault because c is uninitialized

    return 0;
}