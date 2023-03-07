#version 430 core

out vec4 out_color;

uniform vec3 lightColor;
uniform vec3 lightPos[20];
uniform vec3 lightConeDir[20];
//pozycja kamery
uniform vec3 cameraPos;

uniform vec3 spotlightPos;

uniform vec3 spotlightColor;

uniform vec3 spotlightConeDir;


uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;


in vec2 vecTex;

//rzeczywista pozycja fragmentu
in vec3 worldPos;

in vec3 spotlightDir;
//wektor prostopadly do powierzchni wierzcholka
in vec3 vecNormal;


//wektor kierunku widoku/ kierunek
in vec3 viewDirTS;
in vec3 lightDirTS[20];
in vec3 spotlightDirTS;

//jak bardzo domyslnie widoczne sa elementy
const float AMBIENT_STRENGTH = 0.05;
//intensywnosc odblyskow
const float SPECULAR_STRENGTH = 0.2;
//parametr ekspozycji - tone mapping
const float EXPOSITION = 122.0f;
const float PI = 3.14;
const float MULTIPLIER = 15;
// Normal distribution function przybliża ilość powierzchni, która jest ustawiona prostopadle do wektora połówkowego korzystamy z funkcji Trowbridge-Reitz GGX
float DistributionGGX(vec3 normal, vec3 H, float roughness){
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(normal, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}
// G - Geometry function opisuje stopień samo-zacieninienia. Wykorzystujemy Schlick-GGX
float GeometrySchlickGGX(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 normal, vec3 V, vec3 lightDir, float roughness){
    float NdotV = max(dot(normal, V), 0.0);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
// F - Fresnel equation opisuje stopień odbicia w zależności od kąta padania. Wykorzystujemy aproksymację Fresnela-Schlicka
// stopień odbicia w zależności od kąta padania
vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
vec3 PBRLight(vec3 lightDir, vec3 radiance, vec3 normal, vec3 V, vec3 color){
    float metallic  = texture(metallicMap, vecTex*MULTIPLIER).r;
    float roughness = texture(roughnessMap, vecTex*MULTIPLIER).r;
    // światło rozproszone 
	float diffuse=max(0,dot(normal,lightDir));

	vec3 F0 = vec3(0.04); 
    // wałsność materiału
    F0 = mix(F0, color, metallic);

    vec3 H = normalize(V + lightDir);    
        
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(normal, H, roughness);   
        float G   = GeometrySmith(normal, V, lightDir, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(normal, V), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        // światło kierunkowe
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = vec3(0);
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(normal, lightDir), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * color / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
	return Lo;
}

void main()
{
    // jak pada światło w szczeliny
    float ao = texture(aoMap, vecTex*MULTIPLIER).r;
    vec3 normal = vec3(0,0,1);
    //oświetlenie
    vec3 ilumination;
    //kolor światła
    vec3 lColor;

    vec3 textureColor = texture(albedoMap, vecTex*MULTIPLIER).xyz;
    // sztuczne zagłębienia
    normal = texture(normalMap, vecTex*MULTIPLIER).xyz;
    //przekształcenie na [−1,1]
    normal = normalize(normal*2-1);

    //JEEP LIGHTS
    vec3 spotlightDir= normalize(spotlightPos-worldPos);
	float angleAtenuation = clamp((dot(-spotlightDir,spotlightConeDir)-0.8)*3,0,1);
    float distanceFromSpaceshipLight = length(spotlightPos-worldPos);
    lColor = spotlightColor/pow(distanceFromSpaceshipLight,2)*angleAtenuation;
	ilumination+=PBRLight(spotlightDirTS,lColor,normal,viewDirTS, textureColor);


    //WORLD
    //Otaczajace swiatlo
    vec3 ambient = AMBIENT_STRENGTH * lightColor * ao;

    for(int i=0;i<20;i++) {
        vec3 lightDir= normalize(lightPos[i]-worldPos);
        //vec3 lightConeDir = vec3(0.2f,1.3f, 0.f);
        //bez - przy lightDir
	    angleAtenuation = clamp((dot(-lightDir,lightConeDir[i])-0.5)*3,0,1);
        float distanceFromLight = length(lightPos[i]-worldPos);
        lColor = lightColor/pow(distanceFromLight,2)*angleAtenuation;
	    ilumination+=PBRLight(lightDirTS[i],lColor,normal,viewDirTS, textureColor);
    }



    //Tone mapping - Osłabienia światła przy większej odległośći
    vec3 toneMappingColor = 1.0 - exp(-ilumination*EXPOSITION);
    toneMappingColor = toneMappingColor+ambient;
    // kolor wyjściowy
	out_color = vec4(toneMappingColor,1);
}
