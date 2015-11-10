#include "InputManager.h"

double InputManager::_mousePos[2];

bool InputManager::_cursorLocked = false;

bool InputManager::_leftMouseButton = false;
bool InputManager::_prevLeftMouseButton = false;
bool InputManager::_upKey = false;
bool InputManager::_prevUpKey = false;
bool InputManager::_downKey = false;
bool InputManager::_prevDownKey = false;
bool InputManager::_leftKey = false;
bool InputManager::_prevLeftKey = false;
bool InputManager::_rightKey = false;
bool InputManager::_prevRightKey = false;

bool InputManager::_wKey = false;
bool InputManager::_prevWKey = false;
bool InputManager::_aKey = false;
bool InputManager::_prevAKey = false;
bool InputManager::_sKey = false;
bool InputManager::_prevSKey = false;
bool InputManager::_dKey = false;
bool InputManager::_prevDKey = false;
bool InputManager::_shiftKey = false;
bool InputManager::_prevShiftKey = false;
bool InputManager::_ctrlKey = false;
bool InputManager::_prevCtrlKey = false;

bool InputManager::_prevSpaceKey = false;
bool InputManager::_spaceKey = false;

GLFWwindow* InputManager::_window;
float InputManager::_aspectRatio = 0.0f;
int InputManager::_windowSize[2];

void InputManager::Init(GLFWwindow* window)
{
	_window = window;
	glfwGetWindowSize(window, &_windowSize[0], &_windowSize[1]);
	_aspectRatio = (float)_windowSize[0] / (float)_windowSize[1];
}

void InputManager::Update()
{
	_prevLeftMouseButton = _leftMouseButton;
	_leftMouseButton = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	glfwGetCursorPos(_window, &_mousePos[0], &_mousePos[1]);

	bool insideWindow = _mousePos[0] > 0 && _mousePos[0] < _windowSize[0] && _mousePos[1] > 0 && _mousePos[1] < _windowSize[1];
	if (_leftMouseButton && (insideWindow || _cursorLocked))
	{
		glfwSetCursorPos(_window, _windowSize[0] / 2, _windowSize[1] / 2);
		if (!_cursorLocked)
		{
			glfwGetCursorPos(_window, &_mousePos[0], &_mousePos[1]);
			_cursorLocked = true;
		}
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	else
	{
		_cursorLocked = false;
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	_prevDownKey = _downKey;
	_downKey = glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS;
	_prevUpKey = _upKey;
	_upKey = glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS;
	_prevLeftKey = _leftKey;
	_leftKey = glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS;
	_prevRightKey = _rightKey;
	_rightKey = glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS;

	_prevWKey = _wKey;
	_wKey = glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS;
	_prevAKey = _aKey;
	_aKey = glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS;
	_prevSKey = _sKey;
	_sKey = glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS;
	_prevDKey = _dKey;
	_dKey = glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS;
	_prevShiftKey = _shiftKey;
	_shiftKey = glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
	_prevCtrlKey = _ctrlKey;
	_ctrlKey = glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;

	_prevSpaceKey = _spaceKey;
	_spaceKey = glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS;
}

glm::vec2 InputManager::GetMouseCoords()
{
	glm::vec2 ret = glm::vec2();
	ret.x = (((float)_mousePos[0] / (float)_windowSize[0]) * 2.0f - 1.0f) * _aspectRatio;
	ret.y = -(((float)_mousePos[1] / (float)_windowSize[1]) * 2.0f - 1.0f);
	return ret;
}
bool InputManager::leftMouseButton(bool prev) { if (prev) return _prevLeftMouseButton; else return _leftMouseButton; }
bool InputManager::cursorLocked() { return _cursorLocked; }

bool InputManager::upKey(bool prev) { if (prev) return _prevUpKey; else return _upKey; }
bool InputManager::downKey(bool prev) { if (prev) return _prevDownKey; else return _downKey; }
bool InputManager::leftKey(bool prev) { if (prev) return _prevLeftKey; else return _leftKey; };
bool InputManager::rightKey(bool prev) { if (prev) return _prevRightKey; else return _rightKey; };

bool InputManager::wKey(bool prev) { if (prev) return _prevWKey; else return _wKey; }
bool InputManager::aKey(bool prev) { if (prev) return _prevAKey; else return _aKey; }
bool InputManager::sKey(bool prev) { if (prev) return _prevSKey; else return _sKey; }
bool InputManager::dKey(bool prev) { if (prev) return _prevDKey; else return _dKey; }
bool InputManager::shiftKey(bool prev) { if (prev) return _prevShiftKey; else return _shiftKey; }
bool InputManager::ctrlKey(bool prev) { if (prev) return _prevCtrlKey; else return _ctrlKey; }
bool InputManager::spaceKey(bool prev) { if (prev) return _prevSpaceKey; else return _spaceKey; }

