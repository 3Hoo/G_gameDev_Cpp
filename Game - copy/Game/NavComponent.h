#pragma once

#include "MoveComponent.h"

class NavComponent : public MoveComponent
{
public:
	NavComponent(class Actor* owner);

	virtual void Update(float deltaTime) override;

	Vector2 GetNextPoint();
	void TurnTo(const Vector2& pos);

private:
	// 다음 이동 지점을 저장하는 벡터
	Vector2 mNextPoint;
};

