#include "EnemyRoamerTank.h"

EnemyRoamerTank::EnemyRoamerTank(CSCI441::ModelLoader* tankBase, CSCI441::ModelLoader* tankTurret, 
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

void EnemyRoamerTank::moveForward(float tstep){

	position.z += SPEED * tstep * cos(baseRotation.y);
	if (isColliding())
		position.z -= SPEED * tstep * cos(baseRotation.y);

	position.x += SPEED * tstep * sin(baseRotation.y);
	if (isColliding())
		position.x -= SPEED * tstep * sin(baseRotation.y);

}

void EnemyRoamerTank::moveBackward(float tstep){
	position.z -= SPEED * tstep * cos(baseRotation.y);
	if (isColliding())
		position.z += SPEED * tstep * cos(baseRotation.y);

	position.x -= SPEED * tstep * sin(baseRotation.y);
	if (isColliding())
		position.x += SPEED * tstep * sin(baseRotation.y);
}

void EnemyRoamerTank::rotateRight(float tstep){
	baseRotation.y -= ROT_SPEED * tstep;
	turretRotation.y -= ROT_SPEED * tstep;
	if (isColliding()){
		baseRotation.y += ROT_SPEED * tstep;
		turretRotation.y += ROT_SPEED * tstep;
	}
	
}


void EnemyRoamerTank::rotateLeft(float tstep){
	baseRotation.y += ROT_SPEED * tstep;
	turretRotation.y += ROT_SPEED * tstep;
	if (isColliding()){
		baseRotation.y -= ROT_SPEED * tstep;
		turretRotation.y -= ROT_SPEED * tstep;
	}

}

bool EnemyRoamerTank::isColliding(){
	for (glm::vec3& blockPos: blockColliders){
		//if the block is too far away to collide, skip
		if (glm::length(blockPos - position) > 2.0f * blockSize) continue;

		//simple bounding box collision
		//TODO-EXTRA instead of the tankWidth/Length replace with bounding box length and width
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

void EnemyRoamerTank::makeMovement(float tstep, const glm::vec3& playerPos){
	
	playerLoc = playerPos;
	bool playerInSight = canSeePlayer();

	//TRACK THE PLAYER
	if (playerInSight){
		
		glm::vec3 toPlayerVec = glm::normalize(playerLoc - position);
		glm::vec3 enemyHeading = glm::vec3(sin(baseRotation.y), 0.f, cos(baseRotation.y));

		//TODO-EXTRA make it rotate by a fraction rather than constant
		if (glm::cross(toPlayerVec, enemyHeading).y <= 0)
			rotateLeft(tstep);
		else
			rotateRight(tstep);

		if (glm::length(playerLoc - position) > 10.0f){
			moveForward(tstep);
		}

	//MOVE RANDOMLY
	} else {
	
		//TODO move randomly
		timeToChangeMove += tstep;
		if (timeToChangeMove >= 1.0f){

			timeToChangeMove -= 1.0f;

			//Random translate selection
			int randTranslate = rand() % 10;
			if      (randTranslate <= 5) randTranslateDir = 0; //dont move
			else if (randTranslate <= 7) randTranslateDir = 1; //move forward
			else    					 randTranslateDir =-1; //move backwards

			//Random rotate selection
			int randRotate = rand() % 10;
			if      (randRotate <= 5) randRotateDir = 0; //dont move
			else if (randRotate <= 7) randRotateDir = 1; //rotate left
			else    				  randRotateDir =-1; //rotate right

		}

		//translation
		switch(randTranslateDir){
			case 0:
				break;
			case -1:
				moveBackward(tstep);
				break;
			case 1:
				moveForward(tstep);
		}
		//rotation
		switch(randRotateDir){
			case 0:
				break;
			case -1:
				rotateRight(tstep);
				break;
			case 1:
				rotateLeft(tstep);
		}

	}
}

bool EnemyRoamerTank::canSeePlayer(){
	for (glm::vec3& blockPos: blockColliders){
		if (lineRectCollide(blockPos)) return false;
	}
	return true;
}

bool EnemyRoamerTank::lineLineCollide(glm::vec3& a1, glm::vec3& a2, glm::vec3& b1, glm::vec3& b2){
	
	float denominator = ((a2.x - a1.x) * (b2.z - b1.z)) - ((a2.z - a1.z) * (b2.x - b1.x));
    float numerator1 = ((a1.z - b1.z) * (b2.x - b1.x)) - ((a1.x - b1.x) * (b2.z - b1.z));
    float numerator2 = ((a1.z - b1.z) * (a2.x - a1.x)) - ((a1.x - b1.x) * (a2.z - a1.z));

    // Detect coincident lines 
    if (denominator == 0) return numerator1 == 0 && numerator2 == 0;

    float r = numerator1 / denominator;
    float s = numerator2 / denominator;

    return (r >= 0 && r <= 1) && (s >= 0 && s <= 1);
}

bool EnemyRoamerTank::lineRectCollide(glm::vec3& blockPos){
	
	glm::vec3 topLeft = blockPos + glm::vec3(blockSize/2.0f, 0.0f, -blockSize/2.0f);
	glm::vec3 bottomRight = blockPos + glm::vec3(-blockSize/2.0f, 0.0f, blockSize/2.0f);

	if (lineLineCollide(position, playerLoc, topLeft, bottomRight)) return true;
	
	glm::vec3 topRight = blockPos + glm::vec3(blockSize/2.0f, 0.0f, blockSize/2.0f);
	glm::vec3 bottomLeft = blockPos + glm::vec3(-blockSize/2.0f, 0.0f, -blockSize/2.0f);

	if (lineLineCollide(position, playerLoc, topRight, bottomLeft)) return true;

	return false;

}

void EnemyRoamerTank::rotateTurretRight(float tstep){
	turretRotation.y -= ROT_SPEED * tstep;
}

void EnemyRoamerTank::rotateTurretLeft(float tstep){
	turretRotation.y += ROT_SPEED * tstep;
}

void EnemyRoamerTank::setBlockColliders(vector<glm::vec3>& blockPos, const float& blockDim){
	blockColliders = blockPos;
	blockSize = blockDim;
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