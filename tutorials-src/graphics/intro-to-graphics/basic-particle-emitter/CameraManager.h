#pragma once
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class RenderObject;

class CameraManager
{
public:
	static void Init(float aspectRatio, float fov, float near, float far);
	static void Update(float dt);
	static void DumpData(); 
	static glm::mat4 ViewMat();
	static glm::mat4 ProjMat();
	static glm::vec4 CamPos();
	static RenderObject* Skybox();
	static void Skybox(RenderObject* newSkybox);
private:
	static glm::mat4 _proj;
	static glm::mat4 _view;
	static glm::vec4 _camPos;

	static glm::vec2 _position;

	static RenderObject* _skybox;
};
