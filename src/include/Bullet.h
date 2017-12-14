#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/modelLoader3.hpp>

#include <iostream>
#include <vector>
using namespace std;

class Bullet{

public:

	Bullet(CSCI441::ModelLoader* base,
			   glm::vec3 pos = glm::vec3(0,0,0),
			   glm::vec3 dir = glm::vec3(1,1,1),
         int maxBounces = 1, float speed = 20.0f);
	

	void moveForward(float tstep);

	bool isColliding();
	void setBlockColliders(vector<glm::vec3>& blockPos, const float& blockDim);

	void drawBullet(GLint vpos_loc, GLint vnorm_loc, GLint vtex_loc,
		      GLint md_loc, GLint ms_loc, GLint s_loc, GLint ma_loc, GLint txtr);

	void setPosition(glm::vec3 pos);
  void bounce(glm::vec3 bouncePlane);

	glm::vec3 getPosition();
	glm::vec3 getDirection();
	glm::mat4 getModelMatrix();

private:

	//physical properties about the model
	glm::vec3 position, direction;

	//colliders
	vector<glm::vec3> blockColliders;
	float blockSize;

	//transform matrix to handle translation, rotation, and scaling associated to the model
	glm::mat4 modelMatrix;

	//storage location for the drawable model
	CSCI441::ModelLoader *base   = NULL;

	float speed;
  int maxBounces;
};  		
