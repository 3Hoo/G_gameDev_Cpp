#pragma once
#include "Actor.h"
#include "SoundEvent.h"

class CameraActor : public Actor
{
public:
	CameraActor(class Game* game);
	
	void UpdateActor(float deltaTime) override;
	void ActorInput(const struct InputState& state) override;

	void SetFootstepSurface(float value);

private:
	class MoveComponent* mMoveComp;
	class AudioComponent* mAudioComp;
	SoundEvent mFootstep;
	float mLastFootstep;
};

