#version 330 core

in vec3 Normal_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 EyeDirection_cameraspace;

void main()
{
	vec3 LightColor = vec3(1.0, 1.0, 1.0);

	vec3 MaterialDiffuseColor = vec3(1.0, 0.0, 0.0);
	vec3 MaterialAmbientColor = 0.3 * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(1.0, 1.0, 1.0);
	float MaterialShininess = 5;

	// Normal of the computed fragment, in camera space
 	vec3 n = normalize(Normal_cameraspace);
 	// Direction of the light (from the fragment to the light)
 	vec3 l = normalize(LightDirection_cameraspace);
	// Cosine of the angle between surface normal and light direction
	float cosTheta = clamp(dot(n,l), 0, 1);

	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_cameraspace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp(dot(E, R), 0, 1);

	vec3 color = MaterialAmbientColor * LightColor
					+ MaterialDiffuseColor * LightColor * cosTheta
					+ MaterialSpecularColor * LightColor * pow(cosAlpha, MaterialShininess);

	gl_FragColor = vec4(color, 1.0);
}
