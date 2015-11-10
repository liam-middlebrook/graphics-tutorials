#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

class InputManager
{
public:
	static void Init(GLFWwindow* window);

	static void Update();

	static glm::vec2 GetMouseCoords();
	static bool leftMouseButton(bool prev = false);
	static bool cursorLocked();

	static bool downKey(bool prev = false);
	static bool upKey(bool prev = false);
	static bool leftKey(bool prev = false);
	static bool rightKey(bool prev = false);

	static bool wKey(bool prev = false);
	static bool aKey(bool prev = false);
	static bool sKey(bool prev = false);
	static bool dKey(bool prev = false);
	static bool shiftKey(bool prev = false);
	static bool ctrlKey(bool prev = false);
	static bool spaceKey(bool prev = false);
private:

	static double _mousePos[2];
	static bool _leftMouseButton;
	static bool _prevLeftMouseButton;
	static bool _cursorLocked;

	static bool _upKey;
	static bool _prevUpKey;
	static bool _downKey;
	static bool _prevDownKey;
	static bool _leftKey;
	static bool _prevLeftKey;
	static bool _rightKey;
	static bool _prevRightKey;
	
	static bool _wKey;
	static bool _prevWKey;
	static bool _aKey;
	static bool _prevAKey;
	static bool _sKey;
	static bool _prevSKey;
	static bool _dKey;
	static bool _prevDKey;
	static bool _shiftKey;
	static bool _prevShiftKey;
	static bool _ctrlKey;
	static bool _prevCtrlKey;

	static bool _prevSpaceKey;
	static bool _spaceKey;

	static GLFWwindow* _window;
	static float _aspectRatio;
	static int _windowSize[2];
};
