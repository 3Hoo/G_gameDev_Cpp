#include "Game.h"
#include "Actor.h"

const int thickness = 15;

Game::Game() 
	: mIsRunning(true),
	mWindow(nullptr),
	mRenderer(nullptr),
	mTicksCount(0),
	mPaddleDir(0),
	mPaddleDir2(0),
	paddleH(thickness * 6.0)
{}

bool Game::Initialize()
{
	int sdlResult = SDL_Init(SDL_INIT_VIDEO); // SDL_Init은 초기화 하려는 모든 서브시스템들의 비트OR 연산값을 파라미터로 받는다. 현재는 비디오만 하면 됨

	if (sdlResult != 0) // 0이 아니면 초기화 실패! => SDL 없이는 게임 진행 불가(화면이 안보임)
	{
		SDL_Log("Unable to initialize SDL : %s", SDL_GetError()); // SDL_GetError() 는 c스타일의 문자열을 반환한다
		return false;
	}


	// 초기화에 성공했으면 SDL_CreateWindow로 화면을 띄울 차례다
	// 윈도우 제목, 윈도우의 x,y 좌표(화면 좌측상단 기준), 윈도우의 높이와 넓이, 윈도우 생성관련 플래그
	mWindow = SDL_CreateWindow("Game Programming in C++ (CH1)", 100, 100, 1024, 768, 0);

	if (!mWindow)
	{
		SDL_Log("Failed to create window : %s", SDL_GetError());
		return false;
	}


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

	// 공 생성
	Balls.push_back(Ball{ 1024.0f / 2.0f, 768.0f / 2.0f, -200.0f, 235.0f });
	Balls.push_back(Ball{ 1024.0f / 4.0f, 768.0f / 5.0f, 200.0f, -235.0f });

	// 패들과 공의 위치 초기화
	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f / 2.0f;
	mPaddlePos2.x = 1014.0f;
	mPaddlePos2.y = 768.0f / 2.0f;

	return true;
}

void Game::ShutDown()
{
	// SDL_DestroyWindow로 SDL_Window 객체를 파괴

	SDL_DestroyWindow(mWindow);

	// SDL_DestroyRenderer로 렌더러 객체 파괴
	SDL_DestroyRenderer(mRenderer);

	// mActors 제거. ~Actor 함수가 RemoveActor를 호출하므로 다른 스타일의 루프를 사용하자
	while (!mActors.empty())
	{
		delete mActors.back();
	}

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
	
	//-----------------------------------------------------------------------------------------------------------------------------

	// mPaddleDir = 패들을 위로 움직이면 -1, 아래로 움직이면 +1. 동시에 누르면 0이 되도록 하자
	mPaddleDir = 0; // 키보드를 누르지 않을 때 0으로 초기화
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}

	mPaddleDir2 = 0;
	if (state[SDL_SCANCODE_I])
	{
		mPaddleDir2 -= 1;
	}
	if (state[SDL_SCANCODE_K])
	{
		mPaddleDir2 += 1;
	}
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
	/*
	// Paddle의 위치 갱신하기
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;

		// 화면을 벗어나지 못하도록 조정
		if (mPaddlePos.y < (paddleH / 2.0f) + thickness)
		{
			mPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}
	if (mPaddleDir2 != 0)
	{
		mPaddlePos2.y += mPaddleDir2 * 300.0f * deltaTime;

		if (mPaddlePos2.y < paddleH / 2.0f + thickness)
		{
			mPaddlePos2.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos2.y > 768.0f - paddleH / 2.0f - thickness)
		{
			mPaddlePos2.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	//-----------------------------------------------------------------------------------------------------------------------------

	// 공의 위치 갱신하기
	for (unsigned int i = 0; i < Balls.size(); i++)
	{
		Balls[i].Pos.x += Balls[i].Vec.x * deltaTime;
		Balls[i].Pos.y += Balls[i].Vec.y * deltaTime;

		// 공이 상단 벽과 부딪히면 y방향을 반전시킨다
		// A프레임에서 벽과 충돌했는데 B프레임에서 아직 벽에서 벗어나지 못했다면 다시 y가 반전된다! 즉 벽에 계속 붙어있게 된다 => 추가 조건을 사용한다
		// 공이 상단 벽에 부딪히고 & 방향이 위쪽으로 향한다면
		if (Balls[i].Pos.y < thickness && Balls[i].Vec.y < 0.0f)
		{
			Balls[i].Vec.y *= -1;
		}
		if (Balls[i].Pos.y > 768.0 - thickness && Balls[i].Vec.y > 0.0f)	// 하단 벽
		{
			Balls[i].Vec.y *= -1;
		}

		// 공이 패들과 부딪힐 때
		float diff = mPaddlePos.y - Balls[i].Pos.y;
		diff = (diff > 0.0f) ? diff : -1 * diff;
		if (diff <= paddleH / 2.0f &&
			Balls[i].Pos.x <= 20.0f && Balls[i].Pos.x >= 15 &&
			Balls[i].Vec.x < 0.0f)
		{
			Balls[i].Vec.x *= -1;
		}
		float diff2 = mPaddlePos2.y - Balls[i].Pos.y;
		diff2 = (diff2 > 0.0f) ? diff2 : -1 * diff2;
		if (diff2 <= paddleH / 2.0f &&
			Balls[i].Pos.x >= 1024.0f - 22.5f && Balls[i].Pos.x <= 1024.0f - 17.5f &&
			Balls[i].Vec.x > 0.0f)
		{
			Balls[i].Vec.x *= -1;
		}
	}
	*/

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
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255); // 후면 버퍼를 지우기 위한 하나의 색 지정
	SDL_RenderClear(mRenderer); // 1

	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255); // 사각형을 그리기 위한 하나의 색 지정
	for (unsigned int i = 0; i < Balls.size(); i++)
	{
		SDL_Rect ball{					// 사각형 구조체
		static_cast<int>(Balls[i].Pos.x - thickness / 2),	// 왼쪽 상단 좌표
		static_cast<int>(Balls[i].Pos.y - thickness / 2),	// 왼쪽 상단 좌표
		thickness,					// 너비
		thickness					// 높이
		};
		SDL_RenderFillRect(mRenderer, &ball);	// 2  공 그리기
	}
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x - thickness / 2),
		static_cast<int>(mPaddlePos.y - 3 * thickness),
		thickness,
		thickness * 6
	};
	SDL_Rect paddle2{
		static_cast<int>(mPaddlePos2.x - thickness / 2),
		static_cast<int>(mPaddlePos2.y - 3 * thickness),
		thickness,
		thickness * 6
	};
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &paddle);	// 2  패들 그리기
	SDL_RenderFillRect(mRenderer, &paddle2);	// 2  패들 그리기
	SDL_RenderFillRect(mRenderer, &wall);   // 2  벽 그리기
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);   // 2  벽 그리기

	
	SDL_RenderPresent(mRenderer); // 3

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