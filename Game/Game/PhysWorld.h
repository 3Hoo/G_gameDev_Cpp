#pragma once

#include <vector>
#include <functional>
#include <algorithm>
#include "Math.h"
#include "Collision.h"

class PhysWorld
{
public:
	PhysWorld(class Game* game);

	// 콜리전 result에 유용한 정보를 저장하는 구조체
	struct CollisionInfo
	{
		// 충돌 지점
		Vector3 mPoint;
		// 충돌 시의 법선 벡터
		Vector3 mNormal;
		// 충돌한 컴포넌트
		class BoxComponent* mBox;
		// 컴포넌트의 소유자 액터
		class Actor* mActor;
	};

	// 박스 컴포넌트의 추가/제거
	void AddBox(class BoxComponent* box);
	void RemoveBox(class BoxComponent* box);

	/* 내부 함수들 */
	
	// 선분을 파라미터로 받아 선분이 박스와 교차하면 true를 리턴
	// 가장 가까운 교차가 가장 중요한 교차임을 가정
	bool SegmentCast(const LineSegment& l, CollisionInfo& outColl);

	void TestPairwise(std::function<void(Actor*, Actor*)> f);

	// 세계의 모든 상자 쌍 조합에 충돌  테스트를 수행한다. SAP 알고리즘을 이용해서 효율성을 높였다
	// 만약 박스가 서로 교차하면, 유저가 정의한 함수 f를 호출한다
	void TestSweepAndPrune(std::function<void(Actor*, Actor*)> f);


private:
	class Game* mGame;
	std::vector<class BoxComponent*> mBoxes;
};

