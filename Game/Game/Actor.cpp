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
		ComputeWorldTransform();

		UpdateComponents(deltaTime);
		UpdateActor(deltaTime);

		// UpdageActor�� ���� ���������� ���Ͱ� �ٽ� ��ȯ�Ǿ��� �Ŷ�� �����ؼ� �ѹ� �� ȣ���Ѵ�
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
		// ������ ȸ�� �̵� ������ �����Ѵ�
		mWorldTransform = Matrix4::CreateScale(mScale);
		mWorldTransform *= Matrix4::CreateFromQuaternion(mRotation);
		mWorldTransform *= Matrix4::CreateTranslation(mPosition);
		//mWorldTransform *= Matrix4::CreateTranslation(Vector3(-512.0f, -384.0f, 0.0f));

		// ������Ʈ�鿡�� ���躯ȯ�� ���ŵǾ��ٰ� ����
		for (auto comp : mComponents)
		{
			comp->OnUpdateWorldTransform();
		}
	}
}

void Actor::RotateToNewForward(const Vector3& forward)
{
	// ���� ���Ϳ� ���� ������ ������ ���Ѵ�
	float dot = Vector3::Dot(Vector3::UnitX, forward);
	float angle = Math::Acos(dot);

	// ���� ������ +X�ΰ�?
	if (dot > 0.9999f)
	{
		SetRotation(Quaternion::Identity);
	}
	// -X�� ���ϰ� �ִ°�?
	else if (dot < -0.9999f)
	{
		SetRotation(Quaternion(Vector3::UnitZ, Math::Pi));
	}
	else
	{
		// ������ ���� ���� ���� �������� ȸ���϶�
		Vector3 axis = Vector3::Cross(Vector3::UnitZ, forward);
		axis.Normalize();
		SetRotation(Quaternion(axis, angle));
	}
}