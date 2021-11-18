#pragma once

#include <cstdint>

class Component
{
public:
	// ������
	// ������Ʈ �������� �������� ������Ʈ�� �� ���� ���ŵȴ�
	Component(class Actor* owner, int updateOrder = 100);

	// �Ҹ���
	virtual ~Component();

	// ��Ÿ �ð����� �� ������Ʈ�� ������Ʈ
	virtual void Update(float deltaTime);
	int GetUpdateOrder() const { return mUpdateOrder; }

	virtual void ProcessInput(const uint8_t* keyState);

protected:
	// ������ ����
	class Actor* mOwner;

	// ������Ʈ�� ������Ʈ ����
	// ���� ī�޶� ������Ʈ�� ������ ������Ʈ���� ���� ������Ʈ �ǹ�����? �ɸ��ʹ� ���������� �ʾҴµ� ī�޶� ���� ������������
	// �׷��� ������Ʈ���� ������Ʈ ������ �߿��� ���̴�
	int mUpdateOrder;
};

