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

struct MarkerModel {
	MarkerModel():
		scene(NULL),
		rbdlModel(NULL)
	{}
	MarkerModel(Scene* scene_) :
		scene (scene_),
		rbdlModel (NULL)
	{}
	MarkerModel(const MarkerModel &model);
	MarkerModel& operator=(const MarkerModel &model);

	Scene *scene;
	RigidBodyDynamics::Model *rbdlModel;

	std::vector<BodyObject> bodies;
	std::vector<JointObject> joints;
	std::vector<JointTransformObject> jointTransforms;

	bool loadFromFile (const char* filename);
	bool saveToFile (const char* filename);
};

/* MARKER_MODEL_H */
#endif
