#pragma once

#include "SpriteComponent.h"
#include <vector>


// �ִϸ��̼� �ؽ�ó�� �ִϸ��̼��� �� ������ ��(�� ������)�� �����ϴ� ����ü
struct Animations
{
	std::vector<SDL_Texture*> AnimTextures;
	int length;
	bool loop;
};

class AnimSpriteComponent :public SpriteComponent
{
public:
	AnimSpriteComponent(class Actor* owner, int drawOrder = 100);

	// �ִϸ��̼��� �����Ӹ��� ���� (Component�� Update�Լ��� �������̵�)
	void Update(float deltaTime) override;

	void Draw(SDL_Renderer* renderer) override;

	// �ִϸ��̼ǿ� ���Ǵ� �ؽ�ó ����
	void SetAnimTextures(const std::vector<SDL_Texture*>& textures);

	// �ִϸ��̼� FPS�� ��ų� �����ϴ� �Լ�
	float GetAnimFPS() const { return mAnimFPS; }
	void SetAnimFPS(float fps) { mAnimFPS = fps; }

	void SetAnimMode(int mode) { mAnimMode = mode; }
	int GetAnimMode() { return mAnimMode; }

	void SetIsPlaying(bool play) { mIsPlaying = play; }
	bool GetIsPlaying() { return mIsPlaying; }

private:
	// �ִϸ��̼ǿ� ���Ǵ� ��� �ؽ�ó
	std::vector<SDL_Texture*> mAnimTextures;

	// ���� ���� �ִϸ��̼��� �����ϵ��� �ִϸ��̼� �ؽ�ó���� �����ϴ� ����ü ����
	std::vector<Animations> mAnimations;

	// ���� ������
	float mCurrFrame;

	// �ִϸ��̼� ������ ����Ʈ
	float mAnimFPS;

	// � �ִϸ��̼��� ������� ����
	int mAnimMode;

	// �ִϸ��̼��� �� �� �ִ��� ����
	int mAnimNum;

	// �ִϸ��̼��� ����ǰ� �ִ��� �˾ƺ���
	bool mIsPlaying;
};

