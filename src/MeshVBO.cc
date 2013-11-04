/*
 * QtGLAppBase - Simple Qt Application to get started with OpenGL stuff.
 *
 * Copyright (c) 2011-2012 Martin Felis <martin.felis@iwr.uni-heidelberg.de>
 *
 * Licensed under the MIT license. See LICENSE for more details.
 */

#include "MeshVBO.h"

#include "GL/glew.h" 

#include "SimpleMath/SimpleMathGL.h"
#include "string_utils.h"

#include <string.h>
#include <iomanip>
#include <fstream>
#include <limits>
#include <iostream>

using namespace std;

// warning vbos seem to be buggy!
const bool use_vbo = true;

MeshVBO::MeshVBO (const MeshVBO& mesh)
{
	vbo_id = 0;
	started = mesh.started;
	smooth_shading = mesh.smooth_shading;
	buffer_size = 0;
	normal_offset = 0;
	color_offset = 0;
	bbox_min = mesh.bbox_min;
	bbox_max = mesh.bbox_max;

	vertices = mesh.vertices;
	normals = mesh.normals;
	colors = mesh.colors;

	if (mesh.vbo_id != 0) {
		generate_vbo();
	}
}

MeshVBO& MeshVBO::operator=(const MeshVBO& mesh) 
{
	if (this != &mesh) {
		vbo_id = 0;
		started = mesh.started;
		smooth_shading = mesh.smooth_shading;
		buffer_size = 0;
		normal_offset = 0;
		color_offset = 0;
		bbox_min = mesh.bbox_min;
		bbox_max = mesh.bbox_max;

		vertices = mesh.vertices;
		normals = mesh.normals;
		colors = mesh.colors;

		if (mesh.vbo_id != 0) {
			generate_vbo();
		}
	}

	return *this;
}

void MeshVBO::begin() {
	started = true;

	vertices.resize(0);
	normals.resize(0);
	colors.resize(0);
}

void MeshVBO::end() {
	if (normals.size()) {
		if (normals.size() != vertices.size()) {
			std::cerr << "Error: number of normals must equal the number of vertices specified!" << endl;
			abort();
		}
	}

	started = false;
}

unsigned int MeshVBO::generate_vbo() {
	bool have_normals = false;
	bool have_colors = false;
		
	if (normals.size() != 0)
		have_normals = true;

	if (colors.size() != 0)
		have_colors = true;

	assert (vbo_id == 0);
	assert (started == false);
	assert (vertices.size() != 0);
	assert (!have_normals || (normals.size() == vertices.size()));
	assert (!have_colors || (colors.size() == vertices.size()));

	// create the buffer
	glGenBuffers (1, &vbo_id);

	// initialize the buffer object
	glBindBuffer (GL_ARRAY_BUFFER, vbo_id);

	buffer_size = sizeof(float) * 4 * vertices.size();
	normal_offset = 0;
	color_offset = 0;
	
	if (have_normals) {
		normal_offset = buffer_size;
		buffer_size += sizeof(float) * 3 * normals.size();
	}
	if (have_colors) {
		color_offset = buffer_size;
		buffer_size += sizeof(float) * 4 * colors.size();
	}

	glBufferData (GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);

	// fill the data
	
	float *raw_buffer = (float*) glMapBuffer (GL_ARRAY_BUFFER, GL_READ_WRITE);
	memcpy (&(raw_buffer[0]), &vertices[0], sizeof(float) * 4 * vertices.size());

	if (have_normals)
		memcpy (&(raw_buffer[4 * vertices.size()]), &normals[0], sizeof(float) * 3 * normals.size());

	if (have_colors)
		memcpy (&(raw_buffer[4 * (vertices.size() + normals.size())]), &colors[0], sizeof(float) * 4 * colors.size());

	glUnmapBuffer (GL_ARRAY_BUFFER);

	glBindBuffer (GL_ARRAY_BUFFER, 0);

	return vbo_id;
}

void MeshVBO::delete_vbo() {
	if (vbo_id != 0) {
		glDeleteBuffers (1, &vbo_id);
	}

	vbo_id = 0;
}

void MeshVBO::debug_vbo () {
	assert (vbo_id != 0 && "MeshVBO not initialized!");

	glBindBuffer (GL_ARRAY_BUFFER, vbo_id);

	float *raw_buffer =(float*) glMapBuffer (GL_ARRAY_BUFFER, GL_READ_ONLY);
	cout << "vertices = " << endl;
	for (unsigned int i=0; i < vertices.size(); i++) {
		cout << "  [" << i << "] = " << raw_buffer[i*4] << ", " << raw_buffer[i*4 + 1] << ", " << raw_buffer[i*4+2] << endl;
	}

	cout << "normals = " << endl;
	for (unsigned int i=vertices.size(); i < vertices.size() + normals.size(); i++) {
		cout << "  [" << i << "] = " << raw_buffer[i*3] << ", " << raw_buffer[i*3 + 1] << ", " << raw_buffer[i*3+2] << endl;
	}

	cout << "colors = " << endl;
	for (unsigned int i=vertices.size() + normals.size(); i < vertices.size() + normals.size() + colors.size(); i++) {
		cout << "  [" << i << "] = " << raw_buffer[i*4] << ", " << raw_buffer[i*4 + 1] << ", " << raw_buffer[i*4+2] << endl;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer (GL_ARRAY_BUFFER, 0);
}

void MeshVBO::addVertex4f (float x, float y, float z, float w) {
	Vector4f vertex;
	vertex[0] = x;
	vertex[1] = y;
	vertex[2] = z;
	vertex[3] = w;
	vertices.push_back(vertex);

	bbox_max[0] = max (vertex[0], bbox_max[0]);
	bbox_max[1] = max (vertex[1], bbox_max[1]);
	bbox_max[2] = max (vertex[2], bbox_max[2]);

	bbox_min[0] = min (vertex[0], bbox_min[0]);
	bbox_min[1] = min (vertex[1], bbox_min[1]);
	bbox_min[2] = min (vertex[2], bbox_min[2]);
}

void MeshVBO::addVertex4fv (const float vert[4]) {
	addVertex4f (vert[0], vert[1], vert[2], vert[3]);
}

void MeshVBO::addVertex3fv (const float vert[3]) {
	addVertex4f (vert[0], vert[1], vert[2], 1.f);
}

void MeshVBO::addVertex3f (float x, float y, float z) {
	addVertex4f (x, y, z, 1.0f);
}

void MeshVBO::addNormal (float x, float y, float z) {
	Vector3f normal;
	normal[0] = x;
	normal[1] = y;
	normal[2] = z;
	normals.push_back (normal);
}

void MeshVBO::addNormalfv (const float normal[3]) {
	addNormal (normal[0], normal[1], normal[2]);
}

void MeshVBO::addColor4f (float r, float g, float b, float a) {
	Vector4f color;
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
	colors.push_back (color);
}

void MeshVBO::addColor4fv (const float color[4]) {
	addColor4f (color[0], color[1], color[2], color[3]);
}

void MeshVBO::addColor3f (float r, float g, float b) {
	addColor4f (r, g, b, 1.0f);
}

void MeshVBO::addColor3fv (const float color[3]) {
	addColor4f (color[0], color[1], color[2], 1.f);
}
void MeshVBO::draw(unsigned int mode) {
	if (vbo_id == 0)
		generate_vbo();

	if (smooth_shading)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);

	if (use_vbo) {
		glBindBuffer (GL_ARRAY_BUFFER, vbo_id);

		glVertexPointer (4, GL_FLOAT, 0, NULL);

		if (normals.size() != 0) {
			glNormalPointer (GL_FLOAT, 0, (const GLvoid *) normal_offset);
		}

		if (colors.size() != 0) {
			glColorPointer (4, GL_FLOAT, 0, (const GLvoid *) (color_offset));
		}
		
		glEnableClientState (GL_VERTEX_ARRAY);

		if (normals.size() != 0) {
			glEnableClientState (GL_NORMAL_ARRAY);
		} else {
			glDisableClientState (GL_NORMAL_ARRAY);
		}

		if (colors.size() != 0) {
			glEnableClientState (GL_COLOR_ARRAY);
		} else {
			glDisableClientState (GL_COLOR_ARRAY);
		}

		glDrawArrays (mode, 0, vertices.size());
		glBindBuffer (GL_ARRAY_BUFFER, 0);
	} else {
		glBegin (mode);
		for (int vi = 0; vi < vertices.size(); vi++) {
			if (colors.size() != 0)
				glColor3fv (colors[vi].data());
			if (normals.size() != 0)
				glNormal3fv (normals[vi].data());
			glVertex3fv (vertices[vi].data());

		}
		glEnd();
	}
}

void MeshVBO::join (const Matrix44f &transformation, const MeshVBO &other) {
	bool have_normals = false;
	bool have_colors = false;
	bool other_have_normals = false;
	bool other_have_colors = false;

	if (normals.size() != 0)
		have_normals = true;
	if (colors.size() != 0)
		have_colors = true;

	if (other.normals.size() != 0)
		other_have_normals = true;
	if (other.colors.size() != 0)
		other_have_colors = true;

	if (vertices.size() == 0) {
		have_normals = other_have_normals;
		have_colors = other_have_colors;
	}

	if (have_normals != other_have_normals) {
		cerr << "Error: cannot merge meshes as one has normals defined and the other hasn't!" << endl;
		abort();
	}

	if (have_colors != other_have_colors) {
		cerr << "Error: cannot merge meshes as one has colors defined and the other hasn't!" << endl;
		abort();
	}

	Matrix33f rotation = transformation.block<3,3>(0,0);

	for (unsigned int i = 0; i < other.vertices.size(); i++) {
		addVertex4fv ((other.vertices[i].transpose() * transformation).data());
		if (have_normals)
			addNormalfv ((other.normals[i].transpose() * rotation).data());
		if (have_colors)
			addColor4fv (other.colors[i].data());
	}
}

MeshVBO CreateUVSphere (unsigned int rows, unsigned int segments) {
	MeshVBO result;
	result.begin();

	float row_d = 1. / (rows);
	float angle_d = 2 * M_PI / static_cast<float>(segments);

	for (unsigned int j = 0; j < rows; j++) {
		float alpha0 = j * row_d * M_PI;
		float alpha1 = (j + 1) * row_d * M_PI;

		float r0 = sin (alpha0);
		float r1 = sin (alpha1);

		float h0 = cos (alpha0);
		float h1 = cos (alpha1);

		for (unsigned int i = 0; i < segments; i++) {
			Vector3f v0, v1, v2, v3;
			float a0 = (i - 0.5) * angle_d;
			float a1 = (i + 0.5) * angle_d;

	    v0 = Vector3f (r1 * cos(a0), h1, r1 * sin (a0));
	    v1 = Vector3f (r1 * cos(a1), h1, r1 * sin (a1));
	    v2 = Vector3f (r0 * cos(a1), h0, r0 * sin (a1));
	    v3 = Vector3f (r0 * cos(a0), h0, r0 * sin (a0));

			result.addVertex3f (v0[0], v0[1], v0[2]);
			result.addNormalfv ((v0 * 1.f/ v0.norm()).data());

			result.addVertex3f (v2[0], v2[1], v2[2]);
			result.addNormalfv ((v2 * 1.f/ v2.norm()).data());

			result.addVertex3f (v1[0], v1[1], v1[2]);
			result.addNormalfv ((v1 * 1.f/ v1.norm()).data());

			result.addVertex3f (v0[0], v0[1], v0[2]);
			result.addNormalfv ((v0 * 1.f/ v0.norm()).data());

			result.addVertex3f (v3[0], v3[1], v3[2]);
			result.addNormalfv ((v3 * 1.f/ v3.norm()).data());

			result.addVertex3f (v2[0], v2[1], v2[2]);
			result.addNormalfv ((v2 * 1.f/ v2.norm()).data());
		}
	}
	
	result.end();

	return result;
}

MeshVBO CreateCuboid (float width, float height, float depth) {
	MeshVBO result;
	result.begin();

	Vector3f v0 (  0.5 * width, -0.5 * height,  0.5 * depth);
	Vector3f v1 (  0.5 * width, -0.5 * height, -0.5 * depth);
	Vector3f v2 (  0.5 * width,  0.5 * height, -0.5 * depth);
	Vector3f v3 (  0.5 * width,  0.5 * height,  0.5 * depth);

	Vector3f v4 ( -0.5 * width, -0.5 * height,  0.5 * depth);
	Vector3f v5 ( -0.5 * width, -0.5 * height, -0.5 * depth);
	Vector3f v6 ( -0.5 * width,  0.5 * height, -0.5 * depth);
	Vector3f v7 ( -0.5 * width,  0.5 * height,  0.5 * depth);

	Vector3f normal;
	// +x
	normal = Vector3f (1., 0., 0.);
	result.addVertex3fv (v0.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v1.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v2.data());
	result.addNormalfv (normal.data());

	result.addVertex3fv (v2.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v3.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v0.data());
	result.addNormalfv (normal.data());

	// +y
	normal = Vector3f (0., 1., 0.);
	result.addVertex3fv (v3.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v2.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v6.data());
	result.addNormalfv (normal.data());

	result.addVertex3fv (v6.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v7.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v3.data());
	result.addNormalfv (normal.data());

	// +z
	normal = Vector3f (0., 0., 1.);
	result.addVertex3fv (v4.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v0.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v3.data());
	result.addNormalfv (normal.data());

	result.addVertex3fv (v3.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v7.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v4.data());
	result.addNormalfv (normal.data());

	// -x
	normal = Vector3f (-1., 0., 0.);
	result.addVertex3fv (v5.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v4.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v7.data());
	result.addNormalfv (normal.data());

	result.addVertex3fv (v7.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v6.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v5.data());
	result.addNormalfv (normal.data());

	// -y
	normal = Vector3f (0., -1., 0.);
	result.addVertex3fv (v0.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v4.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v5.data());
	result.addNormalfv (normal.data());

	result.addVertex3fv (v5.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v1.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v0.data());
	result.addNormalfv (normal.data());

	// -z
	normal = Vector3f (0., 0., -1.);
	result.addVertex3fv (v1.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v5.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v6.data());
	result.addNormalfv (normal.data());

	result.addVertex3fv (v6.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v2.data());
	result.addNormalfv (normal.data());
	result.addVertex3fv (v1.data());
	result.addNormalfv (normal.data());

	result.end();

	return result;
}

MeshVBO CreateGrid (unsigned int cells_x, unsigned int cells_z, Vector3f color1, Vector3f color2) {
	assert (cells_x > 0);
	assert (cells_z > 0);

	MeshVBO result;

	result.begin();
	Vector3f normal (0.f, 1.f, 0.f);

	for (unsigned int i = 0; i < cells_x; i++) {
		for (unsigned int j = 0; j < cells_z; j++) {
			Vector3f color;

			if ((i + j) % 2 == 0) {
				color = color1;
			} else {
				color = color2;
			}

			Vector3f center ((i + 0.5f) - cells_x * 0.5f, 0.f, (j + 0.5f) - cells_z * 0.5f);

			result.addVertex3fv ((center + Vector3f (0.5f, 0.f, 0.5f)).data());
			result.addNormalfv (normal.data());
			result.addColor3fv (color.data());

			result.addVertex3fv ((center + Vector3f (-0.5f, 0.f, 0.5f)).data());
			result.addNormalfv (normal.data());
			result.addColor3fv (color.data());

			result.addVertex3fv ((center + Vector3f (-0.5f, 0.f, -0.5f)).data());
			result.addNormalfv (normal.data());
			result.addColor3fv (color.data());

			result.addVertex3fv ((center + Vector3f (-0.5f, 0.f, -0.5f)).data());
			result.addNormalfv (normal.data());
			result.addColor3fv (color.data());

			result.addVertex3fv ((center + Vector3f (0.5f, 0.f, -0.5f)).data());
			result.addNormalfv (normal.data());
			result.addColor3fv (color.data());

			result.addVertex3fv ((center + Vector3f (0.5f, 0.f, 0.5f)).data());
			result.addNormalfv (normal.data());
			result.addColor3fv (color.data());
		}
	}

	result.end();

	return result;
}
