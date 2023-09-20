//Header Files
#include <stdio.h>
#include <stdlib.h>
#include "OGLCU.h"
#include <C:/glew/include/GL/glew.h>	//this must be above gl.h

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#pragma comment(lib, "cudart.lib")

FILE *gpFile = NULL;

//sine wave variables
const unsigned int meshWidth = 8192;
const unsigned int meshHeight = 8192;

#define MYARRAYSIZE meshWidth * meshHeight * 4

float pos[meshWidth][meshHeight][4];

GLuint vbo_gpu;
cudaError_t cudaResult;

float animationTime = 0.0f;

//CUDA Kernel
__global__ void sineWaveKernel(float4 *position, unsigned int width, unsigned int height, float time)
{
	//code
	unsigned int i = (blockIdx.x * blockDim.x) + threadIdx.x;
	unsigned int j = (blockIdx.y * blockDim.y) + threadIdx.y;

	float u = (float)i / (float)width;
	float v = (float)j / (float)height;

	u = u * 2.0f - 1.0f;
	v = v * 2.0f - 1.0f;

	float frequency = 4.0f;
	float w = sinf((u * frequency) + time) * cosf((v * frequency) + time) * 0.5f;

	position[(j * width) + i] = make_float4(u, w, v, 1.0f);
}

void cudaInit(GLuint vbo, struct cudaGraphicsResource * graphicsResource)
{
	if (fopen_s(&gpFile, "cudaLog.txt", "w") == 0)
		fprintf(gpFile, "Log File for CUDA Created Successfully!\n");

	//CUDA Initialization
	int dev_count = 0;
	cudaResult = cudaGetDeviceCount(&dev_count);
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile, "Cuda device count failed!!\n");
		exit(EXIT_FAILURE);
	}
	else if (dev_count == 0)
	{
		fprintf(gpFile, "No CUDA Supported devices\n");
		exit(EXIT_FAILURE);
	}
	fprintf(gpFile, "Number of CUDA Supported devices: %d\n", dev_count);
	
	//select cuda supported device
	cudaSetDevice(0);	//selecting the default 0th cuda supported device

	//create cuda-opengl inter-operability resource
	cudaResult = cudaGraphicsGLRegisterBuffer(&graphicsResource, vbo, cudaGraphicsMapFlagsWriteDiscard);
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile, "cudaGraphicsGLRegisterBuffer() failed!!\n");
		exit(EXIT_FAILURE);
	}
}

void cudaSineWave(struct cudaGraphicsResource * graphicsResource, int width, int height, float time)
{
	//variable declarations
	float4 *pPos = NULL;
	size_t numBytes;
	//cuda code
	//map cuda graphics resource
	cudaResult = cudaGraphicsMapResources(1, &graphicsResource, 0);
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile, "cudaGraphicsMapResources() failed!!\n");
		exit(EXIT_FAILURE);
	}

	//ask cuda to give mapped pointer of mapped resource
	cudaResult = cudaGraphicsResourceGetMappedPointer((void **)&pPos, &numBytes, graphicsResource);
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile, "cudaGraphicsResourceGetMappedPointer() failed!!\n");
		exit(EXIT_FAILURE);
	}

	//call cuda sineWave Kernel
	dim3 block(8, 8, 1);
	dim3 grid(width, height, 1);

	sineWaveKernel <<<grid, block>>> (pPos, width, height, time);

	//unmap given mapped pointer to use on cpu
	cudaResult = cudaGraphicsUnmapResources(1, &graphicsResource, 0);
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile, "cudaGraphicsUnmapResources() failed!!\n");
		exit(EXIT_FAILURE);
	}
}
