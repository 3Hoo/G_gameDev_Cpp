#pragma once
#include "MoveComponent.h"
#include "Math.h"
#include "Collision.h"

class BallMove : public MoveComponent
{
public:
	BallMove(class Actor* owner, int updateOrder = 100);
	~BallMove();

	void Update(float deltaTime) override;
	void SetPlayer(class Actor* player) { mPlayer = player; }

private:
	class Actor* mPlayer;
};

