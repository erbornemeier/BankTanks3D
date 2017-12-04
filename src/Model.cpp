#include "Model.h"

Model::Model(char* modelFile, glm::vec3 pos, glm::vec3 rot){
	//this->modelLoader = new CSCI441::ModelLoader();
	//this->modelLoader->loadModelFile(modelFile);

	this->position = pos;
	this->rotation = rot;
}

void Model::draw(GLint vpos_loc, GLint vnorm_loc, GLint vtex_loc, 
		    GLint md_loc, GLint ms_loc, GLint s_loc, GLint ma_loc, GLint txtr){

	modelLoader->draw( vpos_loc, vnorm_loc, vtex_loc, md_loc, ms_loc, s_loc, ma_loc, txtr);
}