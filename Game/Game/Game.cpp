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
	//int sdlResult = SDL_Init(SDL_INIT_VIDEO); // SDL_Init�� �ʱ�ȭ �Ϸ��� ��� ����ý��۵��� ��ƮOR ���갪�� �Ķ���ͷ� �޴´�. ����� ������ �ϸ� ��
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0) // 0�� �ƴϸ� �ʱ�ȭ ����! => SDL ���̴� ���� ���� �Ұ�(ȭ���� �Ⱥ���)
	{
		SDL_Log("Unable to initialize SDL : %s", SDL_GetError()); // SDL_GetError() �� c��Ÿ���� ���ڿ��� ��ȯ�Ѵ�
		return false;


	}
	else
	{
		SDL_Log("TEST # SDL Initialize complete");
	}
	
	// ��Ʈ �������� ���� SDL TTF �ʱ�ȭ
	if (!TTF_Init())
	{
		SDL_Log("Unable to initialize TTF : %s", SDL_GetError());
	}
	else
	{
		SDL_Log("TEST # SDL TTF Initialize Complete");
	}

	// Renderer ����
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

	// AudioSystem ����
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

	// InpuySystem ����
	mInputSystem = new InputSystem();
	if (!mInputSystem->Initialize())
	{
		SDL_Log("Failed to initialize Input System");
		return false;
	}
	mInputSystem->SetRelativeMouseMode(true);

	// ���ο� �������踦 ����
	mPhysWorld = new PhysWorld(this);

	// ��� ���͵��� ���ӿ� �����Ѵ�
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
	
	// SDL TTF ����
	TTF_Quit();

	// �׸��� SDL_Quit�� SDL�� �ݴ´�
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
	* �÷��̾ �����츦 �̵���Ű�ų� ũ�⸦ �����ϴ� ���� "�׼�"�� �� ���� �ִ�
	* �̷� �׼��� ǥ���ϴ� �Ϲ����� ����� "�̺�Ʈ"�� ����ϴ� ���̴�.
	* ������ ������ �ϸ� ���α׷��� �ü���κ��� �̺�Ʈ�� �ް� �� �̺�Ʈ�� �����ϴ� ����� �����ϴ� ���̴�
	*
	* SDL�� �ü���κ��� ���� �̺�Ʈ�� ������ Queue�� �����Ѵ�. �̴� "�Է���ġ�� �Է�" + "���� ������ ������ �׼� �̺�Ʈ"���� �����Ѵ�
	* �� �����Ӹ��� ������ ť�� �̺�Ʈ�� �ִ��� Polling�ϰ�, �̺�Ʈ�� �������� ó������ �����Ѵ�
	*
	* �̺�Ʈ�� �Է� Ÿ���̹Ƿ� �� �Լ����� ó���ϴ� ���� ��ġ�� �´�
	* Event Queue�� Ư�� �����ӿ��� ���� �̺�Ʈ�� ������ �� �����Ƿ� ť�� �ִ� ��� �̺�Ʈ�� ��ȸ�ϸ鼭 �ٷ�� �Ѵ�
	*/

	// SDL_Event �����ʹ� ť���� �� ���ŵ� �̺�Ʈ�� ���� ������ �����Ѵ�
	SDL_Event event;

	// InpuySystem�� SDL_PollEvent ������ ȣ���ϴ� �Լ���, PrepareForUpdate()�� ȣ��
	mInputSystem->PrepareForUpdate();

	// SDL_PollEvent�� �̺�Ʈ�� �߰��ϸ� true�� �����Ѵ�
	while (SDL_PollEvent(&event))
	{	
		// event�� type ��� ������ �̺�Ʈ�� Ÿ�� ������ �����Ѵ� => �̸� �̿��� switch�� �̺�Ʈ�� ó���Ѵ�
		switch (event.type)
		{
		case SDL_QUIT:	// ������ ���콺�� X�� ������ �����츦 ���� ���� �̺�Ʈ
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

	// InputSystem�� SDL_PollEvent ���Ŀ� ȣ���ϴ� �Լ���, Update()�� ȣ��
	mInputSystem->Update();
	const InputState& state = mInputSystem->GetState();
	int x, y;
	Uint32 mousestate = SDL_GetMouseState(&x, &y);
	const Uint8* keyboardstate = SDL_GetKeyboardState(NULL);
	/*
	// SDL_GetKeyboardState(NULL)�� Ű������ ���� ���¸� �����ϴ� "�迭�� ������"�� �����Ѵ�
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// esc�� ������ ������ ����ǰ� ����
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
	* ��ǥ �������� 60fps(1�ʿ� ���� ����(������)�� 60�� �ݺ��ȴ�) �� ��, ��ǥ ��Ÿ �ð�(�����Ӱ� ������ ������ �ð�)�� 16.6ms (1/60s = 16.6ms)�̴�
	* �׷��� ���� �������� �� 15ms���� �Ϸ�Ǹ�? 1.6ms�� �߰������� ��ٸ��� �����ؾ� �Ѵ�. �̸� "������ ����(Frame Limit)"��� �Ѵ�
	*
	* SDL�� �� ����� �����Ѵ�. ������ �����Ӱ� 16ms(60fps)�� ������� �����ϰ� ���ش�
	*/
	// ������ ������ ���ķ� 16ms�� ����� ������ ���
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	//-----------------------------------------------------------------------------------------------------------------------------

	/* 1
	* ���� �ϵ������ ������ ����Ʈ�� ������� ������ ����ٸ� ��� �ɱ� (�� �����Ӹ��� enemy.x += 5�ȼ�)
	* 8MHZ ���μ������� ���ϴ� �ӵ��� �����δٸ�, 32MHZ ���μ���������? �ӵ��� 4�谡 �ȴ�!!!!
	*
	* �̸� �����ϱ� ���ؼ� ������ ""Delta Time = ������ ������ ���ķ� ����� "���� �ð�""" �� ����Ѵ�
	* ��, �����Ӹ��� �ȼ� �̵��� �����ϴ� ���� �ϴ϶�, �ʴ� �ȼ� �̵��� �����Ѵ�
	* �ڼ��� ������ p.61 ����
	*/

	// Delta Time = ���� ������ ƽ�� - ���� ������ ƽ��
	// SDL_GetTicks()�� ���� SDL_Init() ���ķ� ����� �и��ʸ� ���� �� �ִ�
	// (�и��� ������ ������ ����� �� ������ ��ȯ)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	// ���� �������� ���� ƽ���� ����
	//mTicksCount = SDL_GetTicks();
	

	//-----------------------------------------------------------------------------------------------------------------------------

	/* 3
	* ����� �߿� �극��ũ ����Ʈ�� 5�ʵ��� ���μ����� ������Ű��? => �ſ� ū ��Ÿ �ð��� ���� �ȴ� => ����ġ�� �ʰ��� ���̴�
	* ��, �ʹ� ū ��Ÿ �ð��� �����ؾ� �� �ʿ䰡 �ִ�
	*/
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	// ���� �������� ���� ƽ���� ����
	mTicksCount = SDL_GetTicks();

	//-----------------------------------------------------------------------------------------------------------------------------

	if (mGameState == GameState::EGamePlay)
	{
		// ���� ����
		mUpdatingActors = true;
		for (auto actor : mActors)
		{
			actor->Update(deltaTime);
		}
		mUpdatingActors = false;


		// ��� ���� ���͵��� mActors�� �̵�
		for (auto pending : mPendingActors)
		{
			pending->ComputeWorldTransform();
			mActors.emplace_back(pending);
		}
		mPendingActors.clear();

		// ���� ���͵��� �ӽ� ���Ϳ� �߰�
		std::vector<Actor*> deadActors;
		for (auto actor : mActors)
		{
			if (actor->GetState() == Actor::EDead)
			{
				deadActors.emplace_back(actor);
			}
		}

		// ���� ���� ����
		for (auto actor : deadActors)
		{
			delete actor;
		}
	}
	// ��� ���͵��� ���� ��, UIScreen�� ������ �߻��Ѵ�
		// UIScreen Stack�� �ݺ��ϸ鼭 Ȱ��ȭ�� UIScreen Update �Լ��� ȣ��
	for (auto ui : mUIStack)
	{
		if (ui->GetState() == UIScreen::EActive)
		{
			ui->Update(deltaTime);
		}
	}
	// UIScreen ���� ��, ���� ��ũ���� �ִٸ� �����Ѵ�
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
	// ���͸� ���� ���̶�� ��� ���Ϳ� ���͸� �߰�
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
	// ���Ͱ� ��� �ִ��� �˾ƺ��� ���� iter
	auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);

	if (iter != mPendingActors.end())
	{
		// iter �κ��� ��Ҹ� ������ ������ �ű��, pop off �Ѵ�
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
	// UIScreen(HUD) ����
	mHUD = new HUD(this);

	// ���� ����
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

	// ��� ����
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
	q = Quaternion(Vector3::UnitX, Math::PiOver2);	// x���� �������� 90�� ȸ��. ���⼭ x���� ī�޶� �������� �� �޼���ǥ��. �ʱ� floor mesh�� �ٴڿ� ���� ������
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

	// ī�޶� ���� ����
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

	
	// ����� ������Ʈ�� ���� sphere ����
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
	// ~Actor���� RemoveActor�� ȣ���ϹǷ� �ٸ� ������ �ݺ����� �����Ѵ�
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