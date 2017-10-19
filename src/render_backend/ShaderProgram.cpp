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

bool ShaderProgram::loadShader(const std::string& shaderFilename, GLenum shaderType)
{
	std::ifstream shaderFile(shaderFilename);

	std::string shaderString;
	for (std::string line; std::getline(shaderFile, line); )
		shaderString.append(line).append("\n");
	auto source = shaderString.data();

	auto shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	// Handle potential compilation errors
	bool success = CommonOpenGl::getGlShaderCompileStatus(shader);
	if (!success) {
		std::cerr << "Shader compilation error: " << shaderFilename << "\n";
		std::cerr << CommonOpenGl::getGlShaderInfoLog(shader) << "\n";
		glDeleteShader(shader);
	} else {
		m_shaders.push_back(shader);
	}

	return success;
}

bool ShaderProgram::createProgram()
{
	auto program = glCreateProgram();
	for (auto shader : m_shaders)
		glAttachShader(program, shader);
	glLinkProgram(program);

	// Handle potential linking errors
	bool success = CommonOpenGl::getGlProgramLinkStatus(program);
	if (!success) {
		std::cerr << "Program linker error:\n";
		std::cerr << CommonOpenGl::getGlProgramInfoLog(program) << "\n";
		glDeleteProgram(program);
	} else {
		m_program = program;
	}

	return success;
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

GLuint ShaderProgram::getUniformLocation(const std::string& name) const
{
	GLint uniform;
	assert(m_program != 0);
	uniform = glGetUniformLocation(m_program, name.c_str());
	if (uniform == -1)
		std::cerr << "Shader uniform '" << name << "' could not be located (glGetUniformLocation returned -1).\n";
	return static_cast<GLuint>(uniform);
}

GLuint ShaderProgram::getAttribLocation(const std::string& name) const
{
	GLint uniform;
	assert(m_program != 0);
	uniform = glGetAttribLocation(m_program, name.c_str());
	if (uniform == -1)
		std::cerr << "Shader attribute '" << name << "' could not be located (glGetAttribLocation returned -1).\n";
	return static_cast<GLuint>(uniform);
}
