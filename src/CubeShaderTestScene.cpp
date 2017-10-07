#include "CubeShaderTestScene.h"

#include <string>
#include <utility>

#include "CommonOpenGl.h"

using Triangle = glm::fvec3[3];

static const Triangle cube_triangles[] = {
	{{-0.5f,-0.5f,-0.5f},
	 {-0.5f,-0.5f, 0.5f},
	 {-0.5f, 0.5f, 0.5f}},

	{{ 0.5f, 0.5f,-0.5f},
	 {-0.5f,-0.5f,-0.5f},
	 {-0.5f, 0.5f,-0.5f}},

	{{ 0.5f,-0.5f, 0.5f},
	 {-0.5f,-0.5f,-0.5f},
	 { 0.5f,-0.5f,-0.5f}},

	{{ 0.5f, 0.5f,-0.5f},
	 { 0.5f,-0.5f,-0.5f},
	 {-0.5f,-0.5f,-0.5f}},

	{{-0.5f,-0.5f,-0.5f},
	 {-0.5f, 0.5f, 0.5f},
	 {-0.5f, 0.5f,-0.5f}},

	{{ 0.5f,-0.5f, 0.5f},
	 {-0.5f,-0.5f, 0.5f},
	 {-0.5f,-0.5f,-0.5f}},

	{{-0.5f, 0.5f, 0.5f},
	 {-0.5f,-0.5f, 0.5f},
	 { 0.5f,-0.5f, 0.5f}},

	{{ 0.5f, 0.5f, 0.5f},
	 { 0.5f,-0.5f,-0.5f},
	 { 0.5f, 0.5f,-0.5f}},

	{{ 0.5f,-0.5f,-0.5f},
	 { 0.5f, 0.5f, 0.5f},
	 { 0.5f,-0.5f, 0.5f}},

	{{ 0.5f, 0.5f, 0.5f},
	 { 0.5f, 0.5f,-0.5f},
	 {-0.5f, 0.5f,-0.5f}},

	{{ 0.5f, 0.5f, 0.5f},
	 {-0.5f, 0.5f,-0.5f},
	 {-0.5f, 0.5f, 0.5f}},

	{{ 0.5f, 0.5f, 0.5f},
	 {-0.5f, 0.5f, 0.5f},
	 { 0.5f,-0.5f, 0.5f}}
};

void CubeShaderTestScene::initializeSceneContent()
{
	const std::size_t component_count = (sizeof(cube_triangles) / sizeof(Triangle)) * 3 * 3;
	m_vertices.reserve(component_count);
	m_normals.reserve(component_count);

	for (const auto& triangleVertices : cube_triangles) {
		// Store all vertex components of the triangle
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				m_vertices.push_back(triangleVertices[i][j]);
			}
		}

		// Compute the triangle normal
		glm::fvec3 edge1 = triangleVertices[1] - triangleVertices[2];
		glm::fvec3 edge2 = triangleVertices[2] - triangleVertices[0];
		glm::fvec3 normal = glm::normalize(glm::cross(edge1, edge2));

		// Store triangle normal for each vertex
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) m_normals.push_back(normal[j]);
		}
	}

	// The number of cubes per edge of the "cube grid"
	const int edgeLength = 2;
	// The distance in cubes between adjacent cubes
	const int distance = 3;
	// The displacement of the whole grid in order to center it
	const float disp = ((edgeLength - 1) * distance) / 2.0;
	// Total number of cubes in the grid
	const int instanceCount = edgeLength * edgeLength * edgeLength;

	//const int edgeLength = 10;
	//const int edgeLength = 42;
	//const int edgeLength = 72;

	// Generate the initial cube grid
	const glm::fmat4 id(1.0f);
	m_model_mats.reserve(instanceCount);
	m_model_colors.reserve(instanceCount);
	for (int i = 0; i < edgeLength; i++) {
		for (int j = 0; j < edgeLength; j++) {
			for (int k = 0; k < edgeLength; k++) {
				m_model_mats.push_back(glm::translate(id, glm::fvec3(i*distance - disp, j*distance - disp, k*distance - disp)));
				m_model_colors.emplace_back(1.0f, 0.0f, 0.0f);
			}
		}
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Generate buffer for vertex positions
	glGenBuffers(1, &m_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat), m_vertices.data(), GL_STATIC_DRAW);

	// Generate buffer for vertex normals
	glGenBuffers(1, &m_normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(GLfloat), m_normals.data(), GL_STATIC_DRAW);

	// Generate buffer for model colors
	glGenBuffers(1, &m_model_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_model_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, m_model_colors.size() * 3 * sizeof(GLfloat), glm::value_ptr(m_model_colors[0]), GL_STATIC_DRAW);

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

		m_view_mat_location = glGetUniformLocation(m_shaderProgram.program(), "viewMat");
		m_projection_mat_location = glGetUniformLocation(m_shaderProgram.program(), "projectionMat");
		m_model_mat_location = glGetAttribLocation(m_shaderProgram.program(), "modelMat");
		m_model_color_location = glGetAttribLocation(m_shaderProgram.program(), "vertexColor");
		m_vert_pos_location = glGetAttribLocation(m_shaderProgram.program(), "vertexPosition_modelspace");
		m_vert_norm_location = glGetAttribLocation(m_shaderProgram.program(), "vertexNormal_modelspace");
	}

	// Set the vertex attribute pointers for the vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glEnableVertexAttribArray(m_vert_pos_location);
	glVertexAttribPointer(m_vert_pos_location, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*) 0);
	glVertexAttribDivisor(m_vert_pos_location, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
	glEnableVertexAttribArray(m_vert_norm_location);
	glVertexAttribPointer(m_vert_norm_location, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*) 0);
	glVertexAttribDivisor(m_vert_norm_location, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_model_color_buffer);
	glEnableVertexAttribArray(m_model_color_location);
	glVertexAttribPointer(m_model_color_location, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*) 0);
	glVertexAttribDivisor(m_model_color_location, 1);

	// Set the vertex attribute pointers for the model matrices (matrix is represented by 4 vectors)
	glBindBuffer(GL_ARRAY_BUFFER, m_model_mat_buffer);
	for (int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(m_model_mat_location + i);
		glVertexAttribPointer(m_model_mat_location + i, 4, GL_FLOAT, GL_FALSE,
							  sizeof(GLfloat) * 4 * 4, (void*)(sizeof(GLfloat) * i * 4));
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
	const glm::fmat4 v = m_camera->viewMatrix();
	const glm::fmat4 p = m_camera->projectionMatrix();

	// Get dt since last render for constant rotation speed
	const double currentTime = glfwGetTime();
	const double dt = currentTime - m_lastTime;
	m_lastTime = currentTime;

	// Rotate all model matrices in random directions
	/*
	for (auto& modelMat : m_model_mats)
		modelMat = glm::rotate(modelMat, (float)(2*dt), glm::fvec3(m_random(), m_random(), m_random()));
	*/

	if (CommonOpenGl::getGlValue<GLint>(GL_VERTEX_ARRAY_BINDING) != m_vao)
		glBindVertexArray(m_vao);

	// Update the model matrix buffer
	const int cubeCount = m_model_mats.size();
	glBindBuffer(GL_ARRAY_BUFFER, m_model_mat_buffer);
	glBufferData(GL_ARRAY_BUFFER, cubeCount * (4*4) * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, cubeCount * (4*4) * sizeof(GLfloat), glm::value_ptr(m_model_mats[0]));

	m_shaderProgram.useProgram();

	// Update the view and projection matrices
	glUniformMatrix4fv(m_view_mat_location, 1, GL_FALSE, (const GLfloat*) glm::value_ptr(v));
	glUniformMatrix4fv(m_projection_mat_location, 1, GL_FALSE, (const GLfloat*) glm::value_ptr(p));
	// Draw multiple instances of the cube
	glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertices.size() / 3, cubeCount);

	//glUseProgram(0);
	//glBindVertexArray(0);
}
