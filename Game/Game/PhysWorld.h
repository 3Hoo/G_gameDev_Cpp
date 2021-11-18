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

	// �ݸ��� result�� ������ ������ �����ϴ� ����ü
	struct CollisionInfo
	{
		// �浹 ����
		Vector3 mPoint;
		// �浹 ���� ���� ����
		Vector3 mNormal;
		// �浹�� ������Ʈ
		class BoxComponent* mBox;
		// ������Ʈ�� ������ ����
		class Actor* mActor;
	};

	// �ڽ� ������Ʈ�� �߰�/����
	void AddBox(class BoxComponent* box);
	void RemoveBox(class BoxComponent* box);

	/* ���� �Լ��� */
	
	// ������ �Ķ���ͷ� �޾� ������ �ڽ��� �����ϸ� true�� ����
	// ���� ����� ������ ���� �߿��� �������� ����
	bool SegmentCast(const LineSegment& l, CollisionInfo& outColl);

	void TestPairwise(std::function<void(Actor*, Actor*)> f);

	// ������ ��� ���� �� ���տ� �浹  �׽�Ʈ�� �����Ѵ�. SAP �˰����� �̿��ؼ� ȿ������ ������
	// ���� �ڽ��� ���� �����ϸ�, ������ ������ �Լ� f�� ȣ���Ѵ�
	void TestSweepAndPrune(std::function<void(Actor*, Actor*)> f);


private:
	class Game* mGame;
	std::vector<class BoxComponent*> mBoxes;
};

