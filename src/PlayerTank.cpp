#include "PlayerTank.h"

PlayerTank::PlayerTank(CSCI441::ModelLoader* tankBase, CSCI441::ModelLoader* tankTurret, 
					   glm::vec3 pos, glm::vec3 rot, glm::vec3 scl){
	this->base = tankBase;
	this->turret = tankTurret;

	this->position = pos;
	this->baseRotation = rot;
	this->scale = scl;

	//TODO change these to more accurate values
	//right now they are equal to override the lack of a bounding box
	this->tankLength = 5.0f * scl.z;
	this->tankWidth  = 5.0f * scl.x;
}

void PlayerTank::moveForward(float tstep){
	position.z += SPEED * tstep * cos(baseRotation.y);
	if (isColliding()) 
		position.z -= SPEED * tstep * cos(baseRotation.y);

	position.x += SPEED * tstep * sin(baseRotation.y);
	if (isColliding())
		position.x -= SPEED * tstep * sin(baseRotation.y);
}

void PlayerTank::moveBackward(float tstep){
	position.z -= SPEED * tstep * cos(baseRotation.y);
	if (isColliding()) 
		position.z += SPEED * tstep * cos(baseRotation.y);

	position.x -= SPEED * tstep * sin(baseRotation.y);
	if (isColliding())
		position.x += SPEED * tstep * sin(baseRotation.y);
}

void PlayerTank::rotateRight(float tstep){
	baseRotation.y -= ROT_SPEED * tstep;
	turretRotation.y -= ROT_SPEED * tstep;

	if (isColliding()){
		baseRotation.y += ROT_SPEED * tstep;
		turretRotation.y += ROT_SPEED * tstep;
	}
}

void PlayerTank::rotateLeft(float tstep){
	baseRotation.y += ROT_SPEED * tstep;
	turretRotation.y += ROT_SPEED * tstep;

	if (isColliding()){
		baseRotation.y -= ROT_SPEED * tstep;
		turretRotation.y -= ROT_SPEED * tstep;
	}
}

void PlayerTank::rotateTurretRight(float tstep){
	turretRotation.y -= ROT_SPEED * tstep;
}

void PlayerTank::rotateTurretLeft(float tstep){
	turretRotation.y += ROT_SPEED * tstep;
}

bool PlayerTank::isColliding(){
	for (glm::vec3& blockPos: blockColliders){
		//if the block is too far away to collide, skip
		if (glm::length(blockPos - position) > 2.0f * blockSize) continue;

		//simple bounding box collision
		//TODO instead of the tankWidth/Length replace with bounding box length and width
		if ((position.x + tankWidth/2.0f) >=  (blockPos.x - blockSize/2.0f) && 
			(position.x - tankWidth/2.0f) <=  (blockPos.x + blockSize/2.0f) &&
			(position.z + tankLength/2.0f) >= (blockPos.z - blockSize/2.0f) && 
			(position.z - tankLength/2.0f) <= (blockPos.z + blockSize/2.0f)){
			return true;
		}
	}

	//no collisions
	return false;
}

void PlayerTank::setBlockColliders(vector<glm::vec3>& blockPos, const float& blockDim){
	blockColliders = blockPos;
	blockSize = blockDim;
}

void PlayerTank::setScale(glm::vec3 scale){
	this->scale = scale;
}

void PlayerTank::drawBase(GLint vpos_loc, GLint vnorm_loc, GLint vtex_loc, 
		    GLint md_loc, GLint ms_loc, GLint s_loc, GLint ma_loc, GLint txtr){

	base->draw( vpos_loc, vnorm_loc, vtex_loc, md_loc, ms_loc, s_loc, ma_loc, txtr);
}

void PlayerTank::drawTurret(GLint vpos_loc, GLint vnorm_loc, GLint vtex_loc, 
		    GLint md_loc, GLint ms_loc, GLint s_loc, GLint ma_loc, GLint txtr){

	turret->draw( vpos_loc, vnorm_loc, vtex_loc, md_loc, ms_loc, s_loc, ma_loc, txtr);
}

void PlayerTank::setPosition(glm::vec3 pos){
	position = pos;
}

glm::mat4 PlayerTank::getModelMatrix(){
	glm::mat4 m;
	m = glm::translate(m, position);
	m = glm::rotate(m, baseRotation.y, glm::vec3(0,1,0));
	m = glm::scale(m, scale);
	return m;
}

glm::mat4 PlayerTank::getTurretModelMatrix(){
	glm::mat4 m;
	m = glm::translate(m, position + glm::vec3(0,0.1,0));
	m = glm::rotate(m, turretRotation.y, glm::vec3(0,1,0));
	m = glm::scale(m, scale);
	return m;
}

glm::vec3 PlayerTank::getPosition(){
	return position;
}

glm::vec3 PlayerTank::getBaseRotation(){
	return baseRotation;
}
