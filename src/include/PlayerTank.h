#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/modelLoader3.hpp>

#include <iostream>

class PlayerTank{

public:

	PlayerTank(CSCI441::ModelLoader* tankBase, 
			   glm::vec3 pos = glm::vec3(0,0,0), 
			   glm::vec3 rot = glm::vec3(0,0,0),
			   glm::vec3 scl = glm::vec3(1,1,1));
	

	void moveForward(float tstep);
	void moveBackward(float tstep);
	void rotateLeft(float tstep);
	void rotateRight(float tstep);
	void setScale(glm::vec3 scale);

	void draw(GLint vpos_loc, GLint vnorm_loc, GLint vtex_loc, 
		      GLint md_loc, GLint ms_loc, GLint s_loc, GLint ma_loc, GLint txtr);

	glm::vec3 getPosition();
	glm::vec3 getBaseRotation();
	glm::mat4 getModelMatrix();

private:

	//physical properties about the model
	glm::vec3 position, baseRotation, turretRotation, scale; 

	//transform matrix to handle translation, rotation, and scaling associated to the model
	glm::mat4 modelMatrix;

	//storage location for the drawable model
	CSCI441::ModelLoader* model = NULL;

	const float SPEED = 5.0f, ROT_SPEED = 2.5f;
};  		