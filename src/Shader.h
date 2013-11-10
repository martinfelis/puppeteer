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
