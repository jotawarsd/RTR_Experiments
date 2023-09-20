var canvas = null;
var gl = null;
var bFullscreen = false;

var canvas_original_width;
var canvas_original_height;

//OpenGL Variables
const WebGLMacros =
{
	SDJ_ATTRIBUTE_POSITION: 0,
	SDJ_ATTRIBUTE_COLOR: 1,
	SDJ_ATTRIBUTE_NORMAL: 2,
	SDJ_ATTRIBUTE_TEXTURE0: 3,
};

var shaderProgramObject = null;
var vao;
var vbo_position;
var vbo_normals;

//uniforms
var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

var meshSizeUniform;

//lighting uniforms
var laUniform = new Array(4);
var ldUniform = new Array(4);
var lsUniform = new Array(4);
var lightPositionUniform = new Array(4);

var kaUniform = new Array(4);
var kdUniform = new Array(4);
var ksUniform = new Array(4);
var materialShininessUniform;

var animationUniform;
var perspectiveProjectionMatrix;

//Water variables
var meshWidth = 32;
var meshHeight = 32;

var MYARRAYSIZE = meshWidth * meshHeight * 4;

let position = [];
let normals = [];

var pos;

//lighting attributes
var lightAmbient;
var lightDiffuse;
var lightSpecular;
var lightPosition;

var materialAmbient;
var materialDiffuse;
var materialSpecular;
var materialShininess;

var bLight = true;

var animationTime = 0.0;

//request animation frame
var requestAnimationFrame =
	window.requestAnimationFrame ||
	window.mozRequestAnimationFrame ||
	window.webkitRequestAnimationFrame ||
	window.oRequestAnimationFrame ||
	window.msRequestAnimationFrame;

function main()
{
	//code
	//get canvas
	canvas = document.getElementById("SDJ:WebGL");
	if (!canvas)
		console.log("Obtaining canvas failed!!\n");
	else
		console.log("Obtaining canvas successful!!\n");

	//backup canvas dimensions
	canvas_original_width = canvas.width;
	canvas_original_height = canvas.height;

	//OGL Functions
	initialize();
	resize();
	display();  

	//Event handler
	window.addEventListener("resize", resize, false);
	window.addEventListener("keydown", keyDown, false);
	window.addEventListener("click", mouseDown, false);
}

function toggleFullscreen()
{
	//code
	var fullscreen_element =
		document.fullscreenElement ||
		document.mozFullScreenElement ||
		document.webkitFullscreenElement ||
		document.msFullscreenElement ||
		null;

	if (fullscreen_element == null)     //if not fullscreen
	{
		if (canvas.requestFullscreen)
			canvas.requestFullscreen();
		else if (canvas.mozRequestFullScreen)
			canvas.mozRequestFullScreen();
		else if (canvas.webkitRequestFullscreen)
			canvas.webkitRequestFullscreen();
		else if (canvas.msRequestFullscreen)
			canvas.msRequestFullscreen();
		bFullscreen = true;
	}
	else
	{
		if (document.exitFullscreen)
			document.exitFullscreen();
		else if (document.mozExitFullScreen)
			document.mozExitFullScreen();
		else if (document.webkitExitFullscreen)
			document.webkitExitFullscreen;
		else if (document.msExitFullscreen)
			document.msExitFullscreen;
		bFullscreen = false;
	}
}

function initialize()
{
	//code
	//get webgl2 Context from canvas
	gl = canvas.getContext("webgl2", {alpha:false});
	if (!gl)
		console.log("Obtaining webgl2 context failed!!\n");
	else
		console.log("obtaining webgl2 context successful!!\n");

	//set viewport width and height
	gl.viewportWidth = canvas.width;
	gl.viewportHeight = canvas.height;

	//OpenGL Initialization----------
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
		"uniform int u_lightingEnabled;" +
		"uniform int u_meshSize;" +
		"out vec3 transformedNormals;" +
		"out vec3 lightDirection;" +
		"out vec3 viewerVector;" +
		"flat out int lightingEnabled;" +
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
		"py += Ai * pow(piy, 1.0);" +
		"niy = 0.02 * Ai * sin((wi * dot(Di, pos)) + (phase * u_time));" +
		"ny += niy;" +
		"}" +
		"gl_PointSize = 2.0;" +
		"vec4 position = vec4(pos_x, py * 0.004, pos_z, 1.0);" +
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * position;" +
		"vec3 o_normal = vec3(a_normal.x, ny, a_normal.z);" +
		"if (u_lightingEnabled == 1)" +
		"	{" +
		"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * position;" +
		"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" +
		"transformedNormals = normalMatrix * o_normal;" +
		"lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;" +
		"viewerVector = -eyeCoordinates.xyz;" +
		"	}" +
		"lightingEnabled = u_lightingEnabled;" +
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
		"flat in int lightingEnabled;" +
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
		"if (lightingEnabled == 1)" +
		"{" +
		"vec3 ambient = u_la * u_ka;" +
		"vec3 normalized_transformed_normals = normalize(transformedNormals);" +
		"vec3 normalized_light_direction = normalize(lightDirection);" +
		"vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction, normalized_transformed_normals), 0.0);" +
		"vec3 reflectionVector = reflect(-normalized_light_direction, normalized_transformed_normals);" +
		"vec3 normalized_viewer_vector = normalize(viewerVector);" +
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector, normalized_viewer_vector), 0.0), u_materialShininess);" +
		"phong_ads_color = ambient + diffuse + specular;" +
		"}" +
		"else" +
		"{" +
		"phong_ads_color = vec3(0.0, 0.5, 1.0);" +
		"}" +
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

	shaderProgramObject = gl.createProgram();
	gl.attachShader(shaderProgramObject, vertexShaderObject);
	gl.attachShader(shaderProgramObject, fragmentShaderObject);

	//Prelinking Attributes
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SDJ_ATTRIBUTE_POSITION, "a_position");
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.SDJ_ATTRIBUTE_NORMAL, "a_normal");

	//link program
	gl.linkProgram(shaderProgramObject);
	if (gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS) == false)
	{
		var error = "Shader Program Link Log" + gl.getProgramInfoLog(shaderProgramObject);
		if (error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}

	//post linking - get uniform locations
	modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_modelMatrix");
	viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_viewMatrix");
	projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_projectionMatrix");

	laUniform = gl.getUniformLocation(shaderProgramObject, "u_la");
	ldUniform = gl.getUniformLocation(shaderProgramObject, "u_ld");
	lsUniform = gl.getUniformLocation(shaderProgramObject, "u_ls");
	lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_lightPosition");
	kaUniform = gl.getUniformLocation(shaderProgramObject, "u_ka");
	kdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
	ksUniform = gl.getUniformLocation(shaderProgramObject, "u_ks");
	materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "u_materialShininess");
	lightingEnabledUniform = gl.getUniformLocation(shaderProgramObject, "u_lightingEnabled");

	animationUniform = gl.getUniformLocation(shaderProgramObject, "u_time");
	meshSizeUniform = gl.getUniformLocation(shaderProgramObject, "u_meshSize");

	//declaration of vertex data arrays
	for (let z = 1; z < meshHeight - 1; z++)
	{
		for (let x = 1; x < meshWidth - 1; x++)
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

	pos = new Float32Array(position);
	let normal = new Float32Array(normals);

	//vertex array object
	vao = gl.createVertexArray();
	gl.bindVertexArray(vao);

	vbo_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
		gl.bufferData(gl.ARRAY_BUFFER, pos, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SDJ_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SDJ_ATTRIBUTE_POSITION);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	vbo_normals = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_normals);
		gl.bufferData(gl.ARRAY_BUFFER, normal, gl.STATIC_DRAW);
		gl.vertexAttribPointer(WebGLMacros.SDJ_ATTRIBUTE_NORMAL, 3, gl.FLOAT, false, 0, 0);
		gl.enableVertexAttribArray(WebGLMacros.SDJ_ATTRIBUTE_NORMAL);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);

	lightAmbient = [0.1, 0.1, 0.1];
	lightDiffuse = [1.0, 1.0, 1.0];
	lightSpecular = [1.0, 1.0, 1.0];
	lightPosition = [3.0, 1.0, 0.0, 1.0];
	
	materialAmbient = [0.0, 0.3, 0.61];
	materialDiffuse = [0.0, 0.3, 0.61];
	materialSpecular = [0.0, 0.3, 0.61];
	materialShininess = 2.0;

	//depth functions
	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);

	gl.enable(gl.BLEND);

	//clear screen with blue color
	gl.clearColor(0.0, 0.0, 0.0, 1.0);  //blue

	perspectiveProjectionMatrix = mat4.create();
}

function resize()
{
	//code
	if (bFullscreen == true) {
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
	}
	else {
		canvas.width = canvas_original_width;
		canvas.height = canvas_original_height;
	}

	if (canvas.height == 0)
		canvas.height = 1;
	gl.viewport(0, 0, canvas.width, canvas.height);

	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 1000.0);
}

function display()
{
	//code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	//use shader program object
	gl.useProgram(shaderProgramObject);

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
	gl.uniform1i(meshSizeUniform, meshWidth);

	if (bLight == true)
	{
		gl.uniform1i(lightingEnabledUniform, 1);
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform4fv(lightPositionUniform, lightPosition);

		gl.uniform3fv(kaUniform, materialAmbient);
		gl.uniform3fv(kdUniform, materialDiffuse);
		gl.uniform3fv(ksUniform, materialSpecular);
		gl.uniform1f(materialShininessUniform, materialShininess);
	}
	else
	{
		gl.uniform1i(lightingEnabledUniform, 0);
	}

	//bind vao
	gl.bindVertexArray(vao);
	gl.drawArrays(gl.TRIANGLES, 0, position.length / 3);

	gl.bindVertexArray(null);

	//unuse shader program object
	gl.useProgram(null);

	//update and swap buffers
	update();
	requestAnimationFrame(display, canvas);
}

function update()
{
	//code
	animationTime += 0.0001;
}

function uninitialize()
{
	//code
	if (vao) {
		gl.deleteVertexArray(vao);
		vao = null;
	}

	if (vbo_position) {
		gl.deleteBuffer(vbo_position);
		vbo_position = null;
	}

	//shader uninitialization
	if (shaderProgramObject) { 
		gl.useProgram(shaderProgramObject);
		var shaderObjects = gl.getAttachedShaders(shaderProgramObject);
		for (let i = 0; i < shaderObjects.length; i++) {
			gl.detachShader(shaderProgramObject, shaderObjects[i]);
			gl.deleteShader(shaderObjects[i]);
			shaderObjects[i] = null;
		}
		gl.useProgram(null);
		gl.deleteProgram(shaderProgramObject);
		shaderProgramObject = null;
	}
}

//keyboard event listener
function keyDown(event)
{
	//code
	switch (event.keyCode) {
		case 70:
			toggleFullscreen();
			break;
		case 76:
		case 108:
			if (bLight == false)
				bLight = true;
			else
				bLight = false;
			break;
		case 81:
			uninitialize();
			window.close(); //not all browsers follow this
			break;
	}
}

//mouse event listener
function mouseDown()
{
	//code
}
