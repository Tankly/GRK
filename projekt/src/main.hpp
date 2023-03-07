#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"
#include "Models.hpp"

#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include "SOIL/SOIL.h"

#define STB_IMAGE_IMPLEMENTATION

GLuint programTex;
GLuint programSkybox;
GLuint programPBR;

Core::Shader_Loader shaderLoader;
Core::RenderContext shipContext;

glm::vec3 cameraPos = glm::vec3(0, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);

glm::vec3 spaceshipPos = glm::vec3(0, 0, 0);
glm::vec3 spaceshipDir = glm::vec3(1.f, 0.f, 0.f);

glm::vec3 lightPositions[20];
glm::vec3 lightConeDir[20];
glm::vec3 lightColor = glm::vec3(1, 1, 0.8)*0.3;

glm::vec3 spotlightPos = glm::vec3(0, 0, 0);
glm::vec3 spotlightConeDir = glm::vec3(0, 0, 0);


// 2 - moc �wiat�a
glm::vec3 spotlightColor = glm::vec3(0.5, 0.9, 0.8)*0.5;


GLuint VAO,VBO;
GLuint quadVAO;

float aspectRatio = 1.f;
float localsSpeed = 0;

glm::mat4 createCameraMatrix()
{
	// orientacja kamery up - Y, dir - Z, side - X
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir,glm::vec3(0.f,1.f,0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide,cameraDir));
	glm::mat4 cameraRotrationMatrix = glm::mat4({
		cameraSide.x,cameraSide.y,cameraSide.z,0,
		cameraUp.x,cameraUp.y,cameraUp.z ,0,
		-cameraDir.x,-cameraDir.y,-cameraDir.z,0,
		0.,0.,0.,1.,
		});
	cameraRotrationMatrix = glm::transpose(cameraRotrationMatrix);
	glm::mat4 cameraMatrix = cameraRotrationMatrix * glm::translate(-cameraPos);

	return cameraMatrix;
}

glm::mat4 createPerspectiveMatrix()
{
	
	glm::mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 20.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,aspectRatio,0.,0.,
		0.,0.,(f+n) / (n - f),2*f * n / (n - f),
		0.,0.,-1.,0.,
		});

	
	perspectiveMatrix=glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

// Physically based rendering - funkcja przekazując dane do shaderów vert i farg na podane zmienne 
void drawObjectPBR(
	Core::RenderContext& context,
	glm::mat4 modelMatrix,
	GLuint textureID,
	GLuint roughnessId,
	GLuint metallicId,
	GLuint aoId,
	GLuint normalmapId = NULL
) {
	glUseProgram(programPBR);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programPBR, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programPBR, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(programPBR, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	for (int i = 0; i < 20; i++) {
		glUniform3f(glGetUniformLocation(programPBR, ("lightPos[" + std::to_string(i) + "]").c_str()), lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
		glUniform3f(glGetUniformLocation(programPBR, ("lightConeDir[" + std::to_string(i) + "]").c_str()), lightConeDir[i].x, lightConeDir[i].y, lightConeDir[i].z);
	}

	glUniform3f(glGetUniformLocation(programPBR, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(programPBR, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	glUniform3f(glGetUniformLocation(programPBR, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);


	Core::SetActiveTexture(textureID, "albedoMap", programPBR, 0);
	if (normalmapId) {
		Core::SetActiveTexture(normalmapId, "normalMap", programPBR, 1);
	}
	else {
		Core::SetActiveTexture(texturesNormal::default, "normalMap", programPBR, 1);
	}
	Core::SetActiveTexture(metallicId, "metallicMap", programPBR, 2);
	Core::SetActiveTexture(roughnessId, "roughnessMap", programPBR, 3);
	Core::SetActiveTexture(aoId, "aoMap", programPBR, 3);
	Core::DrawContext(context);
	glUseProgram(0);
}


void drawObjectTexture(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureID, GLuint normalmapId = NULL) {
	glUseProgram(programTex);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programTex, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programTex, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(programTex, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	for (int i = 0; i < 20; i++) {
		glUniform3f(glGetUniformLocation(programTex, ("lightPos[" + std::to_string(i) + "]").c_str()), lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
		glUniform3f(glGetUniformLocation(programTex, ("lightConeDir[" + std::to_string(i) + "]").c_str()), lightConeDir[i].x, lightConeDir[i].y, lightConeDir[i].z);
	}

	glUniform3f(glGetUniformLocation(programTex, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(programTex, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	glUniform3f(glGetUniformLocation(programTex, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);


	Core::SetActiveTexture(textureID, "colorTexture", programTex, 0);
	if(normalmapId)  {
		Core::SetActiveTexture(normalmapId, "normalSampler", programTex, 1);
	}
	else {
		Core::SetActiveTexture(texturesNormal::default, "normalSampler", programTex, 1);
	}
	Core::DrawContext(context);
	glUseProgram(0);
}


void renderSkybox(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureID)
{
	glDepthFunc(GL_LEQUAL);
	glUseProgram(programSkybox);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "transformation"), 1, GL_FALSE, (float*)&transformation);
	Core::SetActiveTexture(textureID, "skybox", programSkybox, 0);
	Core::DrawContext(context);
	glDepthFunc(GL_LESS);
}

void renderScene(GLFWwindow* window)
{
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 transformation;

	if (round(localsSpeed) == 100) {
		localsSpeed = -100;
	}

	localsSpeed += 0.2;

	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
	});

	renderSkybox(
		models::skybox,
		glm::translate(spaceshipPos) * glm::eulerAngleY(glm::pi<float>()),
		textures::skybox
	);

	drawObjectPBR(models::ground,
		glm::mat4(),
		textures::ground,
		texturesRoughness::ground,
		texturesMetallic::ground,
		texturesAO::ground,
		texturesNormal::ground
	);
	drawObjectPBR(models::jeep,
		glm::translate(spaceshipPos) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()),
		textures::jeep,
		texturesRoughness::jeep,
		texturesMetallic::jeep,
		texturesAO::jeep,
		texturesNormal::jeep
	);

	drawObjectPBR(models::jeep,
		glm::translate(glm::vec3(1.f, 0, localsSpeed)),
		textures::jeep,
		texturesRoughness::jeep,
		texturesMetallic::jeep,
		texturesAO::jeep,
		texturesNormal::jeep
	);
	const float wallWidth = 8.f;
	for (int i = -3; i <= 3; i++) {
		drawObjectPBR(models::wall,
			glm::translate(glm::vec3(3.f, 0, wallWidth * i)),
			textures::wall,
			texturesRoughness::wall,
			texturesMetallic::wall,
			texturesAO::wall,
			texturesNormal::wall
		);
		drawObjectPBR(models::wall,
			glm::translate(glm::vec3(-5.f, 0, wallWidth * i)),
			textures::wall,
			texturesRoughness::wall,
			texturesMetallic::wall,
			texturesAO::wall,
			texturesNormal::wall
		);
	}

	const float lampGap = 5.f;
	int positionIndex = 0;
	for (int i = -10; i < 10; i++) {
		drawObjectTexture(models::lamp,
			glm::translate(glm::vec3(2.f, 0, lampGap * i)),
			textures::lamp
		);
		lightPositions[positionIndex] = glm::vec3(2.f, 2.93f, lampGap * i);

		lightConeDir[positionIndex] = glm::vec4(1.f, 0.f, 0.f, 0.f) * glm::eulerAngleXYZ(0.5f, 3.f, -1.5f);
		positionIndex++;
		drawObjectPBR(models::tree,
			glm::translate(glm::vec3(-2.f, 0, lampGap * i)),
			textures::tree,
			texturesRoughness::tree,
			texturesMetallic::tree,
			texturesAO::tree,
			texturesNormal::tree
		);

	}
	spotlightPos = spaceshipPos + glm::vec3(0, 1, 0) + 0.3 * spaceshipDir;
	spotlightConeDir = spaceshipDir;

	glUseProgram(0);
	glfwSwapBuffers(window);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
}


void init(GLFWwindow* window)
{
	spaceshipDir = glm::vec3(glm::eulerAngleY(-1.575f) * glm::vec4(spaceshipDir, 0));
	cameraDir = spaceshipDir;

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	programTex = shaderLoader.CreateProgram("shaders/texture_shader.vert", "shaders/texture_shader.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/skybox_shader.vert", "shaders/skybox_shader.frag");
	programPBR = shaderLoader.CreateProgram("shaders/pbr_texture_shader.vert", "shaders/pbr_texture_shader.frag");

	initLoadModels();
}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(programSkybox);
	shaderLoader.DeleteProgram(programTex);
	shaderLoader.DeleteProgram(programPBR);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::vec3(0.f, 1.f, 0.f);
	
	float angleSpeed = 0.03f;
	float moveSpeed = 0.03f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		spaceshipPos += spaceshipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		spaceshipPos -= spaceshipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		spaceshipPos += spaceshipSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		spaceshipPos -= spaceshipSide * moveSpeed;
	/*
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		spaceshipPos += spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		spaceshipPos -= spaceshipUp * moveSpeed;
	*/
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(spaceshipDir, 0));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(spaceshipDir, 0));

	cameraPos = spaceshipPos - 4 * spaceshipDir + glm::vec3(0, 5, 0) * 0.5f;
	cameraDir = spaceshipDir;

	//cameraDir = glm::normalize(-cameraPos);

}

// funkcja jest glowna petla
void renderLoop(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		renderScene(window);
		glfwPollEvents();
	}
}
//}