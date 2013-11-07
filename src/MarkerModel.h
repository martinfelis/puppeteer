#ifndef MARKER_MODEL_H
#define MARKER_MODEL_H

#include <vector>
#include <map>

#include "SimpleMath/SimpleMath.h"
#include "SimpleMath/SimpleMathGL.h"

struct Scene;
namespace RigidBodyDynamics {
	struct Model;
}

struct LuaTable;

struct JointObject {
	int sceneObjectId;
	int luaFrameId;
};

struct VisualsObject {
	VisualsObject() :
		sceneObjectId(-1),
		luaFrameId(0),
		scale (-1.f, -1.f, -1.f),
		dimensions (1.f, 1.f, 1.f),
		color (1.f, 1.f, 1.f),
		mesh_center (-1.f, -1.f, -1.f),
		translate (-1.f, -1.f, -1.f),
		src ("")
	{}

	int sceneObjectId;
	int luaFrameId;
	Vector3f scale;
	Vector3f dimensions;
	Vector3f color;
	Vector3f mesh_center;
	Vector3f translate;
	std::string src;
};

struct MarkerModel {
	MarkerModel():
		scene(NULL),
		luaTable(NULL),
		rbdlModel(NULL)
	{}
	MarkerModel(Scene* scene_) :
		scene (scene_),
		luaTable (NULL),
		rbdlModel (NULL)
	{}
	~MarkerModel();

	Scene *scene;
	RigidBodyDynamics::Model *rbdlModel;
	LuaTable *luaTable;
	SimpleMath::Dynamic::Matrix<double> generalizedPositions;

	std::vector<JointObject> joints;
	std::vector<VisualsObject> visuals;
	std::map<unsigned int, int> luaToRbdlId;
	std::map<int, unsigned int> rbdlToLuaId;

	bool isJointObject (int objectid) {
		for (size_t i = 0; i < joints.size(); i++) {
			if (joints[i].sceneObjectId == objectid)
				return true;
		}
		return false;
	}

	bool isVisualsObject (int objectid) {
		for (size_t i = 0; i < visuals.size(); i++) {
			if (visuals[i].sceneObjectId == objectid)
				return true;
		}
		return false;
	}

	bool isModelObject (int objectid) {
		if (isVisualsObject(objectid))
			return true;

		return isJointObject(objectid);
	}

	void updateModelState();

	int getObjectIdFromFrameId (int frame_id);
	int getFrameIdFromObjectId (int object_id);
	std::string getFrameName (int frame_id);
	Vector3f getFrameLocationGlobal (int frame_id);
	Vector3f getFrameOrientationGlobalEulerYXZ (int frame_id);
	Vector3f getJointLocationLocal (int frame_id);
	Vector3f getJointOrientationLocalEulerYXZ (int frame_id);
	void setJointLocationLocal (int frame_id, const Vector3f &location);
	void setJointOrientationLocalEulerYXZ (int frame_id, const Vector3f &yxz_euler);

	bool loadFromFile (const char* filename);
	void clearModel();
	void updateFromLua ();

	private:
		MarkerModel(const MarkerModel &model) {}
		MarkerModel& operator=(const MarkerModel &model) {}

};

/* MARKER_MODEL_H */
#endif
