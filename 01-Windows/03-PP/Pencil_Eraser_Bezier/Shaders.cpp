#include "Shaders.h"

extern FILE *gpFile;

// programmable pipeline related global variables

// PC Shader
GLuint shaderProgramObject_PC;

GLuint modelMatrixUniform_PC;
GLuint viewMatrixUniform_PC;
GLuint projectionMatrixUniform_PC;

GLuint colorUniform_PC;
GLuint dimensionsUniform_PC;

// PCT Shader
GLuint shaderProgramObject_PCT;

GLuint modelMatrixUniform_PCT;
GLuint viewMatrixUniform_PCT;
GLuint projectionMatrixUniform_PCT;

GLuint textureSamplerUniform_PCT;
GLuint colorUniform_PCT;
GLuint dimensionsUniform_PCT;
GLuint textureEnabledUniform_PCT;

// PCN Shader
GLuint shaderProgramObject_PCN;

GLuint laUniform_PCN;
GLuint ldUniform_PCN;
GLuint lsUniform_PCN;
GLuint lightPositionUniform_PCN;

GLuint kaUniform_PCN;
GLuint kdUniform_PCN;
GLuint ksUniform_PCN;
GLuint materialShininessUniform_PCN;

GLuint lightingEnabledUniform_PCN;

GLuint modelMatrixUniform_PCN;
GLuint viewMatrixUniform_PCN;
GLuint projectionMatrixUniform_PCN;

GLuint colorUniform_PCN;
GLuint dimensionsUniform_PCN;

// PCNT Shader
GLuint shaderProgramObject_PCNT;

GLuint laUniform_PCNT;
GLuint ldUniform_PCNT;
GLuint lsUniform_PCNT;
GLuint lightPositionUniform_PCNT;

GLuint kaUniform_PCNT;
GLuint kdUniform_PCNT;
GLuint ksUniform_PCNT;
GLuint materialShininessUniform_PCNT;

GLuint lightingEnabledUniform_PCNT;

GLuint modelMatrixUniform_PCNT;
GLuint viewMatrixUniform_PCNT;
GLuint projectionMatrixUniform_PCNT;
GLuint textureSamplerUniform_PCNT;
GLuint colorUniform_PCNT;
GLuint dimensionsUniform_PCNT;
GLuint textureEnabledUniform_PCNT;

extern vmath::mat4 perspectiveProjectionMatrix;

int initializeShaders(void)
{
	// function declarations
	int initializePCShader(void);
	int initializePCTShader(void);
	int initializePCNTShader(void);

	int returnValue = 0;
	if (returnValue = initializePCShader() != 0)
	{
		return returnValue;
	}

	if (returnValue = initializePCTShader() != 0)
	{
		return returnValue;
	}

	if (returnValue = initializePCNTShader() != 0)
	{
		return returnValue;
	}
	return returnValue;
}

int initializePCShader(void)
{
	// code
	// vertex shader code
	const GLchar *vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform vec4 u_dimensions;" \
		"void main(void)" \
		"{" \
		"	vec4 position = a_position * u_dimensions;" \
		"	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * position;" \
		"}";

	// creating shader object
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// giving shader code to shader object
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	// compile the shader
	glCompileShader(vertexShaderObject);

	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	// getting compilation status
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	// do error checking
	if (status == GL_FALSE)
	{
		// Getting length of log of compilation status
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			// Allocate enough memory to the buffer to hold the compilation log
			log = (char*)malloc(infoLogLength);
			if (log == NULL)
			{
				fprintf(gpFile, "PC - Unable to get compilation log for vertex shader.\n");
			}
			else
			{
				GLsizei written;

				// Get the compilation log into allocated buffer
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);

				// Display the contents of buffer (log)
				fprintf(gpFile, "PC - Vertex shader compilation log : %s\n", log);

				// Free the allocated buffer
				free(log);
				log = NULL;
			}
		}
		return UNINITIALIZE;
	}

	// fragment shader code
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"uniform vec4 u_color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"	FragColor = u_color;" \
		"}";

	// creating shader object
	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// giving shader code to shader object
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	// compile the shader
	glCompileShader(fragmentShaderObject);

	// reinitialize
	status = 0;
	infoLogLength = 0;
	log = NULL;

	// getting compilation status
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	// do error checking
	if (status == GL_FALSE)
	{
		// Getting length of log of compilation status
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			// Allocate enough memory to the buffer to hold the compilation log
			log = (char*)malloc(infoLogLength);
			if (log == NULL)
			{
				fprintf(gpFile, "PC - Unable to get compilation log for fragment shader.\n");
			}
			else
			{
				GLsizei written;
				// Get the compilation log into allocated buffer
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);

				// Display the contents of buffer (log)
				fprintf(gpFile, "PC - Fragment shader compilation log : %s", log);

				// Free the allocated buffer
				free(log);
				log = NULL;
			}
		}
		return UNINITIALIZE;
	}

	// Create shader program object
	shaderProgramObject_PC = glCreateProgram();

	// Attach desired shaders to this shader program object
	glAttachShader(shaderProgramObject_PC, vertexShaderObject);
	glAttachShader(shaderProgramObject_PC, fragmentShaderObject);

	// Prelinked binding of shader program object with vertex attributes
	glBindAttribLocation(shaderProgramObject_PC, RRI_ATTRIBUTE_POSITION, "a_position");

	// Link shader program object
	glLinkProgram(shaderProgramObject_PC);

	// reinitialize status and other associated variables
	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetProgramiv(shaderProgramObject_PC, GL_LINK_STATUS, &status);
	// do error checking
	if (status == GL_FALSE)
	{
		// Getting length of log of compilation status
		glGetProgramiv(shaderProgramObject_PC, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			// Allocate enough memory to the buffer to hold the compilation log
			log = (char*)malloc(infoLogLength);
			if (log == NULL)
			{
				fprintf(gpFile, "PC - Unable to get compilation log for fragment shader.\n");
			}
			else
			{
				GLsizei written;
				// Get the compilation log into allocated buffer
				glGetProgramInfoLog(shaderProgramObject_PC, infoLogLength, &written, log);

				// Display the contents of buffer (log)
				fprintf(gpFile, "PC - Shader program link log : %s", log);
				free(log);
				log = NULL;
			}
		}
		return UNINITIALIZE;
	}

	// Postlinked retrieval of uniform location from the shader program object
	modelMatrixUniform_PC = glGetUniformLocation(shaderProgramObject_PC, "u_modelMatrix");
	viewMatrixUniform_PC = glGetUniformLocation(shaderProgramObject_PC, "u_viewMatrix");
	projectionMatrixUniform_PC = glGetUniformLocation(shaderProgramObject_PC, "u_projectionMatrix");
	colorUniform_PC = glGetUniformLocation(shaderProgramObject_PC, "u_color");
	dimensionsUniform_PC = glGetUniformLocation(shaderProgramObject_PC, "u_dimensions");

	return 0;
}

int initializePCTShader(void)
{
	// code
	// vertex shader code
	const GLchar *vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"in vec2 a_texcoords;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform vec4 u_dimensions;" \
		"out vec2 o_texcoords;" \
		"void main(void)" \
		"{" \
		"	vec4 position = a_position * u_dimensions;" \
		"	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * position;" \
		"	o_texcoords = a_texcoords;" \
		"}";

	// creating shader object
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// giving shader code to shader object
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	// compile the shader
	glCompileShader(vertexShaderObject);

	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	// getting compilation status
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	// do error checking
	if (status == GL_FALSE)
	{
		// Getting length of log of compilation status
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			// Allocate enough memory to the buffer to hold the compilation log
			log = (char*)malloc(infoLogLength);
			if (log == NULL)
			{
				fprintf(gpFile, "PCT - Unable to get compilation log for vertex shader.\n");
			}
			else
			{
				GLsizei written;

				// Get the compilation log into allocated buffer
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);

				// Display the contents of buffer (log)
				fprintf(gpFile, "PCT - Vertex shader compilation log : %s\n", log);

				// Free the allocated buffer
				free(log);
				log = NULL;
			}
		}
		return UNINITIALIZE;
	}

	// fragment shader code
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec2 o_texcoords;" \
		"uniform vec4 u_color;" \
		"uniform sampler2D u_textureSampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"	vec3 tex = vec3(texture(u_textureSampler, o_texcoords));" \
		"	FragColor = vec4(tex * vec3(u_color), 1.0);" \
		"}";

	// creating shader object
	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// giving shader code to shader object
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	// compile the shader
	glCompileShader(fragmentShaderObject);

	// reinitialize
	status = 0;
	infoLogLength = 0;
	log = NULL;

	// getting compilation status
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	// do error checking
	if (status == GL_FALSE)
	{
		// Getting length of log of compilation status
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			// Allocate enough memory to the buffer to hold the compilation log
			log = (char*)malloc(infoLogLength);
			if (log == NULL)
			{
				fprintf(gpFile, "PCT - Unable to get compilation log for fragment shader.\n");
			}
			else
			{
				GLsizei written;
				// Get the compilation log into allocated buffer
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);

				// Display the contents of buffer (log)
				fprintf(gpFile, "PCT - Fragment shader compilation log : %s", log);

				// Free the allocated buffer
				free(log);
				log = NULL;
			}
		}
		return UNINITIALIZE;
	}

	// Create shader program object
	shaderProgramObject_PCT = glCreateProgram();

	// Attach desired shaders to this shader program object
	glAttachShader(shaderProgramObject_PCT, vertexShaderObject);
	glAttachShader(shaderProgramObject_PCT, fragmentShaderObject);

	// Prelinked binding of shader program object with vertex attributes
	glBindAttribLocation(shaderProgramObject_PCT, RRI_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(shaderProgramObject_PCT, RRI_ATTRIBUTE_TEXTURE0, "a_texcoords");

	// Link shader program object
	glLinkProgram(shaderProgramObject_PCT);

	// reinitialize status and other associated variables
	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetProgramiv(shaderProgramObject_PCT, GL_LINK_STATUS, &status);
	// do error checking
	if (status == GL_FALSE)
	{
		// Getting length of log of compilation status
		glGetProgramiv(shaderProgramObject_PCT, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			// Allocate enough memory to the buffer to hold the compilation log
			log = (char*)malloc(infoLogLength);
			if (log == NULL)
			{
				fprintf(gpFile, "PCT - Unable to get compilation log for fragment shader.\n");
			}
			else
			{
				GLsizei written;
				// Get the compilation log into allocated buffer
				glGetProgramInfoLog(shaderProgramObject_PCT, infoLogLength, &written, log);

				// Display the contents of buffer (log)
				fprintf(gpFile, "PCT - Shader program link log : %s", log);
				free(log);
				log = NULL;
			}
		}
		return UNINITIALIZE;
	}

	// Postlinked retrieval of uniform location from the shader program object
	modelMatrixUniform_PCT = glGetUniformLocation(shaderProgramObject_PCT, "u_modelMatrix");
	viewMatrixUniform_PCT = glGetUniformLocation(shaderProgramObject_PCT, "u_viewMatrix");
	projectionMatrixUniform_PCT = glGetUniformLocation(shaderProgramObject_PCT, "u_projectionMatrix");
	colorUniform_PCT = glGetUniformLocation(shaderProgramObject_PCT, "u_color");
	dimensionsUniform_PCT = glGetUniformLocation(shaderProgramObject_PCT, "u_dimensions");
	textureSamplerUniform_PCT = glGetUniformLocation(shaderProgramObject_PCT, "u_textureSampler");

	return 0;
}

int initializePCNTShader(void)
{
	// code
	// vertex shader code
	const GLchar *vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"in vec3 a_normal;" \
		"in vec2 a_texcoords;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform vec4 u_lightPosition;" \
		"uniform int u_lightingEnabled;" \
		"out vec3 transformedNormals;" \
		"out vec3 lightDirection;" \
		"out vec3 viewerVector;" \
		"uniform vec4 u_dimensions;" \
		"out vec2 o_texcoords;"
		"void main(void)" \
		"{" \
		"	vec4 position = a_position * u_dimensions;" \
		"	if(u_lightingEnabled == 1)" \
		"	{" \
		"		vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * position;" \
		"		mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
		"		transformedNormals = normalMatrix * a_normal;" \
		"		lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;" \
		"		viewerVector = -eyeCoordinates.xyz;" \
		"	}" \
		"	gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * position;" \
		"	o_texcoords = a_texcoords;" \
		"}";

	// creating shader object
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// giving shader code to shader object
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	// compile the shader
	glCompileShader(vertexShaderObject);

	GLint status;
	GLint infoLogLength;
	char* log = NULL;

	// getting compilation status
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	// do error checking
	if (status == GL_FALSE)
	{
		// Getting length of log of compilation status
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			// Allocate enough memory to the buffer to hold the compilation log
			log = (char*)malloc(infoLogLength);
			if (log == NULL)
			{
				fprintf(gpFile, "Unable to get compilation log for vertex shader.\n");
			}
			else
			{
				GLsizei written;

				// Get the compilation log into allocated buffer
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);

				// Display the contents of buffer (log)
				fprintf(gpFile, "PCNT - Vertex shader compilation log : %s\n", log);

				// Free the allocated buffer
				free(log);
				log = NULL;
			}
		}
		return UNINITIALIZE;
	}

	// fragment shader code
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec3 transformedNormals;" \
		"in vec3 lightDirection;" \
		"in vec3 viewerVector;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_materialShininess;" \
		"uniform int u_lightingEnabled;" \
		"in vec2 o_texcoords;" \
		"uniform sampler2D u_textureSampler;" \
		"uniform vec4 u_color;" \
		"uniform int u_textureEnabled;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"	vec3 phong_ads_light = vec3(1.0f, 1.0f, 1.0f);" \
		"	vec4 color = gl_FrontFacing ? u_color : vec4(0.752941f, 0.752941f, 0.752941f, 1.0f);" \
		"	if(u_lightingEnabled == 1)" \
		"	{" \

		"		vec3 ambient = u_la * u_ka;" \
		"		vec3 normalized_transformed_normals = normalize(transformedNormals);" \
		"		if(gl_FrontFacing == false)" \
		"		{" \
		"			normalized_transformed_normals = -normalized_transformed_normals;" \
		"		}" \
		"		vec3 normalized_lightDirection = normalize(lightDirection);" \
		"		vec3 normalized_viewerVector = normalize(viewerVector);" \
		"		vec3 diffuse = u_ld * u_kd * max(dot(normalized_lightDirection, normalized_transformed_normals), 0.0);" \
		"		vec3 reflectionVector = reflect(-normalized_lightDirection, normalized_transformed_normals);" \
		"		vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector, normalized_viewerVector), 0.0), u_materialShininess);" \
		"		phong_ads_light = ambient + diffuse + specular;" \
		"	}" \
		"	if(u_textureEnabled == 1 && gl_FrontFacing)" \
		"	{" \
		"		vec3 tex = vec3(texture(u_textureSampler, o_texcoords));" \
		"		FragColor = vec4(tex * phong_ads_light * vec3(color), 1.0f);" \
		"	}" \
		"	else" \
		"	{" \
		"		FragColor = vec4(phong_ads_light * vec3(color), 1.0f);" \
		"	}" \
		"}";

	// creating shader object
	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// giving shader code to shader object
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	// compile the shader
	glCompileShader(fragmentShaderObject);

	// reinitialize
	status = 0;
	infoLogLength = 0;
	log = NULL;

	// getting compilation status
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	// do error checking
	if (status == GL_FALSE)
	{
		// Getting length of log of compilation status
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			// Allocate enough memory to the buffer to hold the compilation log
			log = (char*)malloc(infoLogLength);
			if (log == NULL)
			{
				fprintf(gpFile, "PCNT - Unable to get compilation log for fragment shader.\n");
			}
			else
			{
				GLsizei written;
				// Get the compilation log into allocated buffer
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);

				// Display the contents of buffer (log)
				fprintf(gpFile, "PCNT - Fragment shader compilation log : %s", log);

				// Free the allocated buffer
				free(log);
				log = NULL;
			}
		}
		return UNINITIALIZE;
	}

	// Create shader program object
	shaderProgramObject_PCNT = glCreateProgram();

	// Attach desired shaders to this shader program object
	glAttachShader(shaderProgramObject_PCNT, vertexShaderObject);
	glAttachShader(shaderProgramObject_PCNT, fragmentShaderObject);

	// Prelinked binding of shader program object with vertex attributes
	glBindAttribLocation(shaderProgramObject_PCNT, RRI_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(shaderProgramObject_PCNT, RRI_ATTRIBUTE_NORMAL, "a_normal");
	glBindAttribLocation(shaderProgramObject_PCNT, RRI_ATTRIBUTE_TEXTURE0, "a_texcoords");

	// Link shader program object
	glLinkProgram(shaderProgramObject_PCNT);

	// reinitialize status and other associated variables
	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetProgramiv(shaderProgramObject_PCNT, GL_LINK_STATUS, &status);
	// do error checking
	if (status == GL_FALSE)
	{
		// Getting length of log of compilation status
		glGetProgramiv(shaderProgramObject_PCNT, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			// Allocate enough memory to the buffer to hold the compilation log
			log = (char*)malloc(infoLogLength);
			if (log == NULL)
			{
				fprintf(gpFile, "PCNT - Unable to get compilation log for fragment shader.\n");
			}
			else
			{
				GLsizei written;
				// Get the compilation log into allocated buffer
				glGetProgramInfoLog(shaderProgramObject_PCNT, infoLogLength, &written, log);

				// Display the contents of buffer (log)
				fprintf(gpFile, "PCNT - Shader program link log : %s", log);
				free(log);
				log = NULL;
			}
		}
		return UNINITIALIZE;
	}

	// Postlinked retrieval of uniform location from the shader program object
	modelMatrixUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_modelMatrix");
	viewMatrixUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_viewMatrix");
	projectionMatrixUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_projectionMatrix");
	textureSamplerUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_textureSampler");
	laUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_la");
	ldUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_ld");
	lsUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_ls");
	lightPositionUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_lightPosition");
	kaUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_ka");
	kdUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_kd");
	ksUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_ks");
	materialShininessUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_materialShininess");
	lightingEnabledUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_lightingEnabled");
	colorUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_color");
	dimensionsUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_dimensions");
	textureEnabledUniform_PCNT = glGetUniformLocation(shaderProgramObject_PCNT, "u_textureEnabled");

	return 0;
}

void uninitializeShaders(void)
{
	// code
	// PC Shaders
	const GLuint shaderProgramObjects[] = 
	{
		shaderProgramObject_PC,
		shaderProgramObject_PCT,
		shaderProgramObject_PCN,
		shaderProgramObject_PCNT
	};

	for (int i = 0; i < sizeof(shaderProgramObjects); i++)
	{
		GLuint shaderProgramObject = shaderProgramObjects[i];
		if (shaderProgramObject)
		{
			// Use the shader program object again
			glUseProgram(shaderProgramObject);

			GLsizei numAttachedShaders;
			// Get the number of attached shaders
			glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);

			// Create empty buffer to hold array of attached shaders
			GLuint* attachedShaderObjects = NULL;

			// Allocate enough memory to this empty buffer according to attached shaders 
			attachedShaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));

			// Fill buffer with the attached shader objects
			glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, attachedShaderObjects);

			// iterate over the loop of attached shader and detach and delete each shader.
			for (GLsizei i = 0; i < numAttachedShaders; i++)
			{
				glDetachShader(shaderProgramObject, attachedShaderObjects[i]);
				glDeleteShader(attachedShaderObjects[i]);
				attachedShaderObjects[i] = 0;
			}

			// Free the memory allocated for buffer
			free(attachedShaderObjects);
			attachedShaderObjects = NULL;

			// unuse shader program object
			glUseProgram(0);

			// Delete the shader program object
			glDeleteProgram(shaderProgramObject);
			shaderProgramObject = 0;
		}
	}
}