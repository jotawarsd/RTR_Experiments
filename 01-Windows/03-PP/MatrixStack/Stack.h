#pragma once
#include <stdio.h>
#include "vmath.h"
using namespace vmath;

#define MAX_STACK_SIZE 32
#define STACK_OVERFLOW 1
#define STACK_UNDERFLOW -1
#define STACK_OPERATION_SUCCESS 1

int push(mat4);
int pop(mat4 *);
