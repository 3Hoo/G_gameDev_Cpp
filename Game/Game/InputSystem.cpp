#include "InputSystem.h"
#include <SDL.h>


bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const
{
	return mCurrState[keyCode] == 1;
}

ButtonState KeyboardState::GetKeyState(SDL_Scancode keyCode) const
{
	if (mPrevState[keyCode] == 0)
	{
		if (mCurrState[keyCode] == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else
	{
		if (mCurrState[keyCode] == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool MouseState::GetButtionValue(int button) const
{
	return (SDL_BUTTON(button) & mCurrButtons) == 1;
}

ButtonState MouseState::GetButtonState(int button) const
{
	int mask = SDL_BUTTON(button);
	if ((mask & mPrevButtons) == 0)
	{
		if ((mask & mPrevButtons) == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else
	{
		if ((mask & mPrevButtons) == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool ControllerState::GetButtonValue(SDL_GameControllerButton button) const
{
	return mCurrButtons[button] == 1;
}

ButtonState ControllerState::GetButtonState(SDL_GameControllerButton button) const
{
	if (mPrevButtons[button] == 0)
	{
		if (mCurrButtons[button] == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else
	{
		if (mCurrButtons[button] == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool InputSystem::Initialize()
{
	/* Ű���� �ʱ�ȭ */
	// ���� ������ �����͸� �����Ѵ�
	mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
	// ���� ���� �޸𸮸� 0���� �ʱ�ȭ(ä���)
	memset(mState.Keyboard.mPrevState, 0, SDL_NUM_SCANCODES);

	/* ���콺 �ʱ�ȭ */
	// �׳� 0���� �����
	mState.Mouse.mCurrButtons = 0;
	mState.Mouse.mPrevButtons = 0;
	mState.Mouse.mAngularSpeed = 0.0f;
	mState.Mouse.mPitchSpeed = 0.0f;
	mState.Mouse.mIsRelative = false;

	/* ��Ʈ�ѷ� �ʱ�ȭ */
	/* **
	* ��Ʈ�ѷ��� �ʱ�ȭ�ϱ� ���ؼ��� SDL_GameControllerOpen() ȣ��.
	* �ʱ�ȭ ���� -> SDL_Controller ����ü ������ ��ȯ, ���� -> nullptr ��ȯ
	* ��Ʈ�ѷ��� ���¸� �˾ƺ��� ���ؼ���, SDL_Controller* ������ ��ȸ�غ��� �ȴ�
	*/
	InitializeControllers();

	return true;
}

void InputSystem::ShutDown()
{

}

void InputSystem::PrepareForUpdate()
{
	// ��� '����' �����͸� ���� ���۷� �����Ѵ�
	// �� �Լ��� ȣ���� ���� '����' �����Ͷ�, ���� �������� ���������� ������� (���� SDL_PollEvents ȣ����� �ʾ���)

	// Ű���� ����
	memcpy(mState.Keyboard.mPrevState, mState.Keyboard.mCurrState, SDL_NUM_SCANCODES);
	// ���콺 ����
	mState.Mouse.mPrevButtons = mState.Mouse.mCurrButtons;
	// ��Ʈ�ѷ� ����
	for (int i = 0; i < 4; i++)
	{
		auto iter = mController.find(i);
		if (iter != mController.end())
		{
			switch (i)
			{
			case 0:
				memcpy(mState.Controller0.mPrevButtons, mState.Controller0.mCurrButtons, SDL_CONTROLLER_BUTTON_MAX);
				break;
			case 1:
				memcpy(mState.Controller1.mPrevButtons, mState.Controller1.mCurrButtons, SDL_CONTROLLER_BUTTON_MAX);
				break;
			case 2:
				memcpy(mState.Controller2.mPrevButtons, mState.Controller2.mCurrButtons, SDL_CONTROLLER_BUTTON_MAX);
				break;
			case 3:
				memcpy(mState.Controller3.mPrevButtons, mState.Controller3.mCurrButtons, SDL_CONTROLLER_BUTTON_MAX);
				break;
			default:
				break;
			}
		}
	}

	// ���콺 �� �̺�Ʈ��, ��ũ�� ���� �����̴� �����ӿ����� trigger �ǹǷ�, �� SDL_PollEvent �������� mScrollWheel ������ �ʱ�ȭ�Ѵ�
	// �̴� ��ũ������ frame1 ������ ����������, frame2 ������ �������� �ʴ´ٴ� ���� �����Ѵ�.
	// �׷��� frame2������ �߸���(frame1��) ��ũ�� ���� ������� �ʴ´�
	mState.Mouse.mScrollWheel = Vector2::Zero;
}

void InputSystem::Update()
{
	/* ���콺 */
	// ���콺�� ��ġ ����
	mState.Mouse.mAngularSpeed = 0.0f;
	mState.Mouse.mPitchSpeed = 0.0f;
	int x = 0, y = 0;
	if (mState.Mouse.mIsRelative)
	{
		//mState.Mouse.mCurrButtons = SDL_GetRelativeMouseState(&x, &y);
		// ���콺�� ��ġ ����
		if (!mState.Mouse.mIsRelative)
		{
			SetRelativeMouseMode(true);
		}
		mState.Mouse.mCurrButtons = SDL_GetRelativeMouseState(&x, &y);

		// yaw
		// ���콺 �̵��� -500~500 �̶�� ����
		const int maxMouseSpeed = 500;
		// �ʴ� ȸ���� �ִ� �ӵ�
		const float maxAngularSpeed = Math::Pi * 8;
		float angularSpeed = 0.0f;
		if (x != 0)
		{
			// [-1.0, 1.0] ���� ���� ����
			angularSpeed = static_cast<float>(x) / maxMouseSpeed;
			// �ʴ� ȸ���� ���Ѵ�
			angularSpeed *= maxAngularSpeed;
			mState.Mouse.mAngularSpeed = angularSpeed;
		}

		// Compute pitch
		const float maxPitchSpeed = Math::Pi * 8;
		float pitchSpeed = 0.0f;
		if (y != 0)
		{
			// Convert to ~[-1.0, 1.0]
			pitchSpeed = static_cast<float>(y) / maxMouseSpeed;
			pitchSpeed *= maxPitchSpeed;
			mState.Mouse.mPitchSpeed = pitchSpeed;
		}

	}
	else
	{
		mState.Mouse.mCurrButtons = SDL_GetMouseState(&x, &y);
	}
	mState.Mouse.mMousePos.x = static_cast<float>(x);
	mState.Mouse.mMousePos.y = static_cast<float>(y);



	/* ��Ʈ�ѷ� */
	for (int i = 0; i < 4; i++)
	{
		auto iter = mController.find(i);
		if (iter != mController.end())
		{
			switch (i)
			{
			case 0:
				// ��Ʈ�ѷ��� ��ư�� ���� ���¸� ��ȸ�ϰ�, �� ����� ��ư�� ��Ұ��� ����
				for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; j++)
				{
					mState.Controller0.mCurrButtons[j] =
						SDL_GameControllerGetButton(iter->second, SDL_GameControllerButton(j));
				}

				// ��Ʈ�ѷ��� Ʈ���� ���� �а�, filter1D()�� ������� ������ ������ ��ȯ��Ų��
				mState.Controller0.mLeftTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERLEFT));
				mState.Controller0.mRightTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

				// ��Ʈ�ѷ��� ��ƽ ���� �а�, filter2D()�� ������� ������ ������ ��ȯ��Ų��
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTY);	// SDL�� +y�� �Ʒ��� �����̹Ƿ�, ������Ų��
				mState.Controller0.mLeftStick = Filter2D(x, y);
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_RIGHTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_RIGHTY);
				mState.Controller0.mRightStick = Filter2D(x, y);
				break;
			case 1:
				// ��Ʈ�ѷ��� ��ư�� ���� ���¸� ��ȸ�ϰ�, �� ����� ��ư�� ��Ұ��� ����
				for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; j++)
				{
					mState.Controller1.mCurrButtons[j] =
						SDL_GameControllerGetButton(iter->second, SDL_GameControllerButton(j));
				}

				// ��Ʈ�ѷ��� Ʈ���� ���� �а�, filter1D()�� ������� ������ ������ ��ȯ��Ų��
				mState.Controller1.mLeftTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERLEFT));
				mState.Controller1.mRightTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

				// ��Ʈ�ѷ��� ��ƽ ���� �а�, filter2D()�� ������� ������ ������ ��ȯ��Ų��
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTY);	// SDL�� +y�� �Ʒ��� �����̹Ƿ�, ������Ų��
				mState.Controller1.mLeftStick = Filter2D(x, y);
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_RIGHTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_RIGHTY);
				mState.Controller1.mRightStick = Filter2D(x, y);
				break;
			case 2:
				// ��Ʈ�ѷ��� ��ư�� ���� ���¸� ��ȸ�ϰ�, �� ����� ��ư�� ��Ұ��� ����
				for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; j++)
				{
					mState.Controller2.mCurrButtons[j] =
						SDL_GameControllerGetButton(iter->second, SDL_GameControllerButton(j));
				}

				// ��Ʈ�ѷ��� Ʈ���� ���� �а�, filter1D()�� ������� ������ ������ ��ȯ��Ų��
				mState.Controller2.mLeftTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERLEFT));
				mState.Controller2.mRightTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

				// ��Ʈ�ѷ��� ��ƽ ���� �а�, filter2D()�� ������� ������ ������ ��ȯ��Ų��
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTY);	// SDL�� +y�� �Ʒ��� �����̹Ƿ�, ������Ų��
				mState.Controller2.mLeftStick = Filter2D(x, y);
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_RIGHTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_RIGHTY);
				mState.Controller2.mRightStick = Filter2D(x, y);
				break;
			case 3:
				// ��Ʈ�ѷ��� ��ư�� ���� ���¸� ��ȸ�ϰ�, �� ����� ��ư�� ��Ұ��� ����
				for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; j++)
				{
					mState.Controller3.mCurrButtons[j] =
						SDL_GameControllerGetButton(iter->second, SDL_GameControllerButton(j));
				}

				// ��Ʈ�ѷ��� Ʈ���� ���� �а�, filter1D()�� ������� ������ ������ ��ȯ��Ų��
				mState.Controller3.mLeftTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERLEFT));
				mState.Controller3.mRightTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

				// ��Ʈ�ѷ��� ��ƽ ���� �а�, filter2D()�� ������� ������ ������ ��ȯ��Ų��
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTY);	// SDL�� +y�� �Ʒ��� �����̹Ƿ�, ������Ų��
				mState.Controller3.mLeftStick = Filter2D(x, y);
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_RIGHTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_RIGHTY);
				mState.Controller3.mRightStick = Filter2D(x, y);
				break;
			default:
				break;
			}
		}
	}
}

void InputSystem::SetRelativeMouseMode(bool value)
{
	SDL_bool set = value ? SDL_TRUE : SDL_FALSE;
	SDL_SetRelativeMouseMode(set);

	mState.Mouse.mIsRelative = value;
}

void InputSystem::ProcessEvent(SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_MOUSEWHEEL:
		mState.Mouse.mScrollWheel = Vector2(
			static_cast<float>(event.wheel.x),
			static_cast<float>(event.wheel.y)
		);
		break;
	case SDL_CONTROLLERDEVICEADDED:
		InitializeControllers();
		break;
	case SDL_CONTROLLERDEVICEREMOVED:
		RemoveController();
		break;
	default:
		break;
	}
}

float InputSystem::Filter1D(int input)
{
	// ���� ���������� ������ 0%�� �ؼ�
	const int deadZone = 250;
	// ���� �ִ밪���� ũ�� 100%�� �ؼ�
	const int maxValue = 30000;

	float retVal = 0.0f;

	// �Է��� ���밪�� ��´�
	int absValue = input > 0 ? input : -input;
	// ������ ���� �Է°��� ����
	if (absValue > deadZone)
	{
		// ���� ���� �ִ� ������ �м����� ���
		retVal = static_cast<float>(absValue - deadZone) / (maxValue - deadZone);

		// ������ ��ȣ�� ��ġ��Ų��
		retVal = input > 0 ? retVal : -1.0f * retVal;

		// ���� -1.0f �� 1.0f ������ ����� �ʰ� �Ѵ�
		retVal = Math::Clamp(retVal, -1.0f, 1.0f);
	}
	
	return retVal;
}

Vector2 InputSystem::Filter2D(int inputX, int inputY)
{
	const float deadZone = 8000.0f;
	const float maxValue = 30000.0f;

	// 2D ����
	Vector2 dir;
	dir.x = static_cast<float>(inputX);
	dir.y = static_cast<float>(inputY);

	float length = dir.Length();

	// ���̰� ���������� �۴ٸ�, �Է� �������� ó��
	if (length < deadZone)
	{
		dir = Vector2::Zero;
	}
	else
	{
		// �������� �ִ� ���ɿ� ������ �м����� ���
		float f = (length - deadZone) / (maxValue - deadZone);
		// f���� 0.0~1.0 ���� ����
		f = Math::Clamp(f, 0.0f, 1.0f);
		// ���͸� ����ȭ�ѵ� �м�������
		// ���͸� ������
		dir *= f / length;
	}

	return dir;
}

void InputSystem::InitializeControllers()
{
	/* ��Ʈ�ѷ� �ʱ�ȭ */
	/* **
	* ��Ʈ�ѷ��� �ʱ�ȭ�ϱ� ���ؼ��� SDL_GameControllerOpen() ȣ��.
	* �ʱ�ȭ ���� -> SDL_Controller ����ü ������ ��ȯ, ���� -> nullptr ��ȯ
	* ��Ʈ�ѷ��� ���¸� �˾ƺ��� ���ؼ���, SDL_Controller* ������ ��ȸ�غ��� �ȴ�
	*/
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		// �� ���̽�ƽ�� ��Ʈ�ѷ��ΰ�?
		if (SDL_IsGameController(i))
		{
			// ���� ����Ǿ� �ִ� ��Ʈ�ѷ��� Open �ؼ�, �� ������ vector�� �����Ѵ�
			//mController = SDL_GameControllerOpen(0);	// 0�� ��Ʈ�ѷ��� �����Ѵ�
			// ��Ȱ��ȭ �Ϸ���, SDL_GameControllerClose(mController) ȣ���ϸ� �ȴ�
			SDL_GameController* controller = SDL_GameControllerOpen(i);
			// SDL_gameController* �ʿ� �߰�
			mController.emplace(i, controller);
		}
	}

	for (int i = 0; i < 4; i++)
	{
		auto iter = mController.find(i);
		if (iter != mController.end())
		{
			switch (i)
			{
			case 0:
				mState.Controller0.mIsConnected = (mController[i] != nullptr);
				// ��Ʈ�ѷ��� ����/���� ��ư���� 0���� �ʱ�ȭ(ä���)
				memset(mState.Controller0.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
				memset(mState.Controller0.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
				break;
			case 1:
				mState.Controller1.mIsConnected = (mController[i] != nullptr);
				// ��Ʈ�ѷ��� ����/���� ��ư���� 0���� �ʱ�ȭ(ä���)
				memset(mState.Controller1.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
				memset(mState.Controller1.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
				break;
			case 2:
				mState.Controller2.mIsConnected = (mController[i] != nullptr);
				// ��Ʈ�ѷ��� ����/���� ��ư���� 0���� �ʱ�ȭ(ä���)
				memset(mState.Controller2.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
				memset(mState.Controller2.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
				break;
			case 3:
				mState.Controller3.mIsConnected = (mController[i] != nullptr);
				// ��Ʈ�ѷ��� ����/���� ��ư���� 0���� �ʱ�ȭ(ä���)
				memset(mState.Controller3.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
				memset(mState.Controller3.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
				break;
			default:
				break;
			}
		}
	}
}

void InputSystem::RemoveController()
{
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		// �ش� ��Ʈ�� ��Ʈ�ѷ��� ����
		if (!SDL_IsGameController(i))
		{
			// �� ��ȣ�� �ش��ϴ� ��Ʈ�ѷ��� �ʿ� ������, �װ��� �ݰ� �����Ѵ�
			auto iter = mController.find(i);
			if (iter != mController.end())
			{
				SDL_GameControllerClose(iter->second);
				mController.erase(i);
			}
		}
	}
}

void InputSystem::MouseYawPitchRotation()
{
	// ���콺�� ��ġ ����
	int x = 0, y = 0;
	if (!mState.Mouse.mIsRelative)
	{
		SetRelativeMouseMode(true);
	}
	mState.Mouse.mCurrButtons = SDL_GetRelativeMouseState(&x, &y);

	// yaw
	// ���콺 �̵��� -500~500 �̶�� ����
	const int maxMouseSpeed = 500;
	// �ʴ� ȸ���� �ִ� �ӵ�
	const float maxAngularSpeed = Math::Pi * 8;
	float angularSpeed = 0.0f;
	if (x != 0)
	{
		// [-1.0, 1.0] ���� ���� ����
		angularSpeed = static_cast<float>(x) / maxMouseSpeed;
		// �ʴ� ȸ���� ���Ѵ�
		angularSpeed *= maxAngularSpeed;
		mState.Mouse.mAngularSpeed = angularSpeed;
	}

	// Compute pitch
	const float maxPitchSpeed = Math::Pi * 8;
	float pitchSpeed = 0.0f;
	if (y != 0)
	{
		// Convert to ~[-1.0, 1.0]
		pitchSpeed = static_cast<float>(y) / maxMouseSpeed;
		pitchSpeed *= maxPitchSpeed;
		mState.Mouse.mPitchSpeed = pitchSpeed;
	}

	SDL_Log("%f", mState.Mouse.mAngularSpeed);
	//*yaw = angularSpeed;
	//*pitch = pitchSpeed;

	return;
}