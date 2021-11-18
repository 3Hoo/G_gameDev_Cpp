#include "OrbitCamera.h"
#include "Actor.h"

OrbitCamera::OrbitCamera(Actor* owner)
	: CameraComponent(owner)
	, mPitchSpeed(0.0f)
	, mYawSpeed(0.0f)
	, mOffset(Vector3(-400.0f, 0.0f, 0.0f))
	, mUp(Vector3(0.0f, 0.0f, 1.0f))
{

}

void OrbitCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);
	
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
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, mUp);
	SetviewMatrix(view);
}