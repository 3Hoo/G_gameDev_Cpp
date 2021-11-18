#pragma once
#include "Actor.h"
#include "SoundEvent.h"
#include "Collision.h"
#include "Math.h"

class FPSActor : public Actor
{
public:


	FPSActor(class Game* game);
	~FPSActor();

	virtual void UpdateActor(float deltaTime) override;

	virtual void ActorInput(const struct InputState& state) override;

	void SetFootstepSurface(float value);

	// 벽과의 충돌시, 경계를 올바르게 수정해주는 함수. 벽과 충돌하면 -> 더 이상 나아가지 못하게 막는
	void FixCollision();

	void Shoot();

	

private:
	class MoveComponent* mMoveComp;
	class AudioComponent* mAudioComp;
	class MeshComponent* mMeshComp;
	class FPSCamera* mCameraComp;
	class BoxComponent* mBoxComp;
	class Actor* mFPSModel;
	SoundEvent mFootStep;
	float mLastFootStep;

	float mJumpTime = 0.0f;
	float mJumpPower = 300.0f;
};

