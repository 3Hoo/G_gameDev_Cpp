#pragma once
#include "Component.h"
#include "Math.h"
#include <vector>

class MoveComponent : public Component
{
public:
	MoveComponent(class Actor* owner, int updateOrder = 10);
	~MoveComponent();

	void Update(float deltaTime) override;

	float GetAngularSpeed() const { return mAngularSpeed; }
	float GetForwardSpeed() const { return mForwardSpeed; }
	void SetAngularSpeed(const float speed) { mAngularSpeed = speed; }
	void SetForwardSpeed(const float speed) { mForwardSpeed = speed; }
	float GetStrafeSpeed() const { return mStrafeSpeed; }
	void SetStrafeSpeed(const float speed) { mStrafeSpeed = speed; }
	float GetPitchSpeed() const { return mPitchSpeed; }
	void SetPitchSpeed(const float speed) { mPitchSpeed = speed; }

	void AddForce(Vector3 force) { mForce += force; }
	float CalRot(Vector3 force);

	void SetForce(Vector3 force) { mForce = force; }
	void SetMass(const float mass) { mMass = mass; }
	void SetVelocity(const Vector3 v) { mVelocity = v; }
	Vector3 GetForce() const { return mForce; }
	float GetMass() const { return mMass; }
	Vector3 GetVelocity() const { return mVelocity; }

private:
	// ȸ���� ���� (�ʴ� ���Ȱ�)
	float mAngularSpeed;
	float mPitchSpeed;

	// ���� �̵��� ���� (�ʴ� ����)
	float mForwardSpeed;

	// �¿� �̵��� ���� (�ʴ� ����)
	float mStrafeSpeed;

	// z �ӵ�


	float mMass;

	Vector3 mForce;

	Vector3 mVelocity;

	std::vector<Vector3> mForces;

};

