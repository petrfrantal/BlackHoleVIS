#version 150

smooth in vec3 position_v;
smooth in vec3 normal_v;

uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
uniform vec3 pointLight1Position;
uniform vec3 pointLight2Position;
uniform vec3 materialDiffuse;

out vec4 color_f;

vec4 pointLightDiffuseComponent(vec3 position_v, vec3 normal_v, vec3 pointLightPosition, vec3 materialDiffuse, vec3 pointLightDiffuse) {
	vec3 L = normalize(pointLightPosition - position_v);
	float NLdot = max(0.0f, dot(normal_v, L));
	return vec4(materialDiffuse * pointLightDiffuse * NLdot, 0.0f);
}

void main()
{
	mat4 normalMatrix2 = transpose(inverse(modelMatrix));
	// we get the coordinates of position and normal in the world coordinates
	vec3 normalWorldSpace = normalize((normalMatrix2 * vec4(normal_v, 0.0)).xyz);
	vec3 positionWorldSpace = (modelMatrix * vec4(position_v, 1.0)).xyz;

	vec4 outputColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	// light and material specification
	vec3 pointLight1Diffuse = vec3(0.5f, 0.5f, 0.5f);
	vec3 pointLight2Diffuse = vec3(0.5f, 0.5f, 0.5f);

	// use Phong shading with diffuse component only
	outputColor += pointLightDiffuseComponent(positionWorldSpace, normalWorldSpace, pointLight1Position, materialDiffuse, pointLight1Diffuse);
	outputColor += pointLightDiffuseComponent(positionWorldSpace, normalWorldSpace, pointLight2Position, materialDiffuse, pointLight2Diffuse);

	//outputColor = vec4(1.0f, 1.0f, 1.0f, 0.0f);
	color_f = outputColor;
}
