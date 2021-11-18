#pragma once
#pragma comment (lib, "SDL2")
#pragma comment (lib, "SDL2main")
#pragma comment (lib, "SDL2_image")

#include <SDL.h>
#include <vector>
#include <string>
#include <unordered_map>


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
	void AddAsteroid(class Asteroid* as);
	// 운석 삭제
	void RemoveAsteroid(class Asteroid* as);
	// 운석 불러오기
	std::vector<class Asteroid*> GetAsteroids();

	// 스프라이트 컴포넌트 추가/삭제
	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);

	// 파일 이름을 받아서, 해당 텍스처가 맵에 존재하는지 확인. 있으면 텍스처 포인터 리턴, 없으면 파일로부터 텍스처를 로드하는 코드를 실행한다
	SDL_Texture* GetTexture(const std::string& fileName);

private:
	// 게임 루프를 위한 헬퍼 함수
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	
	// 모든 액터를 생성/제거하는 역할
	void LoadData();
	void UnloadData();


	//-----------------------------------------------------------------------------------------------------------------------------


	// SDL이 생성한 윈도우
	SDL_Window* mWindow;

	// OpenGL Context
	SDL_GLContext mContext;

	// 게임이 계속 실행돼야 하는지를 판단
	bool mIsRunning;

	// SDL Renderer
	SDL_Renderer* mRenderer;

	// DeltaTime을 계산하기 위한 변수 = Init으로 게임이 시작하고 지난 틱 수
	Uint32 mTicksCount;


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


	/*
	* 이미지 데이터는 여러 액터가 공유해서 사용하므로 싱글톤 클래스에 가까운 Game에 파일 이름과 SDL_Texture 포인터를 쌍으로 한 맵을 만들어두자
	* 텍스처의 이름을 인자로 받고 해당 이름과 일치하는 SDL_Texture를 반환하는 GetTexture함수를 이용해서 효율적으로 로드한 텍스처를 사용하자
	*/
	std::unordered_map<std::string, SDL_Texture*> mTextures;

	// SpriteComponent를 저장하는 벡터
	std::vector<class SpriteComponent*> mSprites;

	// Ship
	class Ship* mShip;
	// Asteroids
	std::vector<class Asteroid*> mAsteroids;
};

