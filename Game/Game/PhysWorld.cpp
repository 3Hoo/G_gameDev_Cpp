#include "PhysWorld.h"
#include "Game.h"
#include "BoxComponent.h"
#include "Actor.h"

PhysWorld::PhysWorld(Game* game)
	: mGame(game)
{

}

bool PhysWorld::SegmentCast(const LineSegment& l, CollisionInfo& outColl)
{
	bool collided = false;

	// closestT�� ���Ѵ� ������ �ʱ�ȭ
	// �׷��� ������ ������ �׻� closestT�� �����Ѵ�
	float closestT = Math::Infinity;
	Vector3 norm;

	// ��� �ڽ��� ������� �׽�Ʈ
	for (auto box : mBoxes)
	{
		float t;
		// ���а� �ڽ��� �����ϴ°�?
		if (Intersect(l, box->GetWorldBox(), t, norm))
		{
			// �� ������ ���� �������� ���������� ����?
			if (t < closestT)
			{
				closestT = t;
				outColl.mPoint = l.PointOnSegment(t);
				outColl.mNormal = norm;
				outColl.mBox = box;
				outColl.mActor = box->GetOwner();
				collided = true;
			}
		}
	}

	return collided;
}

void PhysWorld::TestPairwise(std::function<void(Actor*, Actor*)> f)
{
	if (mBoxes.size() != 0)
	{
		// Naive implementation O(n^2)
		for (size_t i = 0; i < mBoxes.size(); i++)
		{
			// Don't need to test vs itself and any previous i values
			for (size_t j = i + 1; j < mBoxes.size(); j++)
			{
				BoxComponent* a = mBoxes[i];
				BoxComponent* b = mBoxes[j];
				if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
				{
					// Call supplied function to handle intersection
					f(a->GetOwner(), b->GetOwner());
				}
			}
		}
	}
}

void PhysWorld::TestSweepAndPrune(std::function<void(Actor*, Actor*)> f)
{
	// min.x�� ���� (min.x�� ���� �ͺ��� ���ʴ�� ���ʿ������� �����س��´�)
	std::sort(mBoxes.begin(), mBoxes.end(),
		[](BoxComponent* a, BoxComponent* b) {
		return a->GetWorldBox().mMin.x < b->GetWorldBox().mMin.x;
	});

	for (size_t i = 0; i < mBoxes.size(); i++)
	{
		// for box[i]�� max.x���� ��´�
		BoxComponent* a = mBoxes[i];
		float max = a->GetWorldBox().mMax.x;
		for (size_t j = i + 1; j < mBoxes.size(); j++)
		{
			BoxComponent* b = mBoxes[j];
			// box[j] min.x���� box[i]�� max.x���� ũ�ٸ�,
			// box[i]�� ������ �浹�� ���� �������� �������� �����Ƿ� �������´�
			if (b->GetWorldBox().mMin.x > max)
			{
				break;
			}
			else if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
			{
				f(a->GetOwner(), b->GetOwner());
			}
		}
	}
}

void PhysWorld::AddBox(BoxComponent* box)
{
	mBoxes.emplace_back(box);
}

void PhysWorld::RemoveBox(BoxComponent* box)
{
	auto iter = std::find(mBoxes.begin(), mBoxes.end(), box);
	if (iter != mBoxes.end())
	{
		std::iter_swap(iter, mBoxes.end() - 1);
		mBoxes.pop_back();
	}
}