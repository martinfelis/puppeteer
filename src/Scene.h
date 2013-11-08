#ifndef _SCENE_H
#define _SCENE_H

#include <string>
#include <vector>

#include "MeshVBO.h"
#include "Transformation.h"

Vector4f object_id_to_vector4 (int id);
int vector4_to_object_id (const Vector4f &color);

enum DrawStyle {
	DrawStyleNormal = 0,
	DrawStyleHighlighted,
	DrawStyleSelected,
	DrawStyleHidden
};

struct SceneObject {
	SceneObject () :
		id (-1),
		color (1.f, 1.f, 1.f),
		noDepthTest(false),
		noLighting(false)
	{ }
	virtual ~SceneObject() {}

	int id;
	Vector3f color;
	bool noDepthTest;
	bool noLighting;
	Transformation transformation;
	MeshVBO mesh;
};

struct Scene {
	Scene() : 
		lastObjectId (0),
		selectedObjectId(-1),
		mouseOverObjectId(-1) {	}

	int lastObjectId;
	int selectedObjectId;
	int mouseOverObjectId;

	void init();
	void draw();
	void drawForColorPicking();

	void drawSceneObjectStyled (const SceneObject *object, DrawStyle style);
	void unregisterSceneObject (const int id);

	template <typename T> T* createObject();
	template <typename T> void destroyObject(T* object);
	template <typename T> T* getObject(const int id);

	private:
		std::vector<SceneObject*> objects;
};

template<> inline SceneObject* Scene::createObject<SceneObject>() {
	SceneObject* result = new SceneObject();
	result->id = lastObjectId;
	lastObjectId++;
	objects.push_back(result);
	return result;
}

template<> inline void Scene::destroyObject<SceneObject>(SceneObject* object) {
	unregisterSceneObject (object->id);
	delete object;
}

template<> inline SceneObject* Scene::getObject<SceneObject>(const int id) {
	std::vector<SceneObject*>::iterator obj_iter = objects.begin();

	do {
		if ((*obj_iter)->id == id)  {
			return (*obj_iter);
		}
		obj_iter++;
	} while (obj_iter != objects.end());

	return NULL;
}

/* _SCENE_H */
#endif
