#include "Game.h"
#include <SDL_image.h>
#include <algorithm>
#include "Actor.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "CameraActor.h"
#include "PlaneActor.h"
#include "AudioSystem.h"
#include "AudioComponent.h"
#include "InputSystem.h"
#include "FPSActor.h"
#include "FollowActor.h"
#include "OrbitActor.h"
#include "SplineActor.h"
#include "TargetActor.h"
#include "PhysWorld.h"
#include "Font.h"
#include "UIScreen.h"
#include "HUD.h"
#include "PauseMenu.h"

const int thickness = 15;

Game::Game()
	: mIsRunning(true)
	, mUpdatingActors(false)
	, mTicksCount(0)
	, mRenderer(nullptr)
	, mAudioSystem(nullptr)
	, mGameState(GameState::EGamePlay)
{}

bool Game::Initialize()
{
	//int sdlResult = SDL_Init(SDL_INIT_VIDEO); // SDL_Init은 초기화 하려는 모든 서브시스템들의 비트OR 연산값을 파라미터로 받는다. 현재는 비디오만 하면 됨
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0) // 0이 아니면 초기화 실패! => SDL 없이는 게임 진행 불가(화면이 안보임)
	{
		SDL_Log("Unable to initialize SDL : %s", SDL_GetError()); // SDL_GetError() 는 c스타일의 문자열을 반환한다
		return false;


	}
	else
	{
		SDL_Log("TEST # SDL Initialize complete");
	}
	
	// 폰트 렌더링을 위한 SDL TTF 초기화
	if (!TTF_Init())
	{
		SDL_Log("Unable to initialize TTF : %s", SDL_GetError());
	}
	else
	{
		SDL_Log("TEST # SDL TTF Initialize Complete");
	}

	// Renderer 생성
	mRenderer = new Renderer(this);
	if (!mRenderer->Initialize(1024.0f, 768.0f))
	{
		SDL_Log("Failed to initialize renderer");
		delete mRenderer;
		mRenderer = nullptr;
		return false;
	}
	else
	{
		SDL_Log("TEST # Renderer Created # Game.cpp 49");
	}

	// AudioSystem 생성
	mAudioSystem = new AudioSystem(this);
	if (!mAudioSystem->Initialize())
	{
		SDL_Log("Failed to initialize AudioSystem");
		mAudioSystem->ShutDown();
		delete mAudioSystem;
		mAudioSystem = nullptr;
		return false;
	}
	else
	{
		SDL_Log("TEST # AudioSystem Created # Game.cpp 64");
	}

	// InpuySystem 생성
	mInputSystem = new InputSystem();
	if (!mInputSystem->Initialize())
	{
		SDL_Log("Failed to initialize Input System");
		return false;
	}
	mInputSystem->SetRelativeMouseMode(true);

	// 새로운 물리세계를 생성
	mPhysWorld = new PhysWorld(this);

	// 모든 액터들을 게임에 생성한다
	LoadData();

	mTicksCount = SDL_GetTicks();
	
	return true;
}

void Game::ShutDown()
{
	UnloadData();
	//IMG_Quit();
	if (mRenderer)
	{
		mRenderer->Shutdown();
	}
	if (mAudioSystem)
	{
		mAudioSystem->ShutDown();
	}
	if (mInputSystem)
	{
		mInputSystem->ShutDown();
	}

	delete mRenderer;
	delete mAudioSystem;
	delete mInputSystem;
	
	// SDL TTF 끄기
	TTF_Quit();

	// 그리고 SDL_Quit로 SDL을 닫는다
	SDL_Quit();
}

void Game::RunLoop()
{
	while (mGameState != GameState::EQuit)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	/* 1
	* 플레이어가 윈도우를 이동시키거나 크기를 조정하는 등의 "액션"을 할 수가 있다
	* 이런 액션을 표현하는 일반적인 방법은 "이벤트"를 사용하는 것이다.
	* 유저가 뭔가를 하면 프로그램은 운영체제로부터 이벤트를 받고 이 이벤트에 대응하는 방법을 선택하는 것이다
	*
	* SDL은 운영체제로부터 받은 이벤트를 내부의 Queue로 관리한다. 이는 "입력장치의 입력" + "여러 유형의 윈도우 액션 이벤트"까지 포함한다
	* 매 프레임마다 게임은 큐에 이벤트가 있는지 Polling하고, 이벤트를 무시할지 처리할지 결정한다
	*
	* 이벤트는 입력 타입이므로 이 함수에서 처리하는 것이 이치에 맞다
	* Event Queue는 특정 프레임에서 여러 이벤트를 포함할 수 있으므로 큐에 있는 모든 이벤트를 조회하면서 다뤄야 한다
	*/

	// SDL_Event 포인터는 큐에서 막 제거된 이벤트에 관한 정보를 저장한다
	SDL_Event event;

	// InpuySystem의 SDL_PollEvent 이전에 호출하는 함수인, PrepareForUpdate()를 호출
	mInputSystem->PrepareForUpdate();

	// SDL_PollEvent는 이벤트를 발견하면 true를 리턴한다
	while (SDL_PollEvent(&event))
	{	
		// event의 type 멤버 변수는 이벤트의 타입 정보를 포함한다 => 이를 이용해 switch로 이벤트를 처리한다
		switch (event.type)
		{
		case SDL_QUIT:	// 유저가 마우스로 X를 눌러서 윈도우를 닫을 때의 이벤트
			mGameState = GameState::EQuit;
			break;
		case SDL_KEYDOWN:	// This fires when a key's initially pressed
			if (!event.key.repeat)
			{
				if (mGameState == GameState::EGamePlay)
				{
					HandleKeyPress(event.key.keysym.sym);
				}
				else if (!mUIStack.empty())
				{
					mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
				}
				
			}
			break;
		case SDL_MOUSEWHEEL:
			mInputSystem->ProcessEvent(event);
		case SDL_MOUSEBUTTONDOWN:
			if (mGameState == GameState::EGamePlay)
			{
				HandleKeyPress(event.button.button);
			}
			else
			{
				if (!mUIStack.empty())
				{
					mUIStack.back()->HandleKeyPress(event.button.button);
				}
			}
		default:
			break;
		}
	}

	// InputSystem의 SDL_PollEvent 이후에 호출하는 함수인, Update()를 호출
	mInputSystem->Update();
	const InputState& state = mInputSystem->GetState();
	int x, y;
	Uint32 mousestate = SDL_GetMouseState(&x, &y);
	const Uint8* keyboardstate = SDL_GetKeyboardState(NULL);
	/*
	// SDL_GetKeyboardState(NULL)은 키보드의 현재 상태를 포함하는 "배열의 포인터"를 리턴한다
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// esc를 누르면 게임이 종료되게 하자
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	*/


	if (state.Keyboard.GetKeyState(SDL_SCANCODE_ESCAPE) == EReleased)
	{
		mIsRunning = false;
	}

	if (mGameState == GameState::EGamePlay)
	{
		mUpdatingActors = true;
		for (auto actor : mActors)
		{
			if (actor->GetState() == Actor::EActive)
			{
				actor->ProcessInput(state);
			}
		}
		mUpdatingActors = false;
	}
	else
	{
		if (!mUIStack.empty())
		{
			mUIStack.back()->ProcessInput(keyboardstate);
		}
	}
	
	
	//-----------------------------------------------------------------------------------------------------------------------------

}

void Game::UpdateGame()
{
	/* 2
	* 목표 프레임이 60fps(1초에 게임 루프(프레임)가 60번 반복된다) 일 때, 목표 델타 시간(프레임과 프레임 사이의 시간)은 16.6ms (1/60s = 16.6ms)이다
	* 그런데 만약 프레임이 단 15ms만에 완료되면? 1.6ms를 추가적으로 기다리게 구현해야 한다. 이를 "프레임 제한(Frame Limit)"라고 한다
	*
	* SDL은 이 방법을 제공한다. 다음은 프레임간 16ms(60fps)가 경과함을 보장하게 해준다
	*/
	// 마지막 프레임 이후로 16ms가 경과할 때까지 대기
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	//-----------------------------------------------------------------------------------------------------------------------------

	/* 1
	* 만약 하드웨어의 프레임 레이트를 기반으로 게임을 만든다면 어떻게 될까 (매 프레임마다 enemy.x += 5픽셀)
	* 8MHZ 프로세서에서 원하는 속도로 움직인다면, 32MHZ 프로세서에서는? 속도가 4배가 된다!!!!
	*
	* 이를 방지하기 위해서 게임은 ""Delta Time = 마지막 프레임 이후로 경과된 "게임 시간""" 을 사용한다
	* 즉, 프레임마다 픽셀 이동을 생각하는 것이 하니라, 초당 픽셀 이동을 생각한다
	* 자세한 내용은 p.61 참고
	*/

	// Delta Time = 현재 프레임 틱값 - 이전 프래임 틱값
	// SDL_GetTicks()를 통해 SDL_Init() 이후로 경과된 밀리초를 얻을 수 있다
	// (밀리초 단위인 빼기의 결과를 초 단위로 변환)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	// 다음 프레임을 위해 틱값을 갱신
	//mTicksCount = SDL_GetTicks();
	

	//-----------------------------------------------------------------------------------------------------------------------------

	/* 3
	* 디버거 중에 브레이크 포인트로 5초동안 프로세스를 정지시키면? => 매우 큰 델타 시간을 갖게 된다 => 예상치를 초과할 것이다
	* 즉, 너무 큰 델타 시간에 주의해야 할 필요가 있다
	*/
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	// 다음 프레임을 위해 틱값을 갱신
	mTicksCount = SDL_GetTicks();

	//-----------------------------------------------------------------------------------------------------------------------------

	if (mGameState == GameState::EGamePlay)
	{
		// 액터 갱신
		mUpdatingActors = true;
		for (auto actor : mActors)
		{
			actor->Update(deltaTime);
		}
		mUpdatingActors = false;


		// 대기 중인 액터들을 mActors로 이동
		for (auto pending : mPendingActors)
		{
			pending->ComputeWorldTransform();
			mActors.emplace_back(pending);
		}
		mPendingActors.clear();

		// 죽은 액터들을 임시 벡터에 추가
		std::vector<Actor*> deadActors;
		for (auto actor : mActors)
		{
			if (actor->GetState() == Actor::EDead)
			{
				deadActors.emplace_back(actor);
			}
		}

		// 죽은 액터 제거
		for (auto actor : deadActors)
		{
			delete actor;
		}
	}
	// 모든 액터들을 갱신 후, UIScreen의 갱신이 발생한다
		// UIScreen Stack을 반복하면서 활성화된 UIScreen Update 함수를 호출
	for (auto ui : mUIStack)
	{
		if (ui->GetState() == UIScreen::EActive)
		{
			ui->Update(deltaTime);
		}
	}
	// UIScreen 갱신 후, 죽은 스크린이 있다면 삭제한다
	auto iter = mUIStack.begin();
	while (iter != mUIStack.end())
	{
		if ((*iter)->GetState() == UIScreen::EClosing)
		{
			delete* iter;
			iter = mUIStack.erase(iter);
		}
		else
		{
			++iter;
		}

	}


	mRenderer->Update(deltaTime);
	mAudioSystem->Update(deltaTime);
	mInputSystem->Update();
}

void Game::GenerateOutput()
{
	if (mRenderer)
	{
		mRenderer->Draw();
	}
	else
	{
		SDL_Log("Renderer is nullptr");
	}
}

void Game::AddActor(Actor* actor)
{
	// 액터를 갱신 중이라면 대기 벡터에 액터를 추가
	if (mUpdatingActors)
	{
		mPendingActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor)
{
	// 액터가 어디에 있는지 알아보기 위한 iter
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);

	if (iter != mPendingActors.end())
	{
		// iter 부분의 요소를 벡터의 끝으로 옮기고, pop off 한다
		std::iter_swap(iter, mPendingActors.end() - 1);
		mPendingActors.pop_back();
	}

	iter = std::find(mActors.begin(), mActors.end(), actor);
	
	if (iter != mActors.end())
	{
		std::iter_swap(iter, mActors.end() - 1);
		mActors.pop_back();
	}
}

void Game::AddPlane(PlaneActor* plane)
{
	mPlaneActors.emplace_back(plane);
}
void Game::RemovePlane(PlaneActor* plane)
{
	auto iter = std::find(mPlaneActors.begin(), mPlaneActors.end(), plane);
	mPlaneActors.erase(iter);
}

void Game::LoadData()
{
	// UIScreen(HUD) 생성
	mHUD = new HUD(this);

	// 엑터 생성
	Actor* a = new Actor(this);
	a->SetPosition(Vector3(200.0f, 75.0f, 0.0f));
	a->SetScale(100.0f);
	Quaternion q(Vector3::UnitY, -Math::PiOver2);
	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::Pi + Math::Pi / 4.0f));
	a->SetRotation(q);
	MeshComponent* mc = new MeshComponent(a);
	mc->SetMesh(mRenderer->GetMesh("Assets/Cube.gpmesh"));

	a = new Actor(this);
	a->SetPosition(Vector3(200.0f, -75.0f, 0.0f));
	a->SetScale(3.0f);
	mc = new MeshComponent(a);
	mc->SetMesh(mRenderer->GetMesh("Assets/Sphere.gpmesh"));

	// 배경 설정
	// Setup floor
	const float start = -1250.0f;
	const float size = 250.0f;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			a = new PlaneActor(this);
			a->SetPosition(Vector3(start + i * size, start + j * size, -100.0f));
		}
	}

	// Left/right walls
	q = Quaternion(Vector3::UnitX, Math::PiOver2);	// x축을 기준으로 90도 회전. 여기서 x축은 카메라 기준으로 한 왼손좌표계. 초기 floor mesh는 바닥에 누운 상태임
	for (int i = 0; i < 10; i++)
	{
		a = new PlaneActor(this);
		a->SetPosition(Vector3(start + i * size, start - size, 0.0f));
		a->SetRotation(q);

		a = new PlaneActor(this);
		a->SetPosition(Vector3(start + i * size, -start + size, 0.0f));
		a->SetRotation(q);
	}

	q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::PiOver2));
	// Forward/back walls
	for (int i = 0; i < 10; i++)
	{
		a = new PlaneActor(this);
		a->SetPosition(Vector3(start - size, start + i * size, 0.0f));
		a->SetRotation(q);

		a = new PlaneActor(this);
		a->SetPosition(Vector3(-start + size, start + i * size, 0.0f));
		a->SetRotation(q);
	}

	// Setup lights
	mRenderer->SetAmbientColor(Vector3(0.2f, 0.2f, 0.2f));
	DirectionalLight& dir = mRenderer->GetDirectionalLight();
	dir.mDirection = Vector3(0.0f, -0.707f, -0.707f);
	dir.mDiffuseColor = Vector3(0.78f, 0.88f, 1.0f);
	dir.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);
	/*
	PointLight& point1 = mRenderer->GetPointLight(0);
	PointLight& point2 = mRenderer->GetPointLight(1);
	PointLight& point3 = mRenderer->GetPointLight(2);
	PointLight& point4 = mRenderer->GetPointLight(3);
	point1.mPosition = Vector3(200.0f, 100.0f, 50.0f);
	point1.mDiffuseColor = Vector3(1.0f, 0.0f, 0.0f);
	point1.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);
	point1.mRadius = 200.0f;
	point1.mSpecPower = 10.0f;
	point2.mPosition = Vector3(200.0f, -100.0f, 50.0f);
	point2.mDiffuseColor = Vector3(0.0f, 1.0f, 0.0f);
	point2.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);
	point2.mRadius = 200.0f;
	point2.mSpecPower = 10.0f;
	point3.mPosition = Vector3(-200.0f, -100.0f, 50.0f);
	point3.mDiffuseColor = Vector3(0.0f, 0.0f, 1.0f);
	point3.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);
	point3.mRadius = 200.0f;
	point3.mSpecPower = 10.0f;
	point4.mPosition = Vector3(-200.0f, 100.0f, 50.0f);
	point4.mDiffuseColor = Vector3(1.0f, 1.0f, 1.0f);
	point4.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);
	point4.mRadius = 200.0f;
	point4.mSpecPower = 10.0f; 
	*/

	// 카메라 엑서 생성
	//mCameraActor = new CameraActor(this);
	mFPSActor = new FPSActor(this);
	//mFollowActor = new FollowActor(this);
	//mOrbitActor = new OrbitActor(this);
	//mSplineActor = new SplineActor(this);
	
	// UI elements
	a = new Actor(this);
	a->SetPosition(Vector3(-350.0f, -350.0f, 0.0f));
	SpriteComponent* sc = new SpriteComponent(a);
	sc->SetTexture(mRenderer->GetTexture("Assets/HealthBar.png"));

	a = new Actor(this);
	a->SetPosition(Vector3(-390.0f, 275.0f, 0.0f));
	a->SetScale(0.75f);
	sc = new SpriteComponent(a);
	sc->SetTexture(mRenderer->GetTexture("Assets/Radar.png"));

	a = new Actor(this);
	a->SetScale(2.0f);
	mCrosshair = new SpriteComponent(a);
	mCrosshair->SetTexture(mRenderer->GetTexture("Assets/Crosshair.png"));

	
	// 오디오 컴포넌트를 가진 sphere 생성
	a = new Actor(this);
	a->SetPosition(Vector3(500.0f, -75.0f, 0.0f));
	a->SetScale(1.0f);
	mc = new MeshComponent(a);
	mc->SetMesh(mRenderer->GetMesh("Assets/Sphere.gpmesh"));
	AudioComponent* ac = new AudioComponent(a);
	ac->PlayEvent("event:/FireLoop");

	// Start music
	mMusicEvent = mAudioSystem->PlayEvent("event:/Music");

	// Create target actors
	a = new TargetActor(this);
	a->SetPosition(Vector3(1450.0f, 0.0f, 100.0f));
	a = new TargetActor(this);
	a->SetPosition(Vector3(1450.0f, 0.0f, 400.0f));
	a = new TargetActor(this);
	a->SetPosition(Vector3(1450.0f, -500.0f, 200.0f));
	a = new TargetActor(this);
	a->SetPosition(Vector3(1450.0f, 500.0f, 200.0f));

}

void Game::UnloadData()
{
	// ~Actor에서 RemoveActor를 호출하므로 다른 형태의 반복문을 구현한다
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	if (mRenderer)
	{
		mRenderer->UnloadData();
	}
}



/*
void Game::AddAsteroid(Asteroid* as)
{
	mAsteroids.emplace_back(as);
}

void Game::RemoveAsteroid(Asteroid* as)
{
	auto iter = std::find(mAsteroids.begin(), mAsteroids.end(), as);
	if (iter != mAsteroids.end())
	{
		mAsteroids.erase(iter);
	}
}

std::vector<Asteroid*> Game::GetAsteroids()
{
	return mAsteroids;
}
*/

void Game::HandleKeyPress(int key)
{
	switch (key)
	{
	case '-':
	{
		// Reduce master volume
		float volume = mAudioSystem->GetBusVolume("bus:/");
		volume = Math::Max(0.0f, volume - 0.1f);
		mAudioSystem->SetBusVolume("bus:/", volume);
		break;
	}
	case '=':
	{
		// Increase master volume
		float volume = mAudioSystem->GetBusVolume("bus:/");
		volume = Math::Min(1.0f, volume + 0.1f);
		mAudioSystem->SetBusVolume("bus:/", volume);
		break;
	}
	case 'e':
		// Play explosion
		mAudioSystem->PlayEvent("event:/Explosion2D");
		break;
	case 'm':
		// Toggle music pause state
		mMusicEvent.SetPaused(!mMusicEvent.GetPaused());
		break;
	case 'r':
		// Stop or start reverb snapshot
		if (!mReverbSnap.IsValid())
		{
			mReverbSnap = mAudioSystem->PlayEvent("snapshot:/WithReverb");
		}
		else
		{
			mReverbSnap.Stop();
		}
		break;
	case '1':
		// Set default footstep surface
		mFPSActor->SetFootstepSurface(0.0f);
		break;
	case '2':
		// Set grass footstep surface
		mFPSActor->SetFootstepSurface(0.5f);
		break;
	case SDL_BUTTON_LEFT:
		mFPSActor->Shoot();
		break;
	case 'p':
		new PauseMenu(this);
		break;
	case SDLK_ESCAPE:
		mGameState = GameState::EQuit;
	default:
		break;
	}
}

Font* Game::GetFont(const std::string& fileName)
{
	auto iter = mText.find(fileName);

	if (iter == mText.end())
	{
		Font* font = new Font(this);
		if (font->Load(fileName))
		{
			mText.emplace(fileName, font);
		}
		else
		{
			font->Unload();
			delete font;
			font = nullptr;
		}
		return font;
	}
	else
	{
		return iter->second;
	}
}

const std::vector<UIScreen*>& Game::GetUIStack()
{
	return mUIStack;
}

void Game::PushUI(UIScreen* screen)
{
	mUIStack.emplace_back(screen);
}