/* 
 * Puppeteer - A Motion Capture Mapping Tool
 * Copyright (c) 2013-2016 Martin Felis <martin.felis@iwr.uni-heidelberg.de>.
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

#ifndef _SHADERS_H
#define _SHADERS_H

#include <string>
#include <SimpleMath/SimpleMath.h>

struct ShaderProgram {
	ShaderProgram() :
		vertex_shader_id (0),
		fragment_shader_id (0),
		program_id (0),
		vertex_shader_filename (""),
		fragment_shader_filename ("")
	{}

	unsigned int vertex_shader_id;
	unsigned int fragment_shader_id;
	unsigned int program_id;

	std::string vertex_shader_filename;
	std::string fragment_shader_filename;

	bool loadVertexShader (const char* filename);
	bool loadFragmentShader (const char* filename);
	bool createProgram();
	void setUniformFloat (const char* name, float value);
	void setUniformVector4f (const char* name, const Vector4f &value);

	void deleteShaders();
	void deleteProgram();

	void printLog();

	static ShaderProgram createFromFiles (
			const char* vertex_shader_filename,
			const char* fragment_shader_filename
			);
};

#endif
