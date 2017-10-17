#include "CubeShaderTestScene.h"

#include <string>
#include <utility>

#include "CommonOpenGl.h"
#include "DrawableFactory.h"

void CubeShaderTestScene::initializeSceneContent()
{
	m_cubeDrawableId = m_drawables.registerDrawable(DrawableFactory::createCube());

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

		auto bufferLock = m_drawables.shared_lock();
		auto drawableData = m_drawables.drawable(m_cubeDrawableId);
		InstanceData* data = drawableData.instanceData() + instanceOffset;

		for (int i = 0; i < edgeLength; i++) {
			for (int j = 0; j < edgeLength; j++) {
				for (int k = 0; k < edgeLength; k++) {
					glm::fmat4* model_mat = reinterpret_cast<glm::fmat4*>(&data->model_mat);
					*model_mat = glm::translate(id, glm::fvec3(i*distance - disp, j*distance - disp, k*distance - disp));

					data->color[0] = 255;
					data->color[1] = 0;
					data->color[2] = 0;
					data->color[3] = 0;

					data++;
				}
			}
		}
	}

	{
		InstanceData lightCube;
		lightCube.color[0] = 0;
		lightCube.color[1] = 0;
		lightCube.color[2] = 255;
		lightCube.color[3] = 0;
		glm::fmat4* model_mat = reinterpret_cast<glm::fmat4*>(lightCube.model_mat);
		*model_mat = glm::translate(id, glm::fvec3(0.5f, 1.0f, 6.0f));
		*model_mat = glm::scale(*model_mat, glm::fvec3(0.2f, 0.2f, 0.2f));

		m_drawables.storeInstance(m_cubeDrawableId, lightCube);
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

		m_view_mat_location = glGetUniformLocation(m_shaderProgram.program(), "viewMat");
		m_projection_mat_location = glGetUniformLocation(m_shaderProgram.program(), "projectionMat");
		m_model_mat_location = glGetAttribLocation(m_shaderProgram.program(), "modelMat");
		m_model_color_location = glGetAttribLocation(m_shaderProgram.program(), "vertexColor");
		m_vert_pos_location = glGetAttribLocation(m_shaderProgram.program(), "vertexPosition_modelspace");
		m_vert_norm_location = glGetAttribLocation(m_shaderProgram.program(), "vertexNormal_modelspace");
	}

	const std::size_t fvec3_size = sizeof(GLfloat) * 3;
	const std::size_t fvec4_size = sizeof(GLfloat) * 4;

	InstanceData instanceData;
	const std::size_t color_offset = 0;
	const std::size_t model_mat_offset = reinterpret_cast<char*>(&instanceData.model_mat) - reinterpret_cast<char*>(&instanceData.color[0]);

	// Set the vertex attribute pointers for the vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glEnableVertexAttribArray(m_vert_pos_location);
	glVertexAttribPointer(m_vert_pos_location, 3, GL_FLOAT, GL_FALSE, fvec3_size, (void*) 0);
	glVertexAttribDivisor(m_vert_pos_location, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
	glEnableVertexAttribArray(m_vert_norm_location);
	glVertexAttribPointer(m_vert_norm_location, 3, GL_FLOAT, GL_FALSE, fvec3_size, (void*) 0);
	glVertexAttribDivisor(m_vert_norm_location, 0);

	// Set the vertex attribute pointers for the colors model
	glBindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
	glEnableVertexAttribArray(m_model_color_location);
	glVertexAttribPointer(m_model_color_location, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(InstanceData), (void*)color_offset);
	glVertexAttribDivisor(m_model_color_location, 1);

	// Set the vertex attribute pointers for the model matrices (matrix is represented by 4 vectors)
	for (int i = 0; i < 4; i++) {
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
	auto bufferLock = m_drawables.shared_lock();

	const glm::fmat4 v = m_camera->viewMatrix();
	const glm::fmat4 p = m_camera->projectionMatrix();

	// Get dt since last render for constant rotation speed
	const double currentTime = glfwGetTime();
	const double dt = currentTime - m_lastTime;
	m_lastTime = currentTime;	

	for (auto it = m_drawables.drawablesBegin(); it != m_drawables.drawablesEnd(); ++it) {
		auto drawableData = *it;

		// Rotate all model matrices in random directions
		/*
		for (auto& modelMat : m_model_mats)
			modelMat = glm::rotate(modelMat, (float)(2*dt), glm::fvec3(m_random(), m_random(), m_random()));
		*/

		if (CommonOpenGl::getGlValue<GLint>(GL_VERTEX_ARRAY_BINDING) != m_vao)
			glBindVertexArray(m_vao);

		// Update the model matrix buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
		glBufferData(GL_ARRAY_BUFFER, drawableData.instanceDataSize(), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, drawableData.instanceDataSize(), drawableData.instanceData());

		m_shaderProgram.useProgram();

		// Update the view and projection matrices
		glUniformMatrix4fv(m_view_mat_location, 1, GL_FALSE, (const GLfloat*) glm::value_ptr(v));
		glUniformMatrix4fv(m_projection_mat_location, 1, GL_FALSE, (const GLfloat*) glm::value_ptr(p));

		// Draw multiple instances of the cube
		const GLuint instanceCount = drawableData.instanceCount();
		const GLuint drawableVertexCount = drawableData.vertexCount;
		const GLuint elementCount = drawableVertexCount / 3;

		// TODO: Move element count to property of drawable

		if (instanceCount > 1) {
			glDrawElementsInstanced(drawableData.mode, elementCount, GL_UNSIGNED_INT, nullptr, instanceCount);
		} else {
			glDrawElements(drawableData.mode, elementCount, GL_UNSIGNALED, nullptr);
		}
	}
}
