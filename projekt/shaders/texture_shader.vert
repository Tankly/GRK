#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 transformation;
uniform mat4 modelMatrix;
uniform vec3 lightPos;
uniform vec3 spotlightPos;
uniform vec3 cameraPos;

out vec3 vecNormal;
out vec3 worldPos;
out vec2 vecTex;
out float distanceFromLight;

out vec3 viewDirTS;
out vec3 lightDirTS;
out vec3 spotlightDirTS;

void main()
{
	worldPos = (modelMatrix* vec4(vertexPosition,1)).xyz;
	vecNormal = (modelMatrix* vec4(vertexNormal,0)).xyz;

	distanceFromLight = length(lightPos-worldPos);

	vecTex = vertexTexCoord;
	vecTex.y = 1.0 - vecTex.y;

	gl_Position = transformation * vec4(vertexPosition, 1.0);
	//do przestrzeni œwiata
	vec3 worldTangent = normalize(mat3(modelMatrix)*vertexTangent);
	vec3 worldBitangent = normalize(mat3(modelMatrix)*vertexBitangent);

	mat3 TBN = transpose(mat3(worldTangent, worldBitangent, vecNormal));
	
    //wektor kierunku widoku/ kierunek patrzenia
    vec3 viewDir = normalize(cameraPos - worldPos);
	//wektor kierunku miedzy zrodlem swiatla a pozycja fragmentu
	vec3 lightDir = normalize(lightPos-worldPos);
	//wektor kierunku miedzy statkiem a pozycja fragmentu
	vec3 spotlightDir = normalize(spotlightPos-worldPos);
	//przeniesienie do przestrzeni stycznych
	lightDirTS = normalize(TBN*lightDir);
	viewDirTS = normalize(TBN*viewDir);
	spotlightDirTS = normalize(TBN*spotlightDir);
}
