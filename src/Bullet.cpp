#include "Bullet.h"

Bullet::Bullet(glm::vec3 pos, glm::vec3 dir){
  this->position = pos;
  this->direction = dir;
}

void Bullet::move(float tstep){
  position.z += SPEED * tstep * cos(direction.y);
  position.x += SPEED * tstep * sin(direction.y);
}

glm::vec3 Bullet::getPosition(){
  return position;
}
