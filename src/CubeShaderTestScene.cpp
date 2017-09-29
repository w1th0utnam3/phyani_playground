#include "CubeShaderTestScene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <CommonOpenGL.h>

#include <iostream>

static const GLfloat cube_vertices[] =
{
	-0.5f,-0.5f,-0.5f,
	-0.5f,-0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	 0.5f, 0.5f,-0.5f,
	-0.5f,-0.5f,-0.5f,
	-0.5f, 0.5f,-0.5f,
	 0.5f,-0.5f, 0.5f,
	-0.5f,-0.5f,-0.5f,
	 0.5f,-0.5f,-0.5f,
	 0.5f, 0.5f,-0.5f,
	 0.5f,-0.5f,-0.5f,
	-0.5f,-0.5f,-0.5f,
	-0.5f,-0.5f,-0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f,-0.5f,
	 0.5f,-0.5f, 0.5f,
	-0.5f,-0.5f, 0.5f,
	-0.5f,-0.5f,-0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f,-0.5f, 0.5f,
	 0.5f,-0.5f, 0.5f,
	 0.5f, 0.5f, 0.5f,
	 0.5f,-0.5f,-0.5f,
	 0.5f, 0.5f,-0.5f,
	 0.5f,-0.5f,-0.5f,
	 0.5f, 0.5f, 0.5f,
	 0.5f,-0.5f, 0.5f,
	 0.5f, 0.5f, 0.5f,
	 0.5f, 0.5f,-0.5f,
	-0.5f, 0.5f,-0.5f,
	 0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f,-0.5f,
	-0.5f, 0.5f, 0.5f,
	 0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	 0.5f,-0.5f, 0.5f
};

static const char* vertex_shader_text =
	"uniform mat4 MVP;\n"
	"attribute vec3 vPos;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = MVP * vec4(vPos, 1.0);\n"
	"}\n";

static const char* fragment_shader_text =
	"void main()\n"
	"{\n"
	"	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
	"}\n";

void CubeShaderTestScene::initializeSceneContent()
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices[0], GL_STATIC_DRAW);

	auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);

	auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);

	m_program = glCreateProgram();
	glAttachShader(m_program, vertex_shader);
	glAttachShader(m_program, fragment_shader);
	glLinkProgram(m_program);

	m_mvp_location = glGetUniformLocation(m_program, "MVP");
	m_vpos_location = glGetAttribLocation(m_program, "vPos");

	glEnableVertexAttribArray(m_vpos_location);
	glVertexAttribPointer(m_vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);

	glBindVertexArray(0);
}

void CubeShaderTestScene::cleanupSceneContent()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vertex_buffer);
	glDeleteProgram(m_program);
}

void CubeShaderTestScene::renderSceneContent()
{
	glm::fmat4 m = glm::rotate(glm::fmat4(1.0f), (float) -glfwGetTime(), glm::fvec3(0.5f, 0.5f, 0.5f));
	glm::fmat4 v = m_camera->viewMatrix();
	glm::fmat4 p = m_camera->projectionMatrix();
	glm::fmat4 mvp = p*v*m;

	glBindVertexArray(m_vao);

	glUseProgram(m_program);
	glUniformMatrix4fv(m_mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices)/(sizeof(float)*3));
	glUseProgram(0);

	glBindVertexArray(0);
}
