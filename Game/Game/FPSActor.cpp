#include "FPSActor.h"
#include "Game.h"
#include "Renderer.h"
#include "PlaneActor.h"
#include "MoveComponent.h"
#include "AudioComponent.h"
#include "MeshComponent.h"
#include "BoxComponent.h"
#include "FPSCamera.h"
#include "AudioSystem.h"
#include "InputSystem.h"
#include "BallActor.h"

FPSActor::FPSActor(Game* game)
	: Actor(game)
{
	mMoveComp = new MoveComponent(this);
	mAudioComp = new AudioComponent(this);
	mLastFootStep = 0.0f;
	mFootStep = mAudioComp->PlayEvent("event:/Footstep");
	mFootStep.SetPaused(true);

	mCameraComp = new FPSCamera(this);

	mBoxComp = new BoxComponent(this);
	AABB myBox(Vector3(-25.0f, -25.0f, -87.5f), Vector3(25.0f, 25.0f, 87.5f));
	mBoxComp->SetObjectBox(myBox);
	mBoxComp->SetShouldRotate(false);
	
	mFPSModel = new Actor(game);
	mFPSModel->SetScale(0.75f);
	mMeshComp = new MeshComponent(mFPSModel);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Rifle.gpmesh"));
}

FPSActor::~FPSActor()
{

}

void FPSActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollision();

	// �ֱٿ� �����̰�, ���ڱ� �Ҹ��� ������� �ƴϸ�, ���ڱ� �Ҹ��� ����Ѵ�
	mLastFootStep -= deltaTime;
	if (!Math::NearZero(mMoveComp->GetForwardSpeed()) && mLastFootStep <= 0.0f)
	{
		mFootStep.SetPaused(false);
		mFootStep.Restart();
		mLastFootStep = 0.5f;
	}

	if (GetJumpState() == JumpState::EUp || GetJumpState() == JumpState::EDown)
	{
		Vector3 pos = GetPosition();

		float jumpHeight = -(mJumpTime * mJumpTime) + mJumpPower * mJumpTime;
		mJumpTime += deltaTime;

		if (mJumpTime >= mJumpPower)
		{
			mJumpTime = 0.0f;
			jumpHeight = 0.0f;
			SetJumpState(JumpState::EFloor);
		}
		pos.z = jumpHeight;
		SetPosition(pos);
	}

	// ���� ��ġ�� ���� FPSModel�� ������� ��ġ ����
	const Vector3 modelOffset(Vector3(10.0f, 10.0f, -10.0f));
	Vector3 modelPos = GetPosition();
	modelPos += GetForward() * modelOffset.x;
	modelPos += GetRight() * modelOffset.y;
	modelPos.z += modelOffset.z;
	mFPSModel->SetPosition(modelPos);

	// ���� ȸ�������� ȸ������ �ʱ�ȭ
	Quaternion q = GetRotation();

	// ī�޶��� ��ġ ������ ȸ��
	q = Quaternion::Concatenate(q, Quaternion(GetRight(), mCameraComp->GetPitch()));
	mFPSModel->SetRotation(q);
}

void FPSActor::ActorInput(const struct InputState& state)
{
	float forwardSpeed = 0.0f;
	float strafeSpeed = 0.0f;
	float angularSpeed = 0.0f;
	float pitchSpeed = 0.0f;
	float verticalSpeed = 0.0f;

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
	else if (state.Keyboard.GetKeyState(SDL_SCANCODE_SPACE) == EPressed)
	{
		SetJumpState(JumpState::EUp);
	}
	angularSpeed = inpuySystem->GetState().Mouse.GetAngularSpeed();
	pitchSpeed = inpuySystem->GetState().Mouse.GetPitchSpeed();

	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetStrafeSpeed(strafeSpeed);
	mMoveComp->SetAngularSpeed(angularSpeed);
	//mMoveComp->SetPitchSpeed(pitchSpeed);
	//mCameraComp->SetYawSpeed(angularSpeed);
	mCameraComp->SetPitchSpeed(pitchSpeed);

	if (state.Mouse.GetButtonState(SDL_BUTTON_LEFT) == EPressed)
	{
		Shoot();
	}
}

void FPSActor::SetFootstepSurface(float value)
{
	// Pause here because the way I setup the parameter in FMOD
	// changing it will play a footstep
	mFootStep.SetPaused(true);
	mFootStep.SetParameter("Surface", value);
}

void FPSActor::FixCollision()
{
	// �ڽ��� �����ϱ� ���� �ڽ��� ���� ��ȯ ������ �ʿ��ϴ�
	ComputeWorldTransform();

	const AABB& playerBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();

	auto& planes = GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		// �� planeActor�� �浹�ϴ°�?
		const AABB& planeBox = pa->GetBox()->GetWorldBox();
		if (Intersect(playerBox, planeBox))
		{
			// �� ������ ��ħ�� ���
			float dx1 = planeBox.mMax.x - playerBox.mMin.x;
			float dx2 = planeBox.mMin.x - playerBox.mMax.x;
			float dy1 = planeBox.mMax.y - playerBox.mMin.y;
			float dy2 = planeBox.mMin.y - playerBox.mMax.y;
			float dz1 = planeBox.mMax.z - playerBox.mMin.z;
			float dz2 = planeBox.mMin.z - playerBox.mMax.z;

			// dx1, dx2 �� ������ ���� ���� ���� dx�� ����
			float dx = (Math::Abs(dx1) < Math::Abs(dx2)) ? dx1 : dx2;
			// dy1, dy2 �� ������ ���� ���� ���� dx�� ����
			float dy = (Math::Abs(dy1) < Math::Abs(dy2)) ? dy1 : dy2;
			// dx1, dx2 �� ������ ���� ���� ���� dx�� ����
			float dz = (Math::Abs(dz1) < Math::Abs(dz2)) ? dz1 : dz2;

			// �ּ� ��ħ�� ���� ���� ������ x,y,z ��ġ�� ����
			// dx�� ���� ���� ���
			if (Math::Abs(dx) <= Math::Abs(dy) && Math::Abs(dx) <= Math::Abs(dz))
			{
				pos.x += dx;
			}
			// dy�� ���� ���� ���
			else if (Math::Abs(dy) <= Math::Abs(dx) && Math::Abs(dy) <= Math::Abs(dz))
			{
				pos.y += dy;
			}
			else
			{
				pos.z += dz;
			}

			if (GetJumpState() == JumpState::EDown)
			{
				if (Math::NearZero(Math::Abs(dz)))
				{
					Vector3 pos = GetPosition();
					pos.z = 0.0f;
					SetPosition(pos);
				}
			}

			// ����� ��ġ�� �����ϰ�, �ڽ� ������Ʈ�� ����
			SetPosition(pos);
			mBoxComp->OnUpdateWorldTransform();
		}
	}
}

void FPSActor::Shoot()
{

	// Get start point (in center of screen on near plane)
	Vector3 screenPoint(0.0f, 0.0f, 0.0f);
	Vector3 start = GetGame()->GetRenderer()->Unproject(screenPoint);
	// Get end point (in center of screen, between near and far)
	screenPoint.z = 0.9f;
	Vector3 end = GetGame()->GetRenderer()->Unproject(screenPoint);
	// Get direction vector
	Vector3 dir = end - start;
	dir.Normalize();
	// Spawn a ball
	BallActor* ball = new BallActor(GetGame());
	ball->SetPlayer(this);
	ball->SetPosition(start + dir * 20.0f);
	// Rotate the ball to face new direction
	ball->RotateToNewForward(dir);
	// Play shooting sound
	mAudioComp->PlayEvent("event:/Shot");
}