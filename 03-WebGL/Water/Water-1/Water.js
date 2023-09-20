var vao;
var vbo_position;
var vbo_normals;

//uniforms
var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

var meshSizeUniform;

//lighting uniforms
var laUniform;
var ldUniform;
var lsUniform;
var lightPositionUniform;

var kaUniform;
var kdUniform;
var ksUniform;
var materialShininessUniform;

var animationUniform;
var lightingEnabledUniform;
var perspectiveProjectionMatrix;

var g_shaderProgramObject;

//water variables
var g_meshWidth = 0;
var g_meshHeight = 0;

//lighting attributes
var g_lightAmbient;
var g_lightDiffuse;
var g_lightSpecular;
var g_lightPosition;

var g_materialAmbient;
var g_materialDiffuse;
var g_materialSpecular;
var g_materialShininess;

let position = [];
let normals = [];

function waterInit(shaderProgramObject_Water, px, pz, meshWidth, meshHeight, la, ld, ls, lp, ka, kd, ks, kShininess)
{
	//variable declarations
	//Vertex Shader
	var vertexShaderSourceCode =
		"#version 300 es" +
		"\n" +
		"in vec4 a_position;" +
		"in vec3 a_normal;" +
		"uniform mat4 u_modelMatrix;" +
		"uniform mat4 u_viewMatrix;" +
		"uniform mat4 u_projectionMatrix;" +
		"uniform float u_time;" +
		"uniform vec4 u_lightPosition;" +
		"uniform int u_meshSize;" +
		"out vec3 transformedNormals;" +
		"out vec3 lightDirection;" +
		"out vec3 viewerVector;" +
		"void main(void)" +
		"{" +
		"float pos_x = 2.0 * a_position.x - 1.0;" +
		"float pos_z = 2.0 * a_position.z - 1.0;" +
		"vec3 Di = vec3(1.0, -1.0, 0.0);" +
		"vec3 pos = vec3(pos_x, 0.0, pos_z);" +
		"float Qi = 0.0, Ai = 0.0;" +
		"vec3 x = vec3(pos_x, 0.0, 0.0);" +
		"vec3 y = vec3(0.0, a_position.y, 0.0);" +
		"vec3 z = vec3(0.0, 0.0, pos_z);" +
		"float wi = 0.0;" +
		"float speed = 6.0;" +
		"float phase = 0.0, lamda = 0.0;" +
		"float px = 0.0, py = 0.0, pz = 0.0;" +
		"float pix = 0.0, piy = 0.0, piz = 0.0;" +
		"float nix = 0.0, niy = 0.0, niz = 0.0;" +
		"float nx = 0.0, ny = 0.0, nz = 0.0;" +
		"for (int i = 0; i < 6; i++)" +
		"{" +
		"float i_f = float(i + 1);" +
		"lamda = (4.0 * sqrt(i_f)) - 1.5;" +
		"Ai = (lamda / 6.0) * (2.0 * cos(4.0 * pos_z + 50.0 * u_time));" +
		"wi = (100.0 / lamda);" +
		"phase = wi * speed;" +
		"piy = sin((wi * dot(Di, pos)) + (phase * u_time));" +
		"py += Ai * piy;" +
		"niy = 0.02 * Ai * sin((wi * dot(Di, pos)) + (phase * u_time));" +
		"ny += niy;" +
		"}" +
		"gl_PointSize = 2.0;" +
		"vec4 position = vec4(pos_x, py * 0.004, pos_z, 1.0);" +
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * position;" +
		"vec3 o_normal = vec3(a_normal.x, ny, a_normal.z);" +
		"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * position;" +
		"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" +
		"transformedNormals = normalMatrix * o_normal;" +
		"lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;" +
		"viewerVector = -eyeCoordinates.xyz;" +
		"}";		

	var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
	gl.compileShader(vertexShaderObject);

	if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false)
	{
		var error = "Vertex Shader Compilation Log: " + gl.getShaderInfoLog(vertexShaderObject);
		if (error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	//fragment Shader
	var fragmentShaderSourceCode =
		"#version 300 es" +
		"\n" +
		"precision highp float;" +
		"in vec3 transformedNormals;" +
		"in vec3 lightDirection;" +
		"in vec3 viewerVector;" +
		"uniform vec3 u_la;" +
		"uniform vec3 u_ld;" +
		"uniform vec3 u_ls;" +
		"uniform vec3 u_ka;" +
		"uniform vec3 u_kd;" +
		"uniform vec3 u_ks;" +
		"uniform float u_materialShininess;" +
		"out vec4 FragColor;" +
		"void main(void)" +
		"{" +
		"vec3 phong_ads_color;" +
		"vec3 ambient = u_la * u_ka;" +
		"vec3 normalized_transformed_normals = normalize(transformedNormals);" +
		"vec3 normalized_light_direction = normalize(lightDirection);" +
		"vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction, normalized_transformed_normals), 0.0);" +
		"vec3 reflectionVector = reflect(-normalized_light_direction, normalized_transformed_normals);" +
		"vec3 normalized_viewer_vector = normalize(viewerVector);" +
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector, normalized_viewer_vector), 0.0), u_materialShininess);" +
		"phong_ads_color = ambient + diffuse + specular;" +
		"FragColor = vec4(phong_ads_color, 0.1);" +
		"}";

	var fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
	gl.compileShader(fragmentShaderObject);

	if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false)
	{
		var error = "Fragment Shader Compilation Log: " + gl.getShaderInfoLog(fragmentShaderObject);
		if (error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	shaderProgramObject_Water = gl.createProgram();
	gl.attachShader(shaderProgramObject_Water, vertexShaderObject);
	gl.attachShader(shaderProgramObject_Water, fragmentShaderObject);

	//Prelinking Attributes
	gl.bindAttribLocation(shaderProgramObject_Water, WebGLMacros.SDJ_ATTRIBUTE_POSITION, "a_position");
	gl.bindAttribLocation(shaderProgramObject_Water, WebGLMacros.SDJ_ATTRIBUTE_NORMAL, "a_normal");

	//link program
	gl.linkProgram(shaderProgramObject_Water);
	if (gl.getProgramParameter(shaderProgramObject_Water, gl.LINK_STATUS) == false)
	{
		var error = "Shader Program Link Log" + gl.getProgramInfoLog(shaderProgramObject_Water);
		if (error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	//post linking - get uniform locations
	modelMatrixUniform = gl.getUniformLocation(shaderProgramObject_Water, "u_modelMatrix");
	viewMatrixUniform = gl.getUniformLocation(shaderProgramObject_Water, "u_viewMatrix");
	projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject_Water, "u_projectionMatrix");

	laUniform = gl.getUniformLocation(shaderProgramObject_Water, "u_la");
	ldUniform = gl.getUniformLocation(shaderProgramObject_Water, "u_ld");
	lsUniform = gl.getUniformLocation(shaderProgramObject_Water, "u_ls");
	lightPositionUniform = gl.getUniformLocation(shaderProgramObject_Water, "u_lightPosition");
	kaUniform = gl.getUniformLocation(shaderProgramObject_Water, "u_ka");
	kdUniform = gl.getUniformLocation(shaderProgramObject_Water, "u_kd");
	ksUniform = gl.getUniformLocation(shaderProgramObject_Water, "u_ks");
	materialShininessUniform = gl.getUniformLocation(shaderProgramObject_Water, "u_materialShininess");

	animationUniform = gl.getUniformLocation(shaderProgramObject_Water, "u_time");
	meshSizeUniform = gl.getUniformLocation(shaderProgramObject_Water, "u_meshSize");

	g_meshHeight = meshWidth;
	g_meshHeight = meshHeight;

	//declaration of vertex data arrays
	for (let z = pz; z < pz + meshHeight - 1; z++)
	{
		for (let x = px; x < px + meshWidth - 1; x++)
		{
			var u = x / meshWidth;
			var v = z / meshHeight;
			position.push(u - 0.5 / meshWidth, 0.0, v - 0.5 / meshHeight);
			position.push(u + 0.5 / meshWidth, 0.0, v + 0.5 / meshHeight);
			position.push(u + 0.5 / meshWidth, 0.0, v - 0.5 / meshHeight);

			position.push(u - 0.5 / meshWidth, 0.0, v - 0.5 / meshHeight);
			position.push(u - 0.5 / meshWidth, 0.0, v + 0.5 / meshHeight);
			position.push(u + 0.5 / meshWidth, 0.0, v + 0.5 / meshHeight);

			normals.push(u, 0.0, v);
			normals.push(u, 0.0, v);
			normals.push(u, 0.0, v);

			normals.push(u, 0.0, v);
			normals.push(u, 0.0, v);
			normals.push(u, 0.0, v);
		}
	}

	position = new Float32Array(position);
	normals = new Float32Array(normals);

	//vertex array object
	vao = gl.createVertexArray();
	gl.bindVertexArray(vao);

	vbo_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
		gl.bufferData(gl.ARRAY_BUFFER, position, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SDJ_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SDJ_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	vbo_normals = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_normals);
		gl.bufferData(gl.ARRAY_BUFFER, normals, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SDJ_ATTRIBUTE_NORMAL, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SDJ_ATTRIBUTE_NORMAL);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);

	g_lightAmbient = la;
	g_lightDiffuse = ld;
	g_lightSpecular = ls;
	g_lightPosition = lp;
	
	g_materialAmbient = ka;
	g_materialDiffuse = kd;
	g_materialSpecular = ks;
	g_materialShininess = kShininess;

	g_shaderProgramObject = shaderProgramObject_Water;
}

function waterDisplay(time, perspectiveProjectionMatrix)
{
	//code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	//use shader program object
	gl.useProgram(g_shaderProgramObject);

	//transformations
	var translationMatrix = mat4.create();
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();

	mat4.translate(translationMatrix, translationMatrix, [0.0, -0.2, -1.0]);
	mat4.multiply(modelMatrix, modelMatrix, translationMatrix);

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

	gl.uniform1f(animationUniform, animationTime);
	gl.uniform1i(meshSizeUniform, g_meshWidth);

	gl.uniform3fv(laUniform, g_lightAmbient);
	gl.uniform3fv(ldUniform, g_lightDiffuse);
	gl.uniform3fv(lsUniform, g_lightSpecular);
	gl.uniform4fv(lightPositionUniform, g_lightPosition);

	gl.uniform3fv(kaUniform, g_materialAmbient);
	gl.uniform3fv(kdUniform, g_materialDiffuse);
	gl.uniform3fv(ksUniform, g_materialSpecular);
	gl.uniform1f(materialShininessUniform, g_materialShininess);

	//bind vao
	gl.bindVertexArray(vao);
	gl.drawArrays(gl.TRIANGLES, 0, position.length / 3);

	gl.bindVertexArray(null);

	//unuse shader program object
	gl.useProgram(null);

	//update and swap buffers
	animationTime += time;
}

function waterUninitialize()
{
	if (vao) {
		gl.deleteVertexArray(vao);
		vao = null;
	}

	if (vbo_position) {
		gl.deleteBuffer(vbo_position);
		vbo_position = null;
	}

	if (vbo_normals) {
		gl.deleteBuffer(vbo_normals);
		vbo_normals = null;
	}

	if (g_shaderProgramObject) { 
		gl.useProgram(g_shaderProgramObject);
		var shaderObjects = gl.getAttachedShaders(g_shaderProgramObject);
		for (let i = 0; i < shaderObjects.length; i++) {
			gl.detachShader(g_shaderProgramObject, shaderObjects[i]);
			gl.deleteShader(shaderObjects[i]);
			shaderObjects[i] = null;
		}
		gl.useProgram(null);
		gl.deleteProgram(g_shaderProgramObject);
		g_shaderProgramObject = null;
	}
}
