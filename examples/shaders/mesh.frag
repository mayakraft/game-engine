#version 330 core

#define MAX_LIGHTS 10

struct Light {
	vec3 position;
	vec3 color;
};

uniform Light u_lights[MAX_LIGHTS];
uniform int u_numLights;
uniform vec3 u_ambientColor;
uniform vec3 u_materialColor;

in vec3 v_normal;
in vec3 v_fragPos;

out vec4 outColor;

void main() {
	vec3 ambient = u_ambientColor * u_materialColor;
	vec3 norm = normalize(v_normal);
	vec3 resultColor = ambient;

	for (int i = 0; i < u_numLights; i++) {
		vec3 lightDir = normalize(u_lights[i].position - v_fragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		// vec3 diffuse = diff * u_lights[i].color * 0.5; // this looks cool, but is wrong
		vec3 diffuse = diff * u_lights[i].color * u_materialColor;
		resultColor += diffuse;
	}

	outColor = vec4(resultColor, 1.0);
}
