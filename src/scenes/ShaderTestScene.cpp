#include "ShaderTestScene.h"


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
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

	glEnableVertexAttribArray(m_vpos_location);
	glVertexAttribPointer(m_vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) 0);

	glEnableVertexAttribArray(m_vcol_location);
	glVertexAttribPointer(m_vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) (sizeof(float) * 2));

	glBindVertexArray(0);
}

void ShaderTestScene::cleanupSceneContent()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vertex_buffer);
	glDeleteProgram(m_program);
}

void ShaderTestScene::renderSceneContent()
{
	float ratio;
	int width, height;

	glfwGetFramebufferSize(m_window, &width, &height);
	ratio = width / (float) height;
	glViewport(0, 0, width, height);

	glm::fmat4 m = glm::rotate(glm::fmat4(1.0), (float) glfwGetTime(), glm::fvec3(0.0f, 0.0f, 1.0f));
	glm::fmat4 p = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	glm::fmat4 mvp = p*m;

	glBindVertexArray(m_vao);

	glUseProgram(m_program);
	glUniformMatrix4fv(m_mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glUseProgram(0);

	glBindVertexArray(0);
}
