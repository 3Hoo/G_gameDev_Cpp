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
	// 소유자 액터는 세계 변환 행렬을 계산할 때마다 OnUpdateWorldTransform()을 호출해서 각 컴포넌트에 통지한다
	// AudioComponent는 세계 변환이 갱신될 때마다 mEvents3D에 있는 모든 3D 이벤트의 3D 속성을 갱신해야 한다

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

	// 이 SoundEvent는 3D? 2D?
	if (e.Is3D())
	{
		mEvents3D.emplace_back(e);
		// 초기 3D 설정
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
	// 소멸자에서 호출
	// 게임이 액터의 사운드 이벤트를 중단하고자 할 때도 호출

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

