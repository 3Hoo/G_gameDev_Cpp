#pragma once

#include "MoveComponent.h"
#include "Math.h"

class NavComponent : public MoveComponent
{
public:
	NavComponent(class Actor* owner);

	virtual void Update(float deltaTime) override;
	
	Vector3 GetNextPoint();
	void TurnTo(const Vector3& pos);

private:
	Vector3 mNextPoint;
};

