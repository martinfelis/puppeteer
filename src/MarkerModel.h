#ifndef MARKER_MODEL_H
#define MARKER_MODEL_H

#include <vector>

#include "SimpleMath/SimpleMath.h"
#include "SimpleMath/SimpleMathGL.h"

struct Scene;
namespace RigidBodyDynamics {
	struct Model;
}

struct Joint {
	int sceneObjectId;
	unsigned int rbdlJointId;
};

struct JointTransform {
	int sceneObjectId;
	unsigned int rbdlJointTransformId;

	Vector3f translation;
	SimpleMath::GL::Quaternion rotation;
};

struct Body {
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
		scene (scene_)
	{}
	MarkerModel(const MarkerModel &model);
	MarkerModel& operator=(const MarkerModel &model);

	Scene *scene;
	RigidBodyDynamics::Model *rbdlModel;

	std::vector<Body> bodies;
	std::vector<Joint> joints;
	std::vector<JointTransform> jointTransforms;

	bool loadFromFile (const char* filename);
	bool saveToFile (const char* filename);
};

/* MARKER_MODEL_H */
#endif
