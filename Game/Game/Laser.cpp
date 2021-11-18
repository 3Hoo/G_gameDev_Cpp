#include "Laser.h"
#include "Game.h"
#include "CircleComponent.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Asteroid.h"

Laser::Laser(Game* game)
	: Actor(game)
	, mLifetime(1.0f)
{
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Laser.png"));

	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(800.0f);

	mCircle = new CircleComponent(this);
	mCircle->SetRadius(10.0f);
}

Laser::~Laser()
{
}

void Laser::UpdateActor(float deltaTime)
{
	// 1초가 지났나?
	mLifetime -= deltaTime;
	if (mLifetime <= 0.0f)
	{
		SetState(EDead);
	}

	// 운석과 교차하는가
	for (auto ast : GetGame()->GetAsteroids())
	{
		if (Intersect(*mCircle, *(ast->GetCircle())))
		{
			// 레이저가 운석과 교차한다면, 레이저와 운석이 죽었다고 설정한다
			SetState(EDead);
			ast->SetState(EDead);
			break;
		}
	}

}