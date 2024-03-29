#include"Model.h"
#include"SkyBox.h"
#include "ProceduralGenerator.h"
#include "Light.h"
#include "AudioManager.h"

const unsigned int width = 1920, height = 1080;

int main()
{
	// Initialize GLFW
	glfwInit();

	#pragma region Create Window

	// Lets GLFW know what version of OpenGL is being used. (We are using 3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Lets GLFW know we want only the modern CORE functions.

	// Creates the window we use to display
	GLFWwindow* window = glfwCreateWindow(width, height, "Adrift through the debris", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); // Brings the window into the context of this program
	gladLoadGL(); // Sets up GLAD. Which we use to configure and manage OpenGL so that it can support any platform, GPU etc. 
	glViewport(0, 0, width, height); // viewport of OpenGL in the Window

	#pragma endregion

	// Play Music
	AudioManager::PlayMusic("AudioFiles/230-days-of-winter-154438.mp3");

	#pragma region Lights

	// positions, colours and intensity for point lights.
	LightSettings pointLights[] =
	{
		LightSettings(glm::vec3(8.0f, -8.0f, -55.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 5.0f),
		LightSettings(glm::vec3(12.76f, -8.3f, -52.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f),
		LightSettings(glm::vec3(-110.0f, -100.0f, -90.0f), glm::vec4(1.0f, 0.5f, .5f, 1.0f), 5.0f),
		LightSettings(glm::vec3(100.0f, -60.0f, -70.0f), glm::vec4(0.4f, 0.0f, 1.0f, 1.0f), 5.0f)
	};

	// positions of the Spot Lights - Not currently being used, prefered point ligths (Disabled in shader)
	glm::vec3 SpotLightPositions[] = {
		glm::vec3(8.0f, -8.0f, -55.0f),
		glm::vec3(1.0f,  .3f, -.5f),
		glm::vec3(-110.0f, -100.0f, -90.0f),
		glm::vec3(1.0f, .3f, 1.0f)
	};

	//// Create light mesh
	Light light; Light light1; Light light2; Light light3;
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	#pragma endregion

	#pragma region Shaders

	Shader shaderProgram("default.vert", "default.frag"); // Setting up Shader
	Shader lightShader("light.vert", "light.frag");	// Shader for light cube
	Shader outlineShader("outline.vert", "outline.frag");
	Shader skyboxShader("Skybox.vert", "Skybox.frag");
	lightShader.Activate();
	shaderProgram.Activate();

#pragma endregion

	#pragma region Procedural Generation

		// Procedural Generation - Debris
		ProceduralGenerator pc(glm::vec3(100.0f, -60.0f, -70.0f), 34, 2.5f, .75f, rand() % (100), vector<string>{
				"Models/Debris1/SpaceshipDestroyed.obj",
				"Models/Debris2/SpaceshipDestroyed.obj",
				"Models/Debris3/SpaceshipDestroyed.obj"
		});

		// Procedural Generation 2 - Astroid Belt
		ProceduralGenerator pc2(glm::vec3(-110.0f, -100.0f, -90.0f), 30, 3.0f, .68f, rand() % (100), vector<string>{
				"Models/Rocks/Rock1/Rock1.obj",
				"Models/Rocks/Rock2/Rock1.obj",
				"Models/Rocks/Rock3/Rock1.obj",
				"Models/Rocks/Rock4/Rock1.obj"
		});

		// Procedural Generation 3 - Astroid and Debris
		ProceduralGenerator pc3(glm::vec3(6.0f, 2.0f, -160.0f), 26, 3.0f, .71f, rand() % (100), vector<string>{
				"Models/Rocks/Rock1/Rock1.obj",
				"Models/Rocks/Rock2/Rock1.obj",
				"Models/Debris2/SpaceshipDestroyed.obj",
				"Models/Rocks/Rock3/Rock1.obj",
				"Models/Rocks/Rock4/Rock1.obj"
		});

	#pragma endregion

	// SkyBox
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
	SkyBox skybox;

	#pragma region Loading Models

	Model MainSpaceShipDestroyed("Models/MainDestroyedShip/SpaceshipDestroyed.gltf" ,glm::vec3(10.0f, -8.0f, -60.0f));
	Model DebrisCircle("Models/DebrisCircle/SpaceshipDestroyed.obj", glm::vec3(8.0f, -8.0f, -55.0f));
	Model SmallShip1("Models/SmallShip/SmallShip.obj", glm::vec3(6.0f, -7.0f, -52.0f));
	Model Signature("Models/Signature/Signature.obj", glm::vec3(13.2f, -8.25f, -52.0f));
	Model NormalShip("Models/SpaceShip/Spaceship.gltf", glm::vec3(.0f, 0.0f,  -5.0f));
	
	// Transformations
	DebrisCircle.rotationZ = 12.0f;
	MainSpaceShipDestroyed.rotationZ = 12.0f;
	MainSpaceShipDestroyed.rotationY = 45.0f;
	SmallShip1.scale = 0.30f;
	SmallShip1.rotationY = -45.0f;
	light.mesh.rotationY = 45.0f;
	light.mesh.scale = .5f;

	#pragma endregion

	// Closer objects rendered on top. 
	glEnable(GL_DEPTH_TEST); 

	// Preparing for outline
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Create the camera!
	CamController camera(width, height, glm::vec3(8.0f,20.0f, 10.0f));

	float slowerRotation = 0.0f;
	double prevTime = glfwGetTime();
	bool movingRight = true;
	float timeStarted = 0.0f;
	float duration = 7;
	bool flashing = true;
	float flashingTimeStarted = 0.0f;
	float flashingDuration = 1;

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // Bg colour
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear back buffer
		
		// Time - Allows for things to happen over time. For example the light blinking.
		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1 / 60) {
			slowerRotation += .030;
			prevTime = crntTime;
		}

		// Camera - provide inputs to move the camera and update the matrix for all shaders.
		camera.Inputs(window, crntTime);
		camera.UpdateMatrix(45.0f, .1f, 200.0f);

		#pragma region Moving the small ship with the light.
		if (movingRight)
		{
			// Move right to left
			SmallShip1.position = SmallShip1.position + glm::vec3(0.015f, 0.00f, 0.015f);
			if (crntTime - timeStarted > duration)
			{
				timeStarted = crntTime;
				movingRight = false;
			}
		}
		else
		{
			// Move right to left
			SmallShip1.position = SmallShip1.position - glm::vec3(0.015f, 0.00f, 0.015f);
			if (crntTime - timeStarted > duration)
			{
				timeStarted = crntTime;
				movingRight = true;
			}
		}
		#pragma endregion

		#pragma region Making the light in the main ship flash / blink green

		if (flashing)
		{
			// Brighten
			pointLights[1].intensity += .01f;
			if (pointLights[1].intensity >= 1.0f)
				pointLights[1].intensity = 1.0f;

			if (crntTime - flashingTimeStarted > flashingDuration)
			{
				flashingTimeStarted = crntTime;
				flashing = false;
			}
		}
		else
		{
			// Dim
			pointLights[1].intensity -= .01;
			if (pointLights[1].intensity <= 0)
				pointLights[1].intensity = 0;

			if (crntTime - flashingTimeStarted > flashingDuration)
			{
				flashingTimeStarted = crntTime;
				flashing = true;
			}
		}
		#pragma endregion
		
		// Drawing the Lights
		pointLights[0].position = SmallShip1.position + glm::vec3(0.17f,0.35f,-0.2);
		light.mesh.Draw(lightShader, camera, pointLights[0].position, glm::mat4(1.0f), lightColor, SpotLightPositions, pointLights);
		light2.mesh.Draw(lightShader, camera, pointLights[2].position, glm::mat4(1.0f), lightColor, SpotLightPositions, pointLights);
		light3.mesh.Draw(lightShader, camera, pointLights[3].position, glm::mat4(1.0f), lightColor, SpotLightPositions, pointLights);

		// Drawing the Models
		MainSpaceShipDestroyed.Draw(shaderProgram, camera, lightColor, SpotLightPositions, pointLights);
		DebrisCircle.rotationY = slowerRotation;
		DebrisCircle.Draw(shaderProgram, camera, lightColor, SpotLightPositions, pointLights);
		SmallShip1.Draw(shaderProgram, camera, lightColor, SpotLightPositions, pointLights);
		Signature.Draw(shaderProgram, camera, lightColor, SpotLightPositions, pointLights);

		// Drawing the Procedural Meshes
		pc.Draw(shaderProgram, camera, lightColor, SpotLightPositions, pointLights);
		pc2.Draw(shaderProgram, camera, lightColor, SpotLightPositions, pointLights);
		pc3.Draw(shaderProgram, camera, lightColor, SpotLightPositions, pointLights);

		// Drawing the Skybox
		skybox.Draw(skyboxShader, camera, width, height);

		// Calculating and Drawing the Outline - Reasearch. WIP (Not ready to use, all objects would need more complex transform data)
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		NormalShip.Draw(shaderProgram, camera, lightColor, SpotLightPositions, pointLights);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		outlineShader.Activate();
		glUniform1f(glGetUniformLocation(outlineShader.ID, "outlining"), 1.1f);
		NormalShip.Draw(outlineShader, camera, lightColor, SpotLightPositions, pointLights);
		glStencilMask(0xFF); // Clear
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean everything up after.
	shaderProgram.Delete();
	lightShader.Delete();
	skyboxShader.Delete();
	outlineShader.Delete();

	AudioManager::Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
