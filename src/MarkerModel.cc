#include "MarkerModel.h"
#include "Scene.h"

#include <assert.h>
#include <iostream>
#include <sstream>
#include <fstream>

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

	for (size_t i = 0; i < visuals.size(); i++) {
		scene->destroyObject<VisualsObject>(visuals[i]);
	}

	for (size_t i = 0; i < joints.size(); i++) {
		scene->destroyObject<JointObject>(joints[i]);
	}
}

VectorNd MarkerModel::getModelState() {
	return modelStateQ;
}

std::vector<std::string> MarkerModel::getModelStateNames () {
	std::vector<std::string> result (rbdlModel->q_size, "unnamed");

	for (unsigned int i = 0; i < rbdlModel->q_size; i++) {
		ostringstream num_stream ("");
		num_stream << i;
		result[i] = num_stream.str();
	}
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

	modelStateQ[state_index] = value;

	updateModelState();
	updateSceneObjects();
}

bool MarkerModel::stateIndexIsFrameJointVariable (unsigned int state_index, int frame_id) {
	unsigned int rbdl_id = frameIdToRbdlId[frame_id];
	if (dofIndexToFrameId[state_index] == frame_id) {
		return true;	
	}

	return false;
}

JointObject* MarkerModel::getJointObject (int frame_id) {
	for (size_t i = 0; i < joints.size(); i++) {
		if (joints[i]->frameId == frame_id)
			return joints[i];
	}

	JointObject *joint_object = scene->createObject<JointObject>();
	joint_object->frameId = frame_id;
	joints.push_back (joint_object);
	return joint_object;
}

VisualsObject* MarkerModel::getVisualsObject (int frame_id, int visual_index) {
	for (size_t i = 0; i < visuals.size(); i++) {
		if (visuals[i]->frameId == frame_id && visuals[i]->visualIndex == visual_index)
			return visuals[i];
	}

	VisualsObject *visual_object = scene->createObject<VisualsObject>();
	visual_object->frameId = frame_id;
	visual_object->visualIndex = visual_index;
	visuals.push_back (visual_object);

	return visual_object;
}

int MarkerModel::getFrameMarkerCount(int frame_id) {
	return (*luaTable)["frames"][frame_id]["markers"].length();
}

std::vector<std::string> MarkerModel::getFrameMarkerNames(int frame_id) {
	vector<string> result;
	vector<LuaKey> keys = (*luaTable)["frames"][frame_id]["markers"].keys();

	for (size_t i = 0; i < keys.size(); i++) {
		if (keys[i].type == LuaKey::String) {
			result.push_back (keys[i].string_value);
		}
	}

	return result;
}

std::vector<Vector3f> MarkerModel::getFrameMarkerCoords (int frame_id) {
	vector<Vector3f> result;
	vector<string> names = getFrameMarkerNames(frame_id);

	for (size_t i = 0; i < names.size(); i++) {
		result.push_back ((*luaTable)["frames"][frame_id]["markers"][names[i].c_str()].getDefault<Vector3f>(Vector3f (0.f, 0.f, 0.f)));
	}

	return result;
}

Vector3f MarkerModel::calcMarkerLocalCoords (int frame_id, const Vector3f &coord) {
	updateModelState();
	RBDLVector3d rbdl_coords (coord[0], coord[1], coord[2]);
	unsigned int body_id = frameIdToRbdlId[frame_id];

	RBDLVectorNd Q (rbdlModel->q_size);
	RBDLVector3d rbdl_local = CalcBaseToBodyCoordinates (*rbdlModel, Q, body_id, rbdl_coords, false);

	return ConvertToSimpleMathVec3 (rbdl_local);
}

Vector3f MarkerModel::getMarkerPosition (int frame_id, const char* marker_name) {
	updateModelState();
	Vector3f marker_local_coords ((*luaTable)["frames"][frame_id]["markers"][marker_name].getDefault<Vector3f>(Vector3f (0.f, 0.f, 0.f)));

	RBDLVector3d rbdl_coords (marker_local_coords[0], marker_local_coords[1], marker_local_coords[2]);
	unsigned int body_id = frameIdToRbdlId[frame_id];

	RBDLVectorNd Q (rbdlModel->q_size);
	RBDLVector3d rbdl_global = CalcBodyToBaseCoordinates (*rbdlModel, Q, body_id, rbdl_coords, false);

	return ConvertToSimpleMathVec3 (rbdl_global);
}

void MarkerModel::setFrameMarkerCoord (int frame_id, const char* marker_name, const Vector3f &coord) {
	(*luaTable)["frames"][frame_id]["markers"][marker_name] = coord;
	updateFromLua();
}

void MarkerModel::updateSceneObjects() {
	RBDLVectorNd q = RigidBodyDynamics::Math::VectorNd::Zero(rbdlModel->q_size);

	// first update joints as we can reuse its transformations for the
	// visuals!
	for (size_t i = 0; i < joints.size(); i++) {
		int rbdl_id = joints[i]->rbdlBodyId;

		RBDLVector3d rbdl_vec3 = CalcBodyToBaseCoordinates (*rbdlModel, q, rbdl_id, RigidBodyDynamics::Math::Vector3d (0., 0., 0.), false);
		RBDLMatrix3d rbdl_mat3 = CalcBodyWorldOrientation (*rbdlModel, q, rbdl_id, false);
		Matrix33f rot_mat = ConvertToSimpleMathMat3 (rbdl_mat3.transpose());

		Vector3f joint_position (rbdl_vec3[0], rbdl_vec3[1], rbdl_vec3[2]);

		joints[i]->transformation.rotation = SimpleMath::GL::Quaternion::fromMatrix(rot_mat);
		joints[i]->transformation.translation = joint_position;
	}

	for (size_t i = 0; i < visuals.size(); i++) {
		Transformation joint_transformation = visuals[i]->jointObject->transformation;
		Vector3f mesh_center = (*luaTable)["frames"][visuals[i]->frameId]["visuals"][visuals[i]->visualIndex]["mesh_center"].getDefault<Vector3f>(Vector3f (0.f, 0.f, 0.f));

		joint_transformation.translation = joint_transformation.translation + joint_transformation.rotation.rotate (mesh_center);

		visuals[i]->transformation.translation = joint_transformation.translation;
		visuals[i]->transformation.rotation = joint_transformation.rotation;
	}
}

int MarkerModel::getFrameIdFromObjectId (int object_id) {
	for (size_t i = 0; i < visuals.size(); i++) {
		if (visuals[i]->id == object_id)
			return visuals[i]->frameId;
	}

	for (size_t i = 0; i < joints.size(); i++) {
		if (joints[i]->id == object_id)
			return joints[i]->frameId;
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

int MarkerModel::getVisualsCount (int frame_id) {
	if (frame_id == 0)
		return 0;

	assert (luaTable);

	return (*luaTable)["frames"][frame_id]["visuals"].length();
}

int MarkerModel::getFrameCount() {
	return (*luaTable)["frames"].length();
}

VisualsData MarkerModel::getVisualsData (int frame_id, int visuals_index) {
	assert (frame_id > 0);
	assert (visuals_index > 0 && visuals_index <= getVisualsCount (frame_id));

	return (*luaTable)["frames"][frame_id]["visuals"][visuals_index];
}

int MarkerModel::getObjectIdFromFrameId (int frame_id) {
	for (size_t i = 0; i < visuals.size(); i++) {
		if (visuals[i]->frameId == frame_id)
			return visuals[i]->id;
	}

	for (size_t i = 0; i < joints.size(); i++) {
		if (joints[i]->frameId == frame_id)
			return joints[i]->id;
	}

	cerr << "Could not find object id for frame id " << frame_id << endl;
	abort();
	
	return 0;
}


std::string MarkerModel::getFrameName (int frame_id) {
	return rbdlModel->GetBodyName(frameIdToRbdlId[frame_id]);
}

std::string MarkerModel::getParentName (int frame_id) {
	return (*luaTable)["frames"][frame_id]["parent"].getDefault<string>("ROOT");
}

Vector3f MarkerModel::getFrameLocationGlobal (int frame_id) {
	updateModelState();

	RBDLVectorNd Q (rbdlModel->q_size);

	RBDLVector3d position = RigidBodyDynamics::CalcBodyToBaseCoordinates (*rbdlModel, Q, frameIdToRbdlId[frame_id], RBDLVector3d( 0., 0., 0.), false);

	return Vector3f (position[0], position[1], position[2]);
}

Vector3f MarkerModel::getFrameOrientationGlobalEulerYXZ (int frame_id) {
	updateModelState();

	RBDLVectorNd Q (rbdlModel->q_size);
	RBDLMatrix3d rbdl_orientation = RigidBodyDynamics::CalcBodyWorldOrientation (*rbdlModel, Q, frameIdToRbdlId[frame_id], false);

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
	SpatialTransform transform = rbdlModel->GetJointFrame (frameIdToRbdlId[frame_id]);
	Matrix33f orientation = (*luaTable)["frames"][frame_id]["joint_frame"]["E"].getDefault<Matrix33f>(Matrix33f::Identity(3,3));
	
	return SimpleMath::GL::Quaternion::fromMatrix(orientation).toEulerYXZ();
}

void MarkerModel::adjustParentVisualsScale (int frame_id, const Vector3f &old_r, const Vector3f &new_r) {
	/// \todo adjusting of parent visuals fails when mesh_center moves out of the bounding box of the original bbox.
	Vector3f delta_r = new_r - old_r;
	int parent_id = getParentFrameId (frame_id);
	
	if (parent_id == 0)
		return;

	for (int i = 0; i < (*luaTable)["frames"][parent_id]["visuals"].length(); i++) {
		Vector3f dimensions = (*luaTable)["frames"][parent_id]["visuals"][i + 1]["dimensions"];
		Vector3f mesh_center = (*luaTable)["frames"][parent_id]["visuals"][i + 1]["mesh_center"];

		Vector3f center_to_r = old_r - mesh_center;
		Vector3f delta_dim (
				copysign(delta_r[0], center_to_r[0] * delta_r[0]),
				copysign(delta_r[1], center_to_r[1] * delta_r[1]),
				copysign(delta_r[2], center_to_r[2] * delta_r[2])
				);

		mesh_center = mesh_center + delta_r * 0.5f;
		dimensions = dimensions + delta_dim;

		(*luaTable)["frames"][parent_id]["visuals"][i + 1]["dimensions"] = dimensions;
		(*luaTable)["frames"][parent_id]["visuals"][i + 1]["mesh_center"] = mesh_center;
	}
}

void MarkerModel::setJointLocationLocal (int frame_id, const Vector3f &location) {
	Vector3f old_location = (*luaTable)["frames"][frame_id]["joint_frame"]["r"];
	(*luaTable)["frames"][frame_id]["joint_frame"]["r"] = location;

	adjustParentVisualsScale (frame_id, old_location, location);
	
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

	rbdlToFrameId.clear();
	frameIdToRbdlId.clear();
}

void MarkerModel::updateFromLua() {
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

		for (int di = 0; di < joint.mDoFCount; di++) {
			dofIndexToFrameId[rbdlModel->q_size + di] = i;
		}
		unsigned int rbdl_id = rbdlModel->AddBody (parent_id, joint_frame, joint, body, body_name);
		frameIdToRbdlId[i] = rbdl_id;
		rbdlToFrameId[rbdl_id] = i;

		// Add joint scene object
		JointObject *joint_scene_object = getJointObject(i);
		joint_scene_object->rbdlBodyId = rbdl_id;

		joint_scene_object->color = Vector4f (0.9f, 0.9f, 0.9f, 1.f);
	
		RBDLVectorNd q = RigidBodyDynamics::Math::VectorNd::Zero(rbdlModel->q_size);
		RBDLVector3d rbdl_vec3 = CalcBodyToBaseCoordinates (*rbdlModel, q, rbdl_id, RigidBodyDynamics::Math::Vector3d (0., 0., 0.));
		RBDLMatrix3d rbdl_mat3 = CalcBodyWorldOrientation (*rbdlModel, q, rbdl_id);
		Matrix33f rot_mat = ConvertToSimpleMathMat3 (rbdl_mat3.transpose());

		Vector3f joint_position (rbdl_vec3[0], rbdl_vec3[1], rbdl_vec3[2]);
		joint_scene_object->transformation.rotation = SimpleMath::GL::Quaternion::fromMatrix(rot_mat);
		joint_scene_object->transformation.translation = joint_position;
		joint_scene_object->transformation.scaling = Vector3f (0.025, 0.025, 0.025);
		joint_scene_object->mesh = CreateUVSphere (8, 16);
		joint_scene_object->noDepthTest = true;

		joint_scene_object->frameId = i;

		// add visuals
		for (size_t vi = 0; vi < (*luaTable)["frames"][i]["visuals"].length(); vi++) {
			VisualsData visual_data = (*luaTable)["frames"][i]["visuals"][vi + 1];

			assert ((visual_data.scale - Vector3f (-1.f, -1.f, -1.f)).squaredNorm() < 1.0e-5 && "visuals.scale not (yet) supported!");
			assert ((visual_data.translate - Vector3f (-1.f, -1.f, -1.f)).squaredNorm() < 1.0e-5 && "visuals.translate not (yet) supported!");

			// setup of the scene object
			VisualsObject* visual_scene_object = getVisualsObject (i, vi + 1);
			visual_scene_object->frameId = i;
			visual_scene_object->jointObject = joint_scene_object;
			visual_scene_object->visualIndex = vi + 1;
			visual_scene_object->color = visual_data.color;
			visual_scene_object->color[3] = 0.8;

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
		}
	}

	if (modelStateQ.size() != rbdlModel->q_size)
		modelStateQ = VectorNd::Zero (rbdlModel->q_size);

	updateModelState();
	updateSceneObjects();
}

void MarkerModel::loadStateFromFile (const char* filename) {
	LuaTable state_table = LuaTable::fromFile (filename);
	for (int i = 0; i < modelStateQ.size(); i++) {
		modelStateQ[i] = state_table[i + 1];
	}
	updateModelState();
	updateSceneObjects();
}

void MarkerModel::saveStateToFile (const char* filename) {
	LuaTable state_table = LuaTable::fromLuaExpression ("return {}");
	for (int i = 0; i < modelStateQ.size(); i++) {
		state_table[i + 1] = modelStateQ[i];
	}
	string table_str = state_table.serialize();
	ofstream outfile (filename);
	outfile << table_str;
	outfile.close();
}

bool MarkerModel::loadFromFile(const char* filename) {
	if (rbdlModel) {
		delete rbdlModel;
	}
	rbdlModel = new RigidBodyDynamics::Model;

	luaTable = new LuaTable();

	LuaTable luatable_temp = LuaTable::fromFile (filename);
	*luaTable = luatable_temp;

	luatable_temp.deleteLuaState = false;
	luaTable->deleteLuaState = true;

	updateFromLua();
	
	return true;
}

void MarkerModel::saveToFile(const char* filename) {
	assert (luaTable);
	assert (rbdlModel);

	string table_str = luaTable->serialize();
	ofstream outfile (filename);
	outfile << table_str;
	outfile.close();
}
