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

	// closestT를 무한대 값으로 초기화
	// 그래서 최초의 교차는 항상 closestT를 갱신한다
	float closestT = Math::Infinity;
	Vector3 norm;

	// 모든 박스를 대상으로 테스트
	for (auto box : mBoxes)
	{
		float t;
		// 선분과 박스가 교차하는가?
		if (Intersect(l, box->GetWorldBox(), t, norm))
		{
			// 이 교차가 이전 교차보다 시작점에서 가까운가?
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
	// min.x로 정렬 (min.x가 작은 것부터 차례대로 왼쪽에서부터 정렬해놓는다)
	std::sort(mBoxes.begin(), mBoxes.end(),
		[](BoxComponent* a, BoxComponent* b) {
		return a->GetWorldBox().mMin.x < b->GetWorldBox().mMin.x;
	});

	for (size_t i = 0; i < mBoxes.size(); i++)
	{
		// for box[i]의 max.x값을 얻는다
		BoxComponent* a = mBoxes[i];
		float max = a->GetWorldBox().mMax.x;
		for (size_t j = i + 1; j < mBoxes.size(); j++)
		{
			BoxComponent* b = mBoxes[j];
			// box[j] min.x값이 box[i]의 max.x보다 크다면,
			// box[i]와 가능한 충돌은 내부 루프에서 존재하지 않으므로 빠져나온다
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