#pragma once

#include "Component.h"
#include "SoundEvent.h"
#include <string>
#include <vector>

// ���� �̺�Ʈ�� Ư�� ���͸� ������Ű�� ���� ������Ʈ
// ���Ͱ� �����̸�, ���� �̺�Ʈ�� 3D �Ӽ��� �����Ѵ�
// ���Ͱ� ������, ���Ϳ� ���õ� ���� �̺�Ʈ�� �����Ѵ�
class AudioComponent : public Component
{
public:
	AudioComponent(class Actor* owner, int updateOrder = 200);
	~AudioComponent();

	void Update(float deltaTime) override;
	void OnUpdateWorldTransform() override;

	// AudioSystem�� PlayEvent()�� ȣ���ϰ�, �̺�Ʈ�� 3D ���θ� Ȯ�� �� ��� ���Ϳ� SoundEvent�� �������� ����.
	// �̺�Ʈ�� 3D�̸� SoundEvent�� Set3DAttributes �Լ��� ȣ��
	class SoundEvent PlayEvent(const std::string& name);
	void StopAllEvents();

private:
	std::vector<SoundEvent> mEvents2D;
	std::vector<SoundEvent> mEvents3D;
};

