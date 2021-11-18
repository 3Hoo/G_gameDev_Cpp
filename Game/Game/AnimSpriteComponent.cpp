#include "AnimSpriteComponent.h"
#include "Actor.h"
#include <algorithm>

AnimSpriteComponent::AnimSpriteComponent(Actor* owner, int drawOrder)
	: SpriteComponent(owner, drawOrder)
	, mCurrFrame(0)
	, mAnimFPS(24.0f)
	, mAnimMode(0)
	, mAnimNum(0)
	, mIsPlaying(false)
{

}

void AnimSpriteComponent::Update(float deltaTime)
{
	SpriteComponent::Update(deltaTime);

	if (GetTexture() == mAnimations[mAnimMode].AnimTextures[mAnimations[mAnimMode].length - 1])
	{
		if (mAnimations[mAnimMode].loop)
		{
			
		}
		else
		{
			mCurrFrame = 0;
			SetTexture(mAnimations[mAnimMode].AnimTextures[static_cast<int>(mCurrFrame)]);
			mAnimMode = 0;
			return;
		}
	}

	if (mAnimations[mAnimMode].length > 0)
	{
		// 프레임 레이드와 델타 시간을 기반으로 현재 프레임 갱신
		mCurrFrame += deltaTime * mAnimFPS;

		while (mCurrFrame >= mAnimations[mAnimMode].AnimTextures.size())
		{
			 mCurrFrame -= mAnimations[mAnimMode].AnimTextures.size();
		}

		SetTexture(mAnimations[mAnimMode].AnimTextures[static_cast<int>(mCurrFrame)]);
	}

}

void AnimSpriteComponent::Draw(SDL_Renderer* renderer)
{
	/*
	for (int i = 0 ; i < mAnimNum; i++)
	{
		auto iter = std::find(mAnimations[mAnimMode].AnimTextures.begin(), mAnimations[mAnimMode].AnimTextures.end(), GetTexture());
		if (iter != mAnimations[mAnimMode].AnimTextures.end())
		{
			SpriteComponent::Draw(renderer);
		}
		else
		{
			SetTexture(mAnimations[mAnimMode].AnimTextures[0]);
			SpriteComponent::Draw(renderer);
		}
	}*/
	SpriteComponent::Draw(renderer);
}

void AnimSpriteComponent::SetAnimTextures(const std::vector<SDL_Texture*>& textures)
{
	// 현재 프레임 초기화
	mCurrFrame = 0;

	int l = textures.size();

	//Animations tempAnim = { textures, l, loop };

	//mAnimations.emplace_back(tempAnim);

	mAnimTextures = textures;

	mAnimNum++;

	SetTexture(mAnimations[mAnimNum - 1].AnimTextures[0]);
}