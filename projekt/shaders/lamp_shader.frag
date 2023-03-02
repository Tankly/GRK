#version 430 core

float AMBIENT = 0.1;

in vec3 vPosition;

out vec4 outColor;
void main()
{
	float sinVPositionX = sin(vPosition.z);
	vec3 color = vec3(1,0,0);
	if(sinVPositionX > 0) {
		color = vec3(0,1,0);
	}
	outColor = vec4(color, 1.0);
}
