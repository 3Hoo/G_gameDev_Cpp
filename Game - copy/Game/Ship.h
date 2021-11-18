#pragma once

#include "Actor.h"

class Ship : public Actor
{
public:
	Ship(class Game* game);
	virtual void UpdateActor(float deltaTime) override;

	void ProcessKeyboard(const uint8_t* state);
	float GetRightSpeed() const { return mRightSpeed; }
	float GetDownSpeed() const { return mDownSpeed; }

	virtual void ActorInput(const uint8_t* keyState) override;

private:
	float mRightSpeed;
	float mDownSpeed;

	float mLaserCooldown;
	float mRespawnTime;
	bool mRespawn;

	class CircleComponent* mCircle;

	class AnimSpriteComponent* asc;
	//class TileMapComponent* tmc;
};

