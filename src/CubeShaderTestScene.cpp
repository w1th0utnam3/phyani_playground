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

static const int vertexCount = sizeof(cube_vertices)/(sizeof(GLfloat)*3);

void CubeShaderTestScene::initializeSceneContent()
{
	// The number of cubes per edge of the "cube grid"
	const int edgeLength = 10;
	//const int edgeLength = 42;
	//const int edgeLength = 72;

	// Generate the initial cube grid
	const glm::fmat4 id(1.0f);
	m_model_mats.reserve(edgeLength * edgeLength * edgeLength);
	for (int i = 0; i < edgeLength; i++) {
		for (int j = 0; j < edgeLength; j++) {
			for (int k = 0; k < edgeLength; k++) {
				m_model_mats.push_back(glm::translate(id, glm::fvec3(2*i - edgeLength, 2*j - edgeLength, 2*k - edgeLength)));
				m_model_mats.back() = glm::scale(m_model_mats.back(), 0.4f*glm::fvec3(1.0f, 1.0f, 1.0f));
			}
		}
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Generate buffer for vertex positions
	glGenBuffers(1, &m_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices[0], GL_STATIC_DRAW);

	// Generate buffer for model matrices
	glGenBuffers(1, &m_model_mat_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_model_mat_buffer);
	glBufferData(GL_ARRAY_BUFFER, m_model_mats.size() * (4*4) * sizeof(GLfloat), glm::value_ptr(m_model_mats[0]), GL_STREAM_DRAW);

	// Compile the shader and get attribute locations
	{
		static const std::vector<std::pair<std::string, GLenum>> shaderSources{
			{"shaders/basic.vert", GL_VERTEX_SHADER},
			{"shaders/basic.frag", GL_FRAGMENT_SHADER}
		};

		for (const auto& source : shaderSources)
			m_shaderProgram.loadShader(source.first, source.second);
		m_shaderProgram.createProgram();

		m_view_projection_mat_location = glGetUniformLocation(m_shaderProgram.program(), "view_projection_mat");
		m_model_mat_location = glGetAttribLocation(m_shaderProgram.program(), "model_mat");
		m_vert_pos_location = glGetAttribLocation(m_shaderProgram.program(), "vert_pos");
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

	m_lastTime = glfwGetTime();

	glBindVertexArray(0);
}

void CubeShaderTestScene::cleanupSceneContent()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vertex_buffer);
	glDeleteBuffers(1, &m_model_mat_buffer);
}

void CubeShaderTestScene::renderSceneContent()
{
	// Assesmble the current mvp matrix
	glm::fmat4 m = glm::fmat4(1.0f);
	glm::fmat4 v = m_camera->viewMatrix();
	glm::fmat4 p = m_camera->projectionMatrix();
	glm::fmat4 mvp = p*v*m;

	// Get dt since last render for constant rotation speed
	const double currentTime = glfwGetTime();
	const double dt = currentTime - m_lastTime;
	m_lastTime = currentTime;

	// Rotate all model matrices in random directions
	for (auto& modelMat : m_model_mats)
		modelMat = glm::rotate(modelMat, (float)(2*dt), glm::fvec3(m_random(), m_random(), m_random()));

	if (CommonOpenGl::getGlValue<GLint>(GL_VERTEX_ARRAY_BINDING) != m_vao)
		glBindVertexArray(m_vao);

	// Update the model matrix buffer
	const int cubeCount = m_model_mats.size();
	glBindBuffer(GL_ARRAY_BUFFER, m_model_mat_buffer);
	glBufferData(GL_ARRAY_BUFFER, cubeCount * (4*4) * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, cubeCount * (4*4) * sizeof(GLfloat), glm::value_ptr(m_model_mats[0]));

	m_shaderProgram.useProgram();

	// Update the mvp matrix
	glUniformMatrix4fv(m_view_projection_mat_location, 1, GL_FALSE, (const GLfloat*) glm::value_ptr(mvp));
	// Draw multiple instances of the cube
	glDrawArraysInstanced(GL_TRIANGLES, 0, vertexCount, cubeCount);

	//glUseProgram(0);
	//glBindVertexArray(0);
}
