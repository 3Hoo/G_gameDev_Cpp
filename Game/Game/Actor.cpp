#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include <algorithm>

Actor::Actor(Game* game) 
	: mState(EActive)
	, mPosition(Vector3::Zero)
	, mScale(1.0f)
	, mRotation(Quaternion::Identity)
	, mGame(game)
	, mRecomputeWorldTransform(true)
{
	mGame->AddActor(this);
}

Actor::~Actor()
{
	mGame->RemoveActor(this);

	// component들을 지워야 한다
	// ~Component 함수가 RemoveComponent()를 호출하므로, 다른 스타일의 루프를 사용하자
	while (!mComponents.empty())
	{
		delete mComponents.back();	// 여기서의 delete로 인해 자동으로 mComponents의 소멸자가 호출된다!
	}
}

void Actor::Update(float deltaTime)
{
	if (mState == EActive || mState == EXInput)
	{
		ComputeWorldTransform();

		UpdateComponents(deltaTime);
		UpdateActor(deltaTime);

		// UpdageActor에 의해 잠정적으로 엑터가 다시 변환되었을 거라고 가정해서 한번 더 호출한다
		ComputeWorldTransform();
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

void Actor::ProcessInput(const struct InputState& state)
{
	if (mState == EActive && mState != EXInput)
	{
		for (auto comp : mComponents)
		{
			comp->ProcessInput(state);
		}
		
		ActorInput(state);
	}
}

void Actor::ActorInput(const struct InputState& state)
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

void Actor::ComputeWorldTransform()
{
	if (mRecomputeWorldTransform)
	{
		mRecomputeWorldTransform = false;
		// 스케일 회전 이동 순으로 결합한다
		mWorldTransform = Matrix4::CreateScale(mScale);
		mWorldTransform *= Matrix4::CreateFromQuaternion(mRotation);
		mWorldTransform *= Matrix4::CreateTranslation(mPosition);
		//mWorldTransform *= Matrix4::CreateTranslation(Vector3(-512.0f, -384.0f, 0.0f));

		// 컴포넌트들에게 세계변환이 갱신되었다고 공지
		for (auto comp : mComponents)
		{
			comp->OnUpdateWorldTransform();
		}
	}
}

void Actor::RotateToNewForward(const Vector3& forward)
{
	// 단위 벡터와 진행 방향의 내적을 구한다
	float dot = Vector3::Dot(Vector3::UnitX, forward);
	float angle = Math::Acos(dot);

	// 진행 방향이 +X인가?
	if (dot > 0.9999f)
	{
		SetRotation(Quaternion::Identity);
	}
	// -X로 향하고 있는가?
	else if (dot < -0.9999f)
	{
		SetRotation(Quaternion(Vector3::UnitZ, Math::Pi));
	}
	else
	{
		// 외적을 통해 얻은 축을 기준으로 회전하라
		Vector3 axis = Vector3::Cross(Vector3::UnitZ, forward);
		axis.Normalize();
		SetRotation(Quaternion(axis, angle));
	}
}