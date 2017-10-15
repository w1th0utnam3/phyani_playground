#pragma once

#include <string>
#include <vector>

#include "CommonOpenGl.h"

//! Convenience class for loading shaders from files and compiling them
class ShaderProgram
{
public:
	ShaderProgram();
	//! Destructor frees the loaded shaders and program
	~ShaderProgram();

	//! Loads a shader from the specified file and compiles it.
	void loadShader(const std::string& shaderFilename, GLenum shaderType);
	//! Attaches and links all loaded shaders.
	void createProgram();
	//! Activates this shader program if it isn't already active.
	void useProgram();
	//! Returns the OpenGL index of the shader program.
	GLuint program() const;

private:
	//! The index of the shader program.
	GLuint m_program;
	//! Indices of all compiled shaders.
	std::vector<GLuint> m_shaders;
};
