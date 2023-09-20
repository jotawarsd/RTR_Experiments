#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

//OpenGL Header files
#include <C:/glew/include/GL/glew.h>
#include <GL/gl.h>
#include "OGL.h"
#include "vmath.h"
using namespace vmath;

//global variables
FILE *gpFile = NULL;
FILE *controlPointsFile = NULL;

//Programmable pipeline related global variables
GLuint shaderProgramObject_BezierCurve;
GLuint shaderProgramObject_BezierPoints;
mat4 perspectiveProjectionMatrix;

GLuint vao_bezierPoints;
GLuint vbo_bezierPoints;

enum
{
	SDJ_ATTRIBUTE_POSITION = 0,
	SDJ_ATTRIBUTE_COLOR,
	SDJ_ATTRIBUTE_NORMAL,
	SDJ_ATTRIBUTE_TEXTURE0
};
//uniforms
GLuint mvpMatrixUniform_bc;
GLuint mvpMatrixUniform_bp;
GLuint togglePointUniform;
GLuint numSegmentsUniform;
GLuint numSplinesUniform;
GLuint colorUniform;
GLuint pointsUniform;

const int numSplines = 3;
const int numControlPoints = 1 + (3 * numSplines);
int numSegments = 40;
unsigned int pointShiftIndex = 0;

vec3 controlPoints[numControlPoints] = {
	vec3(-1.5f, 0.0f, 0.0f),
	vec3(-1.75f, 1.0f, 0.0f),
	vec3(-0.25f, 1.0f, 0.0f),
	vec3(-0.5f, 0.0f, 0.0f),
	vec3(-0.75f, -1.0f, 0.0f),
	vec3(0.75f, -1.0f, 0.0f),
	vec3(0.5f, 0.0f, 0.0f),
	vec3(0.25f, 1.0f, 0.0f),
	vec3(1.75f, 1.0f, 0.0f),
	vec3(1.5f, 0.0f, 0.0f),
};

//Function Declarations
void uninitialize(void);

//Bezier Functions
void BezierCurveShaderInitialize(void)
{
	//Vertex Shader
	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"uniform mat4 u_mvpMatrix;" \
		"uniform vec4 color;" \
		"uniform int u_numSegments;" \
		"const int u_numSplines = 3;" \
		"const int numControlPoints = (u_numSplines * 3) + 1;" \
		"uniform vec3 points[numControlPoints];" \
		"out vec4 a_color_out;" \

		"vec3 cubicBezierCurve(vec3 CP[4], float t)" \
		"{" \
		"vec4 A = mix(vec4(CP[0], 1.0f), vec4(CP[1], 1.0f), t);" \
		"vec4 B = mix(vec4(CP[1], 1.0f), vec4(CP[2], 1.0f), t);" \
		"vec4 C = mix(vec4(CP[2], 1.0f), vec4(CP[3], 1.0f), t);" \

		"vec4 P = mix(mix(A, B, t), mix(B, C, t), t);" \
		"return P.xyz;" \
		"}" \

		"vec3 cubicBSpline(vec3 CP[numControlPoints], const int n, float t)" \
		"{" \
		"int i = int(floor(t));" \
		"float s = fract(t);" \

		"if (t >= 3.0)" \
		"return CP[n - 1];" \

		"if (t <= 0.0)" \
		"return CP[0];" \

		"vec3 A = CP[i * 3];" \
		"vec3 B = CP[i * 3 + 1];" \
		"vec3 C = CP[i * 3 + 2];" \
		"vec3 D = CP[i * 3 + 3];" \

		"vec3 segmentControlPoints[4] = {A, B, C, D};" \
		"return cubicBezierCurve(segmentControlPoints, s);" \
		"}" \

		"void main(void)" \
		"{" \
		"float t = float(gl_VertexID) / float(u_numSegments - 1);" \
		"vec3 p = cubicBSpline(points, numControlPoints, t);" \
		"gl_Position = u_mvpMatrix * vec4(p, 1.0f);" \
		"a_color_out = color;" \
		"}";

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
	glCompileShader(vertexShaderObject);

	GLint status;
	GLint infoLogLength;
	char *log = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char *)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "\n\nVertex Shader Compilation Log : %s\n\n", log);
				free(log);
				log = NULL;
				uninitialize();
			}
		}
	}

	//Fragment Shader
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 a_color_out;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = a_color_out;" \
		"}";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "\n\nFragment Shader Compilation Log : %s", log);
				free(log);
				log = NULL;
				uninitialize();
			}
		}
	}

	//Shader Program object
	shaderProgramObject_BezierCurve = glCreateProgram();
	glAttachShader(shaderProgramObject_BezierCurve, vertexShaderObject);
	glAttachShader(shaderProgramObject_BezierCurve, fragmentShaderObject);

	glLinkProgram(shaderProgramObject_BezierCurve);

	//ERROR Checking
	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetProgramiv(shaderProgramObject_BezierCurve, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_BezierCurve, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_BezierCurve, infoLogLength, &written, log);
				fprintf(gpFile, "\n\nShader Program Link log : %s", log);
				free(log);
				log = NULL;
				uninitialize();
			}
		}
	}

	//post link steps
	mvpMatrixUniform_bc = glGetUniformLocation(shaderProgramObject_BezierCurve, "u_mvpMatrix");
	numSegmentsUniform = glGetUniformLocation(shaderProgramObject_BezierCurve, "u_numSegments");
	colorUniform = glGetUniformLocation(shaderProgramObject_BezierCurve, "color");
	pointsUniform = glGetUniformLocation(shaderProgramObject_BezierCurve, "points");
}

void BezierPointsShaderInitialize(void)
{
	//Vertex Shader
	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"uniform mat4 u_mvpMatrix;" \
		"uniform int u_togglePoint;" \
		"flat out int togglePoint;" \
		"void main(void)" \
		"{" \
			"togglePoint = 0;" \
			"if (u_togglePoint == gl_VertexID)" \
			"{ togglePoint = 1; }" \
			"gl_Position = u_mvpMatrix * a_position;" \
		"}";

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
	glCompileShader(vertexShaderObject);

	GLint status;
	GLint infoLogLength;
	char *log = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char *)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "\n\nVertex Shader Compilation Log : %s\n\n", log);
				free(log);
				log = NULL;
				uninitialize();
			}
		}
	}

	//Fragment Shader
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"flat in int togglePoint;" \
		"uniform vec4 color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"if (togglePoint == 1)" \
		"{ FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f); }" \
		"else" \
		"{ FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f); }" \
		"}";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "\n\nFragment Shader Compilation Log : %s", log);
				free(log);
				log = NULL;
				uninitialize();
			}
		}
	}

	//Shader Program object
	shaderProgramObject_BezierPoints = glCreateProgram();
	glAttachShader(shaderProgramObject_BezierPoints, vertexShaderObject);
	glAttachShader(shaderProgramObject_BezierPoints, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject_BezierPoints, SDJ_ATTRIBUTE_POSITION, "a_position");

	glLinkProgram(shaderProgramObject_BezierPoints);

	//ERROR Checking
	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetProgramiv(shaderProgramObject_BezierPoints, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_BezierPoints, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_BezierPoints, infoLogLength, &written, log);
				fprintf(gpFile, "\n\nShader Program Link log : %s", log);
				free(log);
				log = NULL;
				uninitialize();
			}
		}
	}

	//post link steps
	mvpMatrixUniform_bp = glGetUniformLocation(shaderProgramObject_BezierPoints, "u_mvpMatrix");
	togglePointUniform = glGetUniformLocation(shaderProgramObject_BezierPoints, "u_togglePoint");

	//Vertex Data Arrays for bezier points
	//vertex array object
	glGenVertexArrays(1, &vao_bezierPoints);
	glBindVertexArray(vao_bezierPoints);

	//vertex buffer object for ___position___
	glGenBuffers(1, &vbo_bezierPoints);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_bezierPoints);

	glBufferData(GL_ARRAY_BUFFER, sizeof(controlPoints), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(SDJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SDJ_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void BezierCurveDraw(void)
{
	//use shader program object
	glUseProgram(shaderProgramObject_BezierCurve);

	//transformations
	mat4 translationMatrix = mat4::identity();
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	translationMatrix = translate(0.0f, -1.0f, -6.0f);
	modelViewMatrix = translationMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMatrixUniform_bc, 1, GL_FALSE, modelViewProjectionMatrix);

	glUniform4f(colorUniform, 1.0f, 0.0f, 0.0f, 1.0f);
	glUniform1i(numSegmentsUniform, numSegments);
	glUniform1i(numSplinesUniform, numSplines);
	glUniform3fv(pointsUniform, numControlPoints, &(controlPoints[0][0]));

	glDrawArrays(GL_LINE_STRIP, 0, numSegments * numSplines);

	//unuse the shader program object
	glUseProgram(0);
}

void BezierPointsDraw(void)
{
	//use shader program object
	glUseProgram(shaderProgramObject_BezierPoints);

	//transformations
	mat4 translationMatrix = mat4::identity();
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	translationMatrix = translate(0.0f, -1.0f, -6.0f);
	modelViewMatrix = translationMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMatrixUniform_bp, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1i(togglePointUniform, pointShiftIndex);

	glBindVertexArray(vao_bezierPoints);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_bezierPoints);
	glBufferData(GL_ARRAY_BUFFER, sizeof(controlPoints), controlPoints, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPointSize(3.0f);

	glDrawArrays(GL_POINTS, 0, numControlPoints);

	glBindVertexArray(0);

	//unuse the shader program object
	glUseProgram(0);
}

void BezierUninitialize(void)
{
	//shader uninitialization
	const GLuint shaderProgramObjects[] =
	{
		shaderProgramObject_BezierCurve,
		shaderProgramObject_BezierPoints,
	};

	for (int i = 0; i < sizeof(shaderProgramObjects); i++)
	{
		GLuint spo = shaderProgramObjects[i];
		if (spo)
		{
			glUseProgram(spo);
			GLsizei numAttachedShaders;
			glGetProgramiv(spo, GL_ATTACHED_SHADERS, &numAttachedShaders);
			GLuint *shaderObjects = NULL;
			shaderObjects = (GLuint *)malloc(numAttachedShaders * sizeof(GLuint));

			//filling empty buffer with shader objects
			glGetAttachedShaders(spo, numAttachedShaders, &numAttachedShaders, shaderObjects);

			for (GLsizei i = 0; i < numAttachedShaders; i++)
			{
				glDetachShader(spo, shaderObjects[i]);
				glDeleteShader(shaderObjects[i]);
				shaderObjects[i] = 0;
			}

			free(shaderObjects);
			shaderObjects = NULL;
			glUseProgram(0);
			glDeleteProgram(spo);
			spo = 0;
		}
	}
}

