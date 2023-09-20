#include <stdio.h> 
#include <stdlib.h> 

/* Header file: dynamic_array.h */ 
#define SUCCESS 1 
#define INDEX_OUT_OF_RANGE 2 

struct dynamic_array
{
    int* p_arr; 
    size_t N; 
}; 

struct dynamic_array* create_dynamic_array(size_t N); 
void destroy_dynamic_array(struct dynamic_array* p_arr); 

int get(struct dynamic_array* p_arr, size_t index); 
int set(struct dynamic_array* p_arr, size_t index, int new_element); 

/* Header file end */ 

/* Client: dynamic_array_client.c -> includes -> dynamic_array.h */ 
int main(void)
{
    struct dynamic_array* pd_arr = NULL; 
    size_t i; 
    const size_t N = 10; 
    pd_arr = create_dynamic_array(N); 
    for(i = 0; i < N; ++i)
        set(pd_arr, i, (i+1)*10); 
    for(i = 0; i < N; ++i)
        printf("pd_arr[%llu]:%d\n", i, get(pd_arr, i)); 
    destroy_dynamic_array(pd_arr); 
    pd_arr = NULL; 
    exit(EXIT_SUCCESS); 
}
/* Client : End */ 

/* server -> dynamic_array_server.c -> includes -> dynamic_array.h */ 
struct dynamic_array* create_dynamic_array(size_t N)
{
    struct dynamic_array* p_dynamic_array = NULL; 

    p_dynamic_array = (struct dynamic_array*)calloc(1, sizeof(struct dynamic_array)); 
    if(p_dynamic_array == NULL)
    {
        fprintf(stderr, "calloc():out of memory\n"); 
        exit(EXIT_FAILURE); 
    }

    p_dynamic_array->p_arr = (int*)calloc(N, sizeof(int)); 
    if(p_dynamic_array->p_arr == NULL)
    {
        fprintf(stderr, "calloc():out of memory\n"); 
        exit(EXIT_FAILURE); 
    }
    p_dynamic_array->N = N; 
    return (p_dynamic_array); 
}

void destroy_dynamic_array(struct dynamic_array* p_dynamic_array)
{
    free(p_dynamic_array->p_arr); 
    free(p_dynamic_array); 
}

int get(struct dynamic_array*  p_dynamic_array, size_t index)
{
    if(index >= p_dynamic_array->N)
        return (INDEX_OUT_OF_RANGE); 
    return (p_dynamic_array->p_arr[index]); 
}

int set(struct dynamic_array*  p_dynamic_array, size_t index, int new_element)
{
    if(index >= p_dynamic_array->N)
        return (INDEX_OUT_OF_RANGE); 
    p_dynamic_array->p_arr[index] = new_element; 
    return (SUCCESS); 
}
/* Server End */ 