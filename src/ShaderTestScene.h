#pragma once

#include "CommonOpenGL.h"
#include "Scene.h"

class ShaderTestScene : public Scene
{
public:
	ShaderTestScene() = default;
	virtual ~ShaderTestScene() = default;

protected:
	virtual void initializeSceneContent() override;
	virtual void cleanupSceneContent() override;
	virtual void renderSceneContent() override;

private:
	GLuint m_vertex_buffer, m_vertex_shader, m_fragment_shader, m_program, m_vao;
	GLint m_mvp_location, m_vpos_location, m_vcol_location;
};
