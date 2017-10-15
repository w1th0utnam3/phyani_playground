#version 330 core
#define MAX_LIGHTS 10

uniform mat4 viewMat;

in vec3 materialColor;
in vec3 normal_cameraspace;
in vec3 eyeDirection_cameraspace;

void main()
{
	mat4 V = viewMat;

	vec3 LightColor = vec3(1.0, 1.0, 1.0);

	// Define material colors based on the vertex color supplied by the vertex shader
	vec3 materialDiffuseColor = materialColor;
	vec3 materialAmbientColor = 0.3 * materialDiffuseColor;
	vec3 materialSpecularColor = materialDiffuseColor;
	float materialShininess = 5;

	// Position of the light in world space
	vec3 lightPosition_worldspace = vec3(0.5, 1.0, 6.0);

	// Vector that goes from the vertex to the light, in camera space. M is omitted because it's identity.
	vec3 lightPosition_cameraspace = (V * vec4(lightPosition_worldspace, 1.0)).xyz;
	vec3 lightDirection_cameraspace = lightPosition_cameraspace + eyeDirection_cameraspace;

	// Normal of the computed fragment, in camera space
 	vec3 n = normalize(normal_cameraspace);
 	// Direction of the light (from the fragment to the light)
 	vec3 l = normalize(lightDirection_cameraspace);
	// Cosine of the angle between surface normal and light direction
	float cosTheta = clamp(dot(n,l), 0, 1);

	// Eye vector (towards the camera)
	vec3 E = normalize(eyeDirection_cameraspace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp(dot(E, R), 0, 1);

	// Compute color based on light components
	vec3 color = materialAmbientColor * LightColor
					+ materialDiffuseColor * LightColor * cosTheta
					+ materialSpecularColor * LightColor * pow(cosAlpha, materialShininess);

	// Debug output for wrong orientation of triangles
	if (!gl_FrontFacing) {
		color = vec3(0.66, 0.0, 0.66);
	}

	gl_FragColor = vec4(color, 1.0);
}
