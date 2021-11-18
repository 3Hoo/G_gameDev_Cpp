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

	// ������ �浹��, ��踦 �ùٸ��� �������ִ� �Լ�. ���� �浹�ϸ� -> �� �̻� ���ư��� ���ϰ� ����
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

