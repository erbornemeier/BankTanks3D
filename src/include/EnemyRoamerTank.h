#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/modelLoader3.hpp>

#include <iostream>

class EnemyRoamerTank{

public:

	EnemyRoamerTank(CSCI441::ModelLoader* tankBase,
			   CSCI441::ModelLoader* tankTurret, 
			   glm::vec3 pos = glm::vec3(0,0,0), 
			   glm::vec3 rot = glm::vec3(0,0,0),
			   glm::vec3 scl = glm::vec3(1,1,1));
	

	void moveForward(float tstep);
	void moveBackward(float tstep);
	void rotateLeft(float tstep);
	void rotateRight(float tstep);
	void rotateTurretLeft(float timeStep);
	void rotateTurretRight(float timeStep);
	void setScale(glm::vec3 scale);

	void makeMovement(float tstep, const glm::vec3& playerPos);

	bool isColliding();
	void setBlockColliders(vector<glm::vec3>& blockPos, const float& blockDim);

	void drawBase(GLint vpos_loc, GLint vnorm_loc, GLint vtex_loc, 
		      GLint md_loc, GLint ms_loc, GLint s_loc, GLint ma_loc, GLint txtr);
	void drawTurret(GLint vpos_loc, GLint vnorm_loc, GLint vtex_loc, 
		      GLint md_loc, GLint ms_loc, GLint s_loc, GLint ma_loc, GLint txtr);

	glm::vec3 getPosition();
	glm::vec3 getBaseRotation();
	
	glm::mat4 getModelMatrix();
	glm::mat4 getTurretModelMatrix();

private:

	//physical properties about the model
	glm::vec3 position, baseRotation, turretRotation, scale; 

	//transform matrix to handle translation, rotation, and scaling associated to the model
	glm::mat4 modelMatrix;

	//colliders
	vector<glm::vec3> blockColliders;
	float blockSize;
	glm::vec3 playerLoc;

	//bounding box properties
	float tankLength, tankWidth;

	//storage location for the drawable model
	CSCI441::ModelLoader *base   = NULL,
						 *turret = NULL;


	const float SPEED = 10.0f, ROT_SPEED = 2.5f;
	float timeToChangeMove = 1.0f;
	int randTranslateDir = 0,
		randRotateDir    = 0;


	//enemy->player visibility check helper methods
	bool canSeePlayer();
	bool lineLineCollide(glm::vec3& a1, glm::vec3& a2, 
						 glm::vec3& b1, glm::vec3& b2);
	bool lineRectCollide(glm::vec3& blockPos);
};  		