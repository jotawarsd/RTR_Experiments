#include <stdio.h> 
#include <stdlib.h> 

void built_in_data_type(void); 
void array_of_built_in_data_type(void); 

int main(void)
{
    built_in_data_type(); 
    array_of_built_in_data_type(); 
    return (0); 
}

void built_in_data_type(void)
{
    int* p = NULL;  // 1. Declare a pointer to data type and make it NULL 

    p = (int*)malloc(sizeof(int)); // 2. allocate memory on heap and store the base 
                                    // addr returned by malloc() in p 
    memset(p, 0, sizeof(int));      // 3: Initialise memory block to zero 
    // 4. Check the validity of dynamic memory allocation 
    if(p == NULL)
    {
        fprintf(stderr, "malloc():fatal:out of memory\n");  
        exit(EXIT_FAILURE); 
    }

    *p = 500;     // 5-1 : Write 
    printf("*p = %d\n", *p);  // 5-2 : Read 
    free(p); // 6: Release memory 
    p = NULL; // 7: Make pointer NULL 
    
    // If you are defining a local pointer variable and you are storing 
    // an address of dynamically allocated memory block in it 
    // then before returning the function you must do one of the following 
    // three things 
    // 1: Free pointer and make it NULL (as shown above)
    // OR 
    // 2: Return the pointer and ask caller to capture the return value 
    // as a matter of compulson (not as choice) and let caller free it 
    // when it see fit 
    // OR 
    // 3: Copy the address in local pointer in global pointer or 
    // any pointer that will outlive the current function and will 
    // be visible to caller function 
    // If you don't follow at least one of the above steps then 
    // it will result in leaked memory ! 
}

void array_of_built_in_data_type(void)
{
    int* p_arr = NULL; // S1 
    size_t N; 
    size_t i; 

    printf("Enter N:"); 
    scanf("%llu", &N); 

    p_arr = (int*)calloc(N, sizeof(int)); // S2 

    // S3 
    if(p_arr == NULL)
    {
        fprintf(stderr, "malloc():fatal:out of memory\n");  
        exit(EXIT_FAILURE); 
    }

    for(i = 0; i < N; ++i)
        p_arr[i] = (i+1) * 100;     // S4 - 1 

    for(i = 0; i < N; ++i)
        printf("*(p_arr + %llu) = %d\n", i, *(p_arr + i));  // S4 - 2 

    free(p_arr);    // S5 
    p_arr = NULL;   // S6 
}

