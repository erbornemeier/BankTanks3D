#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/modelLoader3.hpp>

#include <iostream>

class Bullet{

public:

  Bullet(glm::vec3 pos = glm::vec3(0,0,0),
         glm::vec3 dir = glm::vec3(1,0,1));


  void move(float tstep);

  glm::vec3 getPosition();

private:

  //physical properties about the model
  glm::vec3 position, direction;

  //transform matrix to handle translation, rotation, and scaling associated to the model
  glm::mat4 modelMatrix;

  const float SPEED = 15.0f;
};
