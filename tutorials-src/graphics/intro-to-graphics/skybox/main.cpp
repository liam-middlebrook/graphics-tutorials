/*
Skybox
(c) 2015
original authors: Benjamin Robbins
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*	This example uses the code established inthe obj loading example to demonstrate rendering a skybox.
*	There is actually very little new code for rendering the skybox. The texture for the box is loaded with SOIL,
*	an inverted cube with carefully adjusted uv coordinates are imported from Maya and loaded with the obj loader 
*	already present. The texture is rendered to the cube and the cube is translated to the location of the camera.
*	A self lit shader has to be used for the skybox since lighting doesn't really work with it. Finally, we make use
*	of the layer system previously established to put the skybox on a seperate layer to render it without affecting the
*	depth buffer.
*	There are 5 static component classes that make up the base functionality for this example
*	
*	1) RenderManager
*	- This class maintains the display list for the scene being rendered and thus handles the processes of updating and drawing all
*	of the RenderObjects that have been instantiated in the scene.
*
*	2) CameraManager
*	- This class maintains data relating to the view and projection matrices used in the rendering pipeline. It also handles updating
*	this data based on user input.
*
*	3) InputManager
*	- This class maintains data for the current state of user input for the mouse and keyboard.
*
*	4) LightingManager
*	- This class maintains data for an array of eight lights, each posessing a trasform, color and power, handles the updating thereof and
*	maintains gpu-side buffers reflecting this data for use in the shaders.
*
*	5) ResourceManager
*	- This class maintains all the data from external sources necessessary to run the program. In this instance, this includes obj loading,
*	parsing, and storage, vao, vbo, and ebo generation and storage, shader loading, compiling and linking, and uniform buffer object generation,
*	binding and storage. 
*	
*	RenderObject
*	- Tracks the instance of an object that can be drawn to the screen. Contains data for transforms, a mesh, a shader, drawing mode (eg triangles,
*	lines), and a layer. The layer is a value that can be used to mask certain objects from a draw call. The object has a layer which is some power
*	of 2 and then there is a mask for the draw call which is a 32 bit number. A bit-wise & between these numbers decides whether or not the object
*	in question is drawn. 
*
*	SHADERS
*	- Now use uniform buffer objects for receiving uniform data.
*	
*	vShader.glsl
*	- Through shader, nothing fancy
*
*	fShader.glsl
*	- Applies a phong lighting model based on the data sent from the lighting manager, functions the same as the phong shader from the geometric
*	lighting improved example.
*/

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <ctime>
#include <random>

#include "CameraManager.h"
#include "LightingManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "RenderObject.h"

GLFWwindow* window;

RenderObject* sphere0;
RenderObject* sphere1;
RenderObject* plane;
RenderObject* cube;
RenderObject* skybox;

Light* light0;
Light* light1;

void InitScene()
{
	light0 = LightingManager::GetLight(0);
	light0->position = glm::vec3(3.0f, 3.0f, 0.0f);
	light0->color = glm::vec4(0.6f, 1.0f, 0.0f, 1.0f);
	light0->diffusePower = 5.0f;

	sphere0 = RenderManager::InitRenderObject(&ResourceManager::sphere, ResourceManager::phongShader, GL_TRIANGLES, 1);
	sphere0->transform().position = glm::vec3(3.0f, 0.0f, 1.5f);
	sphere0->transform().scale = glm::vec3(3.0f, 1.0f, 1.0f);
	
	sphere1 = RenderManager::InitRenderObject(&ResourceManager::sphere, ResourceManager::phongShader, GL_TRIANGLES, 1);
	sphere1->transform().position = glm::vec3(-2.0f, 0.0f, -3.5f);

	cube = RenderManager::InitRenderObject(&ResourceManager::cube, ResourceManager::phongShader, GL_TRIANGLES, 1);
	cube->transform().position = glm::vec3(1.0f, 0.0f, -2.5f);
	cube->transform().scale = glm::vec3(1.0f, 3.0f, 1.0f);

	skybox = RenderManager::InitRenderObject(&ResourceManager::inv_cube, ResourceManager::skyboxShader, GL_TRIANGLES, 2);
	skybox->texture(ResourceManager::skybox);
	CameraManager::Skybox(skybox);
}

void init()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	if (!glfwInit()) exit(EXIT_FAILURE);

	//Create window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(800, 600, "Skybox", NULL, NULL); // Windowed

	//Activate window
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	glewInit();

	ResourceManager::Init();
	RenderManager::Init(1);
	LightingManager::Init();
	InputManager::Init(window);
	CameraManager::Init(800.0f / 600.0f, 45.0f, 0.1f, 100.0f);

	glfwSetTime(0.0);

	time_t timer;
	time(&timer);
	srand((unsigned int)timer);

	InitScene();
}

void step()
{
	// Clear to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	InputManager::Update();

	// Get delta time since the last frame
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	CameraManager::Update(dt);

	RenderManager::Update(dt);

	LightingManager::Update(dt);

	// Disable writing to the depth buffer so we can render the skybox.
	glDepthMask(false);

	RenderManager::Draw(2);

	// Re-enable writing to the depth buffer so that the other objects in the scene can be rendered.
	glDepthMask(true);

	RenderManager::Draw(1);

	// Swap buffers
	glfwSwapBuffers(window);
}

void cleanUp()
{
	RenderManager::DumpData();
	ResourceManager::DumpData();
	CameraManager::DumpData();
	LightingManager::DumpData();

	glfwTerminate();
}

int main()
{
	init();

	while (!glfwWindowShouldClose(window))
	{
		step();
		glfwPollEvents();
	}

	cleanUp();

	return 0;
}
