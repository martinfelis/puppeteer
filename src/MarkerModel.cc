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

	bool verbose = false;
	typedef map<string, unsigned int> StringIntMap;
	StringIntMap body_table_id_map;

	LuaTable model_table = LuaTable::fromFile (filename);
	
	if (model_table["gravity"].exists()) {
		rbdlModel->gravity = model_table["gravity"].get<RigidBodyDynamics::Math::Vector3d>();

		if (verbose)
			cout << "gravity = " << rbdlModel->gravity.transpose() << endl;
	}

	int frame_count = model_table["frames"].length();

	body_table_id_map["ROOT"] = 0;

	for (int i = 1; i <= frame_count; i++) {
		if (!model_table["frames"][i]["parent"].exists()) {
			cerr << "Parent not defined for frame " << i << "." << endl;
			abort();
		}

		string body_name = model_table["frames"][i]["name"].getDefault<string>("");
		string parent_name = model_table["frames"][i]["parent"].get<string>();
		unsigned int parent_id = body_table_id_map[parent_name];

		SpatialTransform joint_frame = model_table["frames"][i]["joint_frame"].getDefault(SpatialTransform());
		Joint joint = model_table["frames"][i]["joint"].getDefault(Joint(JointTypeFixed));
		Body body = model_table["frames"][i]["body"];

		unsigned int body_id = rbdlModel->AddBody (parent_id, joint_frame, joint, body, body_name);
		body_table_id_map[body_name] = body_id;

		if (verbose) {
			cout << "==== Added Body ====" << endl;
			cout << "  body_name  : " << body_name << endl;
			cout << "  body id    : " << body_id << endl;
			cout << "  parent_id  : " << parent_id << endl;
			cout << "  joint dofs : " << joint.mDoFCount << endl;
			for (unsigned int j = 0; j < joint.mDoFCount; j++) {
				cout << "    " << j << ": " << joint.mJointAxes[j].transpose() << endl;
			}
			cout << "  joint_frame: " << joint_frame << endl;
			cout << "  body inertia: " << endl << body.mSpatialInertia << endl;
		}
	}

	return true;
}

bool MarkerModel::saveToFile(const char* filename) {
	assert (0 && !"Not yet implemented!");	
	return false;
}
