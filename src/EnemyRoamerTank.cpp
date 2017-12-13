#include "EnemyRoamerTank.h"

EnemyRoamerTank::EnemyRoamerTank(CSCI441::ModelLoader* tankBase, CSCI441::ModelLoader* tankTurret, 
					   glm::vec3 pos, glm::vec3 rot, glm::vec3 scl){
	this->base = tankBase;
	this->turret = tankTurret;

	this->position = pos;
	this->baseRotation = rot;
	this->scale = scl;
}

void EnemyRoamerTank::moveForward(float tstep){
	position.z += SPEED * tstep * cos(baseRotation.y);
	position.x += SPEED * tstep * sin(baseRotation.y);
}

void EnemyRoamerTank::moveBackward(float tstep){
	position.z -= SPEED * tstep * cos(baseRotation.y);
	position.x -= SPEED * tstep * sin(baseRotation.y);
}

void EnemyRoamerTank::rotateRight(float tstep){
	baseRotation.y -= ROT_SPEED * tstep;
	turretRotation.y -= ROT_SPEED * tstep;
}

void EnemyRoamerTank::rotateLeft(float tstep){
	baseRotation.y += ROT_SPEED * tstep;
	turretRotation.y += ROT_SPEED * tstep;

}

void EnemyRoamerTank::makeMovement(float tstep){
	bool playerInSight = canSeePlayer();

	if (playerInSight){
		//TODO go towards the player
	}
	else {
		//TODO move randomly
	}
}

bool EnemyRoamerTank::canSeePlayer(){
	//TODO
	return false;
}

bool EnemyRoamerTank::lineLineCollide(){
	//TODO
	return false;
}

bool EnemyRoamerTank::lineRectCollide(){
	//TODO
	return false;
}

void EnemyRoamerTank::rotateTurretRight(float tstep){
	turretRotation.y -= ROT_SPEED * tstep;
}

void EnemyRoamerTank::rotateTurretLeft(float tstep){
	turretRotation.y += ROT_SPEED * tstep;
}

void EnemyRoamerTank::setScale(glm::vec3 scale){
	this->scale = scale;
}

void EnemyRoamerTank::drawBase(GLint vpos_loc, GLint vnorm_loc, GLint vtex_loc, 
		    GLint md_loc, GLint ms_loc, GLint s_loc, GLint ma_loc, GLint txtr){

	base->draw( vpos_loc, vnorm_loc, vtex_loc, md_loc, ms_loc, s_loc, ma_loc, txtr);
}

void EnemyRoamerTank::drawTurret(GLint vpos_loc, GLint vnorm_loc, GLint vtex_loc, 
		    GLint md_loc, GLint ms_loc, GLint s_loc, GLint ma_loc, GLint txtr){

	turret->draw( vpos_loc, vnorm_loc, vtex_loc, md_loc, ms_loc, s_loc, ma_loc, txtr);
}

glm::mat4 EnemyRoamerTank::getModelMatrix(){
	glm::mat4 m;
	m = glm::scale(m, scale);
	m = glm::translate(m, position);
	m = glm::rotate(m, baseRotation.y, glm::vec3(0,1,0));
	return m;
}

glm::mat4 EnemyRoamerTank::getTurretModelMatrix(){
	glm::mat4 m;
	m = glm::scale(m, scale);
	m = glm::translate(m, position + glm::vec3(0,0.1,0));
	m = glm::rotate(m, turretRotation.y, glm::vec3(0,1,0));
	return m;
}

glm::vec3 EnemyRoamerTank::getPosition(){
	return position;
}

glm::vec3 EnemyRoamerTank::getBaseRotation(){
	return baseRotation;
}