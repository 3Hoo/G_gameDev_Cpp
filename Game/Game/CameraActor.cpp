#include "CameraActor.h"
#include "MoveComponent.h"
#include <SDL_scancode.h>
#include "Renderer.h"
#include "AudioSystem.h"
#include "Game.h"
#include "AudioComponent.h"
#include "InputSystem.h"

CameraActor::CameraActor(Game* game)
	: Actor(game)
{
	mMoveComp = new MoveComponent(this);
	mAudioComp = new AudioComponent(this);
	mLastFootstep = 0.0f;
	mFootstep = mAudioComp->PlayEvent("event:/Footstep");
	mFootstep.SetPaused(true);
}


void CameraActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	// Play the footstep if we're moving and haven't recently
	mLastFootstep -= deltaTime;
	if ((!Math::NearZero(mMoveComp->GetForwardSpeed()) || !Math::NearZero(mMoveComp->GetStrafeSpeed())) && mLastFootstep <= 0.0f)
	{
		mFootstep.SetPaused(false);
		mFootstep.Restart();
		mLastFootstep = 0.5f;
	}

	Vector3 cameraPos = GetPosition();
	Vector3 target = GetPosition() + GetForward() * 100.0f;
	Vector3 up = Vector3::UnitZ;

	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	GetGame()->GetRenderer()->SetViewMatrix(view);
	GetGame()->GetAudioSystem()->SetListener(view);

	
}

void CameraActor::ActorInput(const struct InputState& state)
{
	float forwardSpeed = 0.0f;
	float strafeSpeed = 0.0f;
	float angularSpeed = 0.0f;
	float pitchSpeed = 0.0f;

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
	angularSpeed = inpuySystem->GetState().Mouse.GetAngularSpeed();
	pitchSpeed = inpuySystem->GetState().Mouse.GetPitchSpeed();

	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetStrafeSpeed(strafeSpeed);
	mMoveComp->SetAngularSpeed(angularSpeed);
}

void CameraActor::SetFootstepSurface(float value)
{
	// Pause here because the way I setup the parameter in FMOD
	// changing it will play a footstep
	mFootstep.SetPaused(true);
	mFootstep.SetParameter("Surface", value);
}
