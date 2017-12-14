#include "Bullet.h"

Bullet::Bullet(CSCI441::ModelLoader* base, glm::vec3 pos, glm::vec3 dir, int maxBounces, float speed){
	this->base = base;

	this->position = pos;
	this->direction = dir;
  this->maxBounces = maxBounces;
	this->speed = speed;
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

void Bullet::bounce(glm::vec3 bouncePlane) {
  // update bounce counter
  // if counter is exceeded, return false to signal application to delete bullet

  // to bounce, flip along x or y plane depending on plane bouncing off

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
