#pragma once

#include <string>
#include <vector>

#include "CommonOpenGl.h"

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	void loadShader(const std::string& shaderFilename, GLenum shaderType);
	void createProgram();
	void useProgram();
	GLuint program() const;

private:
	GLuint m_program;
	std::vector<GLuint> m_shaders;
};
