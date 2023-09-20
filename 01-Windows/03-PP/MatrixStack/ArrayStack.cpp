#include <stdio.h>
#include "vmath.h"
#include "Stack.h"
using namespace vmath;

#define MAX_STACK_SIZE 32
#define STACK_OVERFLOW 1
#define STACK_UNDERFLOW -1
#define STACK_OPERATION_SUCCESS 1

static mat4 stack[MAX_STACK_SIZE];
static int topPointer = -1;

int push(mat4 value)
{
	//code
	topPointer++;
	if (topPointer < 32)
	{
		stack[topPointer] = value;
		return STACK_OPERATION_SUCCESS;
	}
	
}

int pop(mat4 *pValue)
{
	//code
	topPointer--;
	if (topPointer == -1)
		return STACK_UNDERFLOW;

	pValue = &stack[topPointer - 1];	
	topPointer--;
	return STACK_OPERATION_SUCCESS;
}
