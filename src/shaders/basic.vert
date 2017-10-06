#version 330 core

uniform mat4 view_mat;
uniform mat4 projection_mat;
in mat4 model_mat;
in vec3 vertexPosition_modelspace;
in vec3 vertexNormal_modelspace;

out vec3 Normal_cameraspace;
out vec3 LightDirection_cameraspace;
out vec3 EyeDirection_cameraspace;

void main()
{
	mat4 M = model_mat;
	mat4 V = view_mat;
	mat4 P = projection_mat;
	mat4 MVP = P*V*M;

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace, 1.0);

	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPosition_cameraspace =  (V * M * vec4(vertexPosition_modelspace, 1.0)).xyz;
	EyeDirection_cameraspace = vec3(0.0, 0, 0) - vertexPosition_cameraspace;

	// Position of the light in camera space
	vec3 LightPosition_worldspace = vec3(1.1, 1.0, 2.0);

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 LightPosition_cameraspace = (V * vec4(LightPosition_worldspace, 1.0)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

	// Normal of the the vertex, in camera space
	mat4 M_inv_trans = transpose(inverse(M));
	Normal_cameraspace = (V * M_inv_trans * vec4(vertexNormal_modelspace, 0.0)).xyz;
}
