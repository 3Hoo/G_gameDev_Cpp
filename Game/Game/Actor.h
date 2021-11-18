#pragma once

#include <vector>
#include "Math.h"
#include <cstdint>

/*
* ��� ���� ��ü�� �ϳ��� ��Ӵ� Ŭ������ ��ӹ޴´�
* �̷� ������ Monolithic class�� �θ��⵵ �Ѵ�
* 
* ������ �̷��� ���� ������ ������ "���̾Ƹ�� ���" �� ����⵵ �Ѵ�
* �̴� ���� ������ �����Լ��� ��ӹ��� �� �־ ������ �ʷ��� �� �ִ�
*/

/*
* ���� ��ſ� ���� ������ "������Ʈ ���" ���� ��ü ���� �Բ� ����Ѵ�
* ���� ��ü Ŭ������ ����������, ���� ��ü�� ����Ŭ����(�ڽ�)�� ����
* ��ſ� ���� ��ü Ŭ������  �ʿ信 ���� ����� ������ "������Ʈ ��ü"�� "�÷���"�� ���� �ִ�
* 
* ���� ��� ������� �ڵ��� ��ü Ŭ������ "GroundVehicleComponent"�� "WaterVehicleComponent", "DrawComponent", "CollisionComponent" ���� ������ �ִ�
* 
*/

class Actor
{
public:
	// ������ ���¸� �����ϴµ� ���ȴ�
	enum State
	{
		EActive,	// ���� ����
		EPaused,	//
		EXInput,
		EDead		// ���ӿ��� ���͸� �����϶�� ������
	};
	enum JumpState
	{
		EFloor,
		EUp,
		EDown
	};
	JumpState GetJumpState() const { return mJState; }
	void SetJumpState(JumpState state) { mJState = state; }
	
	// ������ �Ҹ���
	Actor(class Game* game);
	virtual ~Actor();

	// Game���� ȣ���ϴ� Update �Լ� (���� �Լ� X)
	void Update(float deltaTime);	// UpdateComponents() -> UpdateActor() ȣ��

	// ���Ϳ� ������ ��� ������Ʈ�� ������Ʈ (���� �Լ� X)
	void UpdateComponents(float deltaTime);

	// Ư�� ���Ϳ� Ưȭ�� ������Ʈ �ڵ� (���� �Լ� O)
	virtual void UpdateActor(float deltaTime);

	// �Է��� ó���ϴ� �Լ� (�������̵� �Ұ�)
	void ProcessInput(const struct InputState& state);
	// Ư�� ���͸� ���� �Է��� ó���ϴ� �Լ� (�����Լ� O)
	virtual void ActorInput(const struct InputState& state);

	// Getter Setter
	const Vector3& GetPosition() const { return mPosition; }
	float GetScale() const { return mScale; }
	Quaternion GetRotation() const { return mRotation; }
	Vector3 GetForward() const { return Vector3::Transform(Vector3::UnitX, mRotation); }
	Vector3 GetRight() const { /*���ʹϾ� ȸ���� ����ؼ� ���� ���� ���� ȸ����Ų��*/	return Vector3::Transform(Vector3::UnitY, mRotation); }

	void SetPosition(const Vector3& pos) { mPosition = pos; mRecomputeWorldTransform = true; }
	void SetScale(const float scale) { mScale = scale; mRecomputeWorldTransform = true; }
	void SetRotation(const Quaternion rot) { mRotation = rot; mRecomputeWorldTransform = true; }

	State GetState() const { return mState; }
	void SetState(State state) { mState = state; }

	class Game* GetGame() const { return mGame; }

	// ������Ʈ �߰� ����
	void AddComponent(class Component* component); // �� ������Ʈ�� �߰��� ������ ������Ʈ ���͸� �����ؼ� ���� ������ �����ش�
	void RemoveComponent(class Component* component);

	// ���� ��ȯ ��� ���
	void ComputeWorldTransform();
	Matrix4 GetWorldTransform() { return mWorldTransform; }

	// ����, ����, ���ʹϾ��� ����ؼ� ȸ������ �����ϴ� ���� �Լ�
	void RotateToNewForward(const Vector3& forward);

private:
	// ������ ����
	State mState;
	JumpState mJState;

	// ��ȯ
	Vector3 mPosition;	// ������ �߽���
	float mScale;		// ������ ���� (100%�̸� 1.0f)
	Quaternion mRotation;	// ȸ�� ���� (����)

	// ���� ��ġ ���
	Matrix4 mWorldTransform;		// ���� ��ȯ ����� �����Ѵ�
	bool mRecomputeWorldTransform;	// ���� ��ȯ ����� ������ �ʿ䰡 �ִ����� ���� (ó���� true�� �ʱ�ȭ�ؼ�, �� ���Ͱ� ��� �� ���� ���� ��ȯ ��� ����� �ϵ��� �����ؾ� �Ѵ�)

	// �� ���Ͱ� ������ ������Ʈ��
	std::vector<class Component*> mComponents;
	class Game* mGame;
};
/*
* �߰� ���� ����(�� ���ʹ� �ٸ� ���͸� �����ϴ� ��� ��)�� ������ �� ���� ���� ������ Game Ŭ������ �����ؾ� �Ѵ�
* �� ���� ����� ���� ��ü�� ���������� ���� ������ Ŭ���� �ν��Ͻ��� "Singleton"���� ����� ��
* ������ �� ������ Ŭ������ ���� �ν��Ͻ��� �ʿ��ϴٰ� �ǴܵǸ� ������ �߻�
* 
* �׷��� "������ ����(Dependency Injection)" ���ٹ��� ����Ѵ�
* ���⼭�� ���� �����ڰ� Game Ŭ������ �����͸� �޾Ƽ� ������ �� ����ϵ��� �Ѵ�
*/