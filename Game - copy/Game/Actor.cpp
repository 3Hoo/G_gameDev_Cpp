#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>

Actor::Actor(Game* game) 
	: mState(EActive)
	, mPosition(Vector2::Zero)
	, mScale(1.0f)
	, mRotation(0.0f)
	, mGame(game)
{
	mGame->AddActor(this);
}

Actor::~Actor()
{
	mGame->RemoveActor(this);

	// component���� ������ �Ѵ�
	// ~Component �Լ��� RemoveComponent()�� ȣ���ϹǷ�, �ٸ� ��Ÿ���� ������ �������
	while (!mComponents.empty())
	{
		delete mComponents.back();	// ���⼭�� delete�� ���� �ڵ����� mComponents�� �Ҹ��ڰ� ȣ��ȴ�!
	}
}

void Actor::Update(float deltaTime)
{
	if (mState == EActive || mState == EXInput)
	{
		UpdateComponents(deltaTime);
		UpdateActor(deltaTime);
	}
}

void Actor::UpdateComponents(float deltaTime)
{
	for (auto component : mComponents)
	{
		component->Update(deltaTime);
	}
}

void Actor::UpdateActor(float deltaTime)
{

}

void Actor::ProcessInput(const uint8_t* keyState)
{
	if (mState == EActive && mState != EXInput)
	{
		for (auto comp : mComponents)
		{
			comp->ProcessInput(keyState);
		}
		
		ActorInput(keyState);
	}
}

void Actor::ActorInput(const uint8_t* keyState)
{

}

void Actor::AddComponent(Component* component)
{
	int myOrder = component->GetUpdateOrder();
	auto iter = mComponents.begin();

	for (; iter != mComponents.end(); ++iter)
	{
		if (myOrder < (*iter)->GetUpdateOrder())
		{
			break;
		}
	}

	mComponents.insert(iter, component);
}

void Actor::RemoveComponent(Component* component)
{
	auto iter = std::find(mComponents.begin(), mComponents.end(), component);

	if (iter != mComponents.end())
	{
		mComponents.erase(iter);
	}
}
