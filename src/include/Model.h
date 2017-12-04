#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/modelLoader3.hpp>
#include <CSCI441/ShaderProgram3.hpp>

#include <iostream>

class Model{

public:

	Model(char* modelFile, glm::vec3 pos = glm::vec3(0,0,0), glm::vec3 rot = glm::vec3(0,0,0));

	void draw(GLint vpos_loc, GLint vnorm_loc, GLint vtex_loc, 
		      GLint md_loc, GLint ms_loc, GLint s_loc, GLint ma_loc, GLint txtr);

private:

	//physical properties about the model
	glm::vec3 position, rotation; 
	//transform matrix to handle translation, rotation, and scaling associated to the model
	glm::mat4 modelMatrix;
	//storage location for the drawable model
	CSCI441::ModelLoader* modelLoader = NULL;

};