#pragma once
#pragma comment (lib, "SDL2")
#pragma comment (lib, "SDL2main")
#pragma comment (lib, "SDL2_image")

#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "SoundEvent.h"
#include "Math.h"

/*
struct Vector2
{
	float x;
	float y;
};

struct Ball
{
	Vector2 Pos;
	Vector2 Vec;
};*/

class Game
{
public:
	enum class GameState
	{
		EGamePlay,
		EPaused,
		EQuit
	};

	Game();

	// 게임 초기화
	bool Initialize();
	// 게임이 끝나기 전까지 게임 루프를 실행
	void RunLoop();
	// 게임 종료
	void ShutDown();

	// 액터 추가
	void AddActor(class Actor* actor);
	// 액터 삭제
	void RemoveActor(class Actor* actor);
	// 운석 추가
	//void AddAsteroid(class Asteroid* as);
	// 운석 삭제
	//void RemoveAsteroid(class Asteroid* as);
	// 운석 불러오기
	//std::vector<class Asteroid*> GetAsteroids();
	void AddPlane(class PlaneActor* plane);
	void RemovePlane(class PlaneActor* plane);
	std::vector<class PlaneActor*>& GetPlanes() { return mPlaneActors; }

	class Renderer* GetRenderer() const { return mRenderer; }
	class AudioSystem* GetAudioSystem() const { return mAudioSystem; }
	class InputSystem* GetInputSystem() const { return mInputSystem; }
	class PhysWorld* GetPhysWorld() const { return mPhysWorld; }

	class Font* GetFont(const std::string& fileName);

	GameState GetGameState() const { return mGameState; }
	void SetGameState(const GameState state) { mGameState = state; SDL_Log("Game State is %d", mGameState); }

	// 참조로 UI스택을 반환
	const std::vector<class UIScreen*>& GetUIStack();
	// 특정 UIScreen을 스택에 추가한다
	void PushUI(class UIScreen* screen);

private:
	// 게임 루프를 위한 헬퍼 함수
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	
	// 모든 액터를 생성/제거하는 역할
	void LoadData();
	void UnloadData();

	
	void HandleKeyPress(int key);

	//-----------------------------------------------------------------------------------------------------------------------------


	// 게임이 계속 실행돼야 하는지를 판단
	bool mIsRunning;


	// DeltaTime을 계산하기 위한 변수 = Init으로 게임이 시작하고 지난 틱 수
	Uint32 mTicksCount;

	// 게임 상태
	GameState mGameState;

	// 화면을 그리기 위한 렌더러
	class Renderer* mRenderer;

	// 소리 출력을 위한 오디오 시스템
	class AudioSystem* mAudioSystem;

	// 입력 처리를 위한 InputSystem
	class InputSystem* mInputSystem;

	// 물리 세계를 위한 포인터
	class PhysWorld* mPhysWorld;

	/*
	* 액터를 게임 루프에 통합하기 위한 멤버들
	* 1. 활성화된 액터 / 2. 대기 액터   이렇게 두 벡터를 만든다
	*
	* 액터를 반복하는 동안(mActors를 반복하는 동안) 새 액터를 생성하는 경우를 다루려면 대기 액터들을 위한 벡터가 필요하다
	* 이 경우 mPendingActors에 요소를 추가한 뒤, mActors의 반복이 끝나면 그때 대기 중인 액터들을 mActors로 이동시킨다
	*/
	// 활성화된 액터
	std::vector<class Actor*> mActors;
	// 대기중인 액터
	std::vector<class Actor*> mPendingActors;

	// 액터가 갱신중(update)인지 확인하는 변수
	bool mUpdatingActors;

	// 폰트를 관리하는 맵
	std::unordered_map<std::string, class Font*> mText;

	// UIScreen Statck
	std::vector<class UIScreen*> mUIStack;

	// Game-specific code
	class CameraActor* mCameraActor;
	class FPSActor* mFPSActor;
	class FollowActor* mFollowActor;
	class OrbitActor* mOrbitActor;
	class SplineActor* mSplineActor;
	class SpriteComponent* mCrosshair;
	SoundEvent mMusicEvent;
	SoundEvent mReverbSnap;
	class HUD* mHUD;

	std::vector<class PlaneActor*> mPlaneActors;
};

