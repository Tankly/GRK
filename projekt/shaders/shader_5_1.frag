#version 430 core

out vec4 out_color;
uniform vec3 modelColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

//rzeczywista pozycja fragmentu
in vec3 worldPos;
//wektor prostopadly do powierzchni wierzcholka
in vec3 vNormal;
void main()
{
    //jak bardzo domyslnie widoczne sa elementy
    float ambientStrength = 0.3;
    //intensywnosc odblyskow
    float specularStrength = 0.2;
    //wektor kierunku miedzy zrodlem swiatla a pozycja fragmentu
    vec3 lightDir = normalize(lightPos - worldPos);
    //rozproszony wplyw swiatla na biezacy fragment max poniewaz powyzej 90 jest ujemna liczba - brak wplywu
    float diff = max(dot(vNormal, lightDir), 0.0);
    //skladnik rozproszenia
    vec3 diffuse = diff * lightColor;
    //Otaczajace swiatlo
    vec3 ambient = ambientStrength * lightColor;
    //wektor kierunku widoku/ kierunek patrzenia 
    vec3 viewDir = normalize(cameraPos - worldPos);
    //wektor odbicia wzdluz osi normalnej / kierunek odbicia
    //minus przy lighDir bo wskazuje on od fragmentu w kierunku zrodla swiatla a reflecrt oczekuje na odwrot
    vec3 reflectDir = reflect(-lightDir, vNormal);
    //skladowa zwierciadlana
    //im mniejsza potega tym odbicie jest bardziej rozproszone a nie w jednym punkcie
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
    //Mieszany Phong - wszystkie opcje
    out_color = vec4((ambient + diffuse + specular) * modelColor, 1.0);
}
