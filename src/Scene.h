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

#ifndef _SCENE_H
#define _SCENE_H

#include <string>
#include <vector>
#include <list>

#include "MeshVBO.h"
#include "Transformation.h"
#include "Shader.h"

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
		color (1.f, 1.f, 1.f, 1.f),
		noDepthTest(false),
		noLighting(false),
		noDraw (false)
	{ }
	virtual ~SceneObject() {}

	int id;
	Vector4f color;
	bool noDepthTest;
	bool noLighting;
	bool noDraw;
	Transformation transformation;
	MeshVBO mesh;
};

struct Light {
	Vector4f ambient;
	Vector4f diffuse;
	Vector4f specular;
	Vector4f position;
};

struct Scene {
	Scene() :
		lastObjectId (0),
		mouseOverObjectId (-1),
		lightingEnabled (true)
	{}

	int lastObjectId;
	std::list<int> selectedObjectIds;
	int mouseOverObjectId;
	bool lightingEnabled;

	void initShaders();
	void draw();
	void drawForColorPicking();

	void selectObject (const int id);
	void unselectObject (const int id);
	void clearSelection () { selectedObjectIds.clear(); }
	bool objectIsSelected (const int id) const;

	void drawSceneObjectStyled (const SceneObject *object, DrawStyle style);
	void unregisterSceneObject (const int id);

	template <typename T> T* createObject();
	template <typename T> void destroyObject(T* object);
	template <typename T> T* getObject(const int &id);

	ShaderProgram defaultShader;

	private:
		std::vector<SceneObject*> objects;
};

template<typename T> inline T* Scene::createObject() {
	T* result = new T();

	result->id = lastObjectId;
	lastObjectId++;
	objects.push_back(result);
	return result;
}

template<typename T> inline void Scene::destroyObject(T* object) {
	unregisterSceneObject (object->id);
	delete object;
}

template<> inline SceneObject* Scene::getObject<SceneObject>(const int &id) {
	std::vector<SceneObject*>::iterator obj_iter = objects.begin();

	do {
		if ((*obj_iter)->id == id)  {
			return (*obj_iter);
		}
		obj_iter++;
	} while (obj_iter != objects.end());

	std::cerr << "Error: could not find object with id " << id << std::endl;
	abort();

	return NULL;
}

/* _SCENE_H */
#endif
