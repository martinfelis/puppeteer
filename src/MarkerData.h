#ifndef MARKER_DATA_H
#define MARKER_DATA_H

#include <string>
#include <vector>

#include "SimpleMath/SimpleMath.h"
#include "SimpleMath/SimpleMathGL.h"

struct C3DFile;
struct Scene;

struct MarkerObject {
	int sceneObjectId;
	std::string markerName;
};

struct MarkerData {
	MarkerData() :
		scene (NULL),
		c3dfile (NULL),
		currentFrame (-1)
	{}
	MarkerData(Scene* scene_) :
		scene (scene_),
		c3dfile (NULL),
		currentFrame (-1)
	{}
	~MarkerData();

	Scene *scene;
	C3DFile *c3dfile;
	int currentFrame;
	std::vector<MarkerObject> markers;

	void enableMarker (const char* marker_name, const Vector3f &color);
	bool loadFromFile (const char* filename);
	Vector3f getMarkerCurrentPosition (const char*);
	int getMarkerIndex (const char* marker_name);
	std::string getMarkerName (int markerIndex);
	int getFirstFrame ();
	int getLastFrame ();
	void setCurrentFrame (int frame_index);

	private:
	MarkerData (const MarkerData &marker_data) {};
	MarkerData& operator= (const MarkerData &marker_data) {};
};

/* MARKER_DATA_H */
#endif

