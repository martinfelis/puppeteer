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
	DrawStyleHighlighted ,
	DrawStyleHidden
};

struct SceneObject {
	SceneObject () :
		name ("unnamed"),
		color (1.f, 1.f, 1.f)
	{ }

	std::string name;
	Vector3f color;
	Transformation transformation;
	MeshVBO mesh;
};

struct Scene {
	Scene() : selectedObjectId(-1), mouseOverObjectId(-1) {	}

	std::vector<SceneObject> objects;

	int selectedObjectId;
	int mouseOverObjectId;

	void init();
	void draw();
	void drawForColourPicking();

	void drawSceneObjectStyled (const SceneObject &object, DrawStyle style);
};

/* _SCENE_H */
#endif
