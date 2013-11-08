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
		name ("unnamed"),
		color (1.f, 1.f, 1.f),
		noDepthTest(false),
		noLighting(false)
	{ }

	int id;
	std::string name;
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

	void drawSceneObjectStyled (const SceneObject &object, DrawStyle style);
	int registerSceneObject (const SceneObject &object);
	void updateSceneObject (const int i, const SceneObject &object);
	void unregisterSceneObject (const int id);
	bool isSceneObject (const int id);

	SceneObject &getObject (int id); 

	private:
		std::vector<SceneObject> objects;
};

/* _SCENE_H */
#endif
