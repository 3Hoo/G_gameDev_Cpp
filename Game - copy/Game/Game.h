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
	void AddAsteroid(class Asteroid* as);
	// � ����
	void RemoveAsteroid(class Asteroid* as);
	// � �ҷ�����
	std::vector<class Asteroid*> GetAsteroids();

	// ��������Ʈ ������Ʈ �߰�/����
	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);

	// ���� �̸��� �޾Ƽ�, �ش� �ؽ�ó�� �ʿ� �����ϴ��� Ȯ��. ������ �ؽ�ó ������ ����, ������ ���Ϸκ��� �ؽ�ó�� �ε��ϴ� �ڵ带 �����Ѵ�
	SDL_Texture* GetTexture(const std::string& fileName);

private:
	// ���� ������ ���� ���� �Լ�
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	
	// ��� ���͸� ����/�����ϴ� ����
	void LoadData();
	void UnloadData();


	//-----------------------------------------------------------------------------------------------------------------------------


	// SDL�� ������ ������
	SDL_Window* mWindow;

	// OpenGL Context
	SDL_GLContext mContext;

	// ������ ��� ����ž� �ϴ����� �Ǵ�
	bool mIsRunning;

	// SDL Renderer
	SDL_Renderer* mRenderer;

	// DeltaTime�� ����ϱ� ���� ���� = Init���� ������ �����ϰ� ���� ƽ ��
	Uint32 mTicksCount;


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


	/*
	* �̹��� �����ʹ� ���� ���Ͱ� �����ؼ� ����ϹǷ� �̱��� Ŭ������ ����� Game�� ���� �̸��� SDL_Texture �����͸� ������ �� ���� ��������
	* �ؽ�ó�� �̸��� ���ڷ� �ް� �ش� �̸��� ��ġ�ϴ� SDL_Texture�� ��ȯ�ϴ� GetTexture�Լ��� �̿��ؼ� ȿ�������� �ε��� �ؽ�ó�� �������
	*/
	std::unordered_map<std::string, SDL_Texture*> mTextures;

	// SpriteComponent�� �����ϴ� ����
	std::vector<class SpriteComponent*> mSprites;

	// Ship
	class Ship* mShip;
	// Asteroids
	std::vector<class Asteroid*> mAsteroids;
};

