#pragma once
#include <SDL_scancode.h>
#include <SDL_gamecontroller.h>
#include <SDL_mouse.h>
#include <unordered_map>
#include "Math.h"

/* ***
* 지금까지는 ProcessInput()이 SDL 함수를 직접 호출해 마우스와 컨트롤러에 접근했다.
* -> 이는 간단한 게임에서는 잘 작동하겠지만, 액터나 컴포넌트를 만드는 프로그래머에게는 SDL 함수 관련 지식은 불필요하다 
* -> 그리고 SDL 입력 함수는 함수 호출 간 상태 차이를 반환하는데, 한 프레임 동안 이런 함수를 두 번 이상 호출하면
*	 첫 호출 다음에는 상태 변화가 없음을 뜻하는 0 값을 얻을 것이다
*/

/* ***
* 이를 해결하기 위해 InputSystem 클래스를 선언한다.
* 이 클래스는 InputState라는 헬퍼 클래스에 데이터를 채우고, ProcessInput()을 사용해서 InputState를 액터/컴포넌트에 상수 참조로 전달한다
* 또한 액터/컴포넌트가 관심 있어 하는 상태를 쉽게 조회하도록, InputState는 몇몇 헬퍼 함수를 가진다
*/

enum ButtonState
{
	ENone,
	EPressed,
	EReleased,
	EHeld
};

/* ***
* 현재 상태를 가리키는 포인터와 이전 상태를 위한 배열을 가진다
* 배열의 크기는 SDL이 키보드 스캔 코드에 대해 사용하는 버퍼 크기와 같다
*/
class KeyboardState
{
public:
	// InputSystem을 friend로 선언해서, 멤버 데이터를 쉽게 갱신하게 한다
	friend class InputSystem;

	// 키의 이진값 true/false를 얻는다. keyCode는 SDL_SCANCODE_ESCAPE 같은 것
	bool GetKeyValue(SDL_Scancode keyCode) const;

	// 현재/이전 프레임을 토대로 상태를 얻는다
	ButtonState GetKeyState(SDL_Scancode keyCode) const;

private:
	// 현재 상태
	const Uint8* mCurrState;
	// 이전 프레임의 상태
	Uint8 mPrevState[SDL_NUM_SCANCODES];
};

/* ***
* Mouse의 상태를 저장하는 class
*/
class MouseState
{
public:
	friend class InputSystem;

	// 마우스 위치
	const Vector2& GetPosition() const { return mMousePos; }
	const float GetAngularSpeed() const { return mAngularSpeed; }
	const float GetPitchSpeed() const { return mPitchSpeed; }

	// 버튼
	// SDL_GetMouseState의 리턴값은 bitmask 이므로, 비트AND 연산을 통해 누르고 있는지 뗐는지 확인할 수 있다
	bool GetButtionValue(int button) const;
	ButtonState GetButtonState(int button) const;

private:
	// 마우스 위치 저장
	Vector2 mMousePos;
	// 버튼 데이터 저장
	Uint32 mCurrButtons;
	Uint32 mPrevButtons;
	// 마우스 스크롤 데이터
	Vector2 mScrollWheel;

	// 상대 모션?
	bool mIsRelative = true;

	// 상대 모션 사용시 사용할 회전속도
	float mAngularSpeed = 0.0f;
	float mPitchSpeed = 0.0f;
};

/* ***
* Controller의 상태를 저장하는 class
*/
class ControllerState
{
public:
	friend class InputSystem;

	// 버튼
	// 컨트롤러의 버튼은 각 이름을 통해서 일일히 하나씩 불러와야 한다
	// 이름은 enum으로 정의되어 있고, SDL_CONTROLLER_BUTTON_A, SDL_CONTROLLER_START, SDL_CONTROLLER_BUTTON_LEFTSTICK 등으로 이루어져 있다
	bool GetButtonValue(SDL_GameControllerButton button) const;
	ButtonState GetButtonState(SDL_GameControllerButton button) const;

	bool GetIsConnected() const { return mIsConnected; }

	float GetLeftTrigger() const { return mLeftTrigger; }
	float GetRightTrigger() const { return mRightTrigger; }

	Vector2 GetLeftStick() const { return mLeftStick; }
	Vector2 GetRightStick() const { return mRightStick; }


private:
	// 현재/이전 버튼
	Uint8 mCurrButtons[SDL_CONTROLLER_BUTTON_MAX];	// SDL_CONTROLLER_BUTTON_MAX는 컨트롤러가 가질 수 있는 버튼의 최대 수를 의미한다
	Uint8 mPrevButtons[SDL_CONTROLLER_BUTTON_MAX];

	// 컨트롤러가 연결되어 있는가?
	bool mIsConnected;

	// 왼쪽, 오른쪽 트리거
	float mLeftTrigger;
	float mRightTrigger;

	// 왼쪽, 오른쪽 스틱
	Vector2 mLeftStick;
	Vector2 mRightStick;
};

// 입력의 현재 상태를 포함하는 Wrapper
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

	// SDL_PollEvents 루프 직전에 호출되는 함수
	void PrepareForUpdate();
	// SDL_PollEvents 루프 직후에 호출되는 함수
	void Update();
	// => 이렇게 함으로써, 입력 장치의 상승에지인지 하강에지인지, 그리고 해당 키를 계속 누르고 있는지 전 프레임과 비교하며 판단 가능함

	const InputState& GetState() const { return mState; }

	// 마우스의 상대 모션을 활성화/비활성화 하는 함수
	// 마우스의 상대 모션을 이용하면, 프레임 간 마우스의 상대적인 변화를 알 수 있다 => 1인칭 게임의 화면 회전 속도, 커서가 윈도우 밖으로 나가기 방지
	void SetRelativeMouseMode(bool value);

	// SDL은 SDL_MOUSEWHEEL 이벤트를 생성한다. 입력 시스템에 스크롤 휠을 지원하기 위해, 입력 시스템에 SDL 이벤트를 전달한다
	// 이 함수는, 전달받은 SDL Event를 처리한다
	void ProcessEvent(union SDL_Event& event);

	// 컨트롤러의 아날로그 스틱의 입력값을 처리하는 함수
	// 0에 가까운 입력값은 0으로 바꾸는 식으로 플레이어의 컨트롤러 입력을 더 안정적으로 코드가 인식할 수 있기 해준다
	// 여기서는 스틱의 움직임의 한 축(x축 움직임 or y축 움직임)만을 다룬다
	float Filter1D(int input);

	// 위의 Filter1D를 2차원으로 바꾼다
	// x, y축에 개별적으로 Filter1D()를 적용하면, 위쪽 방향의 정규화된 벡터 : <0.0,1.0> / 대각선 방향의 정규화 벡터 : <1.0, 1.0>
	// 이 되는데, 두 정규화된 벡터의 길이는 다르다! 즉, 이 벡터로 속도를 구현하면, 케릭터는 대각선 방향으로 갈 때 더 빠르게 가버린다!!!
	// 1D->2D의 아이디어는 간단하다. deadZone과 maxValue를 사각형이나 선이 아닌, 평면의 원으로 생각해서 구현하면 된다
	Vector2 Filter2D(int inputX, int inputY);

	// 컨트롤러의 초기화를 담당하는 함수. 가장 처음과 새로 컨트롤러가 연결되었을 때 호출한다
	void InitializeControllers();
	// 컨트롤러가 제거되었을 때 호출되는 함수. 컨트롤러를 close하고, 해당하는 포인터를 제거한다
	void RemoveController();

	// 마우스의 yaw, pitch 회전을 담당하는 함수
	void MouseYawPitchRotation();

private:
	InputState mState;

	std::unordered_map<int, SDL_GameController*> mController;
};

