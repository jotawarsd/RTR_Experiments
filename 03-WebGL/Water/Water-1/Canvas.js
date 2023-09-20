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
	var lightAmbient = [0.1, 0.1, 0.1];
	var lightDiffuse = [1.0, 1.0, 1.0];
	var lightSpecular = [1.0, 1.0, 1.0];
	var lightPosition = [3.0, 1.0, 0.0, 1.0];
	
	var materialAmbient = [0.0, 0.3, 0.61];
	var materialDiffuse = [0.0, 0.3, 0.61];
	var materialSpecular = [0.0, 0.3, 0.61];
	var materialShininess = 2.0;

	waterInit(shaderProgramObject, 0, 0, 32, 32, lightAmbient, lightDiffuse, lightSpecular, lightPosition, materialAmbient, materialDiffuse, materialSpecular, materialShininess);

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

	//displayWater
	waterDisplay(0.0001, perspectiveProjectionMatrix);

	//update and swap buffers
	requestAnimationFrame(display, canvas);
}

function update()
{
	//code
}

function uninitialize()
{
	//code	
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
