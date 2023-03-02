#version 430 core

float AMBIENT = 0.1;

uniform vec3 lightPos;
uniform sampler2D colorTexture;
uniform sampler2D clouds;

in vec3 vecNormal;
in vec3 worldPos;
in vec2 vecTex;

out vec4 outColor;
void main()
{
	vec3 lightDir = normalize(lightPos-worldPos);
	vec3 normal = normalize(vecNormal);
	vec3 textureColor = texture2D(colorTexture, vecTex*20).xyz;
	vec3 cloudss = texture2D(clouds, vecTex*20).xyz;
	float diffuse=max(0,dot(normal,lightDir));
	outColor = vec4(mix(cloudss,textureColor*min(1,AMBIENT+diffuse),0.8), 1.0);
}
