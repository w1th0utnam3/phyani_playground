#pragma once

#include "Scene.h"

#include <glad/glad.h>

//! A scene with a slowly rotating triangle to demo the use of shaders
class TriangleShaderScene : public Scene
{
public:
	TriangleShaderScene() = default;
	~TriangleShaderScene() = default;

protected:
	virtual void initializeSceneContent();
	virtual void renderSceneContent();

private:
	//! Buffer for triangle vertices
	GLuint m_vertexBuffer;
	
	//! Vertex shader
	GLuint m_vertexShader;
	//! Fragment shader
	GLuint m_fragmentShader;
	//! The linked shader program
	GLuint m_program;

	// Shader attribute locations
	GLint m_mvpLocation, m_vposLocation, m_vcolLocation;
};
