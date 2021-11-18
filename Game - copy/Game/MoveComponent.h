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

	void AddForce(Vector2 force) { mForce += force; }
	float CalRot(Vector2 force);

	void SetForce(Vector2 force) { mForce = force; }
	void SetMass(const float mass) { mMass = mass; }
	void SetVelocity(const Vector2 v) { mVelocity = v; }
	Vector2 GetForce() const { return mForce; }
	float GetMass() const { return mMass; }
	Vector2 GetVelocity() const { return mVelocity; }

private:
	// 회전을 제어 (초당 라디안값)
	float mAngularSpeed;

	// 전방 이동을 제어 (초당 단위)
	float mForwardSpeed;

	float mMass;

	Vector2 mForce;

	Vector2 mVelocity;

	std::vector<Vector2> mForces;

};

