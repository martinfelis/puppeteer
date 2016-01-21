/* 
 * Puppeteer - A Motion Capture Mapping Tool
 * Copyright (c) 2013-2016 Martin Felis <martin.felis@iwr.uni-heidelberg.de>.
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

#ifndef MARKER_MODEL_H
#define MARKER_MODEL_H

#include <vector>
#include <map>

#include "Scene.h"
#include "SimpleMath/SimpleMath.h"
#include "SimpleMath/SimpleMathGL.h"

/* General Notes:
 *
 * Frame ids are positive numbers. 0 is used to mark frames that do not
 * exist.
 */

namespace RigidBodyDynamics {
	struct Model;
}

struct LuaTable;

struct VisualsData {
	VisualsData(): 
		scale (-1.f, -1.f, -1.f),
		dimensions (1.f, 1.f, 1.f),
		color (1.f, 1.f, 1.f, 1.f),
		mesh_center (-1.f, -1.f, -1.f),
		translate (-1.f, -1.f, -1.f),
		orientation (),
		src ("")
	{}

	Vector3f scale;
	Vector3f dimensions;
	Vector4f color;
	Vector3f mesh_center;
	Vector3f translate;
	SimpleMath::GL::Quaternion orientation;
	std::string src;
};

struct JointObject : public SceneObject {
	int frameId;
	unsigned int rbdlBodyId;
};

struct ContactPointObject: public SceneObject {
	int frameId;
	int pointIndex;
	Vector3f localCoords;
	std::string name;
};

struct VisualsObject: public SceneObject {
	VisualsObject(): 
		frameId(0)
	{}

	int frameId;
	int visualIndex;
	JointObject* jointObject;
	VisualsData data;
};

struct ModelMarkerObject: public SceneObject {
	std::string markerName;
	int frameId;
};

/**
 *
 * \todo add support for the deletion of joints and visuals. Currently it
 * is assumed that the number of joints and visuals is constant.
 */
struct Model {
	Model():
		scene(NULL),
		luaTable(NULL),
		rbdlModel(NULL)
	{}
	Model(Scene* scene_) :
		scene (scene_),
		luaTable (NULL),
		rbdlModel (NULL)
	{}
	~Model();

	Scene *scene;
	LuaTable *luaTable;
	RigidBodyDynamics::Model *rbdlModel;
	VectorNd modelStateQ;

	std::vector<JointObject*> joints;
	std::vector<VisualsObject*> visuals;
	std::vector<ModelMarkerObject*> modelMarkers;
	std::vector<ContactPointObject*> contactPoints;

	std::map<unsigned int, int> dofIndexToFrameId;
	std::map<unsigned int, int> frameIdToRbdlId;
	std::map<int, unsigned int> rbdlToFrameId;

	bool isJointObject (int objectid) {
		for (size_t i = 0; i < joints.size(); i++) {
			if (joints[i]->id == objectid)
				return true;
		}
		return false;
	}

	bool isVisualsObject (int objectid) {
		for (size_t i = 0; i < visuals.size(); i++) {
			if (visuals[i]->id == objectid)
				return true;
		}
		return false;
	}

	bool isModelMarkerObject (int objectid) {
		for (size_t i = 0; i < modelMarkers.size(); i++) {
			if (modelMarkers[i]->id == objectid)
				return true;
		}
		return false;
	}

	bool isContactPointObject (int objectid) {
		for (size_t i = 0; i < contactPoints.size(); i++) {
			if (contactPoints[i]->id == objectid)
				return true;
		}
		return false;
	}

	bool isModelObject (int objectid) {
		if (isModelMarkerObject(objectid))
			return true;

		if (isVisualsObject(objectid))
			return true;

		if (isContactPointObject(objectid))
			return true;

		return isJointObject(objectid);
	}

	VectorNd getModelState();
	std::vector<std::string> getModelStateNames();

	void updateModelState();
	void setModelStateValue (unsigned int state_index, double value);
	bool stateIndexIsFrameJointVariable (unsigned int state_index, int frame_id);

	JointObject* getJointObject (int frame_id);
	VisualsObject* getVisualsObject (int frame_id, int visual_index);
	ModelMarkerObject* getModelMarkerObject (int frame_id, const char* marker_name);
	ContactPointObject* getContactPointObject (int contact_point_index);

	int getFrameMarkerCount (int frame_id);
	std::vector<Vector3f> getFrameMarkerCoords (int frame_id);
	std::vector<std::string> getFrameMarkerNames(int frame_id);
	Vector3f calcMarkerLocalCoords (int frame_id, const Vector3f &global_coords);
	Vector3f getMarkerPosition (int frame_id, const char* marker_name);
	void setFrameMarkerCoord (int frame_id, const char* marker_name, const Vector3f &coord);
	void deleteFrameMarker (int frame_id, const char* marker_name);

	int getParentFrameId (int frame_id);
	int getObjectIdFromFrameId (int frame_id);
	int getFrameIdFromObjectId (int object_id);
	int getVisualsCount (int frame_id);
	int getFrameCount ();

	VisualsData getVisualsData (int frame_id, int visuals_index);
	int getFrameId (const char *frame_name);
	std::string getFrameName (int frame_id);
	std::string getParentName (int frame_id);
	Vector3f getFrameLocationGlobal (int frame_id);
	Vector3f getFrameOrientationGlobalEulerYXZ (int frame_id);
	Vector3f getJointLocationLocal (int frame_id);
	Vector3f getJointOrientationLocalEulerYXZ (int frame_id);
	void setJointLocationLocal (int frame_id, const Vector3f &location);
	void setJointOrientationLocalEulerYXZ (int frame_id, const Vector3f &yxz_euler);

	void setVisualDimensions (int frame_id, int visuals_index, const Vector3f &dimensions);
	Vector3f getVisualDimensions (int frame_id, int visuals_index);
	void setVisualScale (int frame_id, int visuals_index, const Vector3f &scale);
	Vector3f getVisualScale (int frame_id, int visuals_index);
	void setVisualTranslate (int frame_id, int visuals_index, const Vector3f &translate);
	Vector3f getVisualTranslate(int frame_id, int visuals_index);
	void setVisualCenter (int frame_id, int visuals_index, const Vector3f &center);
	Vector3f getVisualCenter(int frame_id, int visuals_index);
	void setVisualColor (int frame_id, int visuals_index, const Vector3f &color);
	Vector3f getVisualColor(int frame_id, int visuals_index);

	bool visualUsesTranslate (int frame_id, int visuals_index);
	bool visualUsesDimensions (int frame_id, int visuals_index);

	void adjustParentVisualsScale (int frame_id, const Vector3f &old_r, const Vector3f &new_r);

	void setBodyMass (int frame_id, double mass);
	double getBodyMass (int frame_id);
	void setBodyCOM (int frame_id, const Vector3f &com);
	Vector3f getBodyCOM (int frame_id);
	void setBodyInertia (int frame_id, const Matrix33f &inertia);
	Matrix33f getBodyInertia (int frame_id);

	void setContactPointGlobal (int contact_point_index, const Vector3f &global_coords);
	void setContactPointLocal (int contact_point_index, const Vector3f &local_coords);
	Vector3f getContactPointLocal (int contact_point_index) const;

	bool loadFromFile (const char* filename);
	void saveToFile (const char* filename);
	void loadStateFromFile (const char* filename);
	void saveStateToFile (const char* filename);
	void clearModel();
	void updateFromLua ();
	void updateSceneObjects();

	private:
		Model(const Model &model) {}
		Model & operator=(const Model &model) { return *this; }
};

/* MARKER_MODEL_H */
#endif
