#pragma once
//Header Files
#include <stdio.h>
#include <stdlib.h>
#include <C:/glew/include/GL/glew.h>	//this must be above gl.h

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#pragma comment(lib, "cudart.lib")

extern "C" {void cudaInit(GLuint, struct cudaGraphicsResource *); }

extern "C" {void cudaSineWave(struct cudaGraphicsResource *, int, int, float); }
