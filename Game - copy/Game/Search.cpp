#include <vector>
#include <unordered_map>
#include <queue>

/* 기본 그래프 */
struct GraphNode
{
	// 각 노드는 인접 노드의 포인터를 가지고 있다
	std::vector<GraphNode*> mAdjacent;
};
struct Graph
{
	// 그래프는 노드들을 포함한다
	std::vector<GraphNode*> mNodes;
};


/* 가중치 그래프 */
struct WeightedEdge
{
	// 이 에지에 어떤 노드들이 연결되어 있는가?
	struct WeightedGraphNode* mFrom;
	struct WeightedGraphNode* mTo;

	// 이 에지의 가중치
	float mWeight;
};
struct WeightedGraphNode
{
	// 외부로 향하는 에지들을 저장한다
	std::vector<WeightedEdge*> mEdges;
};
struct WeightedGraph
{
	// 가중 그래프는 가중치 노드들을 포함한다
	std::vector<const WeightedGraphNode*> mNodes;
};


/* BFS 알고리즘 */
using NodeToParentMap = std::unordered_map<const GraphNode*, const GraphNode*>;

bool BFS(const Graph& graph, const GraphNode* start, const GraphNode* goal, NodeToParentMap& outMap)
{
	// 경로를 찾았는지를 알 수 있는 플래그
	bool pathFound = false;

	// 고려해야 할 노드
	std::queue<const GraphNode*> q;

	// 시작 노드를 큐에 넣는다
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

		// 큐에는 없는 인접 노드를 꺼낸다
		for (const GraphNode* node : current->mAdjacent)
		{
			// 시작 노드를 제외하고, 부모가 null이면 큐에 넣지 않았던 노드이다
			const GraphNode* parent = outMap[node];

			if (parent == nullptr && node != start)
			{
				// 이 노드의 부모 노드를 설정하고 큐에 넣는다
				outMap[node] = current;
				q.emplace(node);
			}
		}
	}

	return pathFound;
}


/* GBFS 알고리즘 */
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
	// 원래는 a노드와 goal노드의 실제 유클리드 거리를 리턴한다
	return 0.0f;
}

bool GBFS(const WeightedGraph& graph, const WeightedGraphNode* start, const WeightedGraphNode* goal, GBFSMap& outMap)
{
	std::vector<const WeightedGraphNode*> openSet;

	const WeightedGraphNode* current = start;
	outMap[current].mInClosedSet = true;

	do
	{
		// 인접 노드들을 열린 집합에 추가한다
		for (const WeightedEdge* edge : current->mEdges)
		{
			const WeightedGraphNode* neighbor = edge->mTo;

			// 인접 노드들에 대한 추가 데이터를 확보한다
			GBFSScratch& data = outMap[neighbor];

			// 이 노드가 닫힌 집합에 없다면, 추가한다
			if (!data.mInClosedSet)
			{
				// 인접 노드의 부모 에지를 설정한다
				data.mParentEdge = edge;
				if (!data.mInOpenSet)
				{
					// 이 노드의 휴리스틱을 계산하고 열린 집합에 추가한다
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

		// 열린 집합에서 가장 비용이 낮은 노드를 찾자
		auto iter = std::min_element(openSet.begin(), openSet.end(),
			[&outMap](const WeightedGraphNode* a, const WeightedGraphNode* b)
		{
			return outMap[a].mHeuristic < outMap[b].mHeuristic;
		});
		
		// 현재 노드를 설정하고 이 노드를 열린 집합에서 닫힌 집합으로 이동시킨다
		current = *iter;
		openSet.erase(iter);
		outMap[current].mInOpenSet = false;
		outMap[current].mInClosedSet = true;

	} while (current != goal);

	return (current == goal) ? true : false;
}



/* A* 알고리즘 */
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

	// 시작점을 설정한다
	const WeightedGraphNode* current = start;
	outMap[current].mInClosedSet = true;

	do
	{
		for (const WeightedEdge* edge : current->mEdges)
		{
			// 이웃 노드
			const WeightedGraphNode* neighbor = edge->mTo;
			// 이웃 노드의 AStar 정보
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

		// 현재 노드에서 이웃 노드들(openset에 있는) 중 가장 작은 f(x) 값을 가진 노드에게 current를 넘겨준다
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


/* 틱텍토 게임 트리 */
struct GameState
{
	enum SquareState {Empty, X, O};
	SquareState mBoard[3][3];
};

struct GTNode
{
	// 자식 노드
	std::vector<GTNode*> mChildren;
	
	// 이 노드의 게임 상태
	GameState mState;
};


/* Minplayer, Maxplayer */
float GetScore(const GameState& state)
{
	// 행 빙고를 이루었는가?
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
	// 열 빙고를 이루었는가?
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
	// 대각선 빙고를 이루었는가?
	if ((state.mBoard[0][0] == state.mBoard[1][1] == state.mBoard[2][2]) || (state.mBoard[0][2] == state.mBoard[1][1] == state.mBoard[2][0]))
	{
		if (state.mBoard[1][1] == GameState::X)
			return 1.0f;
		else
			return -1.0f;
	}

	// 무승부
	return 0.0f;
}

float MinPlayer(const GTNode* node);
float MaxPlayer(const GTNode* node)
{
	// 리프 노드라면 점수를 리턴한다
	if (node->mChildren.empty())
	{
		return GetScore(node->mState);
	}

	// 최댓값을 가지는 하위 트리를 찾는다
	float maxValue = -std::numeric_limits<float>::infinity();
	for (const GTNode* child : node->mChildren)
	{
		maxValue = std::max(maxValue, MinPlayer(child));
	}
	return maxValue;
}
float MinPlayer(const GTNode* node)
{
	// 리프 노드라면 점수를 리턴
	if (node->mChildren.empty())
	{
		return GetScore(node->mState);
	}

	// 최솟값을 가지는 하위 트리를 찾는다
	float minValue = std::numeric_limits<float>::infinity();
	for (const GTNode* child : node->mChildren)
	{
		minValue = std::min(minValue, MaxPlayer(child));
	}
	return minValue;
}

const GTNode* MinimaxDecide(const GTNode* root)
{
	// 최댓값을 가진 하위 트리를 찾고 해당 선택을 저장한다
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
	const bool IsTerminal() const { return true; }	// 상태가 종료 상태면 true를 리턴
	const float GetScore() const { return 0.0f; }	// 종료되지 않은 상태의 휴리스틱 값을 리턴하거나, 종료 상태의 점수를 반환
	const std::vector<newGameState*> GetPossibleMoves() const { std::vector<newGameState*> a; return a; };	// 현재 상태 이후에 움직일 수 있는 이동에 대한 게임 상태 백터를 리턴

};
float MinPlayerLimit(const newGameState* state, int depth);
float MaxPlayerLimit(const newGameState* state, int depth)
{
	// depth가 0이거나, 상태가 terminal이면 최대 깊이에 도달했다는 의미
	if (depth == 0 || state->IsTerminal())
	{
		return state->GetScore();
	}

	// 최댓값을 가진 하위 트리를 찾는다
	float maxValue = -std::numeric_limits<float>::infinity();

	for (const newGameState* child : state->GetPossibleMoves())
	{
		maxValue = std::max(maxValue, MinPlayerLimit(child, depth - 1));
	}
	return maxValue;
}
float MinPlayerLimit(const newGameState* state, int depth)
{
	// depth가 0이거나, 상태가 terminal이면 최대 깊이에 도달했다는 의미
	if (depth == 0 || state->IsTerminal())
	{
		return state->GetScore();
	}

	// 최댓값을 가진 하위 트리를 찾는다
	float minValue = std::numeric_limits<float>::infinity();

	for (const newGameState* child : state->GetPossibleMoves())
	{
		minValue = std::min(minValue, MaxPlayerLimit(child, depth - 1));
	}
	return minValue;
}


/* 알파 베타 가지치기 */
float AlphaBetaMin(const newGameState* node, int depth, float alpha, float beta);
const newGameState* AlphaBetaDecide(const newGameState* root, int maxDepth)
{
	const newGameState* choice = nullptr;
	// 알파는 음의 무한대, 베타는 양의 무한대에서 시작한다
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