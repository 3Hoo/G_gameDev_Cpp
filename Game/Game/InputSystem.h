#pragma once
#include <SDL_scancode.h>
#include <SDL_gamecontroller.h>
#include <SDL_mouse.h>
#include <unordered_map>
#include "Math.h"

/* ***
* ���ݱ����� ProcessInput()�� SDL �Լ��� ���� ȣ���� ���콺�� ��Ʈ�ѷ��� �����ߴ�.
* -> �̴� ������ ���ӿ����� �� �۵��ϰ�����, ���ͳ� ������Ʈ�� ����� ���α׷��ӿ��Դ� SDL �Լ� ���� ������ ���ʿ��ϴ� 
* -> �׸��� SDL �Է� �Լ��� �Լ� ȣ�� �� ���� ���̸� ��ȯ�ϴµ�, �� ������ ���� �̷� �Լ��� �� �� �̻� ȣ���ϸ�
*	 ù ȣ�� �������� ���� ��ȭ�� ������ ���ϴ� 0 ���� ���� ���̴�
*/

/* ***
* �̸� �ذ��ϱ� ���� InputSystem Ŭ������ �����Ѵ�.
* �� Ŭ������ InputState��� ���� Ŭ������ �����͸� ä���, ProcessInput()�� ����ؼ� InputState�� ����/������Ʈ�� ��� ������ �����Ѵ�
* ���� ����/������Ʈ�� ���� �־� �ϴ� ���¸� ���� ��ȸ�ϵ���, InputState�� ��� ���� �Լ��� ������
*/

enum ButtonState
{
	ENone,
	EPressed,
	EReleased,
	EHeld
};

/* ***
* ���� ���¸� ����Ű�� �����Ϳ� ���� ���¸� ���� �迭�� ������
* �迭�� ũ��� SDL�� Ű���� ��ĵ �ڵ忡 ���� ����ϴ� ���� ũ��� ����
*/
class KeyboardState
{
public:
	// InputSystem�� friend�� �����ؼ�, ��� �����͸� ���� �����ϰ� �Ѵ�
	friend class InputSystem;

	// Ű�� ������ true/false�� ��´�. keyCode�� SDL_SCANCODE_ESCAPE ���� ��
	bool GetKeyValue(SDL_Scancode keyCode) const;

	// ����/���� �������� ���� ���¸� ��´�
	ButtonState GetKeyState(SDL_Scancode keyCode) const;

private:
	// ���� ����
	const Uint8* mCurrState;
	// ���� �������� ����
	Uint8 mPrevState[SDL_NUM_SCANCODES];
};

/* ***
* Mouse�� ���¸� �����ϴ� class
*/
class MouseState
{
public:
	friend class InputSystem;

	// ���콺 ��ġ
	const Vector2& GetPosition() const { return mMousePos; }
	const float GetAngularSpeed() const { return mAngularSpeed; }
	const float GetPitchSpeed() const { return mPitchSpeed; }

	// ��ư
	// SDL_GetMouseState�� ���ϰ��� bitmask �̹Ƿ�, ��ƮAND ������ ���� ������ �ִ��� �ô��� Ȯ���� �� �ִ�
	bool GetButtionValue(int button) const;
	ButtonState GetButtonState(int button) const;

private:
	// ���콺 ��ġ ����
	Vector2 mMousePos;
	// ��ư ������ ����
	Uint32 mCurrButtons;
	Uint32 mPrevButtons;
	// ���콺 ��ũ�� ������
	Vector2 mScrollWheel;

	// ��� ���?
	bool mIsRelative = true;

	// ��� ��� ���� ����� ȸ���ӵ�
	float mAngularSpeed = 0.0f;
	float mPitchSpeed = 0.0f;
};

/* ***
* Controller�� ���¸� �����ϴ� class
*/
class ControllerState
{
public:
	friend class InputSystem;

	// ��ư
	// ��Ʈ�ѷ��� ��ư�� �� �̸��� ���ؼ� ������ �ϳ��� �ҷ��;� �Ѵ�
	// �̸��� enum���� ���ǵǾ� �ְ�, SDL_CONTROLLER_BUTTON_A, SDL_CONTROLLER_START, SDL_CONTROLLER_BUTTON_LEFTSTICK ������ �̷���� �ִ�
	bool GetButtonValue(SDL_GameControllerButton button) const;
	ButtonState GetButtonState(SDL_GameControllerButton button) const;

	bool GetIsConnected() const { return mIsConnected; }

	float GetLeftTrigger() const { return mLeftTrigger; }
	float GetRightTrigger() const { return mRightTrigger; }

	Vector2 GetLeftStick() const { return mLeftStick; }
	Vector2 GetRightStick() const { return mRightStick; }


private:
	// ����/���� ��ư
	Uint8 mCurrButtons[SDL_CONTROLLER_BUTTON_MAX];	// SDL_CONTROLLER_BUTTON_MAX�� ��Ʈ�ѷ��� ���� �� �ִ� ��ư�� �ִ� ���� �ǹ��Ѵ�
	Uint8 mPrevButtons[SDL_CONTROLLER_BUTTON_MAX];

	// ��Ʈ�ѷ��� ����Ǿ� �ִ°�?
	bool mIsConnected;

	// ����, ������ Ʈ����
	float mLeftTrigger;
	float mRightTrigger;

	// ����, ������ ��ƽ
	Vector2 mLeftStick;
	Vector2 mRightStick;
};

// �Է��� ���� ���¸� �����ϴ� Wrapper
struct InputState
{
	KeyboardState Keyboard;
	MouseState Mouse;
	ControllerState Controller0;
	ControllerState Controller1;
	ControllerState Controller2;
	ControllerState Controller3;
};

class InputSystem
{
public:
	bool Initialize();
	void ShutDown();

	// SDL_PollEvents ���� ������ ȣ��Ǵ� �Լ�
	void PrepareForUpdate();
	// SDL_PollEvents ���� ���Ŀ� ȣ��Ǵ� �Լ�
	void Update();
	// => �̷��� �����ν�, �Է� ��ġ�� ��¿������� �ϰ���������, �׸��� �ش� Ű�� ��� ������ �ִ��� �� �����Ӱ� ���ϸ� �Ǵ� ������

	const InputState& GetState() const { return mState; }

	// ���콺�� ��� ����� Ȱ��ȭ/��Ȱ��ȭ �ϴ� �Լ�
	// ���콺�� ��� ����� �̿��ϸ�, ������ �� ���콺�� ������� ��ȭ�� �� �� �ִ� => 1��Ī ������ ȭ�� ȸ�� �ӵ�, Ŀ���� ������ ������ ������ ����
	void SetRelativeMouseMode(bool value);

	// SDL�� SDL_MOUSEWHEEL �̺�Ʈ�� �����Ѵ�. �Է� �ý��ۿ� ��ũ�� ���� �����ϱ� ����, �Է� �ý��ۿ� SDL �̺�Ʈ�� �����Ѵ�
	// �� �Լ���, ���޹��� SDL Event�� ó���Ѵ�
	void ProcessEvent(union SDL_Event& event);

	// ��Ʈ�ѷ��� �Ƴ��α� ��ƽ�� �Է°��� ó���ϴ� �Լ�
	// 0�� ����� �Է°��� 0���� �ٲٴ� ������ �÷��̾��� ��Ʈ�ѷ� �Է��� �� ���������� �ڵ尡 �ν��� �� �ֱ� ���ش�
	// ���⼭�� ��ƽ�� �������� �� ��(x�� ������ or y�� ������)���� �ٷ��
	float Filter1D(int input);

	// ���� Filter1D�� 2�������� �ٲ۴�
	// x, y�࿡ ���������� Filter1D()�� �����ϸ�, ���� ������ ����ȭ�� ���� : <0.0,1.0> / �밢�� ������ ����ȭ ���� : <1.0, 1.0>
	// �� �Ǵµ�, �� ����ȭ�� ������ ���̴� �ٸ���! ��, �� ���ͷ� �ӵ��� �����ϸ�, �ɸ��ʹ� �밢�� �������� �� �� �� ������ ��������!!!
	// 1D->2D�� ���̵��� �����ϴ�. deadZone�� maxValue�� �簢���̳� ���� �ƴ�, ����� ������ �����ؼ� �����ϸ� �ȴ�
	Vector2 Filter2D(int inputX, int inputY);

	// ��Ʈ�ѷ��� �ʱ�ȭ�� ����ϴ� �Լ�. ���� ó���� ���� ��Ʈ�ѷ��� ����Ǿ��� �� ȣ���Ѵ�
	void InitializeControllers();
	// ��Ʈ�ѷ��� ���ŵǾ��� �� ȣ��Ǵ� �Լ�. ��Ʈ�ѷ��� close�ϰ�, �ش��ϴ� �����͸� �����Ѵ�
	void RemoveController();

	// ���콺�� yaw, pitch ȸ���� ����ϴ� �Լ�
	void MouseYawPitchRotation();

private:
	InputState mState;

	std::unordered_map<int, SDL_GameController*> mController;
};

