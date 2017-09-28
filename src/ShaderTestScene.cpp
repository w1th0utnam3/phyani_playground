#include "ShaderTestScene.h"

#include "linmath.h"

static const struct
{
	float x, y;
	float r, g, b;
} vertices[3] =
{
	{ -0.6f, -0.4f, 1.f, 0.f, 0.f },
	{  0.6f, -0.4f, 0.f, 1.f, 0.f },
	{   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char* vertex_shader_text =
	"uniform mat4 MVP;\n"
	"attribute vec3 vCol;\n"
	"attribute vec2 vPos;\n"
	"varying vec3 color;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
	"	color = vCol;\n"
	"}\n";

static const char* fragment_shader_text =
	"varying vec3 color;\n"
	"void main()\n"
	"{\n"
	"	gl_FragColor = vec4(color, 1.0);\n"
	"}\n";

void ShaderTestScene::initializeSceneContent()
{
	glGenBuffers(1, &m_vertex_buffer);

	auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);

	auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);

	m_program = glCreateProgram();
	glAttachShader(m_program, vertex_shader);
	glAttachShader(m_program, fragment_shader);
	glLinkProgram(m_program);

	m_mvp_location = glGetUniformLocation(m_program, "MVP");
	m_vpos_location = glGetAttribLocation(m_program, "vPos");
	m_vcol_location = glGetAttribLocation(m_program, "vCol");
}

void ShaderTestScene::renderSceneContent()
{
	float ratio;
	int width, height;
	mat4x4 m, p, mvp;

	glfwGetFramebufferSize(m_window, &width, &height);
	ratio = width / (float) height;
	glViewport(0, 0, width, height);

	mat4x4_identity(m);
	mat4x4_rotate_Z(m, m, (float) glfwGetTime());
	mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	mat4x4_mul(mvp, p, m);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(m_vpos_location);
	glVertexAttribPointer(m_vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) 0);

	glEnableVertexAttribArray(m_vcol_location);
	glVertexAttribPointer(m_vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) (sizeof(float) * 2));

	glUseProgram(m_program);
	glUniformMatrix4fv(m_mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glUseProgram(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(m_vpos_location);
	glDisableVertexAttribArray(m_vcol_location);
}
