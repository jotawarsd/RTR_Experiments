#include <stdio.h>
#include <stdlib.h> 

struct vec_int
{
	int *p; 
	int size; 
}; 

struct vec_float
{
	float *pf; 
	int size; 
}; 

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE]; 

FILE *gp_mesh_file; 
struct vec_float *gp_vertex, *gp_texture, *gp_normal; 
struct vec_int *gp_vertex_indices, *gp_texture_indices, *gp_normal_indices; 

void load_mesh(void)
{
	struct vec_int *create_vec_int(); 
	struct vec_float *create_vec_float(); 
	int push_back_vec_int(struct vec_int *p_vec_int, int data); 
	int push_back_vec_float(struct vec_float *p_vec_int, float data); 
	void show_vec_float(struct vec_float *p_vec_float); 
	void show_vec_int(struct vec_int *p_vec_int); 
	int destroy_vec_float(struct vec_float *p_vec_float); 


	char *space = " ", *slash = "/", *first_token = NULL, *token; 
	char *f_entries[3] =  { NULL, NULL, NULL }; 
	int nr_pos_cords=0, nr_tex_cords=0, nr_normal_cords=0, nr_faces=0; 
	int i, vi; 

	gp_mesh_file = fopen("MonkeyHead.OBJ", "r"); 
	if(gp_mesh_file == NULL)
	{
		fprintf(stderr, "error in opening file\n"); 
		uninitialize();  
		exit(EXIT_FAILURE); 
	}

	gp_vertex = create_vec_float(); 
	gp_texture = create_vec_float(); 
	gp_normal = create_vec_float(); 

	gp_vertex_indices = create_vec_int(); 
	gp_texture_indices = create_vec_int(); 
	gp_normal_indices = create_vec_int(); 

	while(fgets(buffer, BUFFER_SIZE, gp_mesh_file) != NULL)
	{
		first_token = strtok(buffer, space); 

		if(strcmp(first_token, "v") == 0)
		{
			nr_pos_cords++; 
			while((token = strtok(NULL, space)) != NULL)
				push_back_vec_float(gp_vertex, atof(token)); 

		}
		else if(strcmp(first_token, "vt") == 0)
		{
			nr_tex_cords++; 
			while((token = strtok(NULL, space)) != NULL)
				push_back_vec_float(gp_texture, atof(token)); 
		}
		else if(strcmp(first_token, "vn") == 0)
		{
			nr_normal_cords++; 
			while((token = strtok(NULL, space)) != NULL)
				push_back_vec_float(gp_normal, atof(token)); 
		}
		else if(strcmp(first_token, "f") == 0)
		{
			nr_faces++; 
			for(i = 0; i < 3; i++)
				f_entries[i] = strtok(NULL, space); 
				
			for(i = 0; i < 3; i++)	
			{
				token = strtok(f_entries[i], slash); 
				push_back_vec_int(gp_vertex_indices, atoi(token)-1); 
				token = strtok(NULL, slash); 
				push_back_vec_int(gp_texture_indices, atoi(token)-1); 
				token = strtok(NULL, slash); 
				push_back_vec_int(gp_normal_indices, atoi(token)-1); 
			}
		}
	}

	fclose(gp_mesh_file); 
	gp_mesh_file = NULL;
}
