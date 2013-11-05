#include "MarkerModel.h"
#include "Scene.h"

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
MarkerModel::~MarkerModel() {
	if (rbdlModel) {
		delete rbdlModel;
		rbdlModel = NULL;
	}
}
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

		RigidBodyDynamics::Math::VectorNd q = RigidBodyDynamics::Math::VectorNd::Zero(rbdlModel->q_size);

		// Add joint scene object
		SceneObject joint_scene_object;
		joint_scene_object.name = body_name + "_joint";
		joint_scene_object.color = Vector3f (1.f, 0.f, 0.f);
	
		RigidBodyDynamics::Math::Vector3d rbdl_vec3 = CalcBodyToBaseCoordinates (*rbdlModel, q, body_id, RigidBodyDynamics::Math::Vector3d (0., 0., 0.));
	
		Vector3f joint_position (rbdl_vec3[0], rbdl_vec3[1], rbdl_vec3[2]);
		joint_scene_object.transformation.translation = joint_position;
		joint_scene_object.transformation.scaling = Vector3f (0.05, 0.05, 0.05);
		joint_scene_object.mesh = CreateUVSphere (8, 16);

		scene->objects.push_back (joint_scene_object);
		JointObject joint_object;
		joint_object.sceneObjectId = scene->objects.size() - 1;
		joint_object.rbdlJointId = body_id;

		joints.push_back (joint_object);
	}

	return true;
}

bool MarkerModel::saveToFile(const char* filename) {
	assert (0 && !"Not yet implemented!");	
	return false;
}
