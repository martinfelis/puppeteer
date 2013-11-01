#include "MarkerModel.h"

#include <assert.h>
#include <rbdl/rbdl.h>

MarkerModel::MarkerModel(const MarkerModel &model) :
	scene(model.scene),
	rbdlModel(new RigidBodyDynamics::Model(*model.rbdlModel)),
	bodies(model.bodies),
	joints(model.joints),
	jointTransforms(model.jointTransforms)
{}
MarkerModel& MarkerModel::operator=(const MarkerModel &model) {
	if (this != &model) {
		scene = model.scene;
		rbdlModel = new RigidBodyDynamics::Model(*model.rbdlModel);
		bodies = model.bodies;
		joints = model.joints;
		jointTransforms = model.jointTransforms;
	}

	return *this;
}

bool MarkerModel::loadFromFile(const char* filename) {
	assert (0 && !"Not yet implemented!");	
	return false;
}

bool MarkerModel::saveToFile(const char* filename) {
	assert (0 && !"Not yet implemented!");	
	return false;
}
