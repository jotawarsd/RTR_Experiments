#include <stdio.h> 
#include <stdlib.h> 

#define TRUE 1 

int main(void)
{
    int* pa = NULL; 
    size_t N = 0; 
    int ch; 
    int num; 
    size_t i; 

    while(TRUE)
    {
        printf("Do you want to enter an integer?[1-YES, 0-No]:"); 
        scanf("%d", &ch); 
        if(ch != 1) 
            break;
        printf("Enter an integer:"); 
        scanf("%d", &num); 
        N += 1; 
        pa = (int*)realloc(pa, N * sizeof(int)); 
        pa[N-1] = num; 
    }

    puts("Data entered by the user"); 
    for(i = 0; i < N; ++i)
        printf("pa[%llu]:%d\n", i, pa[i]); 

    free(pa); 
    pa = NULL; 

    return (0); 
}  