#include "Game.h"
#include <SDL_image.h>
#include <algorithm>
#include <glew.h>
#include <wglew.h>
#include "Actor.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "Asteroid.h"

const int thickness = 15;

Game::Game() 
	: mIsRunning(true)
	, mUpdatingActors(false)
	, mWindow(nullptr)
	, mRenderer(nullptr)
	, mTicksCount(0)
{}

bool Game::Initialize()
{
	//int sdlResult = SDL_Init(SDL_INIT_VIDEO); // SDL_Init�� �ʱ�ȭ �Ϸ��� ��� ����ý��۵��� ��ƮOR ���갪�� �Ķ���ͷ� �޴´�. ����� ������ �ϸ� ��

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) // 0�� �ƴϸ� �ʱ�ȭ ����! => SDL ���̴� ���� ���� �Ұ�(ȭ���� �Ⱥ���)
	{
		SDL_Log("Unable to initialize SDL : %s", SDL_GetError()); // SDL_GetError() �� c��Ÿ���� ���ڿ��� ��ȯ�Ѵ�
		return false;
	}


	// �ʱ�ȭ�� ���������� SDL_CreateWindow�� ȭ���� ��� ���ʴ�

	// OpenGl ������ ���� ������, OpenGL�� ����, ���� ���� ���� �Ķ���� �Ӽ� ������ ���� SDL_GL_SetAttribute�� ȣ���Ѵ�
	// ȣ���� �����ϸ� 0��ȯ, �����ϸ� ���� ��ȯ
	// �ھ�, ȣȯ��, ES �� ����ũž�� ���� ������ �ھ� OpenGL �������� ���
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// 3.3�������� ����
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// RGBA ä�θ��� 8��Ʈ ũ���� ���� ���� ��û
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	// ���� ���۸� Ȱ��ȭ
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// OpenGL�� �ϵ���� ������ ����ϵ��� ����
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// ������ ����, �������� x,y ��ǥ(ȭ�� ������� ����), �������� ���̿� ����, ������ �������� �÷���
	mWindow = SDL_CreateWindow("Game Programming in C++ (CH1)", 100, 100, 1024, 768, SDL_WINDOW_OPENGL);
	if (!mWindow)
	{
		SDL_Log("Failed to create window : %s", SDL_GetError());
		return false;
	}

	// ���� OpenGL�� context�� �����. 
	// context�� OpenGL�� �ν��ϴ� ��� ����, ������Ʈ�� �����ϴ� OpenGL�� ������ �����ϸ� �ȴ� (���� ����, �ε�� �̹����� ��, �پ��� OpenGL ������Ʈ���� �����Ѵ�)
	// ������ �ϳ��� OpenGL���α׷��� �ټ��� context�� ������ �� ������, �� å������ �ϳ��� ����Ѵ�
	mContext = SDL_GL_CreateContext(mWindow);	// �� �ڵ�� OpenGL context�� �����쿡 �����Ѵ�

	// OpenGL�� Ȯ�� �ý��۰��� ���� ȣȯ���� ����������, ���ϴ� Ȯ�� ����� �������� ��û�ؾ� �Ѵ�
	// �̸� ����ȭ�ϱ� ���� GLEW ���� �ҽ� ���̺귯���� ����Ѵ�
	// GLEW�� �ڵ������� ���� OpenGL Context �������� �����ϴ� ��� Ȯ�� �Լ��� �ʱ�ȭ�Ѵ�
	// GLEW �ʱ�ȭ
	glewExperimental = GL_TRUE;	// �Ϻ� �÷������� �ھ� context�� ����� �� �߻��� ���� �ִ� �ʱ�ȭ ������ �����Ѵ�
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Failed to initialize GLEW");
		return false;
	}
	// �Ϻ� �÷������� GLEW�� ���� �ڵ带 �������Ƿ�, �������� ��������
	glGetError();


	// Renderer �ʱ�ȭ
	mRenderer = SDL_CreateRenderer(mWindow,		// �������� ���� ������
								   -1,			// � �׷��� ī�带 �������. ������ ���� �����츦 ����� �� �ǹ̰� ����� ��. �Ϲ������� -1(�⺻ ��ī ���)
								   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC  // �ʱ�ȭ �÷���. ����ȭ ������(�׷��� �ϵ���� Ȱ��) |  ��������ȭ Ȱ��ȭ  => �� �Լ��� ������ �÷�����
	);
	if (mRenderer == nullptr)
	{
		SDL_Log("Failed to create Renderer : %s", SDL_GetError());
		return false;
	}


	// 2D �׷���(�̹���)�� ���� SDL_Image �ʱ�ȭ
	if (IMG_Init(IMG_INIT_PNG) == 0) // PNG�� ����� ����. BMP�� SDL�� SDL_image ���̵� �����ϱ� ������ bmp ���Ÿ� �ʿ� ����. 0�̸� �ʱ�ȭ ����
	{
		SDL_Log("Failed to initialize SDL_Image : %s", SDL_GetError());
		return false;
	}

	
	// ��� ���͵��� ���ӿ� �����Ѵ�
	LoadData();

	mTicksCount = SDL_GetTicks();
	
	return true;
}

void Game::ShutDown()
{
	// SDL_DestroyWindow�� SDL_Window ��ü�� �ı�
	SDL_DestroyWindow(mWindow);

	// SDL_GL_DeleteContext()�� OpenGL Context �ı�
	SDL_GL_DeleteContext(mContext);

	// SDL_DestroyRenderer�� ������ ��ü �ı�
	SDL_DestroyRenderer(mRenderer);

	UnloadData();

	IMG_Quit();

	// �׸��� SDL_Quit�� SDL�� �ݴ´�
	SDL_Quit();
}

void Game::RunLoop()
{
	while (mIsRunning)
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

	// SDL_PollEvent�� �̺�Ʈ�� �߰��ϸ� true�� �����Ѵ�
	while (SDL_PollEvent(&event))
	{	
		// event�� type ��� ������ �̺�Ʈ�� Ÿ�� ������ �����Ѵ� => �̸� �̿��� switch�� �̺�Ʈ�� ó���Ѵ�
		switch (event.type)
		{
		case SDL_QUIT:	// ������ ���콺�� X�� ������ �����츦 ���� ���� �̺�Ʈ
			mIsRunning = false;
			break;
		default:
			break;
		}
	}

	// SDL_GetKeyboardState(NULL)�� Ű������ ���� ���¸� �����ϴ� "�迭�� ������"�� �����Ѵ�
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// esc�� ������ ������ ����ǰ� ����
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	mUpdatingActors = true;
	for (auto actor : mActors)
	{
		actor->ProcessInput(state);
	}
	mUpdatingActors = false;
	
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

void Game::GenerateOutput()
{
	/* 1
	* �׷��Ƚ������� Color Buffer�� ȭ�� ��� �ȼ��� ���� ������ �����صд� (x,y) �迭
	* ���� ������ "��� ����" �ܰ� ���� �� �����ӿ��� ������ �׷��� ����� "Color Buffer"�� ����
	* 24��Ʈ Color Depth = RGB 8��Ʈ��. ���⿡ ���� 8��Ʈ => �� 32��Ʈ.  1080p Color Buffer = 1920x1080x4byte = 7.9MB
	* "Frame Buffer"�� ��Ȯ�� ���� = ���� ���� + ��Ÿ ����(���� ���� / ���ٽ� ����) �� ����
	*/

	//-----------------------------------------------------------------------------------------------------------------------------

	/* 2
	* ������ �����Ӽӵ��� ȭ���� ������ ���� �ӵ��� �ٸ� �� �߻��ϴ� Screen Tearing�� �ذ��Ϸ��� 2������ ������ �ʿ��ϴ�
	* 1. ���Ӱ� ���÷��̰� �����ϴ� ���� ���۸� 2����(�ĸ�, ����) �ΰ�, ����� ����Ѵ� => Double Buffering
	*		=> ���� ó���� ������ �ĸ� ���۸� ����Ѵ�
	* 2. ���� X���۸� ���÷��̰� ���� �ִµ� ������ Y���ۿ� �� ���� �ٷ� X�� ����Ϸ��� �Ѵٸ�? => ������ ���÷��̰� ����X�� �� �׸� ������ ��ٷ��� �Ѵ�
	*		=> �̸� "���� ����ȭ" / "Vertical Synchronization" / "vsync" �� �Ѵ�
	* 
	* ����� �ֱ� ���÷��̴� Adaptive Refresh Rate(������ ȭ�� ��� ��) ��� ����� ���. 
	* ���÷��̰� ȭ���� ������ �� ���ӿ� �����ϴ� ���� �ƴ϶�, ������ ���÷��̿��� ���� ȭ���� �������� �˷��ְ� �ϴ� ���
	*/

	//-----------------------------------------------------------------------------------------------------------------------------

	/* 3
	* �⺻������ ȭ���� �׸��� ���� ���� �ܰ�� �̷������
	* 1. �ĸ� ���۸� �ܻ����� Ŭ�����Ѵ� (������ ���� ����)
	* 2. ��ü ���� ����� �׸���
	* 3. ���� ���ۿ� �ĸ� ���۸� ��ȯ�Ѵ�
	*/

	// ������ ȸ������ ����  1
	glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
	// ���� ���� �ʱ�ȭ
	glClear(GL_COLOR_BUFFER_BIT);	// GL_COLOR_BUFFER_BIT�� �� �Լ��� ȣ���ϸ�, ���� ���۸� ������ �������� ä���

	// ����� �׸���  2

	// ���۸� �����ؼ� ����� ����Ѵ�  3
	SDL_GL_SwapWindow(mWindow);		// ���� ���ۿ� �ĸ� ���۸� ��ü�Ѵ�

	//SDL_SetRenderDrawColor(mRenderer, 220, 220, 220, 255); // �ĸ� ���۸� ����� ���� �ϳ��� �� ����
	//SDL_RenderClear(mRenderer); // 1
	// ��� ��������Ʈ���� �׸���
	//for (auto sprite : mSprites)	// 2
	//{
	//	sprite->Draw(mRenderer);
	//}
	//SDL_RenderPresent(mRenderer); // 3
	

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

SDL_Texture* Game::GetTexture(const std::string& fileName)
{
	/*
	// ���Ϸκ��� SDL_Surface ������ �̹��� �ε�
	// �����ϸ� SDL_Surface �����͸� ����, �����ϸ� nullptr
	SDL_Surface* IMG_Load(
		const char* file // �̹��� ���� �̸�
		)

	// SDL_CreateTexutreFromSurface�� SDL_Surface�� SDL�� ȭ�鿡 �׸��� �� �ʿ��� ������ SDL_Texture�� ��ȯ���ش�
	// �����ϸ� SDL_Texture ������ ��ȯ, �����ϸ� nullptr
	SDL_Texture* SDL_CreateTextureFromSurface(
	SDL_Renderer * renderer,	// ����� ������
	SDL_Surface * surface	// ��ȯ�� SDL_Surface
		)
	*/

	SDL_Texture* text = nullptr;

	auto itr = mTextures.find(fileName);

	if (itr != mTextures.end())
	{
		text = itr->second;
	}
	else
	{
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file : %s", fileName.c_str());
			return nullptr;
		}
		text = SDL_CreateTextureFromSurface(mRenderer, surf);
		SDL_FreeSurface(surf);
		
		if (!text)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
		}

		mTextures.emplace(fileName.c_str(), text);
	}

	return text;
}

void Game::LoadData()
{
	// �÷��̾��� ship�� �����
	mShip = new Ship(this);
	mShip->SetPosition(Vector2(512.0f, 384.0f));
	mShip->SetScale(1.0f);


	// � �����
	const int numAsteroids = 20;
	for (int i = 0; i < numAsteroids; i++)
	{
		new Asteroid(this);
	}

	/*
	// ����� ���� ���� �����
	Actor* temp = new Actor(this);
	temp->SetPosition(Vector2(512.0f, 384.0f));
	
	// �� ��� �����
	BGSpriteComponent* bg = new BGSpriteComponent(temp);
	bg->SetScreenSize(Vector2(1024.0f, 768.0f));
	std::vector<SDL_Texture*> bgtextures = {
		GetTexture("Assets/Farback01.png"),
		GetTexture("Assets/Farback02.png")
	};
	bg->SetBGTextures(bgtextures);
	bg->SetScrollSpeed(-100.0f);

	// �� ����� ��� �����
	bg = new BGSpriteComponent(temp, 50);
	bg->SetScreenSize(Vector2(1024.0f, 768.0f));
	bgtextures = {
		GetTexture("Assets/Stars.png"),
		GetTexture("Assets/Stars.png")
	};
	bg->SetBGTextures(bgtextures);
	bg->SetScrollSpeed(-200.0f);*/
}

void Game::UnloadData()
{
	// ~Actor���� RemoveActor�� ȣ���ϹǷ� �ٸ� ������ �ݺ����� �����Ѵ�
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	for (auto j : mTextures)
	{
		SDL_DestroyTexture(j.second);
	}
	mTextures.clear();
}

void Game::AddSprite(SpriteComponent* sprite)
{
	int order = sprite->GetDrawOrder();
	auto iter = mSprites.begin();

	for (; iter != mSprites.end(); ++iter)
	{
		if (order < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	mSprites.insert(iter, sprite);
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}