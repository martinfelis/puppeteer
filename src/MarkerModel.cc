#include "MarkerModel.h"

#include <assert.h>
#include <iostream>

#include <rbdl/rbdl.h>
#include <rbdl/addons/luamodel/luamodel.h>

#include "LuaTypes.h"

using namespace std;
using namespace RigidBodyDynamics;

typedef RigidBodyDynamics::Math::SpatialTransform SpatialTransform;

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
	assert (rbdlModel == NULL);
	rbdlModel = new RigidBodyDynamics::Model;

	LuaTable model_table = LuaTable::fromFile (filename);
	
	if (model_table["gravity"].exists()) {
		rbdlModel->gravity = model_table["gravity"].get<RigidBodyDynamics::Math::Vector3d>();
	}

	int frame_count = model_table["frames"].length();

	for (int i = 1; i <= frame_count; i++) {
		if (!model_table["frames"][i]["parent"].exists()) {
			cerr << "Parent not defined for frame " << i << "." << endl;
			abort();
		}

		string body_name = model_table["frames"][i]["name"].getDefault<string>("");
		string parent_name = model_table["frames"][i]["parent"].get<string>();
		unsigned int parent_id = rbdlModel->GetBodyId(parent_name.c_str());
		if (parent_id == std::numeric_limits<unsigned int>::max()) {
			cerr << "Error: could not find parent body with name '" << parent_name << "'!" << endl;
			abort();
			return false;
		}

		SpatialTransform joint_frame = model_table["frames"][i]["joint_frame"].getDefault(SpatialTransform());
		Joint joint = model_table["frames"][i]["joint"].getDefault(Joint(JointTypeFixed));
		Body body = model_table["frames"][i]["body"];

		unsigned int body_id = rbdlModel->AddBody (parent_id, joint_frame, joint, body, body_name);
	}

	return true;
}

bool MarkerModel::saveToFile(const char* filename) {
	assert (0 && !"Not yet implemented!");	
	return false;
}
