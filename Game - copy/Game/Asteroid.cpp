#include "Asteroid.h"
#include "Game.h"
#include "Random.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CircleComponent.h"

Asteroid::Asteroid(Game* game)
	: Actor(game)
{
	// 랜덤하게 위치와 방향 초기화
	Vector2 randPos = Random::GetVector(Vector2::Zero, Vector2(1024.0f, 768.0f));

	SetPosition(randPos);
	SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

	// 스프라이트 컴포넌트를 생성하고 텍스처를 설정한다
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Asteroid.png"));

	// 이동 컴포넌트를 생성하고 전방 속도를 설정한다
	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(150.0f);
	mc->SetMass(20.0f);
	//std::vector<Vector2> temp;
	//temp.emplace_back(GetForward() * 15000.0f);
	mc->SetForce(GetForward() * 150000.0f);

	// 충돌감지를 위한 원 컴포넌트 생성
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(40.0f);

	// 게임에 액터 추가
	game->AddAsteroid(this);
}

Asteroid::~Asteroid()
{
	GetGame()->RemoveAsteroid(this);
}