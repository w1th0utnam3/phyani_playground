#version 330 core

uniform mat4 view_projection_mat;
attribute mat4 model_mat;
attribute vec3 vert_pos;

void main()
{
	gl_Position = view_projection_mat * model_mat * vec4(vert_pos, 1.0);
}
