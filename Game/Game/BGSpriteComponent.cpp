#include "BGSpriteComponent.h"
#include "Actor.h"

BGSpriteComponent::BGSpriteComponent(Actor* owner, int drawOrder)
	: SpriteComponent(owner, drawOrder)
	, mScreenSize(Vector2::Zero)
	, mScrollSpeed(0)
{

}

void BGSpriteComponent::Update(float deltaTime)
{
	SpriteComponent::Update(deltaTime);

	for (auto& bg : mBGTextures)
	{
		// x 오프셋 값을 갱신
		bg.mOffset.x += deltaTime * mScrollSpeed;
		
		// 이 텍스처가 완전히 왼쪽으로 벗어나면 오프셋 값을 마지막 배경 텍스처의 오른쪽 위치로 초기화한다
		if (bg.mOffset.x < -mScreenSize.x)
		{
			bg.mOffset.x = (mBGTextures.size() - 1) * mScreenSize.x - 1;
		}
	}
}

void BGSpriteComponent::Draw(SDL_Renderer* renderer)
{
	for (auto& tex : mBGTextures)
	{
		SDL_Rect r;

		r.w = static_cast<int>(mScreenSize.x);
		r.h = static_cast<int>(mScreenSize.y);

		// mOffset을 기준으로 위치를 조정한다 => 벡터에 들어있는 순서대로 화면 왼쪽부터 차례대로 그려진다
		r.x = static_cast<int>(mOwner->GetPosition().x - r.w / 2 + tex.mOffset.x);
		r.y = static_cast<int>(mOwner->GetPosition().y - r.h / 2 + tex.mOffset.y);

		// 배경에 그리기
		SDL_RenderCopy(renderer,
			tex.mTexture,
			nullptr,
			&r);
	}
}


void BGSpriteComponent::SetBGTextures(const std::vector<SDL_Texture*>& textures)
{
	int count = 0;

	for (auto tex : textures)
	{
		BGTexture temp;
		temp.mTexture = tex;
		// 각 텍스처의 오프셋은 화면의 너비 * count 이다
		// 그러면 다음 배경화면이 이전 배경화면의 오른쪽에 위치하게 된다
		// 여기서는 각 배경 이미지의 너비가 화면 너비와 같다고 가정한다
		temp.mOffset.x = count * mScreenSize.x;
		temp.mOffset.y = 0;
		mBGTextures.emplace_back(temp);
		count++;
	}
}