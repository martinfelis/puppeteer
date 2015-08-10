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
	buffer_size = mesh.buffer_size;
	normal_offset = mesh.normal_offset;
	color_offset = mesh.color_offset;
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
		memcpy (&(raw_buffer[(4 * vertices.size() + 3 * normals.size())]), &colors[0], sizeof(float) * 4 * colors.size());

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
		for (size_t vi = 0; vi < vertices.size(); vi++) {
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
	if (&other == this) {
		cerr << "Cannot join meshes not supported!" << endl;
		// To fix this create a temporary copy and use that for copying
		abort();
	}
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

void MeshVBO::center() {
	Vector3f displacement = - bbox_min - (bbox_max - bbox_min) * 0.5;
	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i] = vertices[i] + Vector4f (displacement[0], displacement[1], displacement[2], 0.);
	}
	bbox_max += displacement;
	bbox_min += displacement;
}

//
// OBJ loader
//
const string invalid_id_characters = "{}[],;: \r\n\t";

struct FaceInfo {
	FaceInfo () {
		vertex_index[0] = -1;
		vertex_index[1] = -1;
		vertex_index[2] = -1;

		texcoord_index[0] = -1;
		texcoord_index[1] = -1;
		texcoord_index[2] = -1;

		normal_index[0] = -1;
		normal_index[1] = -1;
		normal_index[2] = -1;
	}

	int vertex_index[3];
	int texcoord_index[3];
	int normal_index[3];
};

bool MeshVBO::loadOBJ (const char* filename, const char* object_name, bool strict) {
	string line, original_line;
	ifstream file_stream (filename);

	if (!file_stream) {
		cerr << "Error: Could not open OBJ file '" << filename << "'!" << endl;

		if (strict)
			exit (1);

		return false;
	}

	string current_object_name = "";
	string material_library = "";
	string material_name = "";
	bool smooth_shading = false;
	bool object_found = false;

	std::vector<Vector4f> vertices;
	std::vector<Vector3f> normals;
	std::vector<Vector3f> texture_coordinates;
	std::vector<FaceInfo> face_infos;
	int line_index = 0;

	while (!file_stream.eof()) {
		getline (file_stream, line);
		line_index ++;

//		cout << "reading line " << line_index << endl;

		original_line = line;
		line = trim_line (line);
		if (line.size() == 0)
			continue;

//		cout << "inp: '" << line << "'" << endl;

		if (line.substr (0, 6) == "mtllib") {
			material_library = line.substr (8, line.size());

			continue;
		}

		if (line.substr (0, 6) == "usemtl") {
			if (line.size() > 9) {
				material_name = line.substr (8, line.size());
			}

			continue;
		}

		if (line.substr (0, 2) == "s ") {
			if (line.substr (2, 3) == "off"
					|| line.substr (2, 1) == "0")
				smooth_shading = false;
			else if (line.substr (2, 2) == "on"
					|| line.substr (2, 1) == "1")
				smooth_shading = true;

			continue;
		}

		if (line[0] == 'o') {
			// If we have found our object already we can skip all following
			// objects.
			if (object_found)
				break;

			// we need a copy of the line that still contains the original case
			// as line contains the line transformed to lowercase.
			string object_line = strip_whitespaces(strip_comments(original_line));

			current_object_name = object_line.substr (2, object_line.size());
//			cout << "current_object_name = " << current_object_name << endl;
			if (object_name != NULL && current_object_name == object_name)
				object_found = true;
			
			continue;
		}

		if (line.substr (0,2) == "v ") {
			float v1, v2, v3, v4 = 1.f;
			istringstream values (line.substr (2, line.size()));
			values >> v1;
			values >> v2;
			values >> v3;

			if (!(values >> v4))
				v4 = 1.f;

			vertices.push_back (Vector4f (v1, v2, v3, v4));

//			cerr << "line " << line << " ended up as vertice: " << vertices[vertices.size() -1].transpose() << endl;
			
			continue;
		}

		if (line.substr (0,3) == "vn ") {
			float v1, v2, v3;
			istringstream values (line.substr (3, line.size()));
			values >> v1;
			values >> v2;
			values >> v3;

			normals.push_back (Vector3f (v1, v2, v3));

			continue;
		}	

		if (line.substr (0,3) == "vt ") {
			float v1, v2, v3;
			istringstream values (line.substr (3, line.size()));
			values >> v1;
			values >> v2;
			values >> v3;

			texture_coordinates.push_back (Vector3f (v1, v2, v3));

			continue;
		}	

		if (line.substr (0,2) == "f "
				&& (object_name == NULL || current_object_name == object_name)
				) {
			std::vector<string> tokens = tokenize (line.substr (2, line.size()));
			if (tokens.size() != 3) {
				cerr << "Error: Faces must be triangles! (" << filename << ": " << line_index << ")" << endl;
				cerr << tokens.size() << endl;

				if (strict)
					exit (1);

				return false;
			}

			FaceInfo face_info;

			// read faces
			//
			// the following are valid face definitions:
			//   f v1 v2 v3
			//   f v1/n1 v2/n2 v3/n3
			//   f v1/t1/t1 v2/t2/t2 v3/t3/t3
			//   f v1//t1 v2//t2 v3//t3
			for (int vi = 0; vi < 3; vi++) {
				// first data
				std::vector<string> vertex_tokens = tokenize (tokens[vi], "/");

				if (vertex_tokens.size() == 1) {
					if (vertex_tokens[0].size() > 0) {
						istringstream values (vertex_tokens[0]);
						values >> face_info.vertex_index[vi];
					}
				}

				else if (vertex_tokens.size() == 2) {
					// two possible cases:
					//   v1/t1  or v1//n1
					
					// first one is always the vertex index
					if (vertex_tokens[0].size() > 0) {
						istringstream values (vertex_tokens[0]);
						values >> face_info.vertex_index[vi];
					}

					if (count_char (tokens[vi], "/") == 1) {
						// v1/t1
						if (vertex_tokens[1].size() > 0) {
							istringstream values (vertex_tokens[1]);
							values >> face_info.texcoord_index[vi];
						}
					} else {
						// v1/n1
						if (vertex_tokens[1].size() > 0) {
							istringstream values (vertex_tokens[1]);
							values >> face_info.normal_index[vi];
						}
					}
				}

				else if (vertex_tokens.size() == 3) {
					// two possible cases:
					//   v1/t1/n1

//					cout << "t1 = " << vertex_tokens[0] << " t2 = " << vertex_tokens[1] << " t3 = " << vertex_tokens[2];
					if (vertex_tokens[0].size() > 0) {
						istringstream values (vertex_tokens[0]);
						values >> face_info.vertex_index[vi];
					}
					if (vertex_tokens[1].size() > 0) {
						istringstream values (vertex_tokens[1]);
						values >> face_info.texcoord_index[vi];
					}
					if (vertex_tokens[2].size() > 0) {
						istringstream values (vertex_tokens[2]);
						values >> face_info.normal_index[vi];
					}
				}

//				cout << " parsed line '" << line << "' into: " 
//					<< face_info.vertex_index[vi] << ","
//					<< face_info.texcoord_index[vi] << ","
//					<< face_info.normal_index[vi] << endl;
			}
			face_infos.push_back (face_info);

			continue;
		}
	}

	if (object_name != NULL && object_found == false) {
		cerr << "Warning: could not find object '" << object_name << "' in OBJ file '" << filename << "'" << endl;

		if (strict)
			exit(1);

		return false;
	}

//	cout << "found " << vertices.size() << " vertices" << endl;
//	cout << "found " << normals.size() << " normals" << endl;
//	cout << "found " << face_infos.size() << " faces" << endl;

	this->begin();
	// add all vertices to the MeshVBO
	for (size_t fi = 0; fi < face_infos.size(); fi++) {
		for (int vi = 0; vi < 3; vi ++) {
			Vector4f vertex = vertices.at(face_infos[fi].vertex_index[vi] - 1);
			this->addVertex3f(vertex[0], vertex[1], vertex[2]);
	//		cout << "added vertice " << vertex.transpose() << endl;

			if (face_infos[fi].normal_index[vi] != -1) {
				Vector3f normal = normals.at(face_infos[fi].normal_index[vi] - 1);
				this->addNormal(normal[0], normal[1], normal[2]);
			}
		}
	}

	this->end();

	smooth_shading = smooth_shading;

	file_stream.close();

	return true;
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

MeshVBO CreateGrid (unsigned int cells_u, unsigned int cells_v, const Vector3f &normal, Vector3f color1, Vector3f color2) {
	assert (cells_u > 0);
	assert (cells_v > 0);

	MeshVBO result;

	result.begin();
	Vector3f u_vec_temp (1.f, 0.f, 0.f);
	if (u_vec_temp.dot(normal.normalized()) > 0.8) {
		u_vec_temp = Vector3f (0.f, 1.f, 0.f);
	}
	Vector3f v_vec = normal.cross (u_vec_temp);
	Vector3f u_vec = v_vec.cross (normal);	

	for (unsigned int i = 0; i < cells_u; i++) {
		for (unsigned int j = 0; j < cells_v; j++) {
			Vector3f color;

			if ((i + j) % 2 == 0) {
				color = color1;
			} else {
				color = color2;
			}

			Vector3f center =
				  u_vec * (i + 0.5f) - u_vec * cells_u * 0.5f
				+ v_vec * (j + 0.5f) - v_vec * cells_v * 0.5f;

			result.addVertex3fv ((center + (u_vec + v_vec) * 0.5f).data());
			result.addNormalfv (normal.data());
			result.addColor3fv (color.data());

			result.addVertex3fv ((center + (-u_vec + v_vec) * 0.5f).data());
			result.addNormalfv (normal.data());
			result.addColor3fv (color.data());

			result.addVertex3fv ((center + (-u_vec - v_vec) * 0.5f).data());
			result.addNormalfv (normal.data());
			result.addColor3fv (color.data());

			result.addVertex3fv ((center + (-u_vec - v_vec) * 0.5f).data());
			result.addNormalfv (normal.data());
			result.addColor3fv (color.data());

			result.addVertex3fv ((center + (u_vec - v_vec) * 0.5f).data());
			result.addNormalfv (normal.data());
			result.addColor3fv (color.data());

			result.addVertex3fv ((center + (u_vec + v_vec) * 0.5f).data());
			result.addNormalfv (normal.data());
			result.addColor3fv (color.data());
		}
	}

	result.end();

	return result;
}

MeshVBO CreateCylinder (unsigned int segments) {
	MeshVBO result;
	
	result.begin();

	float delta = 2. * M_PI / static_cast<float>(segments);
	for (unsigned int i = 0; i < segments; i++) {
		float r0 = (i - 0.5) * delta;
		float r1 = (i + 0.5) * delta;

		float c0 = cos (r0); 
		float s0 = sin (r0); 

		float c1 = cos (r1); 
		float s1 = sin (r1); 

		Vector3f normal0 (-c0, -s0, 0.f);
		Vector3f normal1 (-c1, -s1, 0.f);

		Vector3f p0 = normal0 + Vector3f (0., 0.,  0.5f);
		Vector3f p1 = normal0 + Vector3f (0., 0., -0.5f);
		Vector3f p2 = normal1 + Vector3f (0., 0.,  0.5f);
		Vector3f p3 = normal1 + Vector3f (0., 0., -0.5f);

		result.addVertex3fv (p0.data());
		result.addNormalfv (normal0.data());

		result.addVertex3fv (p1.data());
		result.addNormalfv (normal0.data());

		result.addVertex3fv (p2.data());
		result.addNormalfv (normal1.data());

		result.addVertex3fv (p2.data());
		result.addNormalfv (normal1.data());

		result.addVertex3fv (p1.data());
		result.addNormalfv (normal0.data());

		result.addVertex3fv (p3.data());
		result.addNormalfv (normal1.data());
	}

	result.end();

	return result;
}

MeshVBO CreateCapsule (unsigned int rows, unsigned int segments, float length_z, float radius) {

	float cylinder_length = length_z - 2.f * radius;

	MeshVBO result;
	MeshVBO cylinder = CreateCylinder (segments);

	result.join (SimpleMath::GL::ScaleMat44 (radius, radius, cylinder_length), cylinder);

	MeshVBO sphere = CreateUVSphere (rows, segments);
	MeshVBO half_sphere;

	for (unsigned int i = 0; i < sphere.vertices.size() * 0.5; i++) {
		half_sphere.addVertex4fv (sphere.vertices[i].data());
		half_sphere.addNormalfv (sphere.normals[i].data());
	}

	result.join (
			SimpleMath::GL::RotateMat44 (90, 1.f, 0.f, 0.f)
			* SimpleMath::GL::ScaleMat44 (radius, radius, radius)
		  * SimpleMath::GL::TranslateMat44(0.f, 0.f, cylinder_length * 0.5)
			, half_sphere
			);
			 
	result.join (
			SimpleMath::GL::RotateMat44 (-90, 1.f, 0.f, 0.f)
			* SimpleMath::GL::ScaleMat44 (radius, radius, radius)
		  * SimpleMath::GL::TranslateMat44(0.f, 0.f, -cylinder_length * 0.5)
			, half_sphere
			);
	
	return result;
}

