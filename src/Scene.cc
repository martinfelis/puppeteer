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
	for (unsigned int i = 0; i < objects.size(); i++) {
		if (i == selectedObjectId) {
			drawSceneObjectStyled (objects[i], DrawStyleHighlighted);
		} else {
			drawSceneObjectStyled (objects[i], DrawStyleNormal);
		}
	}
}
