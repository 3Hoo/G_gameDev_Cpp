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
	void ProcessInput(const uint8_t* keyState);
	// Ư�� ���͸� ���� �Է��� ó���ϴ� �Լ� (�����Լ� O)
	virtual void ActorInput(const uint8_t* keyState);

	// Getter Setter
	const Vector2& GetPosition() const { return mPosition; }
	float GetScale() const { return mScale; }
	float GetRotation() const { return mRotation; }
	Vector2 GetForward() const { return Vector2(Math::Cos(mRotation), -Math::Sin(mRotation)); }

	void SetPosition(const Vector2& pos) { mPosition = pos; }
	void SetScale(const float scale) { mScale = scale; }
	void SetRotation(const float rot) { mRotation = rot; }

	State GetState() const { return mState; }
	void SetState(State state) { mState = state; }

	class Game* GetGame() { return mGame; }


	// ������Ʈ �߰� ����
	void AddComponent(class Component* component); // �� ������Ʈ�� �߰��� ������ ������Ʈ ���͸� �����ؼ� ���� ������ �����ش�
	void RemoveComponent(class Component* component);

private:
	// ������ ����
	State mState;

	// ��ȯ
	Vector2 mPosition;	// ������ �߽���
	float mScale;		// ������ ���� (100%�̸� 1.0f)
	float mRotation;	// ȸ�� ���� (����)

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