//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>

struct SphereCollider {
	glm::vec3 center;
	float radius;
};
//83.0938  -72.6003   0
//75.1879 -54.4977  0
SphereCollider tank1Collider = { glm::vec3(83.0938f, 2.5f, 72.6003f), 5.0f };
SphereCollider tank2Collider = { glm::vec3(75.1879f, 2.5f, 54.4977f), 5.0f };

struct Waypoint {
	glm::vec3 position;
	glm::vec3 orientation;
};

std::vector<Waypoint> cameraPath = {
	{{121.037f, 1.93454f, 96.9105f}, {120.352f, 1.91011f, 96.1824f}},
	{{119.885f, 2.09422f, 86.4492f}, {119.322f, 2.0916f, 85.6231f}},
	{{116.938f, 2.20068f, 70.9972f}, {116.159f, 2.19981f, 70.3706f}},
	{{107.372f, 2.95103f, 63.0433f}, {106.725f, 3.18023f, 62.3166f}},
	{{100.681f, 10.2566f, 54.3689f}, {100.445f, 10.0321f, 53.4352f}},
	{{100.69f, 10.2468f, 54.3805f}, {100.488f, 10.0159f, 53.4288f}},
	{{100.69f, 10.2468f, 54.3805f}, {99.8928f, 9.92952f, 54.8942f}},
	{{88.5493f, 3.77834f, 62.0905}, {88.0865f, 3.54829f, 62.9465f}},
	{{76.774f, 2.30593f, 68.1294f}, {77.4958f, 2.15467f, 68.8049f}},
	{{74.6272f, 1.13622f, 72.2697f,}, {73.6894f, 1.0317f, 72.6008f}},
	{{74.6341f, 1.13622f, 75.2629f}, {73.7269f, 1.11703f, 74.8427f}},
	{{50.1895f, 1.61003f, 81.5318f}, {49.21f, 1.57338f, 81.3334f}},
	{{19.7739f, 1.50373f, 84.5759f}, {18.8998f, 1.5142f, 84.0903f}},
	{{-27.5649f, 7.29927f, 110.357f}, {-27.2858f, 7.11275f, 109.415f}},
	{{-37.325f, 4.30087f, 52.2267f}, {-37.9859f, 4.10236f, 51.5029f}},
	{{-64.0008f, 3.62854f, 30.9667f}, {-63.0367f, 3.54225f, 31.2178f}},
	{{-25.3564f, 2.28939f, -12.5799f}, {-24.3638f, 2.29899f, -12.7009f}},
	{{-5.81364f, 3.57789f, -3.55631f}, {-6.30952f, 3.46209f, -4.41694f}},
	{{-85.2825f, 41.0746f, -73.1702f}, {-84.6089f, 40.6418f, -72.5712f}},
	{{-53.8635f, 17.0281f, 76.4011f}, {-52.9225f, 16.7199f, 76.2613f}}	
};



int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 16384;
const unsigned int SHADOW_HEIGHT = 16384;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(
	glm::vec3(0.0f, 0.0f, 3.0f),
	glm::vec3(0.0f, 0.0f, -10.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 1.0f;

std::vector<const GLchar*> faces{
   "skybox/graycloud_lf.jpg",
   "skybox/graycloud_rt.jpg",
   "skybox/graycloud_up.jpg",
   "skybox/graycloud_dn.jpg",
   "skybox/graycloud_ft.jpg",
   "skybox/graycloud_bk.jpg"
};

gps::SkyBox mySkyBox;

GLint pointLightLoc;
GLint fogLoc;
bool pointLightEnabled = 0;
bool fogEnabled = 0;

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

float sceneScale = 1.0f;

gps::Model3D scene;
gps::Model3D tankTurret;
gps::Model3D tankShell;
gps::Model3D nanosuit;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D screenQuad;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
gps::Shader skyboxShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

float turretRotationAngle = 0.0f;
float rotationSpeed = 0.35f;
bool shellFired = false;
float shellSpeed = 0.65f;
glm::vec3 shellStartPosition;
glm::vec3 shellDirection;
glm::vec3 shellCurrentPosition;
float shellTravelDistance = 0.0f;
GLint muzzleLoc;
GLint muzzlePosLoc;
GLint leftTankLoc;
GLint rightTankLoc;
bool muzzleFlashActive = false;
bool muzzleFlag = true;
float muzzleFlashTimer = 0.0f;
bool collisionFlag = false;
bool tankLeftActive = false;
bool tankRightActive = false;
float leftTankTimer = 0.0f;
float rightTankTimer = 0.0f;

bool checkCollision(const SphereCollider& collider, const glm::vec3& shellPosition) {
	// Create a rotation matrix for the turret
	glm::mat4 turretRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(turretRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

	// Apply the turret rotation to the shell position
	glm::vec4 rotatedShellPosition = turretRotationMatrix * glm::vec4(shellPosition, 1.0f);

	// Check collision with the rotated position
	float distance = glm::distance(collider.center, glm::vec3(rotatedShellPosition));
	return distance < collider.radius + 1.25f;
}


glm::vec3 interpolate(const glm::vec3& start, const glm::vec3& end, float t) {
	return start + t * (end - start);
}

void animateCamera(float deltaTime) {
	static float totalTime = 0.0f;
	totalTime += deltaTime;
	
	float speed = 1.75f; 

	// Calculate the current segment and interpolation factor
	int segment = static_cast<int>(totalTime / speed);
	float t = fmod(totalTime / speed, 1.0f);

	// Stop updating the camera after the last waypoint
	if (segment >= cameraPath.size() - 1) {
		return;
	}

	// Update camera position and orientation
	myCamera.setPosition(interpolate(cameraPath[segment].position, cameraPath[segment + 1].position, t));
	myCamera.setCameraTarget(interpolate(cameraPath[segment].orientation, cameraPath[segment + 1].orientation, t));
}


GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	// Get the pixel dimensions of the framebuffer
	glfwGetFramebufferSize(window, &retina_width, &retina_height);

	// Update the viewport
	glViewport(0, 0, retina_width, retina_height);

	// Update the projection matrix
	float aspectRatio = static_cast<float>(retina_width) / static_cast<float>(retina_height);
	projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);

	// Update the projection matrix uniform
	myCustomShader.useShaderProgram();
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}


void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;

			if (key == GLFW_KEY_I) {
				pointLightEnabled = !pointLightEnabled;
			}

			if (key == GLFW_KEY_O) {
				fogEnabled = !fogEnabled;
			}

			if (key == GLFW_KEY_B) {
				sceneScale *= 0.9f; // Scale down               
			}
			else if (key == GLFW_KEY_N) {
				sceneScale *= 1.1f; // Scale up
			}

			if (key == GLFW_KEY_K && !shellFired) {
				shellFired = true;
				shellStartPosition = glm::vec3(13.5999f, 2.40652f, 77.952f);
				shellDirection = glm::vec3(sin(glm::radians(116.711f)), 0.0f, cos(glm::radians(116.711f)));
				shellCurrentPosition = shellStartPosition;
				shellTravelDistance = 0.0f;

				muzzleFlashActive = true;
				muzzleFlashTimer = 0.0f; // Reset timer
				//muzzleFlashLightPos = /
			}
		}
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	static float lastX = 800 / 2.0; // Assuming window width is 800
	static float lastY = 600 / 2.0; // Assuming window height is 600
	static bool firstMouse = true;

	// Check if the left mouse button is pressed
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
			// Capture and hide cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.05;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		myCamera.rotate(yoffset, xoffset);

		// Update the view matrix after rotating the camera
		view = myCamera.getViewMatrix();
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		firstMouse = true;
		// Release and show cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void processMovement()
{
	//std::cout << "POS" << '\n';
	//std::cout << myCamera.getPosition().x << ' ' << myCamera.getPosition().y << ' ' << myCamera.getPosition().z << '\n';
	//std::cout << "VIEW" << '\n';
	//std::cout << myCamera.getCameraTarget().x << ' ' << myCamera.getCameraTarget().y << ' ' << myCamera.getCameraTarget().z << '\n';

	if (pressedKeys[GLFW_KEY_Q]) {
		angleY -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angleY += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}


	if (pressedKeys[GLFW_KEY_Z] && !shellFired) {
		turretRotationAngle += rotationSpeed;
	}
	else if (pressedKeys[GLFW_KEY_X] && !shellFired) {
		turretRotationAngle -= rotationSpeed;
	}

	// Normalize the angle to keep it within 0 - 360 degrees
	if (turretRotationAngle >= 360.0f) turretRotationAngle -= 360.0f;
	if (turretRotationAngle < 0.0f) turretRotationAngle += 360.0f;

	// SOLID, WIREFRAME, POINTS
	if (pressedKeys[GLFW_KEY_T]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (pressedKeys[GLFW_KEY_Y]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (pressedKeys[GLFW_KEY_U]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
}

void updateShaderLightingState() {
	myCustomShader.useShaderProgram();
	glUniform1i(pointLightLoc, pointLightEnabled ? 1 : 0);
	glUniform1i(fogLoc, fogEnabled ? 1 : 0);
	glUniform1i(muzzleLoc, muzzleFlashActive ? 1 : 0);	
	glUniform1i(leftTankLoc, tankLeftActive ? 1 : 0);
	glUniform1i(rightTankLoc, tankRightActive ? 1 : 0);
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	//for sRBG framebuffer
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	//for antialising
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	//nanosuit.LoadModel("objects/nanosuit/nanosuit.obj");
	//ground.LoadModel("objects/ground/ground.obj");
	lightCube.LoadModel("objects/cube/cube.obj");
	scene.LoadModel("scene/scene.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");
	tankTurret.LoadModel("objects/TANK_TURRET.obj");
	tankShell.LoadModel("objects/TANK_SHELL.obj");	

	mySkyBox.Load(faces);
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
	depthMapShader.useShaderProgram();
	skyboxShader.loadShader("shaders/skybox.vert", "shaders/skybox.frag");
	skyboxShader.useShaderProgram();
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 300.0f, 20.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


	// PointLight, Fog, Muzzle control
	pointLightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "enablePointLight");
	fogLoc = glGetUniformLocation(myCustomShader.shaderProgram, "enableFog");
	muzzleLoc = glGetUniformLocation(myCustomShader.shaderProgram, "muzzleFlashActive");
	muzzlePosLoc = glGetUniformLocation(myCustomShader.shaderProgram, "shellCurrentPosition");
	leftTankLoc = glGetUniformLocation(myCustomShader.shaderProgram, "tankLeftActive");
	rightTankLoc = glGetUniformLocation(myCustomShader.shaderProgram, "tankRightActive");
}

void initFBO() {
	glGenFramebuffers(1, &shadowMapFBO);
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
	glm::vec4 lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f);
	glm::mat4 lightView = glm::lookAt(glm::vec3(lightRotation), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = 0.1f, far_plane = 500.0f;
	glm::mat4 lightProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}

void updateShell(float deltaTime) {
	if (tankLeftActive) {
		leftTankTimer += deltaTime;
		if (leftTankTimer >= 0.25f) {
			tankLeftActive = false; // Turn off the flash
			collisionFlag = false;
		}
	}

	if (tankRightActive) {
		rightTankTimer += deltaTime;
		if (rightTankTimer >= 0.25f) {
			tankRightActive = false; // Turn off the flash
			collisionFlag = false;
		}
	}

	if (shellFired) {
		shellTravelDistance += shellSpeed;
		shellCurrentPosition = shellStartPosition + shellDirection * shellTravelDistance;

		if (muzzleFlashActive) {
			muzzleFlashTimer += deltaTime;
			if (muzzleFlashTimer >= 0.1f) {
				muzzleFlashActive = false; // Turn off the flash
			}
		}

		if (shellTravelDistance >= 100.0f) {
			shellFired = false;
			muzzleFlag = false;
		}
	}
}


void drawObjects(gps::Shader shader, bool depthPass) {

	shader.useShaderProgram();

	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	/*glm::mat4 scaledModel = glm::scale(model, glm::vec3(sceneScale, sceneScale, sceneScale));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(scaledModel));*/

	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	scene.Draw(shader);

	//8.65437 m   -80.4405 m    1.95773 m
	// Start with an identity matrix
	glm::mat4 modelTurret = glm::mat4(1.0f);
	modelTurret = glm::translate(modelTurret, glm::vec3(8.65437f, 1.95773f, 80.4405f));
	modelTurret = glm::rotate(modelTurret, glm::radians(turretRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelTurret = glm::translate(modelTurret, glm::vec3(-8.65437f, -1.95773f, -80.4405f));

	// Set the model matrix for the turret
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelTurret));

	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	tankTurret.Draw(shader);

	//glm::vec3(13.5999f, 2.40652f, 77.952f)	

	if (shellFired) {
		glm::mat4 shellModel = glm::mat4(1.0f);
		shellModel = glm::translate(shellModel, glm::vec3(8.65437f, 1.95773f, 80.4405f));
		shellModel = glm::rotate(shellModel, glm::radians(turretRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		shellModel = glm::translate(shellModel, glm::vec3(-8.65437f, -1.95773f, -80.4405f));
		shellModel = glm::translate(shellModel, shellCurrentPosition);

		if (!muzzleFlag) {
			glUniform3fv(muzzlePosLoc, 1, glm::value_ptr(shellCurrentPosition));
			muzzleFlag = true;
		}

		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(shellModel));

		if (!depthPass) {
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		}

		tankShell.Draw(shader);
	}
	else {
		glm::mat4 modelShell = glm::mat4(1.0f);
		modelShell = glm::translate(modelShell, glm::vec3(8.65437f, 1.95773f, 80.4405f));
		modelShell = glm::rotate(modelShell, glm::radians(turretRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		modelShell = glm::translate(modelShell, glm::vec3(-8.65437f, -1.95773f, -80.4405f));
		modelShell = glm::translate(modelShell, glm::vec3(13.5999f, 2.40652f, 77.952f));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelShell));

		if (!depthPass) {
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		}

		tankShell.Draw(shader);
	}
}

void renderScene() {

	// depth maps creation pass
	//TODO - Send the light-space transformation matrix to the depth map creation shader and
	//		 render the scene in the depth map

	updateShaderLightingState();

	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawObjects(depthMapShader, true);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		//draw a white cube around the light

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lightCube.Draw(lightShader);

		// Draw skybox as the last object
		glDepthFunc(GL_LEQUAL);  // Change depth function to allow depth equal
		mySkyBox.Draw(skyboxShader, view, projection);
		glDepthFunc(GL_LESS); // Set depth function back to default
	}
}

void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char* argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();

	glCheckError();

	double lastTime = glfwGetTime();
	double deltaTime = 0.0;	

	while (!glfwWindowShouldClose(glWindow)) {

		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;


		animateCamera(deltaTime);
		processMovement();
		updateShell(deltaTime);

		if (shellFired && checkCollision(tank1Collider, shellCurrentPosition) && !collisionFlag) {			
			shellFired = false;			
			muzzleFlag = false;
			collisionFlag = true;
			std::cout << "RIGHT TANK" << '\n';
			tankRightActive = true;
		}

		if (shellFired && checkCollision(tank2Collider, shellCurrentPosition) && !collisionFlag) {
			shellFired = false;			
			muzzleFlag = false;
			collisionFlag = true;
			std::cout << "LEFT TANK" << '\n';
			tankLeftActive = true;
		}

		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
