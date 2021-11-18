
#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)
	: Component(owner)
	, mTexture(nullptr)
	, mDrawOrder(drawOrder)
	, mTexHeight(0)
	, mTexWidth(0)
{
	mOwner->GetGame()->AddSprite(this);
}

SpriteComponent::~SpriteComponent()
{
	mOwner->GetGame()->RemoveSprite(this);
}

void SpriteComponent::Draw(SDL_Renderer* renderer)
{
	/*
	* 여기서는 액터의 위치가 화면상의 액터 위치와 일치한다고 추정한다
	* 슈퍼 마리오 브라더스처럼 화면이 움직이면서 케릭터가 이동하는 경우에는 사용할 수 없는 방식.
	* 이를 구현하려면 "카메라"의 개념이 필요하다 => 9장
	*/
	if (mTexture)
	{
		SDL_Rect r;

		// 소유자의 배율(스케일) 값으로 너비, 높이를 스케일 한다
		r.w = mOwner->GetScale() * mTexWidth;
		r.h = mOwner->GetScale() * mTexHeight;
		// 소유자(액터)의 위치를 중심으로 사각형을 배치한다
		r.x = static_cast<int>(mOwner->GetPosition().x - r.w / 2);
		r.y = static_cast<int>(mOwner->GetPosition().y - r.h / 2);

		// 스프라이트를 그린다
		SDL_RenderCopyEx(renderer,
			mTexture,	// 그릴 텍스처
			nullptr,	// 그릴 텍스처의 영역. 전체면 nullptr
			&r,			// 텍스처를 그릴 화면의 영역
			-Math::ToDegrees(mOwner->GetRotation()),	// 액터는 라디안을, 이 함수는 '도' 단위를 쓰므로, Math.h를 이용해서 라디안을 도로 바꿔준다
			nullptr,	// 회전 기준점. 중앙이면 nullptr
			SDL_FLIP_NONE	// 플립 행동. 보통 이 값을 사용함
		);
	}

}

void SpriteComponent::SetTexture(SDL_Texture* texture)
{
	mTexture = texture;

	// 텍스처의 높이와 너비 구하기. SDL_QueryTexture를 이용한다
	SDL_QueryTexture(texture, nullptr, nullptr, &mTexWidth, &mTexHeight);
}