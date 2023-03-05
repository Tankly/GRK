#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 transformation;
uniform mat4 modelMatrix;
uniform vec3 lightPos[20];
uniform vec3 lightConeDir[20];
uniform vec3 spotlightPos;

uniform vec3 cameraPos;

out vec3 vecNormal;
out vec3 worldPos;
out vec2 vecTex;

out vec3 viewDirTS;
out vec3 lightDirTS[20];
out vec3 spotlightDirTS;


void main()
{
	worldPos = (modelMatrix* vec4(vertexPosition,1)).xyz;
	vecNormal = (modelMatrix* vec4(vertexNormal,0)).xyz;

	vecTex = vertexTexCoord;
	vecTex.y = 1.0 - vecTex.y;

	gl_Position = transformation * vec4(vertexPosition, 1.0);
	//do przestrzeni �wiata
	vec3 worldTangent = normalize(mat3(modelMatrix)*vertexTangent);
	vec3 worldBitangent = normalize(mat3(modelMatrix)*vertexBitangent);

	mat3 TBN = transpose(mat3(worldTangent, worldBitangent, vecNormal));
	for(int i=0;i<20;i++){
		//wektor kierunku miedzy zrodlem swiatla a pozycja fragmentu
		vec3 lightDir = normalize(lightPos[i]-worldPos);
		//przeniesienie do przestrzeni stycznych
		lightDirTS[i] = normalize(TBN*lightDir);
	}
    //wektor kierunku widoku/ kierunek patrzeniaw
    vec3 viewDir = normalize(cameraPos - worldPos);
	//wektor kierunku miedzy statkiem a pozycja fragmentu
	vec3 spotlightDir = normalize(spotlightPos-worldPos);

	//przeniesienie do przestrzeni stycznych
	spotlightDirTS = normalize(TBN*spotlightDir);
	viewDirTS = normalize(TBN*viewDir);

}
