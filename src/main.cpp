/*
 *  CSCI 441, Computer Graphics, Fall 2017
 *
 *  Project: lab12
 *  File: main.cpp
 *
 *  Description:
 *      This file contains the basic setup to work with VAOs & VBOs using a
 *	MD5 model.
 *
 *  Author: Dr. Paone, Colorado School of Mines, 2017
 *
 *
 */

//******************************************************************************

#include <GL/glew.h>
#include <GLFW/glfw3.h>			// include GLFW framework header

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SOIL/SOIL.h>		// for image loading

#include <stdio.h>				// for printf functionality
#include <stdlib.h>				// for exit functionality


#include <CSCI441/FramebufferUtils3.hpp>
#include <CSCI441/objects3.hpp>
#include <CSCI441/ShaderProgram3.hpp>

#include <cstdlib>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

#include "PlayerTank.h"
#include "EnemyRoamerTank.h"
#include "Level.h"
#include "Bullet.h"


//******************************************************************************
//
// Global Parameters

//window params
int windowWidth, windowHeight; 

// input flags
bool controlDown = false;
bool leftMouseDown = false;
bool moveLeft = false;
bool moveRight = false;
bool moveUp = false;
bool moveDown = false;
glm::vec2 mousePosition( -9999.0f, -9999.0f );

//camera paramters
glm::vec3 cameraAngles( 0.5*M_PI/4.0f, 5.125*M_PI/4.0f, 90.0f ); //arcball (theta, phi, radius)
glm::vec3 cameraOffset; 					   //camera offset from player
glm::vec3 upVector( 0.0f,  1.0f,  0.0f );

//platform
GLuint platformVAOd;
GLuint platformTextureHandle;
float platformSize = 200.f;

//skybox
GLuint skyboxVAO;	   
GLuint skyboxTextureHandle;
GLfloat skyboxDim = 1000.0f;

//SHADERS
//texture shader - skybox and levels
CSCI441::ShaderProgram* textureShaderProgram = NULL;
GLint uniform_modelMtx_loc, uniform_viewProjetionMtx_loc, uniform_tex_loc, uniform_color_loc;
GLint attrib_vPos_loc, attrib_vTextureCoord_loc;

//model shader - game objects
CSCI441::ShaderProgram* modelPhongShaderProgram = NULL;
GLint uniform_phong_mv_loc, uniform_phong_v_loc, uniform_phong_p_loc, uniform_phong_norm_loc;
GLint uniform_phong_md_loc, uniform_phong_ms_loc, uniform_phong_ma_loc, uniform_phong_s_loc;
GLint uniform_phong_txtr_loc;
GLint attrib_phong_vpos_loc, attrib_phong_vnorm_loc, attrib_phong_vtex_loc;

//post processing shader - passthrough effects
CSCI441::ShaderProgram* postprocessingShaderProgram = NULL;
GLint uniform_post_proj_loc, uniform_post_fbo_loc;
GLint attrib_post_vpos_loc, attrib_post_vtex_loc;

//framebuffer
GLuint fbo;
int framebufferWidth = 1920, framebufferHeight = 1080;
GLuint framebufferTextureHandle;
GLuint rbo;
GLuint texturedQuadVAO;

//levels
const uint NUM_LEVELS = 10;
uint currentLevel = 0;
vector<Level> levels;
glm::vec3 levelCenteringOffset;
void loadCurrentLevel();

//entities
PlayerTank* playerTank;
vector<EnemyRoamerTank> enemyRoamers;
vector<Bullet> bullets;
//vector<EnemySentryTank*> enemySentrys; TODO
GLuint redTextureHandle, greenTextureHandle, brownTextureHandle;

//models
CSCI441::ModelLoader *tankBaseModel = NULL,
					 *tankTurretModel = NULL;

//******************************************************************************
//
// Helper Functions

void convertSphericalToCartesian() {
	cameraOffset.x = cameraAngles.z * sinf( cameraAngles.x ) * sinf( cameraAngles.y );
	cameraOffset.y = cameraAngles.z * -cosf( cameraAngles.y );
	cameraOffset.z = cameraAngles.z * -cosf( cameraAngles.x ) * sinf( cameraAngles.y );

}

bool registerOpenGLTexture(unsigned char *textureData,
                           unsigned int texWidth, unsigned int texHeight,
                           GLuint &textureHandle) {
    if(textureData == 0) {
        fprintf(stderr,"Cannot register texture; no data specified.");
        return false;
    }

    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

    return true;
}

//******************************************************************************
//
// Event Callbacks

// error_callback() ////////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's error callback.
//	When an error within GLFW occurs, GLFW will tell us by calling
//	this function.  We can then print this info to the terminal to
//	alert the user.
//
////////////////////////////////////////////////////////////////////////////////
static void error_callback(int error, const char* description) {
	fprintf(stderr, "[ERROR]: %s\n", description);
}

// key_callback() //////////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's keypress callback.
//	Responds to key presses and key releases
//
////////////////////////////////////////////////////////////////////////////////
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action == GLFW_PRESS){
		switch(key){
			case 'W':
				moveUp = true;
				break;
			case 'S':
				moveDown = true;
				break;
			case 'A':
				moveLeft = true;
				break;
			case 'D':
				moveRight = true;
				break;
			case 'Q':
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
			case 'N':
				currentLevel = ++currentLevel%NUM_LEVELS;
				loadCurrentLevel();
				break;
      case GLFW_KEY_SPACE:
        Bullet b(tankBaseModel,
                 playerTank->getPosition(),
                 playerTank->getBaseRotation());
        bullets.push_back(b);
        break;
		}
	}
	else if (action == GLFW_RELEASE){
		switch(key){
			case 'W':
				moveUp = false;
				break;
			case 'S':
				moveDown = false;
				break;
			case 'A':
				moveLeft = false;
				break;
			case 'D':
				moveRight = false;
				break;
		}
	}
}

// mouse_button_callback() /////////////////////////////////////////////////////
//
//		We will register this function as GLFW's mouse button callback.
//	Responds to mouse button presses and mouse button releases.  Keeps track if
//	the control key was pressed when a left mouse click occurs to allow
//	zooming of our arcball camera.
//
////////////////////////////////////////////////////////////////////////////////
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS ) {
		leftMouseDown = true;
		controlDown = (mods & GLFW_MOD_CONTROL);
	} else {
		leftMouseDown = false;
		mousePosition.x = -9999.0f;
		mousePosition.y = -9999.0f;
		controlDown = false;
	}
}

// cursor_callback() ///////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's cursor movement callback.
//	Responds to mouse movement.  When active motion is used with the left
//	mouse button an arcball camera model is followed.
//
////////////////////////////////////////////////////////////////////////////////
static void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	// make sure movement is in bounds of the window
	// glfw captures mouse movement on entire screen

	/*if( xpos > 0 && xpos < windowWidth ) {
		if( ypos > 0 && ypos < windowHeight ) {
			// active motion
			if( leftMouseDown ) {
				if( (mousePosition.x - -9999.0f) < 0.001f ) {
					mousePosition.x = xpos;
					mousePosition.y = ypos;
				} else {
					if( !controlDown ) {
							cameraAngles.x += (xpos - mousePosition.x)*0.005f;
						cameraAngles.y += (ypos - mousePosition.y)*0.005f;

						if( cameraAngles.y < 0 ) cameraAngles.y = 0.0f + 0.001f;
						if( cameraAngles.y >= M_PI ) cameraAngles.y = M_PI - 0.001f;
					} else {
						double totChgSq = (xpos - mousePosition.x) + (ypos - mousePosition.y);
						cameraAngles.z += totChgSq*0.03f;

						if( cameraAngles.z <= 2.0f ) cameraAngles.z = 2.0f;
					}
					convertSphericalToCartesian();

					mousePosition.x = xpos;
					mousePosition.y = ypos;
				}
			}
			// passive motion
			else {

			}
		}
	}*/
}

//******************************************************************************
//
// Setup Functions

// setupGLFW() /////////////////////////////////////////////////////////////////
//
//		Used to setup everything GLFW related.  This includes the OpenGL context
//	and our window.
//
////////////////////////////////////////////////////////////////////////////////
GLFWwindow* setupGLFW() {
	// set what function to use when registering errors
	// this is the ONLY GLFW function that can be called BEFORE GLFW is initialized
	// all other GLFW calls must be performed after GLFW has been initialized
	glfwSetErrorCallback(error_callback);

	// initialize GLFW
	if (!glfwInit()) {
		fprintf( stderr, "[ERROR]: Could not initialize GLFW\n" );
		exit(EXIT_FAILURE);
	} else {
		fprintf( stdout, "[INFO]: GLFW initialized\n" );
	}

	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );						// request forward compatible OpenGL context
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );	// request OpenGL Core Profile context
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );		// request OpenGL 3.x context
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );		// request OpenGL 3.3 context

	// create a window for a given size, with a given title
	GLFWwindow *window = glfwCreateWindow(640, 480, "Lab12: FBOs", NULL, NULL);
	if( !window ) {						// if the window could not be created, NULL is returned
		fprintf( stderr, "[ERROR]: GLFW Window could not be created\n" );
		glfwTerminate();
		exit( EXIT_FAILURE );
	} else {
		fprintf( stdout, "[INFO]: GLFW Window created\n" );
	}

	glfwMakeContextCurrent(	window );	// make the created window the current window
	glfwSwapInterval( 1 );				    // update our screen after at least 1 screen refresh

	glfwSetKeyCallback( 			  window, key_callback				  );	// set our keyboard callback function
	glfwSetMouseButtonCallback( window, mouse_button_callback );	// set our mouse button callback function
	glfwSetCursorPosCallback(	  window, cursor_callback  			);	// set our cursor position callback function

	return window;										// return the window that was created
}

// setupOpenGL() ///////////////////////////////////////////////////////////////
//
//      Used to setup everything OpenGL related.
//
////////////////////////////////////////////////////////////////////////////////
void setupOpenGL() {
	glEnable( GL_DEPTH_TEST );					// enable depth testing
	glDepthFunc( GL_LESS );							// use less than depth test

	glFrontFace( GL_CCW );

	glEnable(GL_BLEND);									// enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// use one minus blending equation

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	// clear the frame buffer to black
}

// setupGLEW() /////////////////////////////////////////////////////////////////
//
//      Used to initialize GLEW
//
////////////////////////////////////////////////////////////////////////////////
void setupGLEW() {
	glewExperimental = GL_TRUE;
	GLenum glewResult = glewInit();

	/* check for an error */
	if( glewResult != GLEW_OK ) {
		printf( "[ERROR]: Error initalizing GLEW\n");
		/* Problem: glewInit failed, something is seriously wrong. */
  	fprintf( stderr, "[ERROR]: %s\n", glewGetErrorString(glewResult) );
		exit(EXIT_FAILURE);
	} else {
		 fprintf( stdout, "[INFO]: GLEW initialized\n" );
		 fprintf( stdout, "[INFO]: Status: Using GLEW %s\n", glewGetString(GLEW_VERSION) );
	}

	if( !glewIsSupported( "GL_VERSION_2_0" ) ) {
		printf( "[ERROR]: OpenGL not version 2.0+.  GLSL not supported\n" );
		exit(EXIT_FAILURE);
	}
}

// setupTextures() /////////////////////////////////////////////////////////////
//
//      Load and register all the tetures for our program
//
////////////////////////////////////////////////////////////////////////////////
void setupTextures() {
	platformTextureHandle = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/SandBackground.png" );

	// and get handles for our full skybox
  printf( "[INFO]: registering skybox...\n" );
  fflush( stdout );
  skyboxTextureHandle = CSCI441::TextureUtils::loadAndRegisterTexture("textures/skybox/desert.png");
  redTextureHandle = CSCI441::TextureUtils::loadAndRegisterTexture("textures/red.png");
  greenTextureHandle = CSCI441::TextureUtils::loadAndRegisterTexture("textures/green.png");
  brownTextureHandle = CSCI441::TextureUtils::loadAndRegisterTexture("textures/brown.png");

  printf( "[INFO]: skybox textures read in and registered!\n\n" );
}

void setupShaders() {
	//basic texture shader for platform and skybox
	textureShaderProgram = new CSCI441::ShaderProgram( "shaders/textureShader.v.glsl", "shaders/textureShader.f.glsl" );
		uniform_modelMtx_loc         = textureShaderProgram->getUniformLocation( "modelMtx" );
		uniform_viewProjetionMtx_loc = textureShaderProgram->getUniformLocation( "viewProjectionMtx" );
		uniform_tex_loc              = textureShaderProgram->getUniformLocation( "tex" );
		uniform_color_loc			 = textureShaderProgram->getUniformLocation( "color" );
		attrib_vPos_loc				 = textureShaderProgram->getAttributeLocation( "vPos" );
		attrib_vTextureCoord_loc 	 = textureShaderProgram->getAttributeLocation( "vTextureCoord" );

	//texture shader to implement the phong lighting model
	modelPhongShaderProgram = new CSCI441::ShaderProgram( "shaders/texturingPhong.v.glsl", "shaders/texturingPhong.f.glsl" );
		uniform_phong_mv_loc 		= modelPhongShaderProgram->getUniformLocation( "modelviewMtx" );
		uniform_phong_v_loc 		= modelPhongShaderProgram->getUniformLocation( "viewMtx" );
		uniform_phong_p_loc 		= modelPhongShaderProgram->getUniformLocation( "projectionMtx" );
		uniform_phong_norm_loc 		= modelPhongShaderProgram->getUniformLocation( "normalMtx" );
		uniform_phong_md_loc 		= modelPhongShaderProgram->getUniformLocation( "materialDiffuse" );
		uniform_phong_ms_loc 		= modelPhongShaderProgram->getUniformLocation( "materialSpecular" );
		uniform_phong_ma_loc 		= modelPhongShaderProgram->getUniformLocation( "materialAmbient" );
		uniform_phong_s_loc			= modelPhongShaderProgram->getUniformLocation( "materialShininess" );
		uniform_phong_txtr_loc 		= modelPhongShaderProgram->getUniformLocation( "txtr" );
		attrib_phong_vpos_loc 		= modelPhongShaderProgram->getAttributeLocation( "vPos" );
		attrib_phong_vnorm_loc 		= modelPhongShaderProgram->getAttributeLocation( "vNormal" );
		attrib_phong_vtex_loc 		= modelPhongShaderProgram->getAttributeLocation( "vTexCoord" );

	//framebuffer shader for postprocessing effects
	postprocessingShaderProgram = new CSCI441::ShaderProgram( "shaders/passThrough.v.glsl", "shaders/passThrough.f.glsl" );
		uniform_post_proj_loc		= postprocessingShaderProgram->getUniformLocation( "projectionMtx" );
		uniform_post_fbo_loc		= postprocessingShaderProgram->getUniformLocation( "fbo" );
		attrib_post_vpos_loc		= postprocessingShaderProgram->getAttributeLocation( "vPos" );
		attrib_post_vtex_loc		= postprocessingShaderProgram->getAttributeLocation( "vTexCoord" );
}

// setupBuffers() //////////////////////////////////////////////////////////////
//
//      Create our VAOs & VBOs. Send vertex data to the GPU for future rendering
//
////////////////////////////////////////////////////////////////////////////////
void setupBuffers() {
	struct VertexTextured {
		float x, y, z;
		float s, t;
	};

	//////////////////////////////////////////
	//
	// Model
	tankBaseModel = new CSCI441::ModelLoader();
	tankBaseModel->loadModelFile( "models/tank/TankBase.obj" );
	tankTurretModel = new CSCI441::ModelLoader();
	tankTurretModel->loadModelFile( "models/tank/TankTurret.obj" );

	playerTank = new PlayerTank(tankBaseModel, tankTurretModel);

	//////////////////////////////////////////
	//
	// PLATFORM

	VertexTextured platformVertices[4] = {
			{ -platformSize, 0.0f, -platformSize,   0.0f,  0.0f }, // 0 - BL
			{  platformSize, 0.0f, -platformSize,   1.0f,  0.0f }, // 1 - BR
			{ -platformSize, 0.0f,  platformSize,   0.0f,  -1.0f }, // 2 - TL
			{  platformSize, 0.0f,  platformSize,   1.0f,  -1.0f }  // 3 - TR
	};

	unsigned short platformIndices[4] = { 0, 1, 2, 3 };

	glGenVertexArrays( 1, &platformVAOd );
	glBindVertexArray( platformVAOd );

	GLuint vbods[2];
	glGenBuffers( 2, vbods );

	glBindBuffer( GL_ARRAY_BUFFER, vbods[0] );
	glBufferData( GL_ARRAY_BUFFER, sizeof( platformVertices ), platformVertices, GL_STATIC_DRAW );

	glEnableVertexAttribArray( attrib_vPos_loc );
	glVertexAttribPointer( attrib_vPos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) 0 );

	glEnableVertexAttribArray( attrib_vTextureCoord_loc );
	glVertexAttribPointer( attrib_vTextureCoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) (sizeof(float) * 3) );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbods[1] );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( platformIndices ), platformIndices, GL_STATIC_DRAW );

	//////////////////////////////////////////
	//
	// SKYBOX


	// create our skybox vertex array
	VertexTextured skyboxVertices[36] = {
	{-skyboxDim,-skyboxDim,-skyboxDim, 0.5, 2.0/3 },//12 - left
    {-skyboxDim,-skyboxDim, skyboxDim, 0.25, 2.0/3 },//12 - left
    {-skyboxDim, skyboxDim, skyboxDim, 0.25, 1.0/3 },//12 - left
    {skyboxDim, skyboxDim,-skyboxDim , 0.75, 1.0/3 },//11 - front
    {-skyboxDim,-skyboxDim,-skyboxDim, 0.5, 2.0/3 },//11 - front
    {-skyboxDim, skyboxDim,-skyboxDim, 0.5, 1.0/3 },//11 - front
    {skyboxDim,-skyboxDim, skyboxDim , 0.25, 1.0 },//10 - bottom
    {-skyboxDim,-skyboxDim,-skyboxDim, 0.5, 2.0/3 },//10 - bottom
    {skyboxDim,-skyboxDim,-skyboxDim , 0.5, 1.0 },//10 - bottom
    {skyboxDim, skyboxDim,-skyboxDim , 0.75, 1.0/3 },//9 - front
    {skyboxDim,-skyboxDim,-skyboxDim , 0.75, 2.0/3 },//9 - front
    {-skyboxDim,-skyboxDim,-skyboxDim, 0.5, 2.0/3 },//9 - front
    {-skyboxDim,-skyboxDim,-skyboxDim, 0.5, 2.0/3 },//8 - left
    {-skyboxDim, skyboxDim, skyboxDim, 0.25, 1.0/3 },//8 - left
    {-skyboxDim, skyboxDim,-skyboxDim, 0.5, 1.0/3 },//8 - left
    {skyboxDim,-skyboxDim, skyboxDim , 0.25, 1.0 },//7 - bottom
    {-skyboxDim,-skyboxDim, skyboxDim, 0.25, 2.0/3 },//7 - bottom
    {-skyboxDim,-skyboxDim,-skyboxDim, 0.5, 2.0/3 },//7 - bottom
    {-skyboxDim, skyboxDim, skyboxDim, 0.25, 1.0/3 },//6 - back
    {-skyboxDim,-skyboxDim, skyboxDim, 0.25, 2.0/3 },//6 - back
    {skyboxDim,-skyboxDim, skyboxDim , 0, 2.0/3 },//6 - back
    {skyboxDim, skyboxDim, skyboxDim , 1.0, 1.0/3 },//5 - right
    {skyboxDim,-skyboxDim,-skyboxDim , 0.75, 2.0/3 },//5 - right
    {skyboxDim, skyboxDim,-skyboxDim , 0.75, 1.0/3 },//5 - right
    {skyboxDim,-skyboxDim,-skyboxDim , 0.75, 2.0/3 },//4 - right
    {skyboxDim, skyboxDim, skyboxDim , 1.0, 1.0/3 },//4 - right
    {skyboxDim,-skyboxDim, skyboxDim , 1.0, 2.0/3 },//4 - right
    {skyboxDim, skyboxDim, skyboxDim , 0.25, 0},//3 - top`
    {skyboxDim, skyboxDim,-skyboxDim , 0.5, 0 },//3 - top`
    {-skyboxDim, skyboxDim,-skyboxDim, 0.5, 1.0/3 },//3 - top`
    {skyboxDim, skyboxDim, skyboxDim , 0.25, 0 },//2 - top`
    {-skyboxDim, skyboxDim,-skyboxDim, 0.5, 1.0/3 },//2 - top`
    {-skyboxDim, skyboxDim, skyboxDim, 0.25, 1.0/3 },//2 - top`
    {skyboxDim, skyboxDim, skyboxDim , 0, 1.0/3 },//1 - back
    {-skyboxDim, skyboxDim, skyboxDim, 0.25, 1.0/3 },//1 - back 
    {skyboxDim,-skyboxDim, skyboxDim , 0, 2.0/3 } //1 - back
	};

	// create the index array
	unsigned short skyboxIndicies[36];
	for (int i = 0; i < 36; i++)
		skyboxIndicies[i] = i;

	// generate and bind the VAO
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);
	// generate and bind the array VBO
	GLuint arrayHandle;
	glGenBuffers(1, &arrayHandle);
	glBindBuffer(GL_ARRAY_BUFFER, arrayHandle);
	
	// send the vertex data to the GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	// state where the vertex position is located within our array data
	glEnableVertexAttribArray(attrib_vPos_loc);
	glVertexAttribPointer(attrib_vPos_loc, 
						  3,
						  GL_FLOAT,
						  GL_FALSE,
						  sizeof(VertexTextured),
						  (void*)0);

	// state where the texture coordinate is located within our array data
	glEnableVertexAttribArray( attrib_vTextureCoord_loc);
	glVertexAttribPointer(  attrib_vTextureCoord_loc,
							2,
							GL_FLOAT,
							GL_FALSE,
							sizeof(VertexTextured),
							(void*)(sizeof(float) * 3));

	// TODO #07: generate and bind the element array VBO.  send data to the GPU
	GLuint indexHandle;
	glGenBuffers(1, &indexHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexHandle);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndicies), skyboxIndicies, GL_STATIC_DRAW);


	//////////////////////////////////////////
	//
	// TEXTURED QUAD

	// LOOKHERE #1

	VertexTextured texturedQuadVerts[4] = {
			{ -1.0f, -1.0f, 0.0f,   0.0f,  0.0f }, // 0 - BL
			{  1.0f, -1.0f, 0.0f,   1.0f,  0.0f }, // 1 - BR
			{ -1.0f,  1.0f, 0.0f,   0.0f,  1.0f }, // 2 - TL
			{  1.0f,  1.0f, 0.0f,   1.0f,  1.0f }  // 3 - TR
	};

	unsigned short texturedQuadIndices[4] = { 0, 1, 2, 3 };

	glGenVertexArrays( 1, &texturedQuadVAO );
	glBindVertexArray( texturedQuadVAO );
	glGenBuffers(2, vbods);
	glBindBuffer( GL_ARRAY_BUFFER, vbods[0] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(texturedQuadVerts), texturedQuadVerts, GL_STATIC_DRAW );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbods[1] );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(texturedQuadIndices), texturedQuadIndices, GL_STATIC_DRAW );
	
	postprocessingShaderProgram->useProgram();
	glEnableVertexAttribArray(attrib_post_vpos_loc);
	glVertexAttribPointer(attrib_post_vpos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) 0);
	glEnableVertexAttribArray(attrib_post_vtex_loc);
	glVertexAttribPointer(attrib_post_vtex_loc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*) (sizeof(float) * 3));
}

// setupFramebuffer() //////////////////////////////////////////////////////////
//
//      Create and setup our framebuffer object for the two-pass rendering
//
////////////////////////////////////////////////////////////////////////////////

void setupFramebuffer() {
	// TODO #1 - Setup everything with the framebufferuffer
	//create and bind the frame buffer object
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//create and bind the render buffer object
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	//allocate storage for the render buffer
	// (target, internal format, buffer width, buffer height)
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebufferWidth, framebufferHeight);

	//attach the depth buffer to the frame buffer
	// (framebuffer target, where to attach the rbo, renderbuffer target, descriptor for our render buffer)
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//generate texture handle to store color values
	glGenTextures(1, &framebufferTextureHandle);
    glBindTexture(GL_TEXTURE_2D, framebufferTextureHandle);
    //allocate space for the texture
    glTexImage2D(GL_TEXTURE_2D,  	//texture target
    			 0,				 	//mipmap level
    			 GL_RGBA,			//internal formadfsaft
    			 framebufferWidth,	//width
    			 framebufferHeight, //height
    			 0,					//border
    			 GL_RGBA,			//format
    			 GL_UNSIGNED_BYTE,	//data type
    			 NULL);				//data - passing none
    //set min and mag filters and how to wrap S and T
	glTexParameteri( GL_TEXTURE_2D,  GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,  GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,  GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D,  GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE );
	
	//attach the texture to the framebuffer
	// (target, where to attach texture, texture target, texture handle, mipmap level)
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
						   framebufferTextureHandle, 0);

	//check contents to ensure correct functionality
	CSCI441::FramebufferUtils::printFramebufferStatusMessage(GL_FRAMEBUFFER);
	CSCI441::FramebufferUtils::printFramebufferInfo( GL_FRAMEBUFFER, fbo );
	
}
// setupLevels() //////////////////////////////////////////////////////////
//
//      Loads in NUM_LEVELS number of levels that are LEVEL_SIZE x LEVEL_SIZE text files
//		
//		LEGEND
//	    -----------------
//		P - player
//		E - enemy roamer
//		S - enemy sentry
//		b - wall block
//
////////////////////////////////////////////////////////////////////////////////
void setupLevels(){

	for (uint n = 0; n < NUM_LEVELS; n++){
		string levelFilePath = "levels/Level" + to_string(n+1) + ".txt";
		ifstream levelFile;
		levelFile.open(levelFilePath);
		if (levelFile.fail()){
			cerr << "LEVEL FILE: " << levelFilePath << " COULD NOT BE OPENED, ABORTING" << endl;
			exit(EXIT_FAILURE);
		}
		Level newLevel = Level(levelFile);
		levels.push_back(newLevel);
	}

	//levelCenteringOffset =  glm::vec3( -(Level::LEVEL_DIM_X * Level::BLOCK_DIM / 2.0f),
	//							        Level::BLOCK_DIM / 2.0f - 0.1f,
	//							       -(Level::LEVEL_DIM_Z * Level::BLOCK_DIM / 2.0f));

}

//******************************************************************************
//
// Game Controller functions - this is where we move and control game objects

void loadCurrentLevel(){
	//clear the current enemy vectors
	enemyRoamers.clear();
	//enemySentries.clear(); TODO

	//reset player position
	playerTank->setPosition(levels[currentLevel].getPlayerPos());
	playerTank->setBlockColliders(levels[currentLevel].getBlockPos(), Level::BLOCK_DIM);

	//create new enemies and set positions
	for (glm::vec3& eRPos: levels[currentLevel].getEnemyRoamerPos()){
		EnemyRoamerTank newRoamer(tankBaseModel, tankTurretModel, eRPos);
		newRoamer.setBlockColliders(levels[currentLevel].getBlockPos(), Level::BLOCK_DIM);
		enemyRoamers.push_back(newRoamer);
	}
	/*for (glm::vec3& eSPos: levels[currentLevel].getEnemySentryPos()){
		EnemyRoamerTank newSentry(tankBaseModel, tankTurretModel, eRPos);
		enemySentrys.push_back(newSentry);
	} TODO */
}

void moveHero(float tstep){
	//movement from key presses
	if (moveUp)    playerTank->moveForward(tstep);
	if (moveDown)  playerTank->moveBackward(tstep);
	if (moveRight) playerTank->rotateRight(tstep);
	if (moveLeft)  playerTank->rotateLeft(tstep);
}

void updateScene(float tstep){
	moveHero(tstep);
	for (EnemyRoamerTank& er: enemyRoamers){
		er.makeMovement(tstep, playerTank->getPosition());
	}

  for (int i = bullets.size() - 1; i >= 0; i--){
    bullets[i].moveForward(tstep);
    bool removeBullet = bullets[i].bounce(levels[currentLevel].getBlockPos());
    if (removeBullet) {
      bullets.erase(bullets.begin() + i);
    }
  }
	convertSphericalToCartesian();
}

//******************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

// renderScene() ///////////////////////////////////////////////////////////////
//
//		This method will contain all of the objects to be drawn.
//
////////////////////////////////////////////////////////////////////////////////
void renderScene( glm::mat4 viewMatrix, glm::mat4 projectionMatrix ) {
	glm::mat4 m, vp = projectionMatrix * viewMatrix;

	// draw the skybox
	textureShaderProgram->useProgram();
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &m[0][0]);
	glUniformMatrix4fv(uniform_viewProjetionMtx_loc, 1, GL_FALSE, &vp[0][0]);
	glUniform1ui(uniform_tex_loc, GL_TEXTURE0);
	glm::vec3 white(1,1,1);
	glUniform3fv( uniform_color_loc, 1, &white[0] );

	//draw the skybox
  	glBindVertexArray(skyboxVAO);
	glBindTexture(GL_TEXTURE_2D, skyboxTextureHandle);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

	// draw the platform
	glBindTexture( GL_TEXTURE_2D, platformTextureHandle );
	glBindVertexArray( platformVAOd );
	glDrawElements( GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0 );

	//draw the current level
	for (glm::vec3& blockPos: levels[currentLevel].getBlockPos()){
		m = glm::translate(glm::mat4(), blockPos  + levelCenteringOffset);
		glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &m[0][0]);
		CSCI441::drawSolidCube(Level::BLOCK_DIM);
	}


	//DRAW ENTITIES
	modelPhongShaderProgram->useProgram();
	glUniformMatrix4fv( uniform_phong_v_loc, 1, GL_FALSE, &viewMatrix[0][0] );
	glUniformMatrix4fv( uniform_phong_p_loc, 1, GL_FALSE, &projectionMatrix[0][0] );
	glUniform1i( uniform_phong_txtr_loc, 0 );

	// draw the player
        glBindTexture(GL_TEXTURE_2D, redTextureHandle);
	m = playerTank->getModelMatrix();
	glm::mat4 mv = viewMatrix * m;
	glm::mat4 nMtx = glm::transpose( glm::inverse( mv ) );
	glUniformMatrix4fv( uniform_phong_mv_loc, 1, GL_FALSE, &mv[0][0] );
	glUniformMatrix4fv( uniform_phong_norm_loc, 1, GL_FALSE, &nMtx[0][0] );
	playerTank->drawBase( attrib_phong_vpos_loc, attrib_phong_vnorm_loc, attrib_phong_vtex_loc,
					  uniform_phong_md_loc, uniform_phong_ms_loc, uniform_phong_s_loc, uniform_phong_ma_loc,
					  GL_TEXTURE0);

	m = playerTank->getTurretModelMatrix();
	mv = viewMatrix * m;
	nMtx = glm::transpose( glm::inverse( mv ) );
	glUniformMatrix4fv( uniform_phong_mv_loc, 1, GL_FALSE, &mv[0][0] );
	glUniformMatrix4fv( uniform_phong_norm_loc, 1, GL_FALSE, &nMtx[0][0] );
	playerTank->drawTurret( attrib_phong_vpos_loc, attrib_phong_vnorm_loc, attrib_phong_vtex_loc,
					  uniform_phong_md_loc, uniform_phong_ms_loc, uniform_phong_s_loc, uniform_phong_ma_loc,
					  GL_TEXTURE0);

	// draw the roamers
	for (EnemyRoamerTank& er: enemyRoamers){
                glBindTexture(GL_TEXTURE_2D, greenTextureHandle);
		m = er.getModelMatrix();
		glm::mat4 mv = viewMatrix * m;
		glm::mat4 nMtx = glm::transpose( glm::inverse( mv ) );
		glUniformMatrix4fv( uniform_phong_mv_loc, 1, GL_FALSE, &mv[0][0] );
		glUniformMatrix4fv( uniform_phong_norm_loc, 1, GL_FALSE, &nMtx[0][0] );
		glUniform1i( uniform_phong_txtr_loc, 0 );
		er.drawBase( attrib_phong_vpos_loc, attrib_phong_vnorm_loc, attrib_phong_vtex_loc,
					  uniform_phong_md_loc, uniform_phong_ms_loc, uniform_phong_s_loc, uniform_phong_ma_loc,
					  GL_TEXTURE0);
		m = er.getTurretModelMatrix();
		mv = viewMatrix * m;
		nMtx = glm::transpose( glm::inverse( mv ) );
		glUniformMatrix4fv( uniform_phong_mv_loc, 1, GL_FALSE, &mv[0][0] );
		glUniformMatrix4fv( uniform_phong_norm_loc, 1, GL_FALSE, &nMtx[0][0] );
		er.drawTurret( attrib_phong_vpos_loc, attrib_phong_vnorm_loc, attrib_phong_vtex_loc,
						  uniform_phong_md_loc, uniform_phong_ms_loc, uniform_phong_s_loc, uniform_phong_ma_loc,
						  GL_TEXTURE0);
	}

	// draw the bullets
	for (Bullet& b: bullets){
		m = b.getModelMatrix();
		glm::mat4 mv = viewMatrix * m;
		glm::mat4 nMtx = glm::transpose( glm::inverse( mv ) );
		glUniformMatrix4fv( uniform_phong_mv_loc, 1, GL_FALSE, &mv[0][0] );
		glUniformMatrix4fv( uniform_phong_norm_loc, 1, GL_FALSE, &nMtx[0][0] );
		glUniform1i( uniform_phong_txtr_loc, 0 );
		b.drawBullet( attrib_phong_vpos_loc, attrib_phong_vnorm_loc, attrib_phong_vtex_loc,
					  uniform_phong_md_loc, uniform_phong_ms_loc, uniform_phong_s_loc, uniform_phong_ma_loc,
					  GL_TEXTURE0);
		modelPhongShaderProgram->useProgram();
	}

	//TODO draw the sentries

}

///*****************************************************************************
//
// Our main function

// main() ///////////////////////////////////////////////////////////////
//
//		Really you should know what this is by now.
//
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] ) {
	// GLFW sets up our OpenGL context so must be done first
	GLFWwindow *window = setupGLFW();	// initialize all of the GLFW specific information releated to OpenGL and our window
	setupOpenGL();									// initialize all of the OpenGL specific information
	setupGLEW();									// initialize all of the GLEW specific information
	setupShaders();									// load our shaders into memory
	setupBuffers();									// load all our VAOs and VBOs into memory
	setupTextures();								// load all textures into memory
	setupFramebuffer();								// setup our framebuffer
	setupLevels();
	loadCurrentLevel();
	convertSphericalToCartesian();

	CSCI441::setVertexAttributeLocations( attrib_vPos_loc, -1, attrib_vTextureCoord_loc );
	// strange hack I need to make spheres draw - don't have time to investigate why..
	//it's a bug with my library
	CSCI441::drawSolidSphere( 1, 16, 16 );
	//  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
	//	until the user decides to close the window and quit the program.  Without a loop, the
	//	window will display once and then the program exits.

	srand(time(NULL));
	float start = 0;
	while( !glfwWindowShouldClose(window) ) {	// check if the window was instructed to be closed
		// Get the size of our window framebuffer.  Ideally this should be the same dimensions as our window, but
		// when using a Retina display the actual window can be larger than the requested window.  Therefore
		// query what the actual size of the window we are rendering to is.

		
		convertSphericalToCartesian();
		glfwGetFramebufferSize( window, &windowWidth, &windowHeight );
		
		/////////////////////////////
		// FIRST PASS
		/////////////////////////////
		// TODO #2

		//bind frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glViewport( 0, 0, framebufferWidth, framebufferHeight );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the framebuffer

		// set the projection matrix based on the window size
		// use a perspective projection that ranges
		// with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
		glm::mat4 projectionMatrix = glm::perspective( 45.0f, framebufferWidth / (float) framebufferHeight, 0.001f, 2000.0f );

		// set up our look at matrix to position our camera
		glm::mat4 viewMatrix = glm::lookAt( cameraOffset + playerTank->getPosition(), playerTank->getPosition(), upVector );

		//get the time elapsed after all the processing happened
		float timeStep = glfwGetTime() - start;
		updateScene(timeStep);
		start = glfwGetTime();
		
		// pass our view and projection matrices
		renderScene( viewMatrix, projectionMatrix );

		//flush buffers
		glFlush();

		/////////////////////////////
		// SECOND PASS
		/////////////////////////////
		// TODO #3

		//unbind the framebuffer
		postprocessingShaderProgram->useProgram();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport( 0, 0, windowWidth, windowHeight );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the framebuffer

		projectionMatrix = glm::ortho( -1, 1, -1, 1);
		glUniformMatrix4fv( uniform_post_proj_loc, 1, GL_FALSE, &projectionMatrix[0][0] );

   		glBindTexture(GL_TEXTURE_2D, framebufferTextureHandle);
		
   		glBindVertexArray(texturedQuadVAO);
   		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);
   		

		// Show it all on the screen now!

		glfwSwapBuffers(window);		// flush the OpenGL commands and make sure they get rendered!
		glfwPollEvents();				// check for any events and signal to redraw screen
		
	}

	delete tankBaseModel;
	delete tankTurretModel;

	glfwDestroyWindow( window );// clean up and close our window
	glfwTerminate();						// shut down GLFW to clean up our context

	return EXIT_SUCCESS;				// exit our program successfully!
}
