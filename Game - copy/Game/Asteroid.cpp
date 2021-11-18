#include "Asteroid.h"
#include "Game.h"
#include "Random.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CircleComponent.h"

Asteroid::Asteroid(Game* game)
	: Actor(game)
{
	// �����ϰ� ��ġ�� ���� �ʱ�ȭ
	Vector2 randPos = Random::GetVector(Vector2::Zero, Vector2(1024.0f, 768.0f));

	SetPosition(randPos);
	SetRotation(Random::GetFloatRange(0.0f, Math::TwoPi));

	// ��������Ʈ ������Ʈ�� �����ϰ� �ؽ�ó�� �����Ѵ�
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Asteroid.png"));

	// �̵� ������Ʈ�� �����ϰ� ���� �ӵ��� �����Ѵ�
	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(150.0f);
	mc->SetMass(20.0f);
	//std::vector<Vector2> temp;
	//temp.emplace_back(GetForward() * 15000.0f);
	mc->SetForce(GetForward() * 150000.0f);

	// �浹������ ���� �� ������Ʈ ����
	mCircle = new CircleComponent(this);
	mCircle->SetRadius(40.0f);

	// ���ӿ� ���� �߰�
	game->AddAsteroid(this);
}

Asteroid::~Asteroid()
{
	GetGame()->RemoveAsteroid(this);
}