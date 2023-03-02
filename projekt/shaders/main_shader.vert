#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;

uniform mat4 transformation;
uniform mat4 modelMatrix;
uniform vec3 lightPos;
uniform vec3 cameraPos;

out vec3 vNormal;
out vec3 worldPos;
out float distanceFromLight;

void main()
{
	worldPos = (modelMatrix* vec4(vertexPosition,1)).xyz;
	vNormal = (modelMatrix* vec4(vertexNormal,0)).xyz;

	distanceFromLight = length(lightPos-worldPos);

	gl_Position = transformation * vec4(vertexPosition, 1.0);
}
