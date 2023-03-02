#version 430 core

out vec4 out_color;

uniform vec3 lightColor;
uniform vec3 lightPos;
//pozycja kamery
uniform vec3 cameraPos;

uniform vec3 spotlightPos;
uniform vec3 spotlightColor;
uniform vec3 spotlightConeDir;

uniform sampler2D colorTexture;
in vec2 vecTex;

//rzeczywista pozycja fragmentu
in vec3 worldPos;
//wektor prostopadly do powierzchni wierzcholka
in vec3 vecNormal;
//odległość fragmentu od światła
in float distanceFromLight;

//jak bardzo domyslnie widoczne sa elementy
const float AMBIENT_STRENGTH = 0.05;
//intensywnosc odblyskow
const float SPECULAR_STRENGTH = 0.2;
//parametr ekspozycji
const float EXPOSITION = 122.0f;

vec3 phongLight(vec3 lightDir, vec3 lightColor, vec3 vNormal,vec3 viewDir){
    //rozproszony wplyw swiatla na biezacy fragment max poniewaz powyzej 90 jest ujemna liczba - brak wplywu
    float diff = max(dot(vNormal, lightDir), 0.0);
    //skladnik rozproszenia
    vec3 diffuse = diff * lightColor;
    //wektor odbicia wzdluz osi normalnej / kierunek odbicia
    //minus przy lighDir bo wskazuje on od fragmentu w kierunku zrodla swiatla a reflecrt oczekuje na odwrot
    vec3 reflectDir = reflect(-lightDir, vNormal);

    //skladowa zwierciadlana
    //im mniejsza potega tym odbicie jest bardziej rozproszone a nie w jednym punkcie
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = SPECULAR_STRENGTH * spec * lightColor;

	vec3 resultColor = (diffuse+specular);
	return resultColor;
}

void main()
{
    //oświetlenie
    vec3 ilumination;
    //kolor światła
    vec3 lColor;

    vec3 textureColor = texture2D(colorTexture, vecTex*20).xyz;

     //wektor kierunku widoku/ kierunek patrzenia
    vec3 viewDir = normalize(cameraPos - worldPos);

    //SPACESHIP LIGHTS
    vec3 spotlightDir= normalize(spotlightPos-worldPos);
	float angleAtenuation = clamp((dot(-spotlightDir,spotlightConeDir)-0.8)*3,0,1);
    float distanceFromSpaceshipLight = length(spotlightPos-worldPos);
    lColor = spotlightColor/pow(distanceFromSpaceshipLight,2)*angleAtenuation;
	ilumination+=phongLight(normalize(spotlightDir),lColor,vecNormal,viewDir)*textureColor;

    //WORLD
    lColor = lightColor/pow(distanceFromLight,2);
    //wektor kierunku miedzy zrodlem swiatla a pozycja fragmentu
    vec3 lightDir = normalize(lightPos - worldPos);
    //Otaczajace swiatlo
    vec3 ambient = AMBIENT_STRENGTH * lightColor;

    //Mieszany Phong - wszystkie opcje
    ilumination += (phongLight(lightDir,lColor,vecNormal,viewDir)) * textureColor;
    //Tone mapping - Osłabienia światła przy większej odległośći
    vec3 toneMappingColor = 1.0 - exp(-ilumination*EXPOSITION);
	out_color = vec4(toneMappingColor+(ambient* textureColor),1);
}
