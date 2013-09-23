/*
 * QtGLAppBase - Simple Qt Application to get started with OpenGL stuff.
 *
 * Copyright (c) 2011-2012 Martin Felis <martin.felis@iwr.uni-heidelberg.de>
 *
 * Licensed under the MIT license. See LICENSE for more details.
 */

#ifndef _MESHVBO_H
#define _MESHVBO_H

#include <vector>
#include <iostream>
#include <cstddef>
#include <limits>

#include "SimpleMath/SimpleMath.h"
#include "SimpleMath/SimpleMathGL.h"

typedef ptrdiff_t GLsizeiptr;

/** \brief Loads Wavefront VBO files and prepares them for use in
 * OpenGL.
 */
struct MeshVBO {
	MeshVBO() :
		vbo_id(0),
		started(false),
		smooth_shading(true),
		have_normals (false),
		have_colors (false),
		buffer_size (0),
		normal_offset (0),
		color_offset (0),
		bbox_min (std::numeric_limits<float>::max(),
				std::numeric_limits<float>::max(),
				std::numeric_limits<float>::max()),
		bbox_max (-std::numeric_limits<float>::max(),
				-std::numeric_limits<float>::max(),
				-std::numeric_limits<float>::max())
	{}
	MeshVBO (const MeshVBO& mesh);
	MeshVBO& operator= (const MeshVBO& mesh);
	~MeshVBO() {
		if (vbo_id != 0) {
			delete_vbo();
		}
	}

	void begin();
	void end();

	void addVertice (float x, float y, float z);
	void addVerticefv (float vert[3]);
	void addNormal (float x, float y, float z);
	void addNormalfv (float norm[3]);
	void addColor (float x, float y, float z);
	void addColorfv (float norm[3]);

	unsigned int generate_vbo();
	void delete_vbo();
	void debug_vbo();

	void draw(unsigned int mode);

	unsigned int vbo_id;
	bool started;
	bool smooth_shading;

	bool have_normals;
	bool have_colors;

	GLsizeiptr buffer_size;
	GLsizeiptr normal_offset;
	GLsizeiptr color_offset;
	
	Vector3f bbox_min;
	Vector3f bbox_max;

	std::vector<Vector3f> vertices;
	std::vector<Vector3f> normals;
	std::vector<Vector3f> colors;
};

MeshVBO CreateUVSphere (unsigned int rows, unsigned int segments);

MeshVBO CreateCuboid (float width, float height, float depth);

MeshVBO CreateGrid (unsigned int cells_x, unsigned int cells_z, Vector3f color1, Vector3f color2);

inline MeshVBO CreateCube () {
	return CreateCuboid (1.f, 1.f, 1.f);
}

#endif
