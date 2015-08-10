/* 
 * Puppeteer - A Motion Capture Mapping Tool
 * Copyright (c) 2013-2015 Martin Felis <martin.felis@iwr.uni-heidelberg.de>.
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE* 
 */

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

