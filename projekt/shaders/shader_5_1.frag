#version 430 core

out vec4 out_color;
uniform vec3 modelColor;
uniform vec3 lightDir;
uniform vec3 lightColor;

in vec3 vNormal;
void main()
{
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    out_color = vec4(ambient * modelColor, 1.0);
}
