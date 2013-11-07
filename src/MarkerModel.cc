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

typedef RigidBodyDynamics::Math::VectorNd RBDLVectorNd;
typedef RigidBodyDynamics::Math::Vector3d RBDLVector3d;
typedef RigidBodyDynamics::Math::Matrix3d RBDLMatrix3d;

typedef RigidBodyDynamics::Math::SpatialTransform SpatialTransform;

MarkerModel::MarkerModel(const MarkerModel &model) :
	scene(model.scene),
	rbdlModel(new RigidBodyDynamics::Model(*model.rbdlModel)),
	joints(model.joints),
	visuals(model.visuals)
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
		joints = model.joints;
		visuals = model.visuals;
	}

	return *this;
}

void MarkerModel::updateModelState() {
	RBDLVectorNd Q (rbdlModel->q_size);
	for (size_t i = 0; i < rbdlModel->q_size; i++) {
		Q[i] = generalizedPositions[i];
	}
	RigidBodyDynamics::UpdateKinematicsCustom (*rbdlModel, &Q, NULL, NULL);
}

unsigned int MarkerModel::getFrameIdFromObjectId (int object_id) {
	for (size_t i = 0; i < visuals.size(); i++) {
		if (visuals[i].sceneObjectId == object_id)
			return visuals[i].rbdlFrameId;
	}

	for (size_t i = 0; i < joints.size(); i++) {
		if (joints[i].sceneObjectId == object_id)
			return joints[i].rbdlFrameId;
	}

	cerr << "Could not find frame id for object id " << object_id << endl;
	abort();
	
	return 0;
}

std::string MarkerModel::getFrameName (unsigned int frame_id) {
	return rbdlModel->GetBodyName(frame_id);
}

Vector3f MarkerModel::getFrameLocationGlobal (unsigned int frame_id) {
	updateModelState();

	RBDLVectorNd Q (rbdlModel->q_size);

	RBDLVector3d position = RigidBodyDynamics::CalcBodyToBaseCoordinates (*rbdlModel, Q, frame_id, RBDLVector3d( 0., 0., 0.), false);

	return Vector3f (position[0], position[1], position[2]);
}

Vector3f MarkerModel::getFrameOrientationGlobalEulerYXZ (unsigned int frame_id) {
	updateModelState();

	RBDLVectorNd Q (rbdlModel->q_size);
	RBDLMatrix3d rbdl_orientation = RigidBodyDynamics::CalcBodyWorldOrientation (*rbdlModel, Q, frame_id, false);

	Matrix33f orientation;
	
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			orientation(i,j) = rbdl_orientation(i,j);
		}
	}

	return SimpleMath::GL::Quaternion::fromMatrix(orientation).toEulerYXZ();
}

Vector3f MarkerModel::getJointLocationLocal (unsigned int frame_id) {
	SpatialTransform transform = rbdlModel->GetJointFrame (frame_id);
	return Vector3f (transform.r[0], transform.r[1], transform.r[2]);
}

Vector3f MarkerModel::getJointOrientationLocalEulerYXZ (unsigned int frame_id) {
	SpatialTransform transform = rbdlModel->GetJointFrame (frame_id);
	Matrix33f orientation;
	
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			orientation(i,j) = transform.E(i,j);
		}
	}
	return SimpleMath::GL::Quaternion::fromMatrix(orientation).toEulerYXZ();
}

void MarkerModel::setJointLocation (unsigned int frame_id, const Vector3f &location) {
	assert (0);
}

void MarkerModel::setJointOrientationEulerYXZ (unsigned int frame_id, const Vector3f &yxz_euler) {
	assert (0);
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
		joint_scene_object.color = Vector3f (0.9f, 0.9f, 0.9f);
	
		RigidBodyDynamics::Math::VectorNd q = RigidBodyDynamics::Math::VectorNd::Zero(rbdlModel->q_size);
		RigidBodyDynamics::Math::Vector3d rbdl_vec3 = CalcBodyToBaseCoordinates (*rbdlModel, q, body_id, RigidBodyDynamics::Math::Vector3d (0., 0., 0.));
	
		Vector3f joint_position (rbdl_vec3[0], rbdl_vec3[1], rbdl_vec3[2]);
		joint_scene_object.transformation.translation = joint_position;
		joint_scene_object.transformation.scaling = Vector3f (0.03, 0.03, 0.03);
		joint_scene_object.mesh = CreateUVSphere (8, 16);
		joint_scene_object.noDepthTest = true;

		JointObject joint_object;
		joint_object.sceneObjectId = scene->registerSceneObject (joint_scene_object);
		joint_object.rbdlFrameId = body_id;

		joints.push_back (joint_object);

		// add visuals
		for (size_t vi = 0; vi < model_table["frames"][i]["visuals"].length(); vi++) {
			VisualsObject visual_object = model_table["frames"][i]["visuals"][vi + 1];
			visual_object.rbdlFrameId = body_id;

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

	generalizedPositions = SimpleMath::Dynamic::Matrix<double>::Zero (rbdlModel->q_size);

	return true;
}
