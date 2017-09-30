#pragma once

#include "Scene.h"
#include "CommonOpenGL.h"

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
	GLuint m_vertex_buffer, m_model_mat_buffer, m_vertex_shader, m_fragment_shader, m_program, m_vao;
	GLint m_view_projection_mat_location, m_model_mat_location, m_vert_pos_location;
};
