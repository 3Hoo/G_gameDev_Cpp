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
	// ���� �̵� ������ �����ϴ� ����
	Vector2 mNextPoint;
};

