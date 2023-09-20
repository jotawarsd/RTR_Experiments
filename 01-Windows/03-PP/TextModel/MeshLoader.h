#pragma once
#include <stdio.h>
#include <stdlib.h>

struct vecInt
{
	int *p;
	int size;
};

struct vecFloat
{
	float *pf;
	int size;
};

struct vecInt *createVecInt(FILE *, char *);
struct vecFloat *createVecFloat(FILE *, char *);
void pushBackVecInt(struct vecInt *, int, FILE *, char *);
void pushBackVecFloat(struct vecFloat *, float, FILE *, char *);
void showVecInt(struct vecInt *, FILE *, char);
void showVecFloat(struct vecFloat *pVecFloat, FILE *log, char type);
