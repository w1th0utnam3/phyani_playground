#pragma once

#include "Scene.h"

#include <vector>

#include "DrawableManager.h"
#include "MathHelper.h"
#include "CommonOpenGl.h"
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
	GLuint m_vertex_buffer, m_normal_buffer, m_index_buffer, m_instance_buffer;
	GLuint m_vao;
	GLuint m_view_mat_location, m_projection_mat_location, m_model_mat_location;
	GLuint m_model_color_location, m_vert_pos_location, m_vert_norm_location;

	double m_lastTime;

	struct InstanceData {
		GLubyte color[4];
		GLfloat model_mat[4*4];
	};

	GLuint m_cubeDrawableId, m_sphereDrawableId;

	DrawableManager<InstanceData> m_drawables;
};
