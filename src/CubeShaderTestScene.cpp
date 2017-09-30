#include "CubeShaderTestScene.h"

#include <CommonOpenGL.h>

// Vertices for a cube centered at origin with edge length 1.0
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

// Model matrices for multiple cube instances
static glm::fmat4 model_mats[] =
{
	glm::scale(glm::translate(glm::fmat4(1.0f), glm::fvec3(0.5f, 0.0f, 0.0f)), glm::fvec3(0.5f, 0.5f, 0.5f)),
	glm::scale(glm::translate(glm::fmat4(1.0f), glm::fvec3(-0.5f, 0.0f, 0.0f)), glm::fvec3(0.5f, 0.5f, 0.5f))
};

static const int num_vertices = sizeof(cube_vertices)/(sizeof(GLfloat)*3);
static const int num_cubes = sizeof(model_mats)/sizeof(glm::fmat4);

static const char* vertex_shader_text =
	"uniform mat4 view_projection_mat;\n"
	"attribute mat4 model_mat;\n"
	"attribute vec3 vert_pos;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = view_projection_mat * model_mat * vec4(vert_pos, 1.0);\n"
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

	// Generate buffer for vertex positions
	glGenBuffers(1, &m_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices[0], GL_STATIC_DRAW);

	// Generate buffer for model matrices
	glGenBuffers(1, &m_model_mat_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_model_mat_buffer);
	glBufferData(GL_ARRAY_BUFFER, num_cubes * (4*4) * sizeof(GLfloat), glm::value_ptr(model_mats[0]), GL_STREAM_DRAW);

	// Compile the shader and get attribute locations
	{
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

		m_view_projection_mat_location = glGetUniformLocation(m_program, "view_projection_mat");
		m_model_mat_location = glGetAttribLocation(m_program, "model_mat");
		m_vert_pos_location = glGetAttribLocation(m_program, "vert_pos");
	}

	// Set the vertex attribute pointers for the vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glEnableVertexAttribArray(m_vert_pos_location);
	glVertexAttribPointer(m_vert_pos_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*) 0);
	glVertexAttribDivisor(m_vert_pos_location, 0);

	// Set the vertex attribute pointers for the model matrices (matrix is represented by 4 vectors)
	glBindBuffer(GL_ARRAY_BUFFER, m_model_mat_buffer);
	for (int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(m_model_mat_location + i);
		glVertexAttribPointer(m_model_mat_location + i, 4, GL_FLOAT, GL_FALSE,
							  sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * i * 4));
		// Set the divisor so that one model matrix is used for every instance instead of every vertex
		glVertexAttribDivisor(m_model_mat_location + i, 1);
	}

	glBindVertexArray(0);
}

void CubeShaderTestScene::cleanupSceneContent()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vertex_buffer);
	glDeleteBuffers(1, &m_model_mat_buffer);
	glDeleteProgram(m_program);
}

void CubeShaderTestScene::renderSceneContent()
{
	// Assesmble the current mvp matrix
	glm::fmat4 m = glm::fmat4(1.0f);
	glm::fmat4 v = m_camera->viewMatrix();
	glm::fmat4 p = m_camera->projectionMatrix();
	glm::fmat4 mvp = p*v*m;

	// Compute new model matrices
	model_mats[0] = glm::rotate(glm::scale(glm::translate(glm::fmat4(1.0f), glm::fvec3(0.5f, 0.0f, 0.0f)), glm::fvec3(0.5f, 0.5f, 0.5f)),
								(float) -glfwGetTime(), glm::fvec3(0.0f, 0.0f, 1.0f));
	model_mats[1] = glm::rotate(glm::scale(glm::translate(glm::fmat4(1.0f), glm::fvec3(-0.5f, 0.0f, 0.0f)), glm::fvec3(0.5f, 0.5f, 0.5f)),
								(float) -glfwGetTime(), glm::fvec3(0.0f, 1.0f, 0.0f));

	glBindVertexArray(m_vao);

	// Update the model matrix buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_model_mat_buffer);
	glBufferData(GL_ARRAY_BUFFER, num_cubes * (4*4) * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, num_cubes * (4*4) * sizeof(GLfloat), glm::value_ptr(model_mats[0]));

	glUseProgram(m_program);

	// Update the mvp matrix
	glUniformMatrix4fv(m_view_projection_mat_location, 1, GL_FALSE, (const GLfloat*) glm::value_ptr(mvp));
	// Draw multiple instances of the cube
	glDrawArraysInstanced(GL_TRIANGLES, 0, num_vertices, num_cubes);

	glUseProgram(0);

	glBindVertexArray(0);
}
