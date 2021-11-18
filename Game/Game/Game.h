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

	// ���� �ʱ�ȭ
	bool Initialize();
	// ������ ������ ������ ���� ������ ����
	void RunLoop();
	// ���� ����
	void ShutDown();

	// ���� �߰�
	void AddActor(class Actor* actor);
	// ���� ����
	void RemoveActor(class Actor* actor);
	// � �߰�
	//void AddAsteroid(class Asteroid* as);
	// � ����
	//void RemoveAsteroid(class Asteroid* as);
	// � �ҷ�����
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

	// ������ UI������ ��ȯ
	const std::vector<class UIScreen*>& GetUIStack();
	// Ư�� UIScreen�� ���ÿ� �߰��Ѵ�
	void PushUI(class UIScreen* screen);

private:
	// ���� ������ ���� ���� �Լ�
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	
	// ��� ���͸� ����/�����ϴ� ����
	void LoadData();
	void UnloadData();

	
	void HandleKeyPress(int key);

	//-----------------------------------------------------------------------------------------------------------------------------


	// ������ ��� ����ž� �ϴ����� �Ǵ�
	bool mIsRunning;


	// DeltaTime�� ����ϱ� ���� ���� = Init���� ������ �����ϰ� ���� ƽ ��
	Uint32 mTicksCount;

	// ���� ����
	GameState mGameState;

	// ȭ���� �׸��� ���� ������
	class Renderer* mRenderer;

	// �Ҹ� ����� ���� ����� �ý���
	class AudioSystem* mAudioSystem;

	// �Է� ó���� ���� InputSystem
	class InputSystem* mInputSystem;

	// ���� ���踦 ���� ������
	class PhysWorld* mPhysWorld;

	/*
	* ���͸� ���� ������ �����ϱ� ���� �����
	* 1. Ȱ��ȭ�� ���� / 2. ��� ����   �̷��� �� ���͸� �����
	*
	* ���͸� �ݺ��ϴ� ����(mActors�� �ݺ��ϴ� ����) �� ���͸� �����ϴ� ��츦 �ٷ���� ��� ���͵��� ���� ���Ͱ� �ʿ��ϴ�
	* �� ��� mPendingActors�� ��Ҹ� �߰��� ��, mActors�� �ݺ��� ������ �׶� ��� ���� ���͵��� mActors�� �̵���Ų��
	*/
	// Ȱ��ȭ�� ����
	std::vector<class Actor*> mActors;
	// ������� ����
	std::vector<class Actor*> mPendingActors;

	// ���Ͱ� ������(update)���� Ȯ���ϴ� ����
	bool mUpdatingActors;

	// ��Ʈ�� �����ϴ� ��
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

