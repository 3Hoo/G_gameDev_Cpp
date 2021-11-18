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
	//int sdlResult = SDL_Init(SDL_INIT_VIDEO); // SDL_Init은 초기화 하려는 모든 서브시스템들의 비트OR 연산값을 파라미터로 받는다. 현재는 비디오만 하면 됨

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) // 0이 아니면 초기화 실패! => SDL 없이는 게임 진행 불가(화면이 안보임)
	{
		SDL_Log("Unable to initialize SDL : %s", SDL_GetError()); // SDL_GetError() 는 c스타일의 문자열을 반환한다
		return false;
	}


	// 초기화에 성공했으면 SDL_CreateWindow로 화면을 띄울 차례다

	// OpenGl 윈도우 생성 이전에, OpenGL의 버전, 색상 깊이 등의 파라미터 속성 설정을 위해 SDL_GL_SetAttribute를 호출한다
	// 호출이 성공하면 0반환, 실패하면 음수 반환
	// 코어, 호환성, ES 중 데스크탑에 가장 적합한 코어 OpenGL 프로파일 사용
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// 3.3버전으로 지정
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// RGBA 채널마다 8비트 크기인 색상 버퍼 요청
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	// 더블 버퍼링 활성화
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// OpenGL이 하드웨서 가속을 사용하도록 강제
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// 윈도우 제목, 윈도우의 x,y 좌표(화면 좌측상단 기준), 윈도우의 높이와 넓이, 윈도우 생성관련 플래그
	mWindow = SDL_CreateWindow("Game Programming in C++ (CH1)", 100, 100, 1024, 768, SDL_WINDOW_OPENGL);
	if (!mWindow)
	{
		SDL_Log("Failed to create window : %s", SDL_GetError());
		return false;
	}

	// 이제 OpenGL의 context를 만든다. 
	// context는 OpenGL이 인식하는 모든 상태, 오브젝트를 포함하는 OpenGL의 세계라고 생각하면 된다 (색상 깊이, 로드된 이미지나 모델, 다양한 OpenGL 오브젝트들을 포함한다)
	// 본래는 하나의 OpenGL프로그램에 다수의 context를 생성할 수 있지만, 이 책에서는 하나만 사용한다
	mContext = SDL_GL_CreateContext(mWindow);	// 이 코드로 OpenGL context를 윈도우에 생성한다

	// OpenGL은 확장 시스템과의 하위 호환성을 지원하지만, 원하는 확장 기능을 수동으로 요청해야 한다
	// 이를 간소화하기 위해 GLEW 오픈 소스 라이브러리를 사용한다
	// GLEW는 자동적으로 현재 OpenGL Context 버전에서 지원하는 모든 확장 함수를 초기화한다
	// GLEW 초기화
	glewExperimental = GL_TRUE;	// 일부 플랫폼에서 코어 context를 사용할 때 발생할 수도 있는 초기화 에러를 방지한다
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Failed to initialize GLEW");
		return false;
	}
	// 일부 플랫폼에서 GLEW는 에러 코드를 내보내므로, 에러값을 제거하자
	glGetError();


	// Renderer 초기화
	mRenderer = SDL_CreateRenderer(mWindow,		// 렌더링을 위한 윈도우
								   -1,			// 어떤 그래픽 카드를 사용할지. 게임이 여러 윈도우를 사용할 때 의미가 생기는 값. 일반적으로 -1(기본 그카 사용)
								   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC  // 초기화 플래그. 가속화 렌더러(그래픽 하드웨어 활용) |  수직동기화 활성화  => 이 함수의 유일한 플래그임
	);
	if (mRenderer == nullptr)
	{
		SDL_Log("Failed to create Renderer : %s", SDL_GetError());
		return false;
	}


	// 2D 그래픽(이미지)를 위한 SDL_Image 초기화
	if (IMG_Init(IMG_INIT_PNG) == 0) // PNG를 사용할 것임. BMP는 SDL이 SDL_image 없이도 지원하기 때문에 bmp 쓸거면 필요 없음. 0이면 초기화 실패
	{
		SDL_Log("Failed to initialize SDL_Image : %s", SDL_GetError());
		return false;
	}

	
	// 모든 액터들을 게임에 생성한다
	LoadData();

	mTicksCount = SDL_GetTicks();
	
	return true;
}

void Game::ShutDown()
{
	// SDL_DestroyWindow로 SDL_Window 객체를 파괴
	SDL_DestroyWindow(mWindow);

	// SDL_GL_DeleteContext()로 OpenGL Context 파괴
	SDL_GL_DeleteContext(mContext);

	// SDL_DestroyRenderer로 렌더러 객체 파괴
	SDL_DestroyRenderer(mRenderer);

	UnloadData();

	IMG_Quit();

	// 그리고 SDL_Quit로 SDL을 닫는다
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

	// SDL_PollEvent는 이벤트를 발견하면 true를 리턴한다
	while (SDL_PollEvent(&event))
	{	
		// event의 type 멤버 변수는 이벤트의 타입 정보를 포함한다 => 이를 이용해 switch로 이벤트를 처리한다
		switch (event.type)
		{
		case SDL_QUIT:	// 유저가 마우스로 X를 눌러서 윈도우를 닫을 때의 이벤트
			mIsRunning = false;
			break;
		default:
			break;
		}
	}

	// SDL_GetKeyboardState(NULL)은 키보드의 현재 상태를 포함하는 "배열의 포인터"를 리턴한다
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// esc를 누르면 게임이 종료되게 하자
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

void Game::GenerateOutput()
{
	/* 1
	* 그래픽스에서는 Color Buffer에 화면 모든 픽셀의 색상 정보를 저장해둔다 (x,y) 배열
	* 게임 루프의 "출력 생성" 단계 동안 매 프레임에서 게임은 그래픽 출력을 "Color Buffer"에 쓴다
	* 24비트 Color Depth = RGB 8비트씩. 여기에 알파 8비트 => 총 32비트.  1080p Color Buffer = 1920x1080x4byte = 7.9MB
	* "Frame Buffer"의 정확한 정의 = 색상 버퍼 + 기타 버퍼(깊이 버퍼 / 스텐실 버퍼) 의 조합
	*/

	//-----------------------------------------------------------------------------------------------------------------------------

	/* 2
	* 게임의 프레임속도와 화면의 프레임 갱신 속도가 다를 때 발생하는 Screen Tearing을 해결하려면 2가지의 변경이 필요하다
	* 1. 게임과 디스플레이가 공유하는 색상 버퍼를 2개로(후면, 전면) 두고, 교대로 사용한다 => Double Buffering
	*		=> 가장 처음에 게임은 후면 버퍼를 사용한다
	* 2. 만약 X버퍼를 디스플레이가 쓰고 있는데 게임이 Y버퍼에 다 쓰고 바로 X를 사용하려고 한다면? => 게임은 디스플레이가 버퍼X를 다 그릴 때까지 기다려야 한다
	*		=> 이를 "수직 동기화" / "Vertical Synchronization" / "vsync" 라 한다
	* 
	* 참고로 최근 디스플레이는 Adaptive Refresh Rate(적응적 화면 재생 빈도) 라는 기술을 사용. 
	* 디스플레이가 화면을 갱신할 때 게임에 통지하는 것이 아니라, 게임이 디스플레이에게 언제 화면을 갱신할지 알려주게 하는 기술
	*/

	//-----------------------------------------------------------------------------------------------------------------------------

	/* 3
	* 기본적으로 화면을 그리는 것은 다음 단계로 이루어진다
	* 1. 후면 버퍼를 단색으로 클리어한다 (게임의 현재 버퍼)
	* 2. 전체 게임 장면을 그린다
	* 3. 전면 버퍼와 후면 버퍼를 교환한다
	*/

	// 색상을 회색으로 설정  1
	glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
	// 색상 버퍼 초기화
	glClear(GL_COLOR_BUFFER_BIT);	// GL_COLOR_BUFFER_BIT로 이 함수를 호출하면, 색상 버퍼를 지정된 색상으로 채운다

	// 장면을 그린다  2

	// 버퍼를 스왑해서 장면을 출력한다  3
	SDL_GL_SwapWindow(mWindow);		// 전면 버퍼와 후면 버퍼를 교체한다

	//SDL_SetRenderDrawColor(mRenderer, 220, 220, 220, 255); // 후면 버퍼를 지우기 위한 하나의 색 지정
	//SDL_RenderClear(mRenderer); // 1
	// 모든 스프라이트들을 그린다
	//for (auto sprite : mSprites)	// 2
	//{
	//	sprite->Draw(mRenderer);
	//}
	//SDL_RenderPresent(mRenderer); // 3
	

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
	// 파일로부터 SDL_Surface 상으로 이미지 로드
	// 성공하면 SDL_Surface 포인터를 리턴, 실패하면 nullptr
	SDL_Surface* IMG_Load(
		const char* file // 이미지 파일 이름
		)

	// SDL_CreateTexutreFromSurface는 SDL_Surface를 SDL이 화면에 그리는 데 필요한 포맷인 SDL_Texture로 변환해준다
	// 성공하면 SDL_Texture 포인터 반환, 실패하면 nullptr
	SDL_Texture* SDL_CreateTextureFromSurface(
	SDL_Renderer * renderer,	// 사용할 렌더러
	SDL_Surface * surface	// 변환될 SDL_Surface
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
	// 플레이어의 ship을 만든다
	mShip = new Ship(this);
	mShip->SetPosition(Vector2(512.0f, 384.0f));
	mShip->SetScale(1.0f);


	// 운석 만들기
	const int numAsteroids = 20;
	for (int i = 0; i < numAsteroids; i++)
	{
		new Asteroid(this);
	}

	/*
	// 배경을 위한 액터 만들기
	Actor* temp = new Actor(this);
	temp->SetPosition(Vector2(512.0f, 384.0f));
	
	// 먼 배경 만들기
	BGSpriteComponent* bg = new BGSpriteComponent(temp);
	bg->SetScreenSize(Vector2(1024.0f, 768.0f));
	std::vector<SDL_Texture*> bgtextures = {
		GetTexture("Assets/Farback01.png"),
		GetTexture("Assets/Farback02.png")
	};
	bg->SetBGTextures(bgtextures);
	bg->SetScrollSpeed(-100.0f);

	// 더 가까운 배경 만들기
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
	// ~Actor에서 RemoveActor를 호출하므로 다른 형태의 반복문을 구현한다
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