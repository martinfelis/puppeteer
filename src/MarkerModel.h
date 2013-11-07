#ifndef MARKER_MODEL_H
#define MARKER_MODEL_H

#include <vector>

#include "SimpleMath/SimpleMath.h"
#include "SimpleMath/SimpleMathGL.h"

struct Scene;
namespace RigidBodyDynamics {
	struct Model;
}

struct JointObject {
	int sceneObjectId;
	unsigned int rbdlFrameId;
};

struct VisualsObject {
	VisualsObject() :
		sceneObjectId(-1),
		rbdlFrameId(0),
		scale (-1.f, -1.f, -1.f),
		dimensions (1.f, 1.f, 1.f),
		color (1.f, 1.f, 1.f),
		mesh_center (-1.f, -1.f, -1.f),
		translate (-1.f, -1.f, -1.f),
		src ("")
	{}

	int sceneObjectId;
	unsigned int rbdlFrameId;
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
		rbdlModel(NULL)
	{}
	MarkerModel(Scene* scene_) :
		scene (scene_),
		rbdlModel (NULL)
	{}
	~MarkerModel();
	MarkerModel(const MarkerModel &model);
	MarkerModel& operator=(const MarkerModel &model);

	Scene *scene;
	RigidBodyDynamics::Model *rbdlModel;
	SimpleMath::Dynamic::Matrix<double> generalizedPositions;

	std::vector<JointObject> joints;
	std::vector<VisualsObject> visuals;

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

	unsigned int getFrameIdFromObjectId (int object_id);
	std::string getFrameName (unsigned int frame_id);
	Vector3f getFrameLocationGlobal (unsigned int frame_id);
	Vector3f getFrameOrientationGlobalEulerYXZ (unsigned int frame_id);
	Vector3f getJointLocationLocal (unsigned int frame_id);
	Vector3f getJointOrientationLocalEulerYXZ (unsigned int frame_id);
	void setJointLocation (unsigned int frame_id, const Vector3f &location);
	void setJointOrientationEulerYXZ (unsigned int frame_id, const Vector3f &yxz_euler);

	bool loadFromFile (const char* filename);
};

/* MARKER_MODEL_H */
#endif
