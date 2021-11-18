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
	int sdlResult = SDL_Init(SDL_INIT_VIDEO); // SDL_Init�� �ʱ�ȭ �Ϸ��� ��� ����ý��۵��� ��ƮOR ���갪�� �Ķ���ͷ� �޴´�. ����� ������ �ϸ� ��

	if (sdlResult != 0) // 0�� �ƴϸ� �ʱ�ȭ ����! => SDL ���̴� ���� ���� �Ұ�(ȭ���� �Ⱥ���)
	{
		SDL_Log("Unable to initialize SDL : %s", SDL_GetError()); // SDL_GetError() �� c��Ÿ���� ���ڿ��� ��ȯ�Ѵ�
		return false;
	}


	// �ʱ�ȭ�� ���������� SDL_CreateWindow�� ȭ���� ��� ���ʴ�
	// ������ ����, �������� x,y ��ǥ(ȭ�� ������� ����), �������� ���̿� ����, ������ �������� �÷���
	mWindow = SDL_CreateWindow("Game Programming in C++ (CH1)", 100, 100, 1024, 768, 0);

	if (!mWindow)
	{
		SDL_Log("Failed to create window : %s", SDL_GetError());
		return false;
	}


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

	// �� ����
	Balls.push_back(Ball{ 1024.0f / 2.0f, 768.0f / 2.0f, -200.0f, 235.0f });
	Balls.push_back(Ball{ 1024.0f / 4.0f, 768.0f / 5.0f, 200.0f, -235.0f });

	// �е�� ���� ��ġ �ʱ�ȭ
	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f / 2.0f;
	mPaddlePos2.x = 1014.0f;
	mPaddlePos2.y = 768.0f / 2.0f;

	return true;
}

void Game::ShutDown()
{
	// SDL_DestroyWindow�� SDL_Window ��ü�� �ı�

	SDL_DestroyWindow(mWindow);

	// SDL_DestroyRenderer�� ������ ��ü �ı�
	SDL_DestroyRenderer(mRenderer);

	// mActors ����. ~Actor �Լ��� RemoveActor�� ȣ���ϹǷ� �ٸ� ��Ÿ���� ������ �������
	while (!mActors.empty())
	{
		delete mActors.back();
	}

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
	
	//-----------------------------------------------------------------------------------------------------------------------------

	// mPaddleDir = �е��� ���� �����̸� -1, �Ʒ��� �����̸� +1. ���ÿ� ������ 0�� �ǵ��� ����
	mPaddleDir = 0; // Ű���带 ������ ���� �� 0���� �ʱ�ȭ
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
	/*
	// Paddle�� ��ġ �����ϱ�
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;

		// ȭ���� ����� ���ϵ��� ����
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

	// ���� ��ġ �����ϱ�
	for (unsigned int i = 0; i < Balls.size(); i++)
	{
		Balls[i].Pos.x += Balls[i].Vec.x * deltaTime;
		Balls[i].Pos.y += Balls[i].Vec.y * deltaTime;

		// ���� ��� ���� �ε����� y������ ������Ų��
		// A�����ӿ��� ���� �浹�ߴµ� B�����ӿ��� ���� ������ ����� ���ߴٸ� �ٽ� y�� �����ȴ�! �� ���� ��� �پ��ְ� �ȴ� => �߰� ������ ����Ѵ�
		// ���� ��� ���� �ε����� & ������ �������� ���Ѵٸ�
		if (Balls[i].Pos.y < thickness && Balls[i].Vec.y < 0.0f)
		{
			Balls[i].Vec.y *= -1;
		}
		if (Balls[i].Pos.y > 768.0 - thickness && Balls[i].Vec.y > 0.0f)	// �ϴ� ��
		{
			Balls[i].Vec.y *= -1;
		}

		// ���� �е�� �ε��� ��
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
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255); // �ĸ� ���۸� ����� ���� �ϳ��� �� ����
	SDL_RenderClear(mRenderer); // 1

	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255); // �簢���� �׸��� ���� �ϳ��� �� ����
	for (unsigned int i = 0; i < Balls.size(); i++)
	{
		SDL_Rect ball{					// �簢�� ����ü
		static_cast<int>(Balls[i].Pos.x - thickness / 2),	// ���� ��� ��ǥ
		static_cast<int>(Balls[i].Pos.y - thickness / 2),	// ���� ��� ��ǥ
		thickness,					// �ʺ�
		thickness					// ����
		};
		SDL_RenderFillRect(mRenderer, &ball);	// 2  �� �׸���
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
	SDL_RenderFillRect(mRenderer, &paddle);	// 2  �е� �׸���
	SDL_RenderFillRect(mRenderer, &paddle2);	// 2  �е� �׸���
	SDL_RenderFillRect(mRenderer, &wall);   // 2  �� �׸���
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);   // 2  �� �׸���

	
	SDL_RenderPresent(mRenderer); // 3

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