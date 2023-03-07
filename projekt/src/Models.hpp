#include <string>
#include <filesystem>
#include "SOIL/SOIL.h"

namespace models
{
	Core::RenderContext skybox;
	Core::RenderContext ground;
	Core::RenderContext jeep;
	Core::RenderContext honda;
	Core::RenderContext wall;
	Core::RenderContext lamp;
	Core::RenderContext tree;
}

namespace textures
{
	GLuint skybox;
	GLuint ground;
	GLuint wall;
	GLuint jeep;
	GLuint honda;
	GLuint lamp;
	GLuint tree;
	GLuint clouds;
}
namespace texturesNormal
{
	GLuint jeep;
	GLuint ground;
	GLuint wall;
	GLuint tree;
	GLuint default;
}
namespace texturesMetallic
{
	GLuint jeep;
	GLuint ground;
	GLuint wall;
	GLuint tree;
}
namespace texturesRoughness
{
	GLuint jeep;
	GLuint ground;
	GLuint wall;
	GLuint tree;
}
namespace texturesAO
{
	GLuint jeep;
	GLuint ground;
	GLuint wall;
	GLuint tree;
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
	// parametry opisuj¹ce zachowanie tekstury
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}
 // ao wspó³czynnik cieniowania powierzchni (okresla przyciemniane krawedzi)
void loadGround() {
	loadModelToContext("./models/ground.obj", models::ground);
	textures::ground = Core::LoadTexture("./textures/ground/albedo.png");
	texturesNormal::ground = Core::LoadTexture("./textures/ground/normal.png");
	texturesRoughness::ground = Core::LoadTexture("./textures/ground/roughness.png");
	texturesAO::ground = Core::LoadTexture("./textures/ground/ao.png");
	texturesMetallic::ground = Core::LoadTexture("./textures/ground/metallic.png");
}

void loadJeep() {
	loadModelToContext("./models/jeep/Jeep_Willys.obj", models::jeep);
	textures::jeep = Core::LoadTexture("./textures/jeep/albedo.png");
	texturesNormal::jeep = Core::LoadTexture("./textures/jeep/normal.png");
	texturesRoughness::jeep = Core::LoadTexture("./textures/jeep/roughness.png");
	texturesAO::jeep = Core::LoadTexture("./textures/jeep/ao.png");
	texturesMetallic::jeep = Core::LoadTexture("./textures/jeep/metallic.png");
}
void loadDefault() {
	texturesNormal::default = Core::LoadTexture("textures/normals/default_normals.jpg");
}


void loadTree() {
	loadModelToContext("./models/tree/tree.obj", models::tree);
	textures::tree = Core::LoadTexture("./models/tree/albedo.png");
	texturesNormal::tree = Core::LoadTexture("./textures/tree/normal.png");
	texturesRoughness::tree = Core::LoadTexture("./textures/tree/roughness.png");
	texturesAO::tree = Core::LoadTexture("./textures/tree/ao.png");
	texturesMetallic::tree = Core::LoadTexture("./textures/tree/metallic.png");
}

void loadWall() {
	loadModelToContext("./models/wall.obj", models::wall);
	textures::wall = Core::LoadTexture("./textures/wall/albedo.png");
	texturesNormal::wall = Core::LoadTexture("./textures/wall/normal.png");
	texturesRoughness::wall = Core::LoadTexture("./textures/wall/roughness.png");
	texturesAO::wall = Core::LoadTexture("./textures/wall/ao.png");
	texturesMetallic::wall = Core::LoadTexture("./textures/wall/metallic.png");
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
	loadJeep();
	loadWall();
	loadLamp();
	loadDefault();
	loadTree();
}
