#include "PlayerTank.h"

PlayerTank::PlayerTank(CSCI441::ModelLoader* tankBase, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl){
	this->model = tankBase;

	this->position = pos;
	this->rotation = rot;
	this->scale = scl;
}

void PlayerTank::moveForward(float tstep){
	position.z += SPEED * tstep * cos(rotation.y);
	position.x += SPEED * tstep * sin(rotation.y);
}

void PlayerTank::moveBackward(float tstep){
	position.z -= SPEED * tstep * cos(rotation.y);
	position.x -= SPEED * tstep * sin(rotation.y);
}

void PlayerTank::rotateRight(float tstep){
	rotation.y += ROT_SPEED * tstep;
}

void PlayerTank::rotateLeft(float tstep){
	rotation.y -= ROT_SPEED * tstep;
}

void PlayerTank::setScale(glm::vec3 scale){
	this->scale = scale;
}

void PlayerTank::draw(GLint vpos_loc, GLint vnorm_loc, GLint vtex_loc, 
		    GLint md_loc, GLint ms_loc, GLint s_loc, GLint ma_loc, GLint txtr){

	model->draw( vpos_loc, vnorm_loc, vtex_loc, md_loc, ms_loc, s_loc, ma_loc, txtr);
}

glm::mat4 PlayerTank::getModelMatrix(){
	glm::mat4 m;
	m = glm::scale(m, scale);
	m = glm::rotate(m, rotation.y, glm::vec3(0,1,0));
	m = glm::translate(m, position);
	return m;
}

glm::vec3 PlayerTank::getPosition(){
	return position;
}

glm::vec3 PlayerTank::getRotation(){
	return rotation;
}