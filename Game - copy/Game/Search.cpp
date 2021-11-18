#include <vector>
#include <unordered_map>
#include <queue>

/* �⺻ �׷��� */
struct GraphNode
{
	// �� ���� ���� ����� �����͸� ������ �ִ�
	std::vector<GraphNode*> mAdjacent;
};
struct Graph
{
	// �׷����� ������ �����Ѵ�
	std::vector<GraphNode*> mNodes;
};


/* ����ġ �׷��� */
struct WeightedEdge
{
	// �� ������ � ������ ����Ǿ� �ִ°�?
	struct WeightedGraphNode* mFrom;
	struct WeightedGraphNode* mTo;

	// �� ������ ����ġ
	float mWeight;
};
struct WeightedGraphNode
{
	// �ܺη� ���ϴ� �������� �����Ѵ�
	std::vector<WeightedEdge*> mEdges;
};
struct WeightedGraph
{
	// ���� �׷����� ����ġ ������ �����Ѵ�
	std::vector<const WeightedGraphNode*> mNodes;
};


/* BFS �˰��� */
using NodeToParentMap = std::unordered_map<const GraphNode*, const GraphNode*>;

bool BFS(const Graph& graph, const GraphNode* start, const GraphNode* goal, NodeToParentMap& outMap)
{
	// ��θ� ã�Ҵ����� �� �� �ִ� �÷���
	bool pathFound = false;

	// ����ؾ� �� ���
	std::queue<const GraphNode*> q;

	// ���� ��带 ť�� �ִ´�
	q.emplace(start);

	while (!q.empty())
	{
		const GraphNode* current = q.front();
		q.pop();
		if (current == goal)
		{
			pathFound = true;
			break;
		}

		// ť���� ���� ���� ��带 ������
		for (const GraphNode* node : current->mAdjacent)
		{
			// ���� ��带 �����ϰ�, �θ� null�̸� ť�� ���� �ʾҴ� ����̴�
			const GraphNode* parent = outMap[node];

			if (parent == nullptr && node != start)
			{
				// �� ����� �θ� ��带 �����ϰ� ť�� �ִ´�
				outMap[node] = current;
				q.emplace(node);
			}
		}
	}

	return pathFound;
}


/* GBFS �˰��� */
struct GBFSScratch
{
	const WeightedEdge* mParentEdge = nullptr;
	float mHeuristic = 0.0f;
	bool mInOpenSet = false;
	bool mInClosedSet = false;
};

using GBFSMap = std::unordered_map<const WeightedGraphNode*, GBFSScratch>;

float ComputeHeuristic(const WeightedGraphNode* a, const WeightedGraphNode* goal)
{
	// ������ a���� goal����� ���� ��Ŭ���� �Ÿ��� �����Ѵ�
	return 0.0f;
}

bool GBFS(const WeightedGraph& graph, const WeightedGraphNode* start, const WeightedGraphNode* goal, GBFSMap& outMap)
{
	std::vector<const WeightedGraphNode*> openSet;

	const WeightedGraphNode* current = start;
	outMap[current].mInClosedSet = true;

	do
	{
		// ���� ������ ���� ���տ� �߰��Ѵ�
		for (const WeightedEdge* edge : current->mEdges)
		{
			const WeightedGraphNode* neighbor = edge->mTo;

			// ���� ���鿡 ���� �߰� �����͸� Ȯ���Ѵ�
			GBFSScratch& data = outMap[neighbor];

			// �� ��尡 ���� ���տ� ���ٸ�, �߰��Ѵ�
			if (!data.mInClosedSet)
			{
				// ���� ����� �θ� ������ �����Ѵ�
				data.mParentEdge = edge;
				if (!data.mInOpenSet)
				{
					// �� ����� �޸���ƽ�� ����ϰ� ���� ���տ� �߰��Ѵ�
					data.mHeuristic = ComputeHeuristic(neighbor, goal);
					data.mInOpenSet = true;
					openSet.emplace_back(neighbor);
				}
			}
		}

		if (openSet.empty())
		{
			break;
		}

		// ���� ���տ��� ���� ����� ���� ��带 ã��
		auto iter = std::min_element(openSet.begin(), openSet.end(),
			[&outMap](const WeightedGraphNode* a, const WeightedGraphNode* b)
		{
			return outMap[a].mHeuristic < outMap[b].mHeuristic;
		});
		
		// ���� ��带 �����ϰ� �� ��带 ���� ���տ��� ���� �������� �̵���Ų��
		current = *iter;
		openSet.erase(iter);
		outMap[current].mInOpenSet = false;
		outMap[current].mInClosedSet = true;

	} while (current != goal);

	return (current == goal) ? true : false;
}



/* A* �˰��� */
struct AStarScratch
{
	const WeightedEdge* mParentEdge = nullptr;
	float mHeuristic = 0.0f;
	float mActualFromStart = 0.0f;
	bool mInOpenSet = false;
	bool mInClosedSet = false;
};

using AStarMap = std::unordered_map<const WeightedGraphNode*, AStarScratch>;

bool AStar(const WeightedGraph& graph, const WeightedGraphNode* start, const WeightedGraphNode* goal, AStarMap& outMap)
{
	std::vector< const WeightedGraphNode*> openSet;

	// �������� �����Ѵ�
	const WeightedGraphNode* current = start;
	outMap[current].mInClosedSet = true;

	do
	{
		for (const WeightedEdge* edge : current->mEdges)
		{
			// �̿� ���
			const WeightedGraphNode* neighbor = edge->mTo;
			// �̿� ����� AStar ����
			AStarScratch& data = outMap[neighbor];

			if (!data.mInClosedSet)
			{
				if (!data.mInOpenSet)
				{
					data.mParentEdge = edge;
					data.mHeuristic = ComputeHeuristic(neighbor, goal);
					data.mActualFromStart = outMap[current].mActualFromStart + edge->mWeight;
					data.mInOpenSet = true;
					openSet.emplace_back(neighbor);
				}
				else
				{
					float newActual = outMap[current].mActualFromStart + edge->mWeight;
					if (newActual < data.mActualFromStart)
					{
						data.mActualFromStart = newActual;
						data.mParentEdge = edge;
					}
				}
			}
		}

		if (openSet.empty())
		{
			break;
		}

		// ���� ��忡�� �̿� ����(openset�� �ִ�) �� ���� ���� f(x) ���� ���� ��忡�� current�� �Ѱ��ش�
		auto iter = std::min_element(openSet.begin(), openSet.end(),
			[&outMap](const WeightedGraphNode* a, const WeightedGraphNode* b)
		{
			return (outMap[a].mActualFromStart + outMap[a].mHeuristic) < (outMap[b].mActualFromStart + outMap[b].mHeuristic);
		});

		current = *iter;
		outMap[*iter].mInClosedSet = true;
		outMap[*iter].mInOpenSet = false;
		openSet.erase(iter);

	} while (current != goal);
	
	return (current == goal) ? true : false;
}


/* ƽ���� ���� Ʈ�� */
struct GameState
{
	enum SquareState {Empty, X, O};
	SquareState mBoard[3][3];
};

struct GTNode
{
	// �ڽ� ���
	std::vector<GTNode*> mChildren;
	
	// �� ����� ���� ����
	GameState mState;
};


/* Minplayer, Maxplayer */
float GetScore(const GameState& state)
{
	// �� ���� �̷���°�?
	for (int i = 0; i < 3; i++)
	{
		bool same = true;
		GameState::SquareState v = state.mBoard[i][0];
		for (int j = 1; j < 3; j++)
		{
			if (state.mBoard[i][j] != v)
			{
				same = false;
			}
		}

		if (same)
		{
			if (v == GameState::X)
			{
				return 1.0f;
			}
			else
			{
				return -1.0f;
			}
		}
	}
	// �� ���� �̷���°�?
	for (int i = 0; i < 3; i++)
	{
		bool same = true;
		GameState::SquareState v = state.mBoard[0][i];
		for (int j = 1; j < 3; j++)
		{
			if (state.mBoard[j][i] != v)
			{
				same = false;
			}
		}

		if (same)
		{
			if (v == GameState::X)
			{
				return 1.0f;
			}
			else
			{
				return -1.0f;
			}
		}
	}
	// �밢�� ���� �̷���°�?
	if ((state.mBoard[0][0] == state.mBoard[1][1] == state.mBoard[2][2]) || (state.mBoard[0][2] == state.mBoard[1][1] == state.mBoard[2][0]))
	{
		if (state.mBoard[1][1] == GameState::X)
			return 1.0f;
		else
			return -1.0f;
	}

	// ���º�
	return 0.0f;
}

float MinPlayer(const GTNode* node);
float MaxPlayer(const GTNode* node)
{
	// ���� ����� ������ �����Ѵ�
	if (node->mChildren.empty())
	{
		return GetScore(node->mState);
	}

	// �ִ��� ������ ���� Ʈ���� ã�´�
	float maxValue = -std::numeric_limits<float>::infinity();
	for (const GTNode* child : node->mChildren)
	{
		maxValue = std::max(maxValue, MinPlayer(child));
	}
	return maxValue;
}
float MinPlayer(const GTNode* node)
{
	// ���� ����� ������ ����
	if (node->mChildren.empty())
	{
		return GetScore(node->mState);
	}

	// �ּڰ��� ������ ���� Ʈ���� ã�´�
	float minValue = std::numeric_limits<float>::infinity();
	for (const GTNode* child : node->mChildren)
	{
		minValue = std::min(minValue, MaxPlayer(child));
	}
	return minValue;
}

const GTNode* MinimaxDecide(const GTNode* root)
{
	// �ִ��� ���� ���� Ʈ���� ã�� �ش� ������ �����Ѵ�
	const GTNode* choice = nullptr;
	float maxValue = -std::numeric_limits<float>::infinity();
	
	for (const GTNode* child : root->mChildren)
	{
		float v = MinPlayer(child);
		if (v > maxValue)
		{
			maxValue = v;
			choice = child;
		}
	}
	return choice;
}


struct newGameState
{
public:
	const bool IsTerminal() const { return true; }	// ���°� ���� ���¸� true�� ����
	const float GetScore() const { return 0.0f; }	// ������� ���� ������ �޸���ƽ ���� �����ϰų�, ���� ������ ������ ��ȯ
	const std::vector<newGameState*> GetPossibleMoves() const { std::vector<newGameState*> a; return a; };	// ���� ���� ���Ŀ� ������ �� �ִ� �̵��� ���� ���� ���� ���͸� ����

};
float MinPlayerLimit(const newGameState* state, int depth);
float MaxPlayerLimit(const newGameState* state, int depth)
{
	// depth�� 0�̰ų�, ���°� terminal�̸� �ִ� ���̿� �����ߴٴ� �ǹ�
	if (depth == 0 || state->IsTerminal())
	{
		return state->GetScore();
	}

	// �ִ��� ���� ���� Ʈ���� ã�´�
	float maxValue = -std::numeric_limits<float>::infinity();

	for (const newGameState* child : state->GetPossibleMoves())
	{
		maxValue = std::max(maxValue, MinPlayerLimit(child, depth - 1));
	}
	return maxValue;
}
float MinPlayerLimit(const newGameState* state, int depth)
{
	// depth�� 0�̰ų�, ���°� terminal�̸� �ִ� ���̿� �����ߴٴ� �ǹ�
	if (depth == 0 || state->IsTerminal())
	{
		return state->GetScore();
	}

	// �ִ��� ���� ���� Ʈ���� ã�´�
	float minValue = std::numeric_limits<float>::infinity();

	for (const newGameState* child : state->GetPossibleMoves())
	{
		minValue = std::min(minValue, MaxPlayerLimit(child, depth - 1));
	}
	return minValue;
}


/* ���� ��Ÿ ����ġ�� */
float AlphaBetaMin(const newGameState* node, int depth, float alpha, float beta);
const newGameState* AlphaBetaDecide(const newGameState* root, int maxDepth)
{
	const newGameState* choice = nullptr;
	// ���Ĵ� ���� ���Ѵ�, ��Ÿ�� ���� ���Ѵ뿡�� �����Ѵ�
	float maxValue = -std::numeric_limits<float>::infinity();
	float beta = std::numeric_limits<float>::infinity();

	for (const newGameState* child : root->GetPossibleMoves())
	{
		float v = AlphaBetaMin(child, maxDepth - 1, maxValue, beta);
		if (v > maxValue)
		{
			maxValue = v;
			choice = child;
		}
	}

	return choice;
}

float AlphaBetaMax(const newGameState* node, int depth, float alpha, float beta)
{
	if (depth == 0 || node->IsTerminal())
	{
		return node->GetScore();
	}
	float maxValue = -std::numeric_limits<float>::infinity();
	for (const newGameState* child : node->GetPossibleMoves())
	{
		maxValue = std::max(maxValue, AlphaBetaMin(child, depth - 1, alpha, beta));
		if (maxValue >= beta)
		{
			return maxValue;
		}
		alpha = std::max(maxValue, alpha);
	}
	return maxValue;
}
float AlphaBetaMin(const newGameState* node, int depth, float alpha, float beta)
{
	if (depth == 0 || node->IsTerminal())
	{
		return node->GetScore();
	}
	float minValue = std::numeric_limits<float>::infinity();
	for (const newGameState* child : node->GetPossibleMoves())
	{
		minValue = std::min(minValue, AlphaBetaMax(child, depth - 1, alpha, beta));
		if (minValue <= alpha)
		{
			return minValue;
		}
		beta = std::min(minValue, beta);
	}
	return minValue;
}