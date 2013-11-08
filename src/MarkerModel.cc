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

Matrix33f ConvertToSimpleMathMat3 (const RBDLMatrix3d &mat) {
	Matrix33f result;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result (i,j) = mat(i,j);
		}
	}
	return result;
}

Vector3f ConvertToSimpleMathVec3 (const RBDLVector3d &vec) {
	Vector3f result;
	for (int i = 0; i < 3; i++) {
			result [i] = vec[i];
	}
	return result;
}

MarkerModel::~MarkerModel() {
	if (rbdlModel) {
		delete rbdlModel;
		rbdlModel = NULL;
	}

	if (luaTable) {
		delete luaTable;
		luaTable = NULL;
	}
}

VectorNd MarkerModel::getModelState() {
	return modelStateQ;
}

std::vector<std::string> MarkerModel::getModelStateNames () {
	std::vector<std::string> result (rbdlModel->q_size, "unnamed");

	return result;
}

void MarkerModel::updateModelState() {
	RBDLVectorNd Q (rbdlModel->q_size);
	for (size_t i = 0; i < rbdlModel->q_size; i++) {
		Q[i] = modelStateQ[i];
	}
	RigidBodyDynamics::UpdateKinematicsCustom (*rbdlModel, &Q, NULL, NULL);
}

void MarkerModel::setModelStateValue (unsigned int state_index, double value) {
	assert (state_index < rbdlModel->q_size);

	cout << "setting value " << state_index << endl;

	modelStateQ[state_index] = value;

	updateModelState();
	updateSceneObjects();
}

void MarkerModel::updateSceneObjects() {
	for (size_t i = 0; i < visuals.size(); i++) {
		unsigned int rbdl_id = luaToRbdlId[visuals[i]->luaFrameId];
	}
}

int MarkerModel::getFrameIdFromObjectId (int object_id) {
	for (size_t i = 0; i < visuals.size(); i++) {
		if (visuals[i]->id == object_id)
			return visuals[i]->luaFrameId;
	}

	for (size_t i = 0; i < joints.size(); i++) {
		if (joints[i]->id == object_id)
			return joints[i]->luaFrameId;
	}

	return 0;
}

int MarkerModel::getParentFrameId(int frame_id) {
	if (frame_id == 0)
		return 0;

	string parent_frame_name = (*luaTable)["frames"][frame_id]["parent"].getDefault<string>("ROOT");

	for (int i = 0; i < (*luaTable)["frames"].length(); i++) {
		if ( parent_frame_name == (*luaTable)["frames"][i]["name"].getDefault<string>("ROOT")) {
			return i;
		}
	}

	cerr << "Error: could not determine parent for frame with id '" << frame_id << "'." << endl;
	abort();
}

int MarkerModel::getObjectIdFromFrameId (int frame_id) {
	for (size_t i = 0; i < visuals.size(); i++) {
		if (visuals[i]->luaFrameId == frame_id)
			return visuals[i]->id;
	}

	for (size_t i = 0; i < joints.size(); i++) {
		if (joints[i]->luaFrameId == frame_id)
			return joints[i]->id;
	}

	cerr << "Could not find object id for frame id " << frame_id << endl;
	abort();
	
	return 0;
}


std::string MarkerModel::getFrameName (int frame_id) {
	return rbdlModel->GetBodyName(luaToRbdlId[frame_id]);
}

std::string MarkerModel::getParentName (int frame_id) {
	return (*luaTable)["frames"][frame_id]["parent"].getDefault<string>("ROOT");
}

Vector3f MarkerModel::getFrameLocationGlobal (int frame_id) {
	updateModelState();

	RBDLVectorNd Q (rbdlModel->q_size);

	RBDLVector3d position = RigidBodyDynamics::CalcBodyToBaseCoordinates (*rbdlModel, Q, luaToRbdlId[frame_id], RBDLVector3d( 0., 0., 0.), false);

	return Vector3f (position[0], position[1], position[2]);
}

Vector3f MarkerModel::getFrameOrientationGlobalEulerYXZ (int frame_id) {
	updateModelState();

	RBDLVectorNd Q (rbdlModel->q_size);
	RBDLMatrix3d rbdl_orientation = RigidBodyDynamics::CalcBodyWorldOrientation (*rbdlModel, Q, luaToRbdlId[frame_id], false);

	Matrix33f orientation;
	
	for (size_t i = 0; i < 3; i++) {
		for (size_t j = 0; j < 3; j++) {
			orientation(i,j) = rbdl_orientation(i,j);
		}
	}

	return SimpleMath::GL::Quaternion::fromMatrix(orientation).toEulerYXZ();
}

Vector3f MarkerModel::getJointLocationLocal (int frame_id) {
	return (*luaTable)["frames"][frame_id]["joint_frame"]["r"].getDefault<Vector3f>(Vector3f::Zero());
}

Vector3f MarkerModel::getJointOrientationLocalEulerYXZ (int frame_id) {
	SpatialTransform transform = rbdlModel->GetJointFrame (luaToRbdlId[frame_id]);
	Matrix33f orientation = (*luaTable)["frames"][frame_id]["joint_frame"]["E"].getDefault<Matrix33f>(Matrix33f::Identity(3,3));
	
	return SimpleMath::GL::Quaternion::fromMatrix(orientation).toEulerYXZ();
}

void MarkerModel::adjustParentVisualsScale (int frame_id, const Vector3f &delta_r) {
	int parent_id = getParentFrameId (frame_id);
	
	if (parent_id == 0)
		return;

	for (int i = 0; i < (*luaTable)["frames"][parent_id]["visuals"].length(); i++) {
		Vector3f dimensions = (*luaTable)["frames"][parent_id]["visuals"][i + 1]["dimensions"];
		Vector3f mesh_center = (*luaTable)["frames"][parent_id]["visuals"][i + 1]["mesh_center"];
		mesh_center = mesh_center + delta_r * 0.5f;
		dimensions = dimensions + delta_r;
		(*luaTable)["frames"][parent_id]["visuals"][i + 1]["dimensions"] = dimensions;
		(*luaTable)["frames"][parent_id]["visuals"][i + 1]["mesh_center"] = mesh_center;
	}
}

void MarkerModel::setJointLocationLocal (int frame_id, const Vector3f &location) {
	Vector3f old_location = (*luaTable)["frames"][frame_id]["joint_frame"]["r"];
	(*luaTable)["frames"][frame_id]["joint_frame"]["r"] = location;

	adjustParentVisualsScale (frame_id, location - old_location);
	
	updateFromLua();
}

void MarkerModel::setJointOrientationLocalEulerYXZ (int frame_id, const Vector3f &yxz_euler) {
	Matrix33f matrix = SimpleMath::GL::Quaternion::fromEulerYXZ(yxz_euler).toMatrix().transpose();
	(*luaTable)["frames"][frame_id]["joint_frame"]["E"] = matrix;

	updateFromLua();
}

void MarkerModel::clearModel() {
	delete rbdlModel;
	rbdlModel = new RigidBodyDynamics::Model();

	for (size_t i = 0; i < joints.size(); i++) {
		scene->unregisterSceneObject (joints[i]->id);
	}
	joints.clear();

	for (size_t i = 0; i < visuals.size(); i++) {
		scene->unregisterSceneObject (visuals[i]->id);
	}
	visuals.clear();

	rbdlToLuaId.clear();
	luaToRbdlId.clear();
}

void MarkerModel::updateFromLua() {
	int selected_frame = -1;
	if (isModelObject (scene->selectedObjectId)) {
		selected_frame = getFrameIdFromObjectId (scene->selectedObjectId);
	}

	clearModel();

	assert (luaTable->L);

	if ((*luaTable)["gravity"].exists()) {
		rbdlModel->gravity = (*luaTable)["gravity"].get<RigidBodyDynamics::Math::Vector3d>();
	}

	int frame_count = (*luaTable)["frames"].length();

	for (int i = 1; i <= frame_count; i++) {
		if (!(*luaTable)["frames"][i]["parent"].exists()) {
			cerr << "Parent not defined for frame " << i << "." << endl;
			abort();
		}

		string body_name = (*luaTable)["frames"][i]["name"].getDefault<string>("");
		string parent_name = (*luaTable)["frames"][i]["parent"].get<string>();
		unsigned int parent_id = rbdlModel->GetBodyId(parent_name.c_str());
		if (parent_id == std::numeric_limits<unsigned int>::max()) {
			cerr << "Error: could not find parent body with name '" << parent_name << "'!" << endl;
			abort();
		}

		SpatialTransform joint_frame = (*luaTable)["frames"][i]["joint_frame"].getDefault(SpatialTransform());
		Joint joint = (*luaTable)["frames"][i]["joint"].getDefault(Joint(JointTypeFixed));
		Body body = (*luaTable)["frames"][i]["body"];

		unsigned int rbdl_id = rbdlModel->AddBody (parent_id, joint_frame, joint, body, body_name);
		luaToRbdlId[i] = rbdl_id;
		rbdlToLuaId[rbdl_id] = i;

		// Add joint scene object
		JointObject *joint_scene_object = scene->createObject<JointObject>();

		joint_scene_object->color = Vector3f (0.9f, 0.9f, 0.9f);
	
		RBDLVectorNd q = RigidBodyDynamics::Math::VectorNd::Zero(rbdlModel->q_size);
		RBDLVector3d rbdl_vec3 = CalcBodyToBaseCoordinates (*rbdlModel, q, rbdl_id, RigidBodyDynamics::Math::Vector3d (0., 0., 0.));
		RBDLMatrix3d rbdl_mat3 = CalcBodyWorldOrientation (*rbdlModel, q, rbdl_id);
		Matrix33f rot_mat = ConvertToSimpleMathMat3 (rbdl_mat3.transpose());

		Vector3f joint_position (rbdl_vec3[0], rbdl_vec3[1], rbdl_vec3[2]);
		joint_scene_object->transformation.rotation = SimpleMath::GL::Quaternion::fromMatrix(rot_mat);
		joint_scene_object->transformation.translation = joint_position;
		joint_scene_object->transformation.scaling = Vector3f (0.03, 0.03, 0.03);
		joint_scene_object->mesh = CreateUVSphere (8, 16);
		joint_scene_object->noDepthTest = true;

		joint_scene_object->luaFrameId = i;
		joints.push_back (joint_scene_object);

		// add visuals
		for (size_t vi = 0; vi < (*luaTable)["frames"][i]["visuals"].length(); vi++) {
			VisualsData visual_data = (*luaTable)["frames"][i]["visuals"][vi + 1];

			assert ((visual_data.scale - Vector3f (-1.f, -1.f, -1.f)).squaredNorm() < 1.0e-5 && "visuals.scale not (yet) supported!");
			assert ((visual_data.translate - Vector3f (-1.f, -1.f, -1.f)).squaredNorm() < 1.0e-5 && "visuals.translate not (yet) supported!");

			// setup of the scene object
			VisualsObject* visual_scene_object = scene->createObject<VisualsObject>();
			visual_scene_object->luaFrameId = i;
			visual_scene_object->color = visual_data.color;

			MeshVBO mesh;
			load_obj (mesh, visual_data.src.c_str());	
			visual_scene_object->mesh = mesh;

			// setup of the transformation
			Transformation object_transformation = joint_scene_object->transformation;
			Vector3f bbox_size (mesh.bbox_max - mesh.bbox_min);
			Vector3f scale (
					fabs(visual_data.dimensions[0]) / bbox_size[0],
					fabs(visual_data.dimensions[1]) / bbox_size[1],
					fabs(visual_data.dimensions[2]) / bbox_size[2]
					);

			object_transformation.scaling = scale;

			object_transformation.translation = object_transformation.translation + object_transformation.rotation.rotate (visual_data.mesh_center);
	
			visual_scene_object->transformation = object_transformation;

			// register scene object
			visuals.push_back(visual_scene_object);
		}
	}

	modelStateQ = VectorNd::Zero (rbdlModel->q_size);

	// restore selected frame
	if (selected_frame != -1) {
		scene->selectedObjectId = getObjectIdFromFrameId (selected_frame);
	}
}

bool MarkerModel::loadFromFile(const char* filename) {
	assert (rbdlModel == NULL);
	rbdlModel = new RigidBodyDynamics::Model;

	luaTable = new LuaTable();

	LuaTable luatable_temp = LuaTable::fromFile (filename);
	*luaTable = luatable_temp;

	luatable_temp.deleteLuaState = false;
	luaTable->deleteLuaState = true;

	updateFromLua();
	
	return true;
}
