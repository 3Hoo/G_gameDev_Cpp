#pragma once

#include "SpriteComponent.h"
#include <vector>
#include "Math.h"

/*
* 현재는 배경만 스크롤하는데 초점을 맞춘다
* 가장 쉬운 방법은 배경을 화면 크기의 이미지 세그먼트로 분할하는 것
* 이 이미지 세그먼트는 스크롤이라는 환상을 만들기 위해 프레임마다 위치가 변경된다
*/

class BGSpriteComponent : public SpriteComponent
{
public:
	// 기본 그리기 순서값을 낮은 값으로 설정 (배경이므로)
	BGSpriteComponent(class Actor* owner, int drawOrder = 10);

	// 부모로부터 오버라이딩한 Update, Draw
	void Update(float deltaTime) override;
	void Draw(SDL_Renderer* renderer) override;

	// 배경에 사용되는 텍스처 설정
	void SetBGTextures(const std::vector<SDL_Texture*>& textures);

	// 화면 크기를 얻거나 스크롤 속도값을 설정 및 얻는 함수
	void SetScreenSize(const Vector2& size) { mScreenSize = size; }
	void SetScrollSpeed(float speed) { mScrollSpeed = speed; }
	float GetScrollSpeed() { return mScrollSpeed; }

private:
	// 각 배경 이미지와 배경 이미지의 오프셋 값을 캡슐화한 구조체
	// 오프셋 값은 텍스처의 위치를 프레임마다 갱신하는 데 사용한다
	struct BGTexture
	{
		SDL_Texture* mTexture;
		Vector2 mOffset;
	};

	std::vector<BGTexture> mBGTextures;
	Vector2 mScreenSize;
	float mScrollSpeed;
};

