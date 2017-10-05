#include "ShaderProgram.h"

#include <cassert>
#include <fstream>

ShaderProgram::ShaderProgram()
	: m_program(0)
{
}

ShaderProgram::~ShaderProgram()
{
	if (m_program != 0) glDeleteProgram(m_program);

	for (auto shader : m_shaders)
		glDeleteShader(shader);
}

void ShaderProgram::loadShader(const std::string& shaderFilename, GLenum shaderType)
{
	std::ifstream shaderFile(shaderFilename);

	std::string shaderString;
	for (std::string line; std::getline(shaderFile, line); )
		shaderString.append(line).append("\n");
	auto source = shaderString.data();

	auto shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	m_shaders.push_back(shader);
}

void ShaderProgram::createProgram()
{
	auto program = glCreateProgram();
	for (auto shader : m_shaders)
		glAttachShader(program, shader);
	glLinkProgram(program);

	m_program = program;
}

void ShaderProgram::useProgram()
{
	assert(m_program != 0);
	if (CommonOpenGl::getGlValue<GLint>(GL_CURRENT_PROGRAM) != m_program)
		glUseProgram(m_program);
}

GLuint ShaderProgram::program() const
{
	assert(m_program != 0);
	return m_program;
}
