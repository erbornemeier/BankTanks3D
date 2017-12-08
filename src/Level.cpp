#include "Level.h"

Level::Level(ifstream& file){
	char temp;

	cout << "LEVEL LOADING.........." << endl;
	for (int i = 0; i < LEVEL_DIM_Z; i++) {
		for (int j = 0; j < LEVEL_DIM_X + 2; j++) { //need to add 2 to accept endline chars
			
			temp = file.get();
			cout << temp;

			switch(temp){

				case 'P': //player
					playerPos = glm::vec3( (float)j*BLOCK_DIM, 0.0f, (float)i*BLOCK_DIM );
					break;

				case 'E': //roaming enemy
					enemyRoamerPos.push_back( glm::vec3( (float)j*BLOCK_DIM, 0.0f, (float)i*BLOCK_DIM ));
					break; 

				case 'S': //sentry enemy
					enemySentryPos.push_back( glm::vec3( (float)j*BLOCK_DIM, 0.0f, (float)i*BLOCK_DIM ));
					break;

				case 'b'://block
					blocks.push_back( glm::vec3( (float)j*BLOCK_DIM, 0.0f, (float)i*BLOCK_DIM) );
					break;

			}
		}

	}
	cout << endl << endl;
}

vector<glm::vec3>& Level::getBlockPos(){
	return blocks;
}
