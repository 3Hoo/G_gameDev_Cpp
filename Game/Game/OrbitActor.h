#pragma once
#include "Actor.h"
#include "Math.h"

class OrbitActor : public Actor
{
public:
	OrbitActor(class Game* game);

	virtual void UpdateActor(float deltaTime) override;

	virtual void ActorInput(const struct InputState& state) override;

private:
	class MoveComponent* mMoveComp;
	class OrbitCamera* mCameraComp;
	class MeshComponent* mMeshComp;
	class Actor* mOrbitModel;

};

