#version 330 core

uniform mat4 viewMat;
uniform mat4 projectionMat;

in mat4 modelMat;
in vec4 vertexColor;
in vec3 vertexPosition_modelspace;
in vec3 vertexNormal_modelspace;

out vec3 materialColor;
out vec3 normal_cameraspace;
out vec3 eyeDirection_cameraspace;

void main()
{
	mat4 M = modelMat;
	mat4 V = viewMat;
	mat4 P = projectionMat;
	mat4 MVP = P*V*M;

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace, 1.0);

	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPosition_cameraspace =  (V * M * vec4(vertexPosition_modelspace, 1.0)).xyz;
	eyeDirection_cameraspace = vec3(0.0, 0, 0) - vertexPosition_cameraspace;

	// Normal of the the vertex, in camera space
	mat4 M_inv_trans = transpose(inverse(M));
	normal_cameraspace = (V * M_inv_trans * vec4(vertexNormal_modelspace, 0.0)).xyz;

	// Set the output color
	materialColor = vertexColor.xyz;
}
