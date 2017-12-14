#include "Bullet.h"
#include "Level.h"

#include <stdlib.h>

Bullet::Bullet(CSCI441::ModelLoader* base, glm::vec3 pos, glm::vec3 dir, int maxBounces, float speed){
	this->base = base;

	this->position = pos;
	this->direction = dir;
  this->maxBounces = maxBounces;
	this->speed = speed;
  this->bounces = 0;
}

void Bullet::moveForward(float tstep){
	position.z += speed * tstep * cos(direction.y);

	position.x += speed * tstep * sin(direction.y);
}

void Bullet::drawBullet(GLint vpos_loc, GLint vnorm_loc, GLint vtex_loc,
		    GLint md_loc, GLint ms_loc, GLint s_loc, GLint ma_loc, GLint txtr){

	base->draw( vpos_loc, vnorm_loc, vtex_loc, md_loc, ms_loc, s_loc, ma_loc, txtr);
}

void Bullet::setPosition(glm::vec3 pos){
	position = pos;
}

bool Bullet::bounce(vector<glm::vec3>& wallLocations) {
  bool collisionDetected = false;
  bool leftRight = false;
  for (glm::vec3& wall : wallLocations) {
    if (position.x >= wall.x - Level::BLOCK_DIM / 2 && position.x <= wall.x + Level::BLOCK_DIM / 2
     && position.z >= wall.z - Level::BLOCK_DIM / 2 && position.z <= wall.z + Level::BLOCK_DIM / 2) {
      collisionDetected = true;
      if (abs(position.x - wall.x) > abs(position.z - wall.z)) {
        leftRight = true;
      }
      break;
    }
  }

  if (collisionDetected) {
    // update bounce counter
    bounces += 1;
    // if counter is exceeded, return true to signal application to delete bullet
    if (bounces > maxBounces) {
      return true;
    }

    if (leftRight) {
      direction.y = M_PI * 2 - direction.y;
      if (direction.y < 0) {
        direction.y += 2 * M_PI;
      }
    }
    else {
      direction.y = M_PI - direction.y;
      if (direction.y < 0) {
        direction.y += 2 * M_PI;
      }
    }
  }
  return false;
}

glm::mat4 Bullet::getModelMatrix(){
	glm::mat4 m;
	m = glm::translate(m, position);
	m = glm::rotate(m, direction.y, glm::vec3(0,1,0));
	return m;
}

glm::vec3 Bullet::getPosition(){
	return position;
}

glm::vec3 Bullet::getDirection(){
	return direction;
}
