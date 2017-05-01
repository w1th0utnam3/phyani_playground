#include "TriangleShaderScene.h"

#include "linmath.h"

#include <GLFW\glfw3.h>

void TriangleShaderScene::initializeSceneContent()
{
	static constexpr struct
	{
		float x, y;
		float r, g, b;
	} vertices[3] =
		{
			{-0.6f, -0.4f, 1.f, 0.f, 0.f},
			{0.6f, -0.4f, 0.f, 1.f, 0.f},
			{0.f, 0.6f, 0.f, 0.f, 1.f}
		};

	// Fill vertex buffer with triangle
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	static constexpr const char* vertex_shader_text =
		"uniform mat4 MVP;\n"
		"attribute vec3 vCol;\n"
		"attribute vec2 vPos;\n"
		"varying vec3 color;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
		"    color = vCol;\n"
		"}\n";
	static constexpr const char* fragment_shader_text =
		"varying vec3 color;\n"
		"void main()\n"
		"{\n"
		"    gl_FragColor = vec4(color, 1.0);\n"
		"}\n";

	// Compile vertex shader
	m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_vertexShader, 1, &vertex_shader_text, nullptr);
	glCompileShader(m_vertexShader);

	// Compile fragment shader
	m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_fragmentShader, 1, &fragment_shader_text, nullptr);
	glCompileShader(m_fragmentShader);

	// Link shader program
	m_program = glCreateProgram();
	glAttachShader(m_program, m_vertexShader);
	glAttachShader(m_program, m_fragmentShader);
	glLinkProgram(m_program);

	// Get argument locations
	m_mvpLocation = glGetUniformLocation(m_program, "MVP");
	m_vposLocation = glGetAttribLocation(m_program, "vPos");
	m_vcolLocation = glGetAttribLocation(m_program, "vCol");

	glEnableVertexAttribArray(m_vposLocation);
	glVertexAttribPointer(m_vposLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
	glEnableVertexAttribArray(m_vcolLocation);
	glVertexAttribPointer(m_vcolLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, reinterpret_cast<void*>(sizeof(float) * 2));
}

void TriangleShaderScene::renderSceneContent()
{
	static int width, height;
	static float ratio;
	static mat4x4 m, p, mvp;

	glfwGetFramebufferSize(window(), &width, &height);
	ratio = static_cast<float>(width) / static_cast<float>(height);

	mat4x4_identity(m);
	mat4x4_rotate_Z(m, m, static_cast<float>(0.2 * glfwGetTime()));
	mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	mat4x4_mul(mvp, p, m);

	glUseProgram(m_program);
	glUniformMatrix4fv(m_mvpLocation, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(mvp));
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
