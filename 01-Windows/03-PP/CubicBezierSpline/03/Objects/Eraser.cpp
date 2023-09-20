#include "Eraser.h"
#include "../CommonGeometries.h"

extern GLuint modelMatrixUniform_PCNT;
extern GLuint colorUniform_PCNT;
extern GLuint dimensionsUniform_PCNT;

GLfloat bodyDimensions[] = { 2.0f, 2.0f, 1.0f, 1.0f };

void displayEraser(vmath::mat4 modelMatrix)
{
	// function declarations
	void drawBody(vmath::mat4 modelMatrix, vmath::vec3 translation, Color color);
	void drawEraserEyes(vmath::mat4 modelMatrix);
	void drawEraserMouth(vmath::mat4 modelMatrix);

	// code
	// lower body
	drawBody(modelMatrix, vmath::vec3(0.0f, 0.0f, 0.0f), RED);
	// upper body
	drawBody(modelMatrix, vmath::vec3(0.0f, bodyDimensions[Y] * 2.0f, 0.0f), BLUE);
	drawEraserEyes(modelMatrix);
	drawEraserMouth(modelMatrix);
}

void drawBody(vmath::mat4 modelMatrix, vmath::vec3 translation, Color color)
{
	// function declaration
	void drawCube(vmath::mat4 modelMatrix, vmath::vec3 translation, Color color);

	// code
	glUniform4f(dimensionsUniform_PCNT, bodyDimensions[X], bodyDimensions[Y], bodyDimensions[Z], 1.0f);

	// Do translation, rotation, scale transformations using vmath
	vmath::mat4 translationMatrix = vmath::translate(translation);
	modelMatrix *= translationMatrix;

	// Do necessary matrix multiplications

	// Send above transformation matrix/matrices to the shader in respective matrix uniforms
	glUniformMatrix4fv(modelMatrixUniform_PCNT, /* count */ 1, /* transpose */ GL_FALSE, modelMatrix);
	glUniform4f(colorUniform_PCNT, color.r, color.g, color.b, color.a);

	displayCube();

	glUniform4f(dimensionsUniform_PCNT, 1.0f, 1.0f, 1.0f, 1.0f);
}

void drawEraserEyes(vmath::mat4 modelMatrix)
{
	// code
	vmath::mat4 translationMatrix = vmath::translate(
		0.0f, 
		(bodyDimensions[Y] * 3.0f) - 1.0f, 
		bodyDimensions[Z] + 0.01f
	);

	modelMatrix *= translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform_PCNT, /* count */ 1, /* transpose */ GL_FALSE, modelMatrix);

	displayEyes(modelMatrix, 1.0f, 0.5f);

	glUniform4f(dimensionsUniform_PCNT, 1.0f, 1.0f, 1.0f, 1.0f);
}

void drawEraserMouth(vmath::mat4 modelMatrix)
{
	// code
	vmath::mat4 translationMatrix = vmath::translate(
		0.0f, 
		(bodyDimensions[Y] * 3.0f) - 2.5f, 
		bodyDimensions[Z] + 0.01f
	);

	modelMatrix *= translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform_PCNT, /* count */ 1, /* transpose */ GL_FALSE, modelMatrix);

	displayMouth(modelMatrix, 0.5f);

	glUniform4f(dimensionsUniform_PCNT, 1.0f, 1.0f, 1.0f, 1.0f);
}
