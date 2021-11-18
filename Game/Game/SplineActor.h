#pragma once
#include "Actor.h"

class SplineActor : public Actor
{
public:
	SplineActor(class Game* game);

	virtual void ActorInput(const struct InputState& state) override;
	virtual void UpdateActor(float deltaTime) override;

	void RestartSpline();
private:
	class SplineCamera* mCameraComp;
};


