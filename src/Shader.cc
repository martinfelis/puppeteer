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

#include "Shader.h"

#include "GL/glew.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <assert.h>

using namespace std;

void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
 
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
 
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
        free(infoLog);
    }
}
 
void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
 
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
 
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
        free(infoLog);
    }
}

bool ShaderProgram::loadVertexShader(const char* filename) {
	assert (vertex_shader_id == 0);

	fstream shader_file (filename);
	
	if (!shader_file) {
		cerr << "Error opening file '" << filename << "'!" << endl;
		return false;
	}

	stringstream buf;
	buf << shader_file.rdbuf();
	shader_file.close();

	char *shader_source = new char[buf.str().size() + 1];
	memset (shader_source, 0, buf.str().size() + 1);
	memcpy (shader_source, buf.str().c_str(), buf.str().size());

	unsigned int result_shader = glCreateShader (GL_VERTEX_SHADER);

//	cout << "shader source: " << endl << shader_source << endl;

	glShaderSource (result_shader, 1, (const GLchar**) &shader_source, 0);

	glCompileShader (result_shader);

	int compiler_status = 0;
	glGetShaderiv (result_shader, GL_COMPILE_STATUS, &compiler_status);
	if (compiler_status == GL_FALSE) {
		int log_length;
		glGetShaderiv (result_shader, GL_INFO_LOG_LENGTH, &log_length);
	
		char *info_log = new char[log_length];
		glGetShaderInfoLog (result_shader, log_length, &log_length, info_log);

		cerr << "Error compiling shader '" << filename << "': " << endl << info_log << endl;
		delete[] shader_source;
		delete[] info_log;

		return false;
	}

	delete[] shader_source;

	vertex_shader_id = result_shader;
	vertex_shader_filename = filename;

	return true;
}

bool ShaderProgram::loadFragmentShader (const char* filename) {
	assert (fragment_shader_id == 0);

	fstream shader_file (filename);
	
	if (!shader_file) {
		cerr << "Error opening file '" << filename << "'!" << endl;
		return false;
	}
	stringstream buf;
	buf << shader_file.rdbuf();
	shader_file.close();

	char *shader_source = new char[buf.str().size() + 1];
	memset (shader_source, 0, buf.str().size() + 1);
	memcpy (shader_source, buf.str().c_str(), buf.str().size());

	unsigned int result_shader = glCreateShader (GL_FRAGMENT_SHADER);

//	cout << "shader source: " << endl << shader_source << endl;

	glShaderSource (result_shader, 1, (const GLchar**) &shader_source, 0);

	glCompileShader (result_shader);

	int compiler_status = 0;
	glGetShaderiv (result_shader, GL_COMPILE_STATUS, &compiler_status);
	if (compiler_status == GL_FALSE) {
		int log_length;
		glGetShaderiv (result_shader, GL_INFO_LOG_LENGTH, &log_length);
	
		char *info_log = new char[log_length];
		glGetShaderInfoLog (result_shader, log_length, &log_length, info_log);

		cout << "Error compiling shader '" << filename << "': " << endl << info_log << endl;
		delete[] shader_source;
		delete[] info_log;

		return false;
	}

	delete[] shader_source;

	fragment_shader_id = result_shader;
	fragment_shader_filename = filename;

	return true;
}

bool ShaderProgram::createProgram () {
	assert (vertex_shader_id != 0);
	assert (fragment_shader_id != 0);
	program_id = glCreateProgram();

	glAttachShader (program_id, vertex_shader_id);
	glAttachShader (program_id, fragment_shader_id);

	glLinkProgram (program_id);

	int linker_status;
	glGetProgramiv (program_id, GL_LINK_STATUS, (int *) &linker_status);

	if (linker_status == GL_FALSE) {
		int log_length;
		glGetShaderiv (program_id, GL_INFO_LOG_LENGTH, &log_length);
	
		char *info_log = new char[log_length];
		glGetShaderInfoLog (program_id, log_length, &log_length, info_log);

		cout << "Error linking shaders: " << vertex_shader_filename << " and " << fragment_shader_filename << ":" << endl << info_log << endl;
		delete[] info_log;

		return false;
	}

	return true;
}

void ShaderProgram::setUniformFloat (const char* name, float value) {
	GLint loc = glGetUniformLocation(program_id, name);
	assert (loc != -1);
	glProgramUniform1f(program_id, loc, value);
}

void ShaderProgram::setUniformVector4f (const char* name, const Vector4f  &value) {
	GLint loc = glGetUniformLocation(program_id, name);
	glProgramUniform4f(program_id, loc, value[0], value[1], value[2], value[3]);
}



void ShaderProgram::deleteShaders () {
	glUseProgram(0);

	glDetachShader (program_id, vertex_shader_id);
	glDetachShader (program_id, fragment_shader_id);

	glDeleteShader (vertex_shader_id);
	glDeleteShader (fragment_shader_id);
}

void ShaderProgram::deleteProgram () {
	deleteShaders();

	glUseProgram(0);
	glDeleteProgram (program_id);
}

void ShaderProgram::printLog() {
	printShaderInfoLog(vertex_shader_id);
	printShaderInfoLog(fragment_shader_id);
	printProgramInfoLog(program_id);
}

ShaderProgram ShaderProgram::createFromFiles (
			const char* vertex_shader_filename,
			const char* fragment_shader_filename
			) {
	ShaderProgram program;

	bool result;
	result = program.loadVertexShader(vertex_shader_filename);
	assert (result);

	result = program.loadFragmentShader(fragment_shader_filename);
	assert (result);

	result = program.createProgram();
	assert (result);

	return program;
}
