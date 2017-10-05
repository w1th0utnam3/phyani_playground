#pragma once

#include "Scene.h"

#include <vector>
#include <random>

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
	ShaderProgram m_shaderProgram;
	GLuint m_vertex_buffer, m_model_mat_buffer, m_vertex_shader, m_fragment_shader, m_vao;
	GLint m_view_projection_mat_location, m_model_mat_location, m_vert_pos_location;

	class random_data {
		std::random_device rd;
		std::mt19937 gen;
		std::uniform_real_distribution<float> dis;

	public:
		random_data()
			: gen(rd())
			, dis(0.0, 1.0) {}

		float operator()() {
			return dis(gen);
		}
	};

	random_data m_random;

	double m_lastTime;
	std::vector<glm::fmat4> m_model_mats;
};
