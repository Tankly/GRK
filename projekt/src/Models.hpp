#include <string>
#include <filesystem>
#include "SOIL/SOIL.h"

namespace models
{
	Core::RenderContext skybox;
	Core::RenderContext ground;
	Core::RenderContext spaceship;
	Core::RenderContext honda;
	Core::RenderContext lamp;
}

namespace textures
{
	GLuint skybox;
	GLuint ground;
	GLuint spaceship;
	GLuint honda;
	GLuint lamp;
	GLuint clouds;
}

void loadModelToContext(std::string path, Core::RenderContext& context)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}

void loadSkybox()
{
	loadModelToContext("./models/cube.obj", models::skybox);
	int w, h;

	glGenTextures(1, &textures::skybox);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textures::skybox);
	const char* filepaths[6] = {
		"textures/skybox/posx.png",
		"textures/skybox/negx.png",
		"textures/skybox/posy.png",
		"textures/skybox/negy.png",
		"textures/skybox/posz.png",
		"textures/skybox/negz.png",
	};
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char* image = SOIL_load_image(filepaths[i], &w, &h, 0, SOIL_LOAD_RGBA);
		if (image) {
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image
			);
		}
		else {
			std::cout << "Failed to load texture: " << filepaths[i] << std::endl;
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

void loadGround() {
	loadModelToContext("./models/podloga.obj", models::ground);
	textures::ground = Core::LoadTexture("./textures/ground.jpg");
}

void loadSpaceship() {
	loadModelToContext("./models/spaceship.obj", models::spaceship);
	textures::spaceship = Core::LoadTexture("textures/carbon.jpeg");
}

void loadHonda() {
	loadModelToContext("./models/Sol_Gauntlet_GRS_Rally.obj", models::honda);
	//textures::honda = Core::LoadTexture("textures/ground.png");
}
void loadLamp() {
	loadModelToContext("./models/street_lamp.obj", models::lamp);
	textures::lamp = Core::LoadTexture("textures/street_lamp_textures/diffuse_streetlamp.jpg");
	textures::clouds = Core::LoadTexture("textures/clouds.jpg");
}

void initLoadModels()
{
	loadSkybox();
	loadGround();
	loadSpaceship();
	loadHonda();
	loadLamp();
}
