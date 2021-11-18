#include "AudioComponent.h"
#include "AudioSystem.h"
#include "Actor.h"
#include "Game.h"

AudioComponent::AudioComponent(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
{

}

AudioComponent::~AudioComponent()
{
	StopAllEvents();
}

void AudioComponent::Update(float deltaTime)
{
	Component::Update(deltaTime);

	auto iter = mEvents2D.begin();
	while (iter != mEvents2D.end())
	{
		if (!iter->IsValid())
		{
			iter = mEvents2D.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	// Remove invalid 3D events
	iter = mEvents3D.begin();
	while (iter != mEvents3D.end())
	{
		if (!iter->IsValid())
		{
			iter = mEvents3D.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void AudioComponent::OnUpdateWorldTransform()
{
	// ������ ���ʹ� ���� ��ȯ ����� ����� ������ OnUpdateWorldTransform()�� ȣ���ؼ� �� ������Ʈ�� �����Ѵ�
	// AudioComponent�� ���� ��ȯ�� ���ŵ� ������ mEvents3D�� �ִ� ��� 3D �̺�Ʈ�� 3D �Ӽ��� �����ؾ� �Ѵ�

	Matrix4 world = mOwner->GetWorldTransform();

	for (auto& event : mEvents3D)
	{
		if (event.IsValid())
		{
			event.Set3DAttributes(world);
		}
	}
}

SoundEvent AudioComponent::PlayEvent(const std::string& name)
{
	SoundEvent e = mOwner->GetGame()->GetAudioSystem()->PlayEvent(name);

	// �� SoundEvent�� 3D? 2D?
	if (e.Is3D())
	{
		mEvents3D.emplace_back(e);
		// �ʱ� 3D ����
		e.Set3DAttributes(mOwner->GetWorldTransform());
	}
	else
	{
		mEvents2D.emplace_back(e);
	}

	return e;
}

void AudioComponent::StopAllEvents()
{
	// �Ҹ��ڿ��� ȣ��
	// ������ ������ ���� �̺�Ʈ�� �ߴ��ϰ��� �� ���� ȣ��

	for (auto& iter : mEvents2D)
	{
		iter.Stop();
	}
	mEvents2D.clear();

	for (auto& iter : mEvents3D)
	{
		iter.Stop();
	}
	mEvents3D.clear();
}

