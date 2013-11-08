#ifndef MARKER_MODEL_H
#define MARKER_MODEL_H

#include <vector>
#include <map>

#include "SimpleMath/SimpleMath.h"
#include "SimpleMath/SimpleMathGL.h"

/* General Notes:
 *
 * Frame ids are positive numbers. 0 is used to mark frames that do not
 * exist.
 */

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
	VectorNd modelStateQ;

	std::vector<JointObject> joints;
	std::vector<VisualsObject> visuals;
	std::map<unsigned int, int> luaToRbdlId;
	std::map<int, int> luaToSceneId;
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

	VectorNd getModelState();
	std::vector<std::string> getModelStateNames();

	void updateModelState();
	void setModelStateValue (unsigned int state_index, double value);

	int getParentFrameId (int frame_id);
	int getObjectIdFromFrameId (int frame_id);
	int getFrameIdFromObjectId (int object_id);
	std::string getFrameName (int frame_id);
	std::string getParentName (int frame_id);
	Vector3f getFrameLocationGlobal (int frame_id);
	Vector3f getFrameOrientationGlobalEulerYXZ (int frame_id);
	Vector3f getJointLocationLocal (int frame_id);
	Vector3f getJointOrientationLocalEulerYXZ (int frame_id);
	void setJointLocationLocal (int frame_id, const Vector3f &location);
	void setJointOrientationLocalEulerYXZ (int frame_id, const Vector3f &yxz_euler);
	void adjustParentVisualsScale (int frame_id, const Vector3f &delta_r);

	bool loadFromFile (const char* filename);
	void clearModel();
	void updateFromLua ();
	void updateSceneObjects();

	private:
		MarkerModel(const MarkerModel &model) {}
		MarkerModel& operator=(const MarkerModel &model) {}
};

/* MARKER_MODEL_H */
#endif
