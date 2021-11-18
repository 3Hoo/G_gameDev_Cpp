#include "NavComponent.h"
#include "Actor.h"


NavComponent::NavComponent(Actor* owner)
	: MoveComponent(owner)
	, mNextPoint(Vector2::Zero)
{

}

void NavComponent::Update(float deltaTime)
{
	// 다음 지점에 도착했다면, 새 지점을 얻고 해당 지점을 향해 회전한다
	Vector2 diff = mOwner->GetPosition() - mNextPoint;
	
	if (diff.Length() < 2.0f)
	{
		mNextPoint = GetNextPoint();
		TurnTo(mNextPoint);
	}

	// 액터를 전진시킨다
	MoveComponent::Update(deltaTime);
}

Vector2 NavComponent::GetNextPoint()
{
	return Vector2::Zero;
}

void NavComponent::TurnTo(const Vector2& pos)
{
	// 새로운 점을 향하는 벡터를 구한다
	Vector2 dir = pos - mOwner->GetPosition();

	// 새 각도는 이 방향 벡터의 atan2이다
	// 이 때 SDL화면에서 +y는 화면 아래 방향이므로, y값을 반전시킨다
	float angle = Math::Atan2(-dir.y, dir.x);
	mOwner->SetRotation(angle);
}