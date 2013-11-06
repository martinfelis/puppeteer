#include "MarkerModel.h"
#include "Scene.h"

#include <assert.h>
#include <iostream>

#include <rbdl/rbdl.h>
#include <rbdl/addons/luamodel/luamodel.h>

#include "LuaTypes.h"
#include "objloader.h"

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

			// Add joint scene object
		SceneObject joint_scene_object;

		joint_scene_object.name = body_name + "_joint";
		joint_scene_object.color = Vector3f (1.f, 0.f, 0.f);
	
		RigidBodyDynamics::Math::VectorNd q = RigidBodyDynamics::Math::VectorNd::Zero(rbdlModel->q_size);
		RigidBodyDynamics::Math::Vector3d rbdl_vec3 = CalcBodyToBaseCoordinates (*rbdlModel, q, body_id, RigidBodyDynamics::Math::Vector3d (0., 0., 0.));
	
		Vector3f joint_position (rbdl_vec3[0], rbdl_vec3[1], rbdl_vec3[2]);
		joint_scene_object.transformation.translation = joint_position;
		joint_scene_object.transformation.scaling = Vector3f (0.05, 0.05, 0.05);
		joint_scene_object.mesh = CreateUVSphere (8, 16);
		joint_scene_object.noDepthTest = true;

		JointObject joint_object;
		joint_object.sceneObjectId = scene->registerSceneObject (joint_scene_object);
		joint_object.rbdlJointId = body_id;

		joints.push_back (joint_object);

		// add visuals
		std::vector<VisualsObject> visuals;
		for (size_t vi = 0; vi < model_table["frames"][i]["visuals"].length(); vi++) {
			VisualsObject visual_object = model_table["frames"][i]["visuals"][vi + 1];

			assert ((visual_object.scale - Vector3f (-1.f, -1.f, -1.f)).squaredNorm() < 1.0e-5 && "visuals.scale not (yet) supported!");
			assert ((visual_object.translate - Vector3f (-1.f, -1.f, -1.f)).squaredNorm() < 1.0e-5 && "visuals.translate not (yet) supported!");

			// setup of the scene object
			SceneObject visual_scene_object;
			visual_scene_object.color = visual_object.color;

			MeshVBO mesh;
			load_obj (mesh, visual_object.src.c_str());	
			visual_scene_object.mesh = mesh;

			// setup of the transformation
			Transformation object_transformation = joint_scene_object.transformation;
			Vector3f bbox_size (mesh.bbox_max - mesh.bbox_min);
			Vector3f scale (
					fabs(visual_object.dimensions[0]) / bbox_size[0],
					fabs(visual_object.dimensions[1]) / bbox_size[1],
					fabs(visual_object.dimensions[2]) / bbox_size[2]
					);

			object_transformation.scaling = scale;

			object_transformation.translation = object_transformation.translation + object_transformation.rotation.rotate (visual_object.mesh_center);
			
			visual_scene_object.transformation = object_transformation;

			// register scene object
			visual_object.sceneObjectId = scene->registerSceneObject (visual_scene_object);
			visuals.push_back(visual_object);
		}
	}

	return true;
}

bool MarkerModel::saveToFile(const char* filename) {
	assert (0 && !"Not yet implemented!");	
	return false;
}
