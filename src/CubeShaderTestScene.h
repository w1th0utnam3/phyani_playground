#pragma once

#include "Scene.h"

#include <vector>

#include "MathHelper.h"
#include "CommonOpenGL.h"
#include "ShaderProgram.h"

class CubeShaderTestScene : public Scene
{
public:
	CubeShaderTestScene() = default;
	virtual ~CubeShaderTestScene() = default;

protected:
	virtual void initializeSceneContent() override;
	virtual void cleanupSceneContent() override;
	virtual void renderSceneContent() override;

private:
	uniform_real_rng<float> m_random;

	ShaderProgram m_shaderProgram;
	GLuint m_vertex_buffer, m_model_mat_buffer, m_vertex_shader, m_fragment_shader, m_vao;
	GLint m_view_projection_mat_location, m_model_mat_location, m_vert_pos_location;

	double m_lastTime;
	std::vector<glm::fmat4> m_model_mats;
};
