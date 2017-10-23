#include "CubeShaderTestScene.h"

#include <cstddef>
#include <string>
#include <utility>

#include "CommonOpenGl.h"
#include "DrawableFactory.h"

void CubeShaderTestScene::initializeSceneContent()
{
	m_cubeDrawableId = m_drawables.registerDrawable(DrawableFactory::createCube());
	m_sphereDrawableId = m_drawables.registerDrawable(DrawableFactory::createSphere(4));

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

	const glm::fmat4 id(1.0f);

	// Generate the initial cube grid
	{
		GLuint instanceOffset = m_drawables.createInstances(m_cubeDrawableId, instanceCount);

		// Lock the drawable manager against reallocations
		auto bufferLock = m_drawables.createSharedLock();
		auto drawableData = m_drawables.drawable(m_cubeDrawableId);

		// Lock the drawable in order to write to the instance data buffer
		drawableData.lockUnique();
		InstanceData* data = drawableData.instanceData() + instanceOffset;

		for (int i = 0; i < edgeLength; i++) {
			for (int j = 0; j < edgeLength; j++) {
				for (int k = 0; k < edgeLength; k++) {
					data->model_mat = glm::translate(id, glm::fvec3(i*distance - disp, j*distance - disp, k*distance - disp));

					data->color[0] = 255;
					data->color[1] = 0;
					data->color[2] = 0;
					data->color[3] = 0;

					data++;
				}
			}
		}
	}

	// Draw a sphere that visualizes the position of the shader's light source
	{
		InstanceData lightSphere;
		lightSphere.color[0] = 0;
		lightSphere.color[1] = 0;
		lightSphere.color[2] = 255;
		lightSphere.color[3] = 0;
		lightSphere.model_mat = id;

		m_drawables.storeInstance(m_sphereDrawableId, lightSphere);

		lightSphere.model_mat = glm::translate(id, glm::fvec3(0.5f, 1.0f, 6.0f));
		lightSphere.model_mat = glm::scale(lightSphere.model_mat, glm::fvec3(0.2f, 0.2f, 0.2f));

		m_drawables.storeInstance(m_sphereDrawableId, lightSphere);
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Generate buffer for vertex positions
	glGenBuffers(1, &m_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, m_drawables.vertexBufferSize(), m_drawables.vertexBufferData(), GL_STATIC_DRAW);

	// Generate buffer for vertex normals
	glGenBuffers(1, &m_normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, m_drawables.normalBufferSize(), m_drawables.normalBufferData(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_drawables.indexBufferSize(), m_drawables.indexBufferData(), GL_STATIC_DRAW);

	// Generate buffer for model matrices and colors
	glGenBuffers(1, &m_instance_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(InstanceData), NULL, GL_STREAM_DRAW);

	// Compile the shader and get attribute locations
	{
		static const std::vector<std::pair<std::string, GLenum>> shaderSources{
			{"shaders/basic.vert", GL_VERTEX_SHADER},
			{"shaders/basic.frag", GL_FRAGMENT_SHADER}
		};

		for (const auto& source : shaderSources)
			m_shaderProgram.loadShader(source.first, source.second);
		m_shaderProgram.createProgram();

		m_view_mat_location = m_shaderProgram.getUniformLocation("viewMat");
		m_projection_mat_location = m_shaderProgram.getUniformLocation("projectionMat");
		m_model_mat_location = m_shaderProgram.getAttribLocation("modelMat");
		m_model_color_location = m_shaderProgram.getAttribLocation("vertexColor");
		m_vert_pos_location = m_shaderProgram.getAttribLocation("vertexPosition_modelspace");
		m_vert_norm_location = m_shaderProgram.getAttribLocation("vertexNormal_modelspace");
	}

	const std::size_t fvec3_size = sizeof(GLfloat) * 3;
	const std::size_t fvec4_size = sizeof(GLfloat) * 4;

	static_assert(std::is_standard_layout<InstanceData>::value, "InstanceData must be of standard layout in order to use offsetof");
	const std::size_t color_offset = offsetof(InstanceData, color);
	const std::size_t model_mat_offset = offsetof(InstanceData, model_mat);

	// Set the vertex attribute pointers for the vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glEnableVertexAttribArray(m_vert_pos_location);
	glVertexAttribPointer(m_vert_pos_location, 3, GL_FLOAT, GL_FALSE, fvec3_size, nullptr);
	glVertexAttribDivisor(m_vert_pos_location, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
	glEnableVertexAttribArray(m_vert_norm_location);
	glVertexAttribPointer(m_vert_norm_location, 3, GL_FLOAT, GL_FALSE, fvec3_size, nullptr);
	glVertexAttribDivisor(m_vert_norm_location, 0);

	// Set the vertex attribute pointers for the colors model
	glBindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
	glEnableVertexAttribArray(m_model_color_location);
	glVertexAttribPointer(m_model_color_location, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(InstanceData), (void*)color_offset);
	glVertexAttribDivisor(m_model_color_location, 1);

	// Set the vertex attribute pointers for the model matrices (matrix is represented by 4 vectors)
	for (unsigned int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(m_model_mat_location + i);
		glVertexAttribPointer(m_model_mat_location + i, 4, GL_FLOAT, GL_FALSE,
							  sizeof(InstanceData), (void*)(model_mat_offset + fvec4_size * i));
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
	glDeleteBuffers(1, &m_normal_buffer);
	glDeleteBuffers(1, &m_instance_buffer);

	m_drawables.clear();
}

void CubeShaderTestScene::renderSceneContent()
{
	// Lock the drawable manager against clearing and reallocations
	auto bufferLock = m_drawables.createSharedLock();

	// Get view matrices from the camera
	const glm::fmat4 v = m_camera->viewMatrix();
	const glm::fmat4 p = m_camera->projectionMatrix();

	// Get dt since last render
	const double currentTime = glfwGetTime();
	const double dt = currentTime - m_lastTime;
	m_lastTime = currentTime;

	// Loop over all drawables
	for (const auto drawableData : m_drawables)
	{
		const GLsizei instanceCount = drawableData.instanceCount();
		const GLsizei elementCount = drawableData.indexCount;
		const GLvoid* indexPtrOffset = drawableData.indexPtrOffset;

		// Skip drawables without instances
		if (instanceCount == 0) continue;

		// Bind the VAO if necessary
		if (common_opengl::getGlValue<GLint>(GL_VERTEX_ARRAY_BINDING) != m_vao)
			glBindVertexArray(m_vao);

		// Update the model matrix buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
		glBufferData(GL_ARRAY_BUFFER, drawableData.instanceDataSize(), nullptr, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, drawableData.instanceDataSize(), drawableData.instanceData());

		// Activate the shader if necessary
		m_shaderProgram.useProgram();

		// Update the view and projection matrices according to the current camera configuration
		glUniformMatrix4fv(m_view_mat_location, 1, GL_FALSE, glm::value_ptr(v));
		glUniformMatrix4fv(m_projection_mat_location, 1, GL_FALSE, glm::value_ptr(p));

		// Draw the instances
		glDrawElementsInstancedBaseVertex(drawableData.glMode,
										  elementCount,
										  drawableData.glIndexType, indexPtrOffset,
										  instanceCount,
										  drawableData.baseVertex);
	}
}
