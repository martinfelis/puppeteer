#include <iostream>
#include <assert.h>

#include "GL/glew.h"
#include <GL/glu.h>

#include "Scene.h"
#include "objloader.h"

using namespace std;

Vector4f object_id_to_vector4 (int id) {
	Vector4f result (0.f, 0.f, 0.f, 1.f);

	unsigned char byte = (id + 1 & 0x0000ff);
	result[2] = static_cast<float>(byte) / 255.f;
	byte = (id + 1 >> 8 & 0x0000ff);
	result[1] = static_cast<float>(byte) / 255.f;
	byte = (id + 1 >> 16 & 0x0000ff);
	result[0] = static_cast<float>(byte) / 255.f;
	
	return result;
}

int vector4_to_object_id (const Vector4f &color) {
	int result = 0;

	if (color[0] != 0.f) {
		result += (static_cast<int> (color[0] * 255.f) << 16);
	}
	
	if (color[1] != 0.f) {
		result += (static_cast<int> (color[1] * 255.f) << 8);
	}
	
	if (color[2] != 0.f) {
		result += static_cast<int> (color[2] * 255.f);
	}

	return result - 1;
}

void Scene::init() {
	SceneObject monkeyobject;
	MeshVBO monkey_obj;
	load_obj (monkey_obj, "monkeyhead.obj");
	Matrix44f rot_mat = SimpleMath::GL::RotateMat44 (90.f, 1.f, 0.f, 0.f);
	monkeyobject.mesh.join (rot_mat, monkey_obj);
	monkeyobject.name = "monkeyhead";
	monkeyobject.transformation.translation = Vector3f (2.f, 0.f, 0.f);
	monkeyobject.transformation.rotation = SimpleMath::GL::Quaternion::fromGLRotate (45.f, 1.f, 0.f, 0.f);
	monkeyobject.transformation.scaling = Vector3f (1.f, 1.f, 1.f);
	registerSceneObject (monkeyobject);

	SceneObject boxobject2;
	boxobject2.mesh = CreateCuboid (1.f, 1.f, 1.f);
	boxobject2.name = "box2";
	boxobject2.transformation.translation.set(0., 2., 2.);
	registerSceneObject (boxobject2);

	return;

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
				registerSceneObject (boxobject);
			}
		}
	}
}

void Scene::drawSceneObjectStyled (const SceneObject &object, DrawStyle style) {
	if (style == DrawStyleHidden)
		return;

	glPushMatrix();
	glMultMatrixf (object.transformation.toGLMatrix().data());

	glEnable (GL_DEPTH_TEST);

	if (style == DrawStyleSelected) {
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

		if (!object.noLighting)
			glEnable(GL_LIGHTING);

		glColor3f (0.f, 0.8f, 0.8f);
		const_cast<MeshVBO*>(&(object.mesh))->draw(GL_TRIANGLES);
	} else if (style == DrawStyleHighlighted) {
		glDisable(GL_LIGHTING);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glPushMatrix();
		glScalef (1.03f, 1.03f, 1.03f);
		glColor3f (0.9, 0.9, 0.3);
		const_cast<MeshVBO*>(&(object.mesh))->draw(GL_TRIANGLES);
		glPopMatrix();
		glCullFace(GL_BACK);
		glDisable(GL_CULL_FACE);
		
		if (!object.noLighting)
			glEnable(GL_LIGHTING);

		glColor3f (0.8, 0.8, 0.2);
		const_cast<MeshVBO*>(&(object.mesh))->draw(GL_TRIANGLES);
	} else {
		if (object.noLighting)
			glDisable(GL_LIGHTING);
		else
			glEnable(GL_LIGHTING);

		glColor3fv (object.color.data());
		const_cast<MeshVBO*>(&(object.mesh))->draw(GL_TRIANGLES);
	}

	glPopMatrix();
}

void Scene::draw() {
	std::vector<SceneObject*> depth_ignoring_objects;

	for (int i = 0; i < objects.size(); i++) {
		if (objects[i].noDepthTest) {
			depth_ignoring_objects.push_back (&objects[i]);
			continue;
		}
		if (objects[i].id == selectedObjectId) {
			drawSceneObjectStyled (objects[i], DrawStyleSelected);
		} else if (objects[i].id == mouseOverObjectId) {
			drawSceneObjectStyled (objects[i], DrawStyleHighlighted);
		} else {
			drawSceneObjectStyled (objects[i], DrawStyleNormal);
		}
	}

	glClear (GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < depth_ignoring_objects.size(); i++) {
		drawSceneObjectStyled (*(depth_ignoring_objects[i]), DrawStyleNormal);
		if (depth_ignoring_objects[i]->id == selectedObjectId) {
			drawSceneObjectStyled (*depth_ignoring_objects[i], DrawStyleSelected);
		} else if (depth_ignoring_objects[i]->id == mouseOverObjectId) {
			drawSceneObjectStyled (*depth_ignoring_objects[i], DrawStyleHighlighted);
		} else {
			drawSceneObjectStyled (*depth_ignoring_objects[i], DrawStyleNormal);
		}
	}
}

void Scene::drawForColorPicking() {
	glDisable(GL_LIGHTING);

	std::vector<SceneObject*> depth_ignoring_objects;

	for (int i = 0; i < objects.size(); i++) {
		if (objects[i].noDepthTest) {
			depth_ignoring_objects.push_back (&objects[i]);
			continue;
		}

		glPushMatrix();
		glMultMatrixf (objects[i].transformation.toGLMatrix().data());

		glColor4fv (object_id_to_vector4 (objects[i].id).data());
		const_cast<MeshVBO*>(&(objects[i].mesh))->draw(GL_TRIANGLES);

		glPopMatrix();
	}

	glClear (GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < depth_ignoring_objects.size(); i++) {
		glPushMatrix();
		glMultMatrixf (depth_ignoring_objects[i]->transformation.toGLMatrix().data());

		glColor4fv (object_id_to_vector4 (depth_ignoring_objects[i]->id).data());
		const_cast<MeshVBO*>(&(depth_ignoring_objects[i]->mesh))->draw(GL_TRIANGLES);

		glPopMatrix();
	}
}

int Scene::registerSceneObject (const SceneObject &object) {
	assert (object.id == -1);

	lastObjectId = lastObjectId + 1;
	objects.push_back (object);
	objects[objects.size() - 1].id = lastObjectId;

	return lastObjectId;
}

SceneObject& Scene::getObject (int id) {
	for (size_t i = 0; i < objects.size(); i++) {
		if (objects[i].id == id)
			return objects[i];
	}

	std::cerr << "Error: could not find object with id " << id << "." << std::endl;
	abort();

	return objects[0];
}

