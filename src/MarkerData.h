#ifndef MARKER_DATA_H
#define MARKER_DATA_H

#include <string>
#include <vector>

#include "SimpleMath/SimpleMath.h"
#include "SimpleMath/SimpleMathGL.h"

struct C3DFile;
struct Scene;

struct MarkerObject : public SceneObject {
	std::string markerName;
};

struct MarkerData {
	MarkerData() :
		scene (NULL),
		c3dfile (NULL),
		currentFrame (-1),
		rotateZ(false)
	{}
	MarkerData(Scene* scene_) :
		scene (scene_),
		c3dfile (NULL),
		currentFrame (-1),
		rotateZ(false)
	{}
	~MarkerData();

	Scene *scene;
	C3DFile *c3dfile;
	int currentFrame;
	std::vector<MarkerObject*> markers;
	bool rotateZ;

	bool isMarkerObject(int objectid) {
		for (size_t i = 0; i < markers.size(); i++) {
			if (markers[i]->id == objectid) {
				return true;
			}
		}
		return false;
	}
	void clearMarkers ();
	void enableMarker (const char* marker_name, const Vector3f &color);
	bool loadFromFile (const char* filename);
	bool markerExists (const char* marker_name);
	Vector3f getMarkerCurrentPosition (const char* marker_name);
	std::string getMarkerName (int objectid);
	int getFirstFrame ();
	int getLastFrame ();
	float getFrameRate ();
	void setCurrentFrameNumber (int frame_number);
	void updateMarkerSceneObjects();
	void calcDataBoundingBox (Vector3f &min, Vector3f &max);

	private:
	MarkerData (const MarkerData &marker_data) {};
	MarkerData& operator= (const MarkerData &marker_data) { return *this; };
};

/* MARKER_DATA_H */
#endif

