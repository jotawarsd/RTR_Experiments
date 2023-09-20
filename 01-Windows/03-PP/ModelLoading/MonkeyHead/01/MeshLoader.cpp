#include "MeshLoader.h"
#include <stdio.h>
#include <stdlib.h>

struct vecInt *createVecInt(FILE *log, char *type)
{
	struct vecInt *pVecInt = NULL;

	pVecInt = (struct vecInt *)malloc(sizeof(struct vecInt));
	if (pVecInt == NULL)
	{
		fprintf(log, "malloc(): failed to allocate memory to float array - %s!!\n", type);
		exit(EXIT_FAILURE);
	}

	pVecInt->p = NULL;
	pVecInt->size = 0;

	return pVecInt;
}

struct vecFloat *createVecFloat(FILE *log, char *type)
{
	struct vecFloat *pVecFloat = NULL;

	pVecFloat = (struct vecFloat *)malloc(sizeof(struct vecFloat));
	if (pVecFloat == NULL)
	{
		fprintf(log, "malloc(): failed to allocate memory to float array - %s!!\n", type);
		exit(EXIT_FAILURE);
	}

	pVecFloat->pf = NULL;
	pVecFloat->size = 0;

	return pVecFloat;
}

void pushBackVecInt(struct vecInt *pVecInt, int iData, FILE *log, char *type)
{
	pVecInt->size += 1;
	pVecInt->p = (int *)realloc(pVecInt->p, pVecInt->size * sizeof(int));
	pVecInt->p[pVecInt->size - 1] = iData;

	fprintf(log, "%s, %d\n", type, pVecInt->p[pVecInt->size - 1]);
}

void pushBackVecFloat(struct vecFloat *pVecFloat, float fData, FILE *log, char *type)
{
	pVecFloat->size += 1;
	pVecFloat->pf = (float *)realloc(pVecFloat->pf, pVecFloat->size * sizeof(float));
	pVecFloat->pf[pVecFloat->size - 1] = fData;

	fprintf(log, "%s, %f\n", type, pVecFloat->pf[pVecFloat->size - 1]);
}

void showVecInt(struct vecInt *pVecInt, FILE *log, char type)
{
	int index = 0;

	while (index != pVecInt->size)
	{
		fprintf(log, "%c ", type);
		for (int i = 0; i < 3; i++)
			fprintf(log, "%d ", pVecInt->p[index + i]);

		fprintf(log, "\n");
		index += 3;
	}
}

void showVecFloat(struct vecFloat *pVecFloat, FILE *log, char type)
{
	int index = 0;

	while (index <= pVecFloat->size)
	{
		fprintf(log, "%c %f", type, pVecFloat->pf[index - 1]);
		index++;
		if (index % 3 == 0)
			fprintf(log, "\n");
	}
}
