#include "Pencil.h"
#include "../CommonGeometries.h"
#include "../Texture.h"

extern FILE* gpFile;

// wood
GLuint vertexArrayObject_Wood;
GLuint vertexBufferObject_Wood_Position;
GLuint vertexBufferObject_Wood_Normal;
GLuint vertexBufferObject_Wood_TexCoord;

// lead
GLuint vertexArrayObject_Lead;
GLuint vertexBufferObject_Lead_Position;
GLuint vertexBufferObject_Lead_Normal;
GLuint vertexBufferObject_Lead_TexCoord;

extern GLuint modelMatrixUniform_PCNT;
extern GLuint textureSamplerUniform_PCNT;
extern GLuint colorUniform_PCNT;
extern GLuint dimensionsUniform_PCNT;
extern GLuint textureEnabledUniform_PCNT;


// body
GLfloat pencilBodyDimensions[] = { 1.0f, 10.0f, 1.0f, 1.0f };
GLuint texture_pencil_name;

struct CylinderProperties woodProperties =
{
	30,
	7,
	2.0f,
	1.0f,
	pencilBodyDimensions[X] / 2.0f,
	0,
};

struct CylinderProperties leadProperties =
{
	30,
	7,
	1.0f,
	0.0f,
	pencilBodyDimensions[X] / 2.0f,
	0,
};

void initializePencil(void)
{
	// code
	initializeCylinder(
		&woodProperties,
		&vertexArrayObject_Wood,
		&vertexBufferObject_Wood_Position,
		&vertexBufferObject_Wood_Normal,
		&vertexBufferObject_Wood_TexCoord);

	initializeCylinder(
		&leadProperties,
		&vertexArrayObject_Lead,
		&vertexBufferObject_Lead_Position,
		&vertexBufferObject_Lead_Normal,
		&vertexBufferObject_Lead_TexCoord);

	LoadGLTexture(&texture_pencil_name, "Pencil_Name.png", gpFile);
}

void displayPencil(vmath::mat4 modelMatrix, BOOL showLead)
{
	// function declarations
	void drawTop(vmath::mat4 modelMatrix);
	void drawFerrule(vmath::mat4 modelMatrix);
	void drawBody(vmath::mat4 modelMatrix);
	void drawWood(vmath::mat4 modelMatrix);
	void drawLead(vmath::mat4 modelMatrix);

	// code
	drawBody(modelMatrix);
	drawFerrule(modelMatrix);
	drawTop(modelMatrix);
	drawWood(modelMatrix);
	if (showLead)
	{
		drawLead(modelMatrix);
	}
}

void drawBody(vmath::mat4 modelMatrix)
{
	// function declaration
	void drawRectangle(vmath::mat4 modelMatrix, vmath::vec3 translation, GLfloat rotationAngle, Color color);
	void drawEyes(vmath::mat4 modelMatrix);
	void drawPencilMouth(vmath::mat4 modelMatrix);

	// code
	glUniform4f(dimensionsUniform_PCNT, pencilBodyDimensions[X], pencilBodyDimensions[Y], 1.0f, 1.0f);

	// front
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_pencil_name);
	glUniform1i(textureEnabledUniform_PCNT, 1);
	glUniform1i(textureSamplerUniform_PCNT, /* v0 */ 0);
	Color color = BLACK;
	vmath::vec3 translation = vmath::vec3(0.0f, 0.0f, 0.0f);
	GLfloat rotation = 0.0f;
	drawRectangle(modelMatrix, translation, rotation, color);
	glUniform1i(textureEnabledUniform_PCNT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	drawEyes(modelMatrix);
	drawPencilMouth(modelMatrix);

	// front right
	color = RED;
	translation = vmath::vec3(1.5f * pencilBodyDimensions[X], 0.0f, -0.866025f * pencilBodyDimensions[X]);
	rotation = 60.0f;
	drawRectangle(modelMatrix, translation, rotation, color);
	
	// rear right
	color = RED;
	translation = vmath::vec3(1.5f * pencilBodyDimensions[X], 0.0f, -2.598076f * pencilBodyDimensions[X]);
	rotation = 120.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	// rear
	color = BLACK;
	translation = vmath::vec3(0.0f, 0.0f, -3.464101f * pencilBodyDimensions[X]);
	rotation = 180.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	// rear left
	color = RED;
	translation = vmath::vec3(-1.5f * pencilBodyDimensions[X], 0.0f, -2.598076f * pencilBodyDimensions[X]);
	rotation = 240.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	// front left
	color = RED;
	translation = vmath::vec3(-1.5 * pencilBodyDimensions[X], 0.0f, -0.866025f * pencilBodyDimensions[X]);
	rotation = 300.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	glUniform4f(dimensionsUniform_PCNT, 1.0f, 1.0f, 1.0f, 1.0f);
}

void drawEyes(vmath::mat4 modelMatrix)
{
	// Bind with vertex array object
	// left
	vmath::mat4 translationMatrix = vmath::translate(0.0f, pencilBodyDimensions[Y] - 1.0f, 0.01f);
	modelMatrix *= translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform_PCNT, /* count */ 1, /* transpose */ GL_FALSE, modelMatrix);

	displayEyes(modelMatrix, 0.5f, 0.375);

	glUniform4f(dimensionsUniform_PCNT, 1.0f, 1.0f, 1.0f, 1.0f);
}

void drawPencilMouth(vmath::mat4 modelMatrix)
{
	// Bind with vertex array object
	vmath::mat4 translationMatrix = vmath::translate(0.0f, pencilBodyDimensions[Y] - 2.0f, 0.01f);
	vmath::mat4 tempModelMatrix = modelMatrix * translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform_PCNT, /* count */ 1, /* transpose */ GL_FALSE, tempModelMatrix);

	displayMouth(modelMatrix, 0.375f);

	glUniform4f(dimensionsUniform_PCNT, pencilBodyDimensions[X], pencilBodyDimensions[Y], 1.0f, 1.0f);
}

void drawFerrule(vmath::mat4 modelMatrix)
{
	// function declaration
	void drawRectangle(vmath::mat4 modelMatrix, vmath::vec3 translation, GLfloat rotationAngle, Color color);

	// code
	GLfloat ferruleHeight = 0.2f;
	glUniform4f(dimensionsUniform_PCNT, pencilBodyDimensions[X], ferruleHeight, 1.0f, 1.0f);
	struct Color ferruleColor = { 0.752941f, 0.752941f, 0.752941f, 1.0f };
	GLfloat y = pencilBodyDimensions[Y] + ferruleHeight;

	// front
	Color color = { 0.752941f, 0.752941f, 0.752941f, 1.0f };
	vmath::vec3 translation = vmath::vec3(0.0f, y, 0.0f);
	GLfloat rotation = 0.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	// front right
	translation = vmath::vec3(1.5f * pencilBodyDimensions[X], y, -0.866025f * pencilBodyDimensions[X]);
	rotation = 60.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	// rear right
	translation = vmath::vec3(1.5f * pencilBodyDimensions[X], y, -2.598076f * pencilBodyDimensions[X]);
	rotation = 120.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	// rear
	translation = vmath::vec3(0.0f, y, -3.464101f * pencilBodyDimensions[X]);
	rotation = 180.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	// rear left
	translation = vmath::vec3(-1.5f * pencilBodyDimensions[X], y, -2.598076f * pencilBodyDimensions[X]);
	rotation = 240.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	// front left
	translation = vmath::vec3(-1.5f * pencilBodyDimensions[X], y, -0.866025f * pencilBodyDimensions[X]);
	rotation = 300.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	glUniform4f(dimensionsUniform_PCNT, 1.0f, 1.0f, 1.0f, 1.0f);
}

void drawTop(vmath::mat4 modelMatrix)
{
	// function declaration
	void drawRectangle(vmath::mat4 modelMatrix, vmath::vec3 translation, GLfloat rotationAngle, Color color);

	// code
	GLfloat topHeight = 1.0f;
	GLfloat ferruleHeight = 0.4f;
	GLfloat y = pencilBodyDimensions[Y] + ferruleHeight + topHeight;
	glUniform4f(dimensionsUniform_PCNT, pencilBodyDimensions[X], topHeight, 1.0f, 1.0f);

	// front
	Color color = BLACK;
	vmath::vec3 translation = vmath::vec3(0.0f, y, 0.0f);
	GLfloat rotation = 0.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	// front right
	translation = vmath::vec3(1.5f * pencilBodyDimensions[X], y, -0.866025f * pencilBodyDimensions[X]);
	rotation = 60.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	// rear right
	translation = vmath::vec3(1.5f * pencilBodyDimensions[X], y, -2.598076f * pencilBodyDimensions[X]);
	rotation = 120.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	// rear
	translation = vmath::vec3(0.0f, y, -3.464101f * pencilBodyDimensions[X]);
	rotation = 180.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	// rear left
	translation = vmath::vec3(-1.5f * pencilBodyDimensions[X], y, -2.598076f * pencilBodyDimensions[X]);
	rotation = 240.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	// front left
	translation = vmath::vec3(-1.5f * pencilBodyDimensions[X], y, -0.866025f * pencilBodyDimensions[X]);
	rotation = 300.0f;
	drawRectangle(modelMatrix, translation, rotation, color);

	// top
	translation = vmath::vec3(0.0f, y + topHeight, -1.732050f);
	vmath::mat4 translationMatrix = vmath::translate(translation);
	vmath::mat4 rotationMatrix = vmath::rotate(270.0f, vmath::vec3(1.0f, 0.0f, 0.0f));
	vmath::mat4 tempModelMatrix = modelMatrix * translationMatrix * rotationMatrix;

	// Send above transformation matrix/matrices to the shader in respective matrix uniforms
	glUniformMatrix4fv(modelMatrixUniform_PCNT, /* count */ 1, /* transpose */ GL_FALSE, tempModelMatrix);

	displayHexagon();

	glUniform4f(dimensionsUniform_PCNT, 1.0f, 1.0f, 1.0f, 1.0f);
}

void drawRectangle(vmath::mat4 modelMatrix, vmath::vec3 translation, GLfloat rotationAngle, Color color)
{
	// code
	// Do translation, rotation, scale transformations using vmath
	vmath::mat4 translationMatrix = vmath::translate(translation);
	vmath::mat4 rotationMatrix = vmath::rotate(rotationAngle, vmath::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix *= translationMatrix * rotationMatrix;

	// Do necessary matrix multiplications

	// Send above transformation matrix/matrices to the shader in respective matrix uniforms
	glUniformMatrix4fv(modelMatrixUniform_PCNT, /* count */ 1, /* transpose */ GL_FALSE, modelMatrix);
	glUniform4f(colorUniform_PCNT, color.r, color.g, color.b, color.a);

	displayRectangle();
}

void drawWood(vmath::mat4 modelMatrix)
{
	// code
	glUniform4f(dimensionsUniform_PCNT, pencilBodyDimensions[X], 4.0f, pencilBodyDimensions[X], 1.0f);

	// Do translation, rotation, scale transformations using vmath
	vmath::mat4 translationMatrix = vmath::translate(0.0f, -1.0f * (pencilBodyDimensions[Y] + woodProperties.height * 2.0f), -1.732050f * pencilBodyDimensions[X]);
	vmath::mat4 rotationMatrix = vmath::rotate(180.0f, vmath::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix *= translationMatrix * rotationMatrix;

	// Send above transformation matrix/matrices to the shader in respective matrix uniforms
	glUniformMatrix4fv(modelMatrixUniform_PCNT, /* count */ 1, /* transpose */ GL_FALSE, modelMatrix);
	Color woodColor = { 0.788235f, 0.611704f, 0.450980f, 1.0f };
	glUniform4f(colorUniform_PCNT, woodColor.r, woodColor.g, woodColor.b, woodColor.a);

	displayCylinder(vertexArrayObject_Wood, woodProperties.vertexCount);

	glUniform4f(dimensionsUniform_PCNT, 1.0f, 1.0f, 1.0f, 1.0f);
}

void drawLead(vmath::mat4 modelMatrix)
{
	// code
	glUniform4f(dimensionsUniform_PCNT, pencilBodyDimensions[X], 4.0f, pencilBodyDimensions[X], 1.0f);

	// Do translation, rotation, scale transformations using vmath
	vmath::mat4 translationMatrix = vmath::translate(
		0.0f, 
		-1.0f * (pencilBodyDimensions[Y] + (woodProperties.height * 4.0f) + (leadProperties.height * 2.0f)), 
		-1.732050f * pencilBodyDimensions[X]);
	vmath::mat4 rotationMatrix = vmath::rotate(180.0f, vmath::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix *= translationMatrix * rotationMatrix;

	// Send above transformation matrix/matrices to the shader in respective matrix uniforms
	glUniformMatrix4fv(modelMatrixUniform_PCNT, /* count */ 1, /* transpose */ GL_FALSE, modelMatrix);
	Color leadColor = { 0.360784f, 0.384313f, 0.454901f, 1.0f };
	glUniform4f(colorUniform_PCNT, leadColor.r, leadColor.g, leadColor.b, leadColor.a);

	displayCylinder(vertexArrayObject_Lead, leadProperties.vertexCount);
}
