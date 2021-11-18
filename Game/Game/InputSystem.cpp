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
	/* 키보드 초기화 */
	// 현재 상태의 포인터를 설정한다
	mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
	// 이전 상태 메모리를 0으로 초기화(채운다)
	memset(mState.Keyboard.mPrevState, 0, SDL_NUM_SCANCODES);

	/* 마우스 초기화 */
	// 그냥 0으로 만들어
	mState.Mouse.mCurrButtons = 0;
	mState.Mouse.mPrevButtons = 0;
	mState.Mouse.mAngularSpeed = 0.0f;
	mState.Mouse.mPitchSpeed = 0.0f;
	mState.Mouse.mIsRelative = false;

	/* 컨트롤러 초기화 */
	/* **
	* 컨트롤러를 초기화하기 위해서는 SDL_GameControllerOpen() 호출.
	* 초기화 성공 -> SDL_Controller 구조체 포인터 반환, 실패 -> nullptr 반환
	* 컨트롤러의 상태를 알아보기 위해서는, SDL_Controller* 변수를 조회해보면 된다
	*/
	InitializeControllers();

	return true;
}

void InputSystem::ShutDown()
{

}

void InputSystem::PrepareForUpdate()
{
	// 모든 '현재' 데이터를 이전 버퍼로 복사한다
	// 이 함수를 호출할 때의 '현재' 데이터란, 이전 프레임의 데이터임을 명심하자 (아직 SDL_PollEvents 호출되지 않았음)

	// 키보드 복사
	memcpy(mState.Keyboard.mPrevState, mState.Keyboard.mCurrState, SDL_NUM_SCANCODES);
	// 마우스 복사
	mState.Mouse.mPrevButtons = mState.Mouse.mCurrButtons;
	// 컨트롤러 복사
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

	// 마우스 휠 이벤트는, 스크롤 휠이 움직이는 프레임에서만 trigger 되므로, 매 SDL_PollEvent 이전마다 mScrollWheel 변수를 초기화한다
	// 이는 스크롤휠이 frame1 에서는 움직이지만, frame2 에서는 움직이지 않는다는 것을 보장한다.
	// 그래서 frame2에서는 잘못된(frame1의) 스크롤 값을 보고받지 않는다
	mState.Mouse.mScrollWheel = Vector2::Zero;
}

void InputSystem::Update()
{
	/* 마우스 */
	// 마우스의 위치 저장
	mState.Mouse.mAngularSpeed = 0.0f;
	mState.Mouse.mPitchSpeed = 0.0f;
	int x = 0, y = 0;
	if (mState.Mouse.mIsRelative)
	{
		//mState.Mouse.mCurrButtons = SDL_GetRelativeMouseState(&x, &y);
		// 마우스의 위치 저장
		if (!mState.Mouse.mIsRelative)
		{
			SetRelativeMouseMode(true);
		}
		mState.Mouse.mCurrButtons = SDL_GetRelativeMouseState(&x, &y);

		// yaw
		// 마우스 이동은 -500~500 이라고 가정
		const int maxMouseSpeed = 500;
		// 초당 회전의 최대 속도
		const float maxAngularSpeed = Math::Pi * 8;
		float angularSpeed = 0.0f;
		if (x != 0)
		{
			// [-1.0, 1.0] 으로 범위 한정
			angularSpeed = static_cast<float>(x) / maxMouseSpeed;
			// 초당 회전을 곱한다
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



	/* 컨트롤러 */
	for (int i = 0; i < 4; i++)
	{
		auto iter = mController.find(i);
		if (iter != mController.end())
		{
			switch (i)
			{
			case 0:
				// 컨트롤러의 버튼에 대한 상태를 조회하고, 그 결과를 버튼의 요소값에 저장
				for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; j++)
				{
					mState.Controller0.mCurrButtons[j] =
						SDL_GameControllerGetButton(iter->second, SDL_GameControllerButton(j));
				}

				// 컨트롤러의 트리거 값을 읽고, filter1D()를 적용시켜 적절한 범위로 변환시킨다
				mState.Controller0.mLeftTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERLEFT));
				mState.Controller0.mRightTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

				// 컨트롤러의 스틱 값을 읽고, filter2D()를 적용시켜 적절한 범위로 변환시킨다
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTY);	// SDL은 +y가 아래쪽 방향이므로, 반전시킨다
				mState.Controller0.mLeftStick = Filter2D(x, y);
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_RIGHTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_RIGHTY);
				mState.Controller0.mRightStick = Filter2D(x, y);
				break;
			case 1:
				// 컨트롤러의 버튼에 대한 상태를 조회하고, 그 결과를 버튼의 요소값에 저장
				for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; j++)
				{
					mState.Controller1.mCurrButtons[j] =
						SDL_GameControllerGetButton(iter->second, SDL_GameControllerButton(j));
				}

				// 컨트롤러의 트리거 값을 읽고, filter1D()를 적용시켜 적절한 범위로 변환시킨다
				mState.Controller1.mLeftTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERLEFT));
				mState.Controller1.mRightTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

				// 컨트롤러의 스틱 값을 읽고, filter2D()를 적용시켜 적절한 범위로 변환시킨다
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTY);	// SDL은 +y가 아래쪽 방향이므로, 반전시킨다
				mState.Controller1.mLeftStick = Filter2D(x, y);
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_RIGHTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_RIGHTY);
				mState.Controller1.mRightStick = Filter2D(x, y);
				break;
			case 2:
				// 컨트롤러의 버튼에 대한 상태를 조회하고, 그 결과를 버튼의 요소값에 저장
				for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; j++)
				{
					mState.Controller2.mCurrButtons[j] =
						SDL_GameControllerGetButton(iter->second, SDL_GameControllerButton(j));
				}

				// 컨트롤러의 트리거 값을 읽고, filter1D()를 적용시켜 적절한 범위로 변환시킨다
				mState.Controller2.mLeftTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERLEFT));
				mState.Controller2.mRightTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

				// 컨트롤러의 스틱 값을 읽고, filter2D()를 적용시켜 적절한 범위로 변환시킨다
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTY);	// SDL은 +y가 아래쪽 방향이므로, 반전시킨다
				mState.Controller2.mLeftStick = Filter2D(x, y);
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_RIGHTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_RIGHTY);
				mState.Controller2.mRightStick = Filter2D(x, y);
				break;
			case 3:
				// 컨트롤러의 버튼에 대한 상태를 조회하고, 그 결과를 버튼의 요소값에 저장
				for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; j++)
				{
					mState.Controller3.mCurrButtons[j] =
						SDL_GameControllerGetButton(iter->second, SDL_GameControllerButton(j));
				}

				// 컨트롤러의 트리거 값을 읽고, filter1D()를 적용시켜 적절한 범위로 변환시킨다
				mState.Controller3.mLeftTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERLEFT));
				mState.Controller3.mRightTrigger = Filter1D(SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

				// 컨트롤러의 스틱 값을 읽고, filter2D()를 적용시켜 적절한 범위로 변환시킨다
				x = SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTX);
				y = -SDL_GameControllerGetAxis(iter->second, SDL_CONTROLLER_AXIS_LEFTY);	// SDL은 +y가 아래쪽 방향이므로, 반전시킨다
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
	// 값이 데드존보다 작으면 0%로 해석
	const int deadZone = 250;
	// 값이 최대값보다 크면 100%로 해석
	const int maxValue = 30000;

	float retVal = 0.0f;

	// 입력의 절대값을 얻는다
	int absValue = input > 0 ? input : -input;
	// 데드존 내의 입력값은 무시
	if (absValue > deadZone)
	{
		// 데드 존과 최댓값 사이의 분숫값을 계산
		retVal = static_cast<float>(absValue - deadZone) / (maxValue - deadZone);

		// 원래의 부호와 일치시킨다
		retVal = input > 0 ? retVal : -1.0f * retVal;

		// 값이 -1.0f 와 1.0f 범위를 벗어나지 않게 한다
		retVal = Math::Clamp(retVal, -1.0f, 1.0f);
	}
	
	return retVal;
}

Vector2 InputSystem::Filter2D(int inputX, int inputY)
{
	const float deadZone = 8000.0f;
	const float maxValue = 30000.0f;

	// 2D 벡터
	Vector2 dir;
	dir.x = static_cast<float>(inputX);
	dir.y = static_cast<float>(inputY);

	float length = dir.Length();

	// 길이가 데드존보다 작다면, 입력 없음으로 처리
	if (length < deadZone)
	{
		dir = Vector2::Zero;
	}
	else
	{
		// 데드존과 최댓값 동심원 사이의 분숫값을 계산
		float f = (length - deadZone) / (maxValue - deadZone);
		// f값을 0.0~1.0 으로 고정
		f = Math::Clamp(f, 0.0f, 1.0f);
		// 벡터를 정규화한뒤 분숫값으로
		// 벡터를 스케일
		dir *= f / length;
	}

	return dir;
}

void InputSystem::InitializeControllers()
{
	/* 컨트롤러 초기화 */
	/* **
	* 컨트롤러를 초기화하기 위해서는 SDL_GameControllerOpen() 호출.
	* 초기화 성공 -> SDL_Controller 구조체 포인터 반환, 실패 -> nullptr 반환
	* 컨트롤러의 상태를 알아보기 위해서는, SDL_Controller* 변수를 조회해보면 된다
	*/
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		// 이 조이스틱이 컨트롤러인가?
		if (SDL_IsGameController(i))
		{
			// 현재 연결되어 있는 컨트롤러를 Open 해서, 그 정보를 vector에 저장한다
			//mController = SDL_GameControllerOpen(0);	// 0번 컨트롤러를 오픈한다
			// 비활성화 하려면, SDL_GameControllerClose(mController) 호출하면 된다
			SDL_GameController* controller = SDL_GameControllerOpen(i);
			// SDL_gameController* 맵에 추가
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
				// 컨트롤러의 이전/현재 버튼들을 0으로 초기화(채운다)
				memset(mState.Controller0.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
				memset(mState.Controller0.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
				break;
			case 1:
				mState.Controller1.mIsConnected = (mController[i] != nullptr);
				// 컨트롤러의 이전/현재 버튼들을 0으로 초기화(채운다)
				memset(mState.Controller1.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
				memset(mState.Controller1.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
				break;
			case 2:
				mState.Controller2.mIsConnected = (mController[i] != nullptr);
				// 컨트롤러의 이전/현재 버튼들을 0으로 초기화(채운다)
				memset(mState.Controller2.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
				memset(mState.Controller2.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
				break;
			case 3:
				mState.Controller3.mIsConnected = (mController[i] != nullptr);
				// 컨트롤러의 이전/현재 버튼들을 0으로 초기화(채운다)
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
		// 해당 포트에 컨트롤러가 없고
		if (!SDL_IsGameController(i))
		{
			// 그 번호에 해당하는 컨트롤러가 맵에 있으면, 그것을 닫고 제거한다
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
	// 마우스의 위치 저장
	int x = 0, y = 0;
	if (!mState.Mouse.mIsRelative)
	{
		SetRelativeMouseMode(true);
	}
	mState.Mouse.mCurrButtons = SDL_GetRelativeMouseState(&x, &y);

	// yaw
	// 마우스 이동은 -500~500 이라고 가정
	const int maxMouseSpeed = 500;
	// 초당 회전의 최대 속도
	const float maxAngularSpeed = Math::Pi * 8;
	float angularSpeed = 0.0f;
	if (x != 0)
	{
		// [-1.0, 1.0] 으로 범위 한정
		angularSpeed = static_cast<float>(x) / maxMouseSpeed;
		// 초당 회전을 곱한다
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