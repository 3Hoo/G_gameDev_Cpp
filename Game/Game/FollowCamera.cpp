#include "FollowCamera.h"
#include "Actor.h"
#include "InputSystem.h"

FollowCamera::FollowCamera(Actor* owner)
	: CameraComponent(owner)
	, mVertDist(150.0f)
	, mHorzDist(350.0f)
	, mTargetDist(100.0f)
	, mSpringConstant(64.0f)
	, mIsMouseRightButton(false)
	, mPitchSpeed(0.0f)
	, mYawSpeed(0.0f)
	, mOffset(Vector3(-350.0f, 150.0f, 0.0f))
	, mUp(Vector3(0.0f, 0.0f, 1.0f))
{

}

static int n = -1;
static int t = 0;

void FollowCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);

	// ������ ��������κ��� spring dampening ���� ���
	float dampening = 2.0f * Math::Sqrt(mSpringConstant);

	if (!mIsMouseRightButton)
	{
		// �̻����� ��ġ ���
		Vector3 idealPos = ComputeCameraPos();

		// �̻����� ��ġ�� ���� ��ġ�� ���� ���
		Vector3 diff = mActualPos - idealPos;
		// �������� ���ӵ��� ����Ѵ�
		Vector3 acel = -mSpringConstant * diff - dampening * mVelocity;

		// �ӵ� ����
		mVelocity += acel * deltaTime;
		// ���� ī�޶��� ��ġ ����
		mActualPos += mVelocity * deltaTime;

		// Ÿ���� ������ �տ� �ִ� ����� �ǹ�
		Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;

		// �� ����� �����ϱ� ����, ���� ��ġ�� ����Ѵ�
		Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
		SetviewMatrix(view);
	}
	else
	{
		// ���� ������ ���� ���Ϳ� yaw�� ����� ���ʹϾ� ����
		Quaternion yaw(Vector3::UnitZ, mYawSpeed * deltaTime);
		// �����°� ���� ���͸� yaw ���ʹϾ��� ����ؼ� ��ȯ
		mOffset = Vector3::Transform(mOffset, yaw);
		mUp = Vector3::Transform(mUp, yaw);

		// ���� ���ͷκ��� ī�޶��� ����/������ ���� ���
		// ���� ���� : mOwner.position - (mOwner.position + offset) = -offset
		Vector3 forward = -1.0f * mOffset;
		forward.Normalize();
		// ���⺤�Ϳ� ���溤�͸� �����ؼ�, ������ ���͸� ã�Ƴ���
		Vector3 right = Vector3::Cross(mUp, forward);
		right.Normalize();

		// ī�޶� ������ ���͸� �������� ȸ���ϴ� pitch ���ʹϾ� ����
		Quaternion pitch(right, mPitchSpeed * deltaTime);
		// ī�޶� �����°� ���⺤��(ī�޶��� ���� ����)�� pitch ���ʹϾ����� ȸ��
		mOffset = Vector3::Transform(mOffset, pitch);
		mUp = Vector3::Transform(mUp, pitch);

		// ��ȯ ����� ���
		Vector3 target = mOwner->GetPosition();
		Vector3 cameraPos = target + mOffset;
		Vector3 idealPos = cameraPos;
		Vector3 diff = mActualPos - idealPos;
		Vector3 acel = -mSpringConstant * diff - dampening * mVelocity;
		mVelocity += acel * deltaTime;
		mActualPos += mVelocity * deltaTime;

		Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, mUp);
		SetviewMatrix(view);
	}

}

void FollowCamera::ProcessInput(const struct InputState& state)
{
	if (state.Mouse.GetButtonState(SDL_BUTTON_RIGHT) == EHeld)
	{
		mIsMouseRightButton = true;
	}
	else
	{
		mIsMouseRightButton = false;
		if (n >= 0)
		{
			n--;
		}
	}
}

void FollowCamera::SnapToIdeal()
{
	// ���� ��ġ�� �̻����� ��ġ�� ����
	mActualPos = ComputeCameraPos();
	// �ӵ��� 0���� �ʱ�ȭ
	mVelocity = Vector3::Zero;
	// Ÿ�� ������ �� ����� ���
	Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
	Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
	SetviewMatrix(view);
}

Vector3 FollowCamera::ComputeCameraPos() const
{
	// �������� ���� �� ���ʿ� ī�޶� ��ġ ����
	Vector3 cameraPos = mOwner->GetPosition();
	cameraPos -= mOwner->GetForward() * mHorzDist;
	cameraPos += Vector3::UnitZ * mVertDist;
	return cameraPos;
}