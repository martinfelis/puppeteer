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
	unsigned int rbdlJointId;
};

struct JointTransformObject {
	int sceneObjectId;
	unsigned int rbdlJointTransformId;

	Vector3f translation;
	SimpleMath::GL::Quaternion rotation;
};

struct BodyObject {
	int sceneObjectId;
	unsigned int rbdlBodyId;

	float mass;
	Vector3f com;
	Matrix33f inertia;
};

struct VisualsObject {
	VisualsObject() :
		sceneObjectId(-1),
		scale (-1.f, -1.f, -1.f),
		dimensions (1.f, 1.f, 1.f),
		color (1.f, 1.f, 1.f),
		mesh_center (-1.f, -1.f, -1.f),
		translate (-1.f, -1.f, -1.f),
		src ("")
	{}

	int sceneObjectId;
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

	std::vector<BodyObject> bodies;
	std::vector<JointObject> joints;
	std::vector<JointTransformObject> jointTransforms;

	bool isJointObject (int objectid) {
		for (size_t i = 0; i < joints.size(); i++) {
			if (joints[i].sceneObjectId == objectid)
				return true;
		}
		return false;
	}

	bool loadFromFile (const char* filename);
	bool saveToFile (const char* filename);
};

/* MARKER_MODEL_H */
#endif
