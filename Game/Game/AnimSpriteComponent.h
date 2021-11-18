#pragma once

#include "SpriteComponent.h"
#include <vector>


// 애니메이션 텍스처와 애니메이션의 총 프레임 수(몇 장인지)를 저장하는 구조체
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

	// 애니메이션을 프레임마다 갱신 (Component의 Update함수를 오버라이드)
	void Update(float deltaTime) override;

	void Draw(SDL_Renderer* renderer) override;

	// 애니메이션에 사용되는 텍스처 설정
	void SetAnimTextures(const std::vector<SDL_Texture*>& textures);

	// 애니메이션 FPS를 얻거나 설정하는 함수
	float GetAnimFPS() const { return mAnimFPS; }
	void SetAnimFPS(float fps) { mAnimFPS = fps; }

	void SetAnimMode(int mode) { mAnimMode = mode; }
	int GetAnimMode() { return mAnimMode; }

	void SetIsPlaying(bool play) { mIsPlaying = play; }
	bool GetIsPlaying() { return mIsPlaying; }

private:
	// 애니메이션에 사용되는 모든 텍스처
	std::vector<SDL_Texture*> mAnimTextures;

	// 여러 개의 애니메이션을 지원하도록 애니메이션 텍스처들을 저장하는 구조체 백터
	std::vector<Animations> mAnimations;

	// 현재 프레임
	float mCurrFrame;

	// 애니메이션 프레임 레이트
	float mAnimFPS;

	// 어떤 애니메이션을 재생할지 정함
	int mAnimMode;

	// 애니메이션이 몇 개 있는지 저장
	int mAnimNum;

	// 애니메이션이 재생되고 있는지 알아보자
	bool mIsPlaying;
};

