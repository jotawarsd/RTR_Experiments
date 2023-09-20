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
var vbo;

var mvpMatrixUniform;
var animationUniform;
var perspectiveProjectionMatrix;

//Water variables
var meshWidth = 512;
var meshHeight = 512;

var MYARRAYSIZE = meshWidth * meshHeight * 4;

var pos = new Float32Array(MYARRAYSIZE);
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
	gl = canvas.getContext("webgl2");
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
		"uniform mat4 u_mvpMatrix;" +
		"uniform float u_time;" +
		"void main(void)" +
		"{" +
		"vec3 Di = vec3(1.0, -1.0, 0.0);" +
		"vec3 pos = vec3(a_position.x, 0.0, a_position.z);" +
		"float Qi = 0.0, Ai = 0.0;" +
		"vec3 x = vec3(a_position.x, 0.0, 0.0);" +
		"vec3 z = vec3(0.0, 0.0, a_position.z);" +
		"float wi = 0.0;" +
		"float speed = 1.5;" +
		"float phase = 0.0, lamda = 0.0;" +
		"float px = 0.0, py = 0.0, pz = 0.0;" +
		"float pix = 0.0, piy = 0.0, piz = 0.0;" +
		"for (int i = 0; i < 10; i++)" +
		"{" +
		"float i_f = float(i + 1);" +
		"lamda = (4.0 * sqrt(i_f)) - 1.5;" +
		"Ai = (lamda / 4.0) * (2.0 + sin(a_position.z * 20.0 * a_position.x));" +
		"wi = (128.0 / lamda);" +
		"phase = wi * speed;" +
		"Qi = 0.001;" +
		"pix = Qi * Ai * dot(Di, x) * cos((wi * dot(Di, pos)) + (phase * u_time));" +
		"piz = Qi * Ai * dot(Di, z) * cos((wi * dot(Di, pos)) + (phase * u_time));" +
		"piy = Ai * sin((wi * dot(Di, pos)) + (phase * u_time));" +
		"px += pix;" +
		"py += piy;" +
		"pz += piz;" +
		"}" +
		"gl_PointSize = 2.0;" +
		"gl_Position = u_mvpMatrix * vec4(a_position.x + px, py * 0.002, a_position.z + pz, 1.0);" +
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
		"out vec4 FragColor;" +
		"void main(void)" +
		"{" +
		"FragColor = vec4(0.0, 0.5, 1.0, 1.0);" +
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
	mvpMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_mvpMatrix");
	animationUniform = gl.getUniformLocation(shaderProgramObject, "u_time");

	//declarations of vertex data arrays
	for (var i = 0; i < meshWidth; i++)
	{
		for (var j = 0; j < meshHeight; j += 4)
		{
			var u = i / meshWidth;
			var v = j / meshHeight;

			u = (u * 2.0) - 1.0;
			v = (v * 2.0) - 1.0;

			pos[i * meshWidth + j + 0] = u;
			pos[i * meshWidth + j + 1] = 0.0;
			pos[i * meshWidth + j + 2] = v;
			pos[i * meshWidth + j + 3] = 1.0;
		}
	}

	//vertex array object
	vao = gl.createVertexArray();
	gl.bindVertexArray(vao);

	vbo = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo);

	gl.bufferData(gl.ARRAY_BUFFER, pos, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.SDJ_ATTRIBUTE_POSITION, 4, gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.SDJ_ATTRIBUTE_POSITION);

	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null);

	//depth functions
	gl.clearDepth(1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);

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

	mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);
}

function display()
{
	//code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	//use shader program object
	gl.useProgram(shaderProgramObject);

	//transformations
	var translationMatrix = mat4.create();
	var modelViewMatrix = mat4.create();
	var modelViewProjectionMatrix = mat4.create();

	mat4.translate(translationMatrix, translationMatrix, [0.0, -0.1, -1.0]);
	mat4.multiply(modelViewMatrix, modelViewMatrix, translationMatrix);
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);
	gl.uniform1f(animationUniform, animationTime);

	//bind vao
	gl.bindVertexArray(vao);
	gl.drawArrays(gl.POINTS, 0, meshWidth * meshHeight);
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
	animationTime += 0.001;
}

function uninitialize()
{
	//code
	if (vao) {
		gl.deleteVertexArray(vao);
		vao = null;
	}

	if (vbo) {
		gl.deleteBuffer(vbo);
		vbo = null;
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
