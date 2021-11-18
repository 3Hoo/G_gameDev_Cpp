#include "FPSCamera.h"
#include "Actor.h"

FPSCamera::FPSCamera(Actor* owner)
	: CameraComponent(owner)
	, mPitchSpeed(0.0f)
	, mYawSpeed(0.0f)
	, mMaxPitch(Math::Pi / 3.0f)
	, mPitch(0.0f)
{

}

void FPSCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);

	// ī�޶��� ��ġ�� �������� ��ġ
	Vector3 cameraPos = mOwner->GetPosition();

	// pitch�� �������� �������� �߽����� �� ���ʹϾ� ȸ������ ǥ���Ѵ�
	// pitch�� ���� �������� yaw(����)�� ����(���溤�Ͱ� �ٲ��, �������͵� �ٲ��)�ϹǷ� �ܼ��� y���� �ƴϴ�

	// pitch �ӵ��� pitch�� ����
	mPitch += mPitchSpeed * deltaTime;
	// pitch ���� [-max, max] ���� ����
	mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);
	// �������� ���� �� ���Ϳ� ���� pitch ȸ���� ��Ÿ���� ���ʹϾ����� ǥ��
	Quaternion q(mOwner->GetRight(), mPitch);
	// pitch ���ʹϾ����� �������� ���� ���͸� ȸ��
	Vector3 viewForward = Vector3::Transform(mOwner->GetForward(), q);
	// ���� ���� 100 ���� �տ� Ÿ�� ��ġ ����
	Vector3 target = cameraPos + viewForward * 100.0f;
	// pitch ���ʹϾ����� �����൵ ȸ����Ų��
	Vector3 up = Vector3::Transform(Vector3::UnitZ, q);

	// look at ��� ����, �� ��ķ� ����
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	SetviewMatrix(view);
}