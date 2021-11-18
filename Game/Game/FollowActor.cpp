#include "FollowActor.h"
#include "Game.h"
#include "Renderer.h"
#include "MoveComponent.h"
#include "MeshComponent.h"
#include "FollowCamera.h"
#include "InputSystem.h"

FollowActor::FollowActor(class Game* game)
	: Actor(game)
{
	mMeshComp = new MeshComponent(this);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/RacingCar.gpmesh"));
	SetPosition(Vector3(0.0f, 0.0f, -100.0f));

	mMoveComp = new MoveComponent(this);
	mCameraComp = new FollowCamera(this);
	mCameraComp->SnapToIdeal();
}

void FollowActor::ActorInput(const struct InputState& state)
{
	float forwardSpeed = 0.0f;
	float strafeSpeed = 0.0f;
	float angularSpeed = 0.0f;
	float pitchSpeed = 0.0f;
	float yawSpeed = 0.0f;

	InputSystem* inpuySystem = GetGame()->GetInputSystem();

	if (state.Keyboard.GetKeyState(SDL_SCANCODE_W) == EHeld)
	{
		forwardSpeed += 300.0f;
	}
	else if (state.Keyboard.GetKeyState(SDL_SCANCODE_S) == EHeld)
	{
		forwardSpeed -= 300.0f;
	}
	else if (state.Keyboard.GetKeyState(SDL_SCANCODE_A) == EHeld)
	{
		strafeSpeed -= 300.0f;
	}
	else if (state.Keyboard.GetKeyState(SDL_SCANCODE_D) == EHeld)
	{
		strafeSpeed += 300.0f;
	}
	yawSpeed = inpuySystem->GetState().Mouse.GetAngularSpeed();
	pitchSpeed = inpuySystem->GetState().Mouse.GetPitchSpeed();

	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetStrafeSpeed(strafeSpeed);
	mMoveComp->SetAngularSpeed(yawSpeed);
	mCameraComp->SetPitchSpeed(pitchSpeed);
	mCameraComp->SetYawSpeed(yawSpeed);
}

void FollowActor::SetVisible(bool visible)
{
	
}