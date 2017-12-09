#pragma once

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <vector>
#include <iostream>
using namespace std;

class Level{

	public:

		//constructors
		Level(ifstream& file);

		//getters
		glm::vec3& getPlayerPos();
		vector<glm::vec3>& getEnemyRoamerPos();
		vector<glm::vec3>& getEnemySentryPos();
		vector<glm::vec3>& getBlockPos();

		//constants
		const static uint BLOCK_DIM = 5;
		const static uint LEVEL_DIM_X = 32;
		const static uint LEVEL_DIM_Z = 16;

	private:

		//level attributes
		glm::vec3 playerPos;
		vector<glm::vec3> enemyRoamerPos;
		vector<glm::vec3> enemySentryPos;
		vector<glm::vec3> blocks;

};