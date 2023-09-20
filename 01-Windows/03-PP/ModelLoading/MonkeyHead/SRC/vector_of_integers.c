#include <stdio.h> 
#include <stdlib.h> 

#define SUCCESS 1 
#define VECTOR_EMPTY 2 

struct vec_int 
{
    int* pa; 
    size_t N; 
}; 

struct vec_int* create_vector(void); 
void destroy_vector(struct vec_int* p_vec_int); 
void push_back(struct vec_int* p_vec_int, int new_element); 
int pop_back(struct vec_int* p_vec_int, int* p_pop_data); 

int main(void)
{
    struct vec_int* p_vec_int = NULL; 
    int i; 

    p_vec_int = create_vector(); 
    for(i = 0; i < 150; i += 10)
        push_back(p_vec_int, i); 

    int num; 
    while(pop_back(p_vec_int, &num) != VECTOR_EMPTY)
        printf("Poped data = %d\n", num); 

    destroy_vector(p_vec_int); 
    p_vec_int = NULL; 

    return (0); 
}

struct vec_int* create_vector(void)
{
    struct vec_int* p_vec_int = NULL; 

    p_vec_int = (struct vec_int*)malloc(sizeof(struct vec_int)); 
    if(p_vec_int == NULL)
    {
        fprintf(stderr, "malloc():fatal:out of memory\n"); 
        exit(EXIT_FAILURE); 
    }

    p_vec_int->pa = NULL; 
    p_vec_int->N = 0; 

    return (p_vec_int); 
}

void destroy_vector(struct vec_int* p_vec_int)
{
    free(p_vec_int->pa); 
    free(p_vec_int); 
}

void push_back(struct vec_int* p_vec_int, int new_element)
{
    p_vec_int->N += 1; 
    p_vec_int->pa = (int*)realloc(p_vec_int->pa, p_vec_int->N * sizeof(int)); 
    p_vec_int->pa[p_vec_int->N - 1] = new_element; 
}

int pop_back(struct vec_int* p_vec_int, int* p_pop_data)
{
    if(p_vec_int->N == 0)
        return (VECTOR_EMPTY); 
    
    *p_pop_data = p_vec_int->pa[p_vec_int->N - 1]; 
    p_vec_int->N -= 1; 
    p_vec_int->pa = (int*)realloc(p_vec_int->pa, p_vec_int->N * sizeof(int)); 
    
    return (SUCCESS); 
}


// 1. realloc() minimize. 
// 2. class version assuming vector of integer 
// 3. iterator design pattern 
// 4. class template, iterator design pattern, minimize realloc() calls 