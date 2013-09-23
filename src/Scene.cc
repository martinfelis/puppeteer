#include "GL/glew.h"
#include <GL/glu.h>

#include "Scene.h"
#include "objloader.h"

void Scene::init() {
	SceneObject monkeyobject;
	load_obj (monkeyobject.mesh, "monkeyhead.obj");
	monkeyobject.name = "monkeyhead";
	objects.push_back (monkeyobject);

	SceneObject boxobject;
	boxobject.mesh = CreateCuboid (1.f, 1.f, 1.f);
	boxobject.name = "box";
	boxobject.transformation.translation.set(0., 2., 0.);
	objects.push_back (boxobject);

	SceneObject boxobject2;
	boxobject2.mesh = CreateCuboid (1.f, 1.f, 1.f);
	boxobject2.name = "box2";
	boxobject2.transformation.translation.set(0., 2., 2.);
	objects.push_back (boxobject2);
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
	}

	glColor3fv (object.color.data());
	const_cast<MeshVBO*>(&(object.mesh))->draw(GL_TRIANGLES);

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

	result[0] = static_cast<float>((id + 1) & 0xff0000) / 255.f;
	result[1] = static_cast<float>((id + 1) & 0x00ff00) / 255.f;
	result[2] = static_cast<float>((id + 1) & 0x0000ff) / 255.f;

	return result;
}

int Scene::colorToObjectId (const Vector4f &color) {

	int result = 0;
	result = static_cast<int> (
			color[0] * powf(2,32)
			+ color[1] * powf(2,16)
			+ color[2] * powf(2, 8)
			);

	return result - 1;
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
