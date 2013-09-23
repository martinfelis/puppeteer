#include "GL/glew.h"
#include <GL/glu.h>

#include "Scene.h"
#include "objloader.h"

void Scene::init() {
	SceneObject monkeyobject;
	load_obj (monkeyobject.mesh, "monkeyhead.obj");
	monkeyobject.name = "monkeyhead";
	objects.push_back (monkeyobject);

	SceneObject boxobject2;
	boxobject2.mesh = CreateCuboid (1.f, 1.f, 1.f);
	boxobject2.name = "box2";
	boxobject2.transformation.translation.set(0., 2., 2.);
	objects.push_back (boxobject2);

	int box_count_x = 10;
	int box_count_y = 5;
	int box_count_z = 10;

	for (int i = 0; i < box_count_x; i++) {
		for (int j = 0; j < box_count_y; j++) {
			for (int k = 0; k < box_count_z; k++) {
				SceneObject boxobject;
				boxobject.mesh = CreateCuboid (0.2, 0.2, 0.2);
				boxobject.transformation.translation.set (
						float (i - box_count_x / 2.f),
						float (j),
						float (k - box_count_z / 2.f)
						);
				objects.push_back(boxobject);
			}
		}
	}

	/*
	assert (Vector4f (0.f, 0.f, 1.f, 1.f) == objectIdToColor (254));
	assert (Vector4f (0.f, 1./255.f, 0.f, 1.f) == objectIdToColor (255));
//	assert (Vector4f (0.f, 0.f, 0.f, 1.f) == objectIdToColor (0));
	std::cout << "object id = " << colorToObjectId (Vector4f (0.f, 0.f, 1.f, 1.f)) << std::endl;
	assert (255 == colorToObjectId (Vector4f (0.f, 0.f, 1.f, 1.f)));
	std::cout << "object id = " << colorToObjectId (Vector4f (0.f, 1./255.f, 0.f, 1.f)) << std::endl;
	assert (255 == colorToObjectId (Vector4f (0.f, 1./255.f, 0.f, 1.f)));
	*/
}

void Scene::drawSceneObjectStyled (const SceneObject &object, DrawStyle style) {
	if (style == DrawStyleHidden)
		return;

	glPushMatrix();
	glMultMatrixf (object.transformation.toGLMatrix().data());

	if (style == DrawStyleHighlighted) {
		glDisable(GL_LIGHTING);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glPushMatrix();
		glScalef (1.03f, 1.03f, 1.03f);
		glColor3f (1.f, 0.f, 0.f);
		const_cast<MeshVBO*>(&(object.mesh))->draw(GL_TRIANGLES);
		glPopMatrix();
		glCullFace(GL_BACK);
		glDisable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glColor3f (0.f, 0.8f, 0.8f);
		const_cast<MeshVBO*>(&(object.mesh))->draw(GL_TRIANGLES);
	} else {
		glColor3fv (object.color.data());
		const_cast<MeshVBO*>(&(object.mesh))->draw(GL_TRIANGLES);
	}

	glPopMatrix();
}

void Scene::draw() {
	selectedObjectId = mouseOverObjectId;

	for (unsigned int i = 0; i < objects.size(); i++) {
		if (i == selectedObjectId) {
			drawSceneObjectStyled (objects[i], DrawStyleHighlighted);
		} else {
			drawSceneObjectStyled (objects[i], DrawStyleNormal);
		}
	}
}

Vector4f Scene::objectIdToColor (int id) {
	Vector4f result (0.f, 0.f, 0.f, 1.f);

	unsigned int uid = static_cast<unsigned int>(id + 1);

//	result[0] = static_cast<float>(uid & 0xff0000) / 255.f;
//	result[1] = static_cast<float>(uid & 0x00ff00) >> 8) / 255.f;
	result[1] = static_cast<float>((uid >> 8) & 0x0000ff) / 255.f;
	result[2] = static_cast<float>(uid & 0x0000ff) / 255.f;

	return result;
}

int Scene::colorToObjectId (const Vector4f &color) {

	int result = 0;

	if (color[0] != 0) {
		result += static_cast<int> (color[0]) * 256 * 256 * 256 - 3;
	} else if (color[1] != 0) {
		result += static_cast<int> (color[1]) * 255 * 255 - 2;
	} else if (color[2] != 0) {
		result += static_cast<int> (color[2]) * 256 - 1;
	}

	return result;
}

void Scene::drawForColourPicking() {
	glDisable(GL_LIGHTING);
	for (unsigned int i = 0; i < objects.size(); i++) {
		glPushMatrix();
		glMultMatrixf (objects[i].transformation.toGLMatrix().data());

		glColor4fv (objectIdToColor (i).data());
		const_cast<MeshVBO*>(&(objects[i].mesh))->draw(GL_TRIANGLES);

		glPopMatrix();
	}
}
