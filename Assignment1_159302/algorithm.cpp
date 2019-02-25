#include "algorithm.h"
#include "Deque.h"
#include "HashMap.h"
#include "ArrayMap.h"
#include "PriorityQueue.h"
#include "Vector.h"
#include "StackSlabAllocator.h"

using namespace std;

#define USE_HASH_MAP

template<class T, class H, class E>
#ifdef USE_HASH_MAP
using VisitedList = HashMap<T, H, E>;
#else
using VisitedList = ArrayMap<T, E>;
#endif

/*
* Stores the state of the puzzle
*/
template<size_t mWidth, size_t mHeight, class TileType = char>
struct Map
{
	constexpr static size_t width = mWidth, height = mHeight;
	TileType tiles[width * height];
	size_t emptyPos;

	bool operator==(const Map<width, height>& other) const
	{
		constexpr size_t tileCount = width * height;
		for (size_t i = 0u; i != tileCount; ++i)
		{
			if (tiles[i] != other.tiles[i])
			{
				return false;
			}
		}
		return emptyPos == other.emptyPos;
	}

	bool operator!=(const Map<width, height>& other) const
	{
		return !(*this == other);
	}
};

/*
* Stores the state of the puzzle with the number of moves required to get to that state from the starting state
*/
template<size_t width, size_t height, class TileType = char>
struct MapWithDepth : Map<width, height, TileType>
{
	unsigned int depth;
};

/*
* Used to create a singly linked list such as the path to the current state.
*/
template<class value_type>
struct ListNode
{
	value_type data;
	ListNode* next;
};

/*
* Allows another data structure such as a HashMap to effectively store pointers to the elements in a container such as a PriorityQueue despited them constantly moving.
*/
template<class Map_t>
struct LocationTracker
{
	Map_t data;

	LocationTracker(Map_t data) : data(data) {}

	LocationTracker(const LocationTracker& other) : data(other.data)
	{
		data->data.positionInQueue = this;
	}

	void operator=(const LocationTracker& other)
	{
		data = other.data;
		data->data.positionInQueue = this;
	}
};

/*
* Stores the state of the puzzle plus its heuristic information used by A*
*/
template<size_t width, size_t height, class TileType = char>
struct MapWithHuristic : Map<width, height, TileType>
{
	unsigned int g;
	unsigned int f;
#ifdef USE_HASH_MAP
	LocationTracker<ListNode<MapWithHuristic>*>* positionInQueue;
#endif
};

/*
* Hashes the a state of the puzzle.
*/
template<class Map_t, size_t multiplier = 31u>
struct Hasher
{
	size_t operator()(const Map_t* value) const
	{
		size_t result = value->emptyPos;
		for (const auto tile : value->tiles)
		{
			result = result * multiplier + tile;
		}
		return result;
	}
};

/*
* Compares the values pointed to by two pointers for equality.
*/
template<class Map_t>
struct EqualityTester
{
	constexpr bool operator()(const Map_t* value1, const Map_t* value2) const
	{
		return *value1 == *value2;
	}
};

/*
* Hashes a puzzle state that is using a LocationTracker.
*/
template<class Map_t, size_t multiplier = 31u>
struct LocationTrackerHasher : Hasher<Map_t, multiplier>
{
	size_t operator()(const LocationTracker<ListNode<Map_t>*>& value) const
	{
		return (*reinterpret_cast<const Hasher<Map_t, multiplier>*>(this))(&value.data->data);
	}
};

/*
* Compares two puzzle states that are using a LocationTracker for equality.
*/
template<class Map_t>
struct LocationTrackerEqualityTester
{
	constexpr bool operator()(const LocationTracker<ListNode<Map_t>*>& value1, const LocationTracker<ListNode<Map_t>*>& value2) const
	{
		return value1.data->data == value2.data->data;
	}
};

/*
* Hashes a value stored in a list node not the list node itself.
*/
template<class Map_t, size_t multiplier = 31u>
struct ListNodeHasher : Hasher<Map_t, multiplier>
{
	size_t operator()(const ListNode<Map_t>* value) const
	{
		return (*reinterpret_cast<const Hasher<Map_t, multiplier>*>(this))(&value->data);
	}
};

/*
* Tests if to values stored in list nodes are equal.
*/
template<class Map_t>
struct ListNodeEqualityTester
{
	constexpr bool operator()(const ListNode<Map_t>* value1, const ListNode<Map_t>* value2) const
	{
		return value1->data == value2->data;
	}
};

/*
* Returns true if the first heuristic is greater than the second. Can be used to create a min PriorityQueue by heuristic.
*/
template<class Map_t>
struct GreaterHeuistic
{
	constexpr bool operator()(const ListNode<Map_t>* value1, const ListNode<Map_t>* value2) const
	{
		return value1->data.f > value2->data.f;
	}
};

template<class Map_t>
struct GreaterHeuisticTracker
{
	constexpr bool operator()(const LocationTracker<ListNode<Map_t>*>& value1, const LocationTracker<ListNode<Map_t>*>& value2) const
	{
		return value1.data->data.f > value2.data->data.f;
	}
};

/*
* Converts a path as a linked list of states into a std:string of moves.
*/
template<size_t width, size_t height, class TileType, template<size_t, size_t, class> class Map>
static std::string packPath(ListNode<Map<width, height, TileType>>* finalPath)
{
	std::string moves;
	size_t length = 0u;
	for (ListNode<Map<width, height, TileType>>* current = finalPath->next; current != nullptr; current = current->next)
	{
		++length;
	}
	moves.resize(length);
	for (size_t i = length; i != 0u;)
	{
		--i;
		size_t newEmptyPos = finalPath->data.emptyPos;
		finalPath = finalPath->next;
		size_t oldEmptyPos = finalPath->data.emptyPos;
		size_t difference = newEmptyPos - oldEmptyPos;
		if (difference == 1)
		{
			moves[i] = 'R';
		}
		else if (difference == width)
		{
			moves[i] = 'D';
		}
		else if (difference == std::numeric_limits<size_t>::max())
		{
			moves[i] = 'L';
		}
		else
		{
			moves[i] = 'U';
		}
	}
	return moves;
}

template<class Queue, class Visited, class Map, class Allocator>
void expandPathDepthFirstVisited(Map& currentState, ListNode<Map>* currentPath, size_t newPos, size_t oldPos, Queue& posiblePaths,
	Visited& visited, Allocator& allocator)
{
	ListNode<Map>* currentNode = (ListNode<Map>*)allocator.getNext();
	new(currentNode) ListNode<Map>{ currentState, currentPath };
	currentNode->data.emptyPos = newPos;
	currentNode->data.tiles[oldPos] = currentNode->data.tiles[currentNode->data.emptyPos];
	currentNode->data.tiles[currentNode->data.emptyPos] = 0;
	if (visited.find(&currentNode->data) != visited.end())
	{
		currentNode->~ListNode<Map>();
		allocator.replaceLast();
	}
	else
	{
		visited.insert(&currentNode->data);
		posiblePaths.push_back(currentNode);
	}
}

template<class Queue, class Visited, class Map, class Allocator>
void expandPathDepthLimitedVisited(Map& currentState, ListNode<Map>* currentPath, size_t newPos, size_t oldPos, Queue& posiblePaths,
	Visited& visited, Allocator& allocator)
{
	ListNode<Map>* currentNode = (ListNode<Map>*)allocator.getNext();
	new(currentNode) ListNode<Map>{ currentState, currentPath };
	currentNode->data.emptyPos = newPos;
	currentNode->data.tiles[oldPos] = currentNode->data.tiles[currentNode->data.emptyPos];
	currentNode->data.tiles[currentNode->data.emptyPos] = 0;
	currentNode->data.depth = currentState.depth + 1u;
	auto visitedState = visited.find(&currentNode->data);
	if (visitedState != visited.end())
	{
		if ((*visitedState)->depth > currentNode->data.depth)
		{
			*visitedState = &currentNode->data;
			posiblePaths.push_back(currentNode);
		}
		else
		{
			currentNode->~ListNode<Map>();
			allocator.replaceLast();
		}
	}
	else
	{
		visited.insert(&currentNode->data);
		posiblePaths.push_back(currentNode);
	}
}

template<class Queue, class Map, class Allocator>
void expandPathDepthFirst(Map& currentState, ListNode<Map>* currentPath, size_t newPos, size_t oldPos, Queue& posiblePaths, Allocator& allocator)
{
	ListNode<Map>* currentNode = (ListNode<Map>*)allocator.getNext();
	new(currentNode) ListNode<Map>{ currentState, currentPath };
	currentNode->data.emptyPos = newPos;
	currentNode->data.tiles[oldPos] = currentNode->data.tiles[currentNode->data.emptyPos];
	currentNode->data.tiles[currentNode->data.emptyPos] = 0;
	posiblePaths.push_back(currentNode);
}

template<class Queue, class Map, class Allocator>
void expandPathDepthLimited(Map& currentState, ListNode<Map>* currentPath, size_t newPos, size_t oldPos, Queue& posiblePaths, Allocator& allocator)
{
	expandPathDepthFirst(currentState, currentPath, newPos, oldPos, posiblePaths, allocator);
	posiblePaths.back()->data.depth = currentState.depth + 1u;
}


///////////////////////////////////////////////////////////////////////////////////////////
//
// Search Algorithm:  Breadth-First Search 
//
// Move Generator:  
//
////////////////////////////////////////////////////////////////////////////////////////////
string breadthFirstSearch(string const& initialState, string const& goalState, int &numOfStateExpansions, int& maxQLength, float &actualRunningTime){
    string path;
	clock_t startTime;
    //add necessary variables here
	constexpr size_t width = 3, height = 3;
	using Map_t = Map<width, height>;
	numOfStateExpansions = 0;
	Map<width, height> goal;
	Deque<ListNode<Map_t>*> posiblePaths;
	ListNode<Map_t>* currentNode;
	//Deallocates memory when destructed
	StackSlabAllocator<sizeof(ListNode<Map_t>), alignof(ListNode<Map_t>), (sizeof(ListNode<Map_t>) + sizeof(void*) > 204u ? 20u :
		(4096u - sizeof(void*)) / sizeof(ListNode<Map_t>) * sizeof(ListNode<Map_t>))> allocator; 


    //algorithm implementation
	// cout << "------------------------------" << endl;
 //    cout << "<<breadthFirstSearch>>" << endl;
 //    cout << "------------------------------" << endl;
    
	startTime = clock();
	try
	{


		for (size_t i = 0; i != goalState.size(); ++i)
		{
			if (goalState[i] == '0')
			{
				goal.tiles[i] = 0;
				goal.emptyPos = i;
			}
			else
			{
				goal.tiles[i] = goalState[i] - '0';
			}
		}

		currentNode = (ListNode<Map_t>*)allocator.getNext();
		new(currentNode) ListNode<Map_t>{ Map<width, height>(), nullptr };
		for (size_t i = 0; i != initialState.size(); ++i)
		{
			if (initialState[i] == '0')
			{
				currentNode->data.tiles[i] = 0;
				currentNode->data.emptyPos = i;
			}
			else
			{
				currentNode->data.tiles[i] = initialState[i] - '0';
			}
		}

		posiblePaths.emplace_back(currentNode);
		maxQLength = 1;
		bool foundGoal = false;
		while (!posiblePaths.empty())
		{
			ListNode<Map_t>* currentPath = posiblePaths.front();
			if (currentPath->data == goal)
			{
				foundGoal = true;
				break;
			}
			posiblePaths.pop_front();
			Map_t& currentState = currentPath->data;
			//expand path
			size_t emptyPos = currentState.emptyPos;
			size_t x = emptyPos % width;
			size_t y = emptyPos / width;
			if (y != 0u)
			{
				expandPathDepthFirst(currentState, currentPath, emptyPos - width, emptyPos, posiblePaths, allocator);
			}
			if (x != width - 1u)
			{
				expandPathDepthFirst(currentState, currentPath, emptyPos + 1u, emptyPos, posiblePaths, allocator);
			}
			if (y != height - 1u)
			{
				expandPathDepthFirst(currentState, currentPath, emptyPos + width, emptyPos, posiblePaths, allocator);
			}
			if (x != 0u)
			{
				expandPathDepthFirst(currentState, currentPath, emptyPos - 1u, emptyPos, posiblePaths, allocator);
			}

			if (posiblePaths.size() > (unsigned int)maxQLength)
			{
				maxQLength = (int)posiblePaths.size();
			}
			++numOfStateExpansions;
		}

		if (foundGoal)
		{
			path = packPath(posiblePaths.front());
		}
		else
		{
			path = "";
		}
	}
	catch (std::bad_alloc e)
	{
		path = "OOM"; //Out of memory
	}
	
	actualRunningTime = ((float)(clock() - startTime)/CLOCKS_PER_SEC);

	return path;	
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// Search Algorithm:  Breadth-First Search with VisitedList
//
// Move Generator:  
//
////////////////////////////////////////////////////////////////////////////////////////////
string breadthFirstSearch_with_VisitedList(string const& initialState, string const& goalState, int &numOfStateExpansions, int& maxQLength, float &actualRunningTime){
	string path;
	clock_t startTime;
	//add necessary variables here
	constexpr size_t width = 3, height = 3;
	using Map_t = Map<width, height>;
	numOfStateExpansions = 0;
	Map<width, height> goal;
	VisitedList<Map_t*, Hasher<Map_t, 10>, EqualityTester<Map_t>> visited;
	Deque<ListNode<Map_t>*> posiblePaths;
	ListNode<Map_t>* currentNode;
	StackSlabAllocator<sizeof(ListNode<Map_t>), alignof(ListNode<Map_t>), (sizeof(ListNode<Map_t>) + sizeof(void*) > 204u ? 20u :
		(4096u - sizeof(void*)) / sizeof(ListNode<Map_t>) * sizeof(ListNode<Map_t>))> allocator;


    //algorithm implementation
	// cout << "------------------------------" << endl;
 //    cout << "<<breadthFirstSearch_with_VisitedList>>" << endl;
 //    cout << "------------------------------" << endl;

	startTime = clock();

	try
	{

		for (size_t i = 0; i != goalState.size(); ++i)
		{
			if (goalState[i] == '0')
			{
				goal.tiles[i] = 0;
				goal.emptyPos = i;
			}
			else
			{
				goal.tiles[i] = goalState[i] - '0';
			}
		}

		currentNode = (ListNode<Map_t>*)allocator.getNext();
		new(currentNode) ListNode<Map_t>{ Map<width, height>(), nullptr };
		for (size_t i = 0; i != initialState.size(); ++i)
		{
			if (initialState[i] == '0')
			{
				currentNode->data.tiles[i] = 0;
				currentNode->data.emptyPos = i;
			}
			else
			{
				currentNode->data.tiles[i] = initialState[i] - '0';
			}
		}

		posiblePaths.emplace_back(currentNode);
		visited.insert(&currentNode->data);
		maxQLength = 1;
		bool goalFount = false;
		while (!posiblePaths.empty())
		{
			ListNode<Map<width, height>>* currentPath = posiblePaths.front();
			if (currentPath->data == goal)
			{
				goalFount = true;
				break;
			}
			posiblePaths.pop_front();
			Map<width, height>& currentState = currentPath->data;
			//expand path
			size_t emptyPos = currentState.emptyPos;
			size_t x = emptyPos % width;
			size_t y = emptyPos / width;
			if (y != 0u)
			{
				expandPathDepthFirstVisited(currentState, currentPath, emptyPos - width, emptyPos, posiblePaths, visited, allocator);
			}
			if (x != width - 1u)
			{
				expandPathDepthFirstVisited(currentState, currentPath, emptyPos + 1u, emptyPos, posiblePaths, visited, allocator);
			}
			if (y != height - 1u)
			{
				expandPathDepthFirstVisited(currentState, currentPath, emptyPos + width, emptyPos, posiblePaths, visited, allocator);
			}
			if (x != 0u)
			{
				expandPathDepthFirstVisited(currentState, currentPath, emptyPos - 1u, emptyPos, posiblePaths, visited, allocator);
			}

			if (posiblePaths.size() > (unsigned int)maxQLength)
			{
				maxQLength = (int)posiblePaths.size();
			}
			++numOfStateExpansions;
		}

		if (goalFount)
		{
			path = packPath(posiblePaths.front());
		}
		else
		{
			path = "";
		}

	} 
	catch (std::bad_alloc)
	{
		path = "OOM";
	}

	actualRunningTime = ((float)(clock() - startTime) / CLOCKS_PER_SEC);

	return path;
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// Search Algorithm:  
//
// Move Generator:  
//
////////////////////////////////////////////////////////////////////////////////////////////
string progressiveDeepeningSearch_No_VisitedList(string const& initialState, string const& goalState, int &numOfStateExpansions, int& maxQLength, float &actualRunningTime, int ultimateMaxDepth){
    string path;
	clock_t startTime;
	//add necessary variables here
	constexpr size_t width = 3, height = 3;
	using Map_t = MapWithDepth<width, height>;
	unsigned int maxDepthLimit = 1u;
	numOfStateExpansions = 0;
	Map<width, height> goal;
	Deque<ListNode<Map_t>*> posiblePaths;
	ListNode<Map_t>* currentNode;
	StackSlabAllocator<sizeof(ListNode<Map_t>), alignof(ListNode<Map_t>), (sizeof(ListNode<Map_t>) + sizeof(void*) > 204u ? 20u :
		(4096u - sizeof(void*)) / sizeof(ListNode<Map_t>) * sizeof(ListNode<Map_t>))> allocator;

    //algorithm implementation
	// cout << "------------------------------" << endl;
 //    cout << "<<progressiveDeepeningSearch_No_VisitedList>>" << endl;
 //    cout << "------------------------------" << endl;

	startTime = clock();

	try
	{

		for (size_t i = 0; i != goalState.size(); ++i)
		{
			if (goalState[i] == '0')
			{
				goal.tiles[i] = 0;
				goal.emptyPos = i;
			}
			else
			{
				goal.tiles[i] = goalState[i] - '0';
			}
		}

		currentNode = (ListNode<Map_t>*)allocator.getNext();
		new(currentNode) ListNode<Map_t>{ Map_t(), nullptr };
		for (size_t i = 0; i != initialState.size(); ++i)
		{
			if (initialState[i] == '0')
			{
				currentNode->data.tiles[i] = 0;
				currentNode->data.emptyPos = i;
			}
			else
			{
				currentNode->data.tiles[i] = initialState[i] - '0';
			}
		}
		currentNode->data.depth = 0u;

		posiblePaths.emplace_back(currentNode);
		auto start = currentNode;
		maxQLength = 1;

		bool goalFound = false;
		while (true)
		{
			bool solutionMightExist = false;
			while (!posiblePaths.empty())
			{
				ListNode<MapWithDepth<width, height>>* currentPath = posiblePaths.back();
				if (currentPath->data == goal)
				{
					goalFound = true;
					break;
				}
				posiblePaths.pop_back();
				MapWithDepth<width, height>& currentState = currentPath->data;
				if (currentState.depth != maxDepthLimit)
				{
					//expand path
					size_t emptyPos = currentState.emptyPos;
					size_t x = emptyPos % width;
					size_t y = emptyPos / width;
					if (y != 0u)
					{
						expandPathDepthLimited(currentState, currentPath, emptyPos - width, emptyPos, posiblePaths, allocator);
					}
					if (x != width - 1u)
					{
						expandPathDepthLimited(currentState, currentPath, emptyPos + 1u, emptyPos, posiblePaths, allocator);
					}
					if (y != height - 1u)
					{
						expandPathDepthLimited(currentState, currentPath, emptyPos + width, emptyPos, posiblePaths, allocator);
					}
					if (x != 0u)
					{
						expandPathDepthLimited(currentState, currentPath, emptyPos - 1u, emptyPos, posiblePaths, allocator);
					}

					if (posiblePaths.size() > (unsigned int)maxQLength)
					{
						maxQLength = (int)posiblePaths.size();
					}
				}
				else
				{
					solutionMightExist = true;
				}
				++numOfStateExpansions;
			}
			if (goalFound || !solutionMightExist) break;
			++maxDepthLimit;
			posiblePaths.push_back(start);
		}

		if (goalFound)
		{
			path = packPath(posiblePaths.back());
		}
		else
		{
			path = "";
		}

	}
	catch (std::bad_alloc)
	{
		path = "OOM";
	}

	ultimateMaxDepth = maxDepthLimit;
	actualRunningTime = ((float)(clock() - startTime) / CLOCKS_PER_SEC);

	return path;
}
	



///////////////////////////////////////////////////////////////////////////////////////////
//
// Search Algorithm:  
//
// Move Generator:  
//
////////////////////////////////////////////////////////////////////////////////////////////
string progressiveDeepeningSearch_with_NonStrict_VisitedList(string const& initialState, string const& goalState, int &numOfStateExpansions, int& maxQLength, float &actualRunningTime, int ultimateMaxDepth){
    string path;
	clock_t startTime;
    //add necessary variables here
	constexpr size_t width = 3, height = 3;
	using Map_t = MapWithDepth<width, height>;
	unsigned int maxDepthLimit = 1u;
	numOfStateExpansions = 0;
	Map<width, height> goal;
	Deque<ListNode<Map_t>*> posiblePaths;
	VisitedList<Map_t*, Hasher<Map_t, 10>, EqualityTester<Map_t>> visited;
	ListNode<Map_t>* currentNode;
	StackSlabAllocator<sizeof(ListNode<Map_t>), alignof(ListNode<Map_t>), (sizeof(ListNode<Map_t>) + sizeof(void*) > 204u ? 20u :
		(4096u - sizeof(void*)) / sizeof(ListNode<Map_t>) * sizeof(ListNode<Map_t>))> allocator;

    //algorithm implementation
	// cout << "------------------------------" << endl;
 //    cout << "<<progressiveDeepeningSearch_with_NonStrict_VisitedList>>" << endl;
 //    cout << "------------------------------" << endl;

	startTime = clock();

	try
	{

		for (size_t i = 0; i != goalState.size(); ++i)
		{
			if (goalState[i] == '0')
			{
				goal.tiles[i] = 0;
				goal.emptyPos = i;
			}
			else
			{
				goal.tiles[i] = goalState[i] - '0';
			}
		}

		currentNode = (ListNode<Map_t>*)allocator.getNext();
		new(currentNode) ListNode<Map_t>{ Map_t(), nullptr };
		for (size_t i = 0; i != initialState.size(); ++i)
		{
			if (initialState[i] == '0')
			{
				currentNode->data.tiles[i] = 0;
				currentNode->data.emptyPos = i;
			}
			else
			{
				currentNode->data.tiles[i] = initialState[i] - '0';
			}
		}
		currentNode->data.depth = 0u;

		posiblePaths.emplace_back(currentNode);
		auto start = currentNode;
		visited.insert(&currentNode->data);
		maxQLength = 1;

		bool goalFound = false;
		while (true)
		{
			bool solutionMightExist = false;
			while (!posiblePaths.empty())
			{
				ListNode<MapWithDepth<width, height>>* currentPath = posiblePaths.back();
				if (currentPath->data == goal)
				{
					goalFound = true;
					break;
				}
				posiblePaths.pop_back();
				MapWithDepth<width, height>& currentState = currentPath->data;
				if (currentState.depth != maxDepthLimit)
				{
					//expand path
					size_t emptyPos = currentState.emptyPos;
					size_t x = emptyPos % width;
					size_t y = emptyPos / width;
					if (y != 0u)
					{
						expandPathDepthLimitedVisited(currentState, currentPath, emptyPos - width, emptyPos, posiblePaths, visited, allocator);
					}
					if (x != width - 1u)
					{
						expandPathDepthLimitedVisited(currentState, currentPath, emptyPos + 1u, emptyPos, posiblePaths, visited, allocator);
					}
					if (y != height - 1u)
					{
						expandPathDepthLimitedVisited(currentState, currentPath, emptyPos + width, emptyPos, posiblePaths, visited, allocator);
					}
					if (x != 0u)
					{
						expandPathDepthLimitedVisited(currentState, currentPath, emptyPos - 1u, emptyPos, posiblePaths, visited, allocator);
					}

					if (posiblePaths.size() > (unsigned int)maxQLength)
					{
						maxQLength = (int)posiblePaths.size();
					}
				}
				else
				{
					solutionMightExist = true;
				}
				++numOfStateExpansions;
			}
			if (goalFound || !solutionMightExist) break;
			++maxDepthLimit;
			posiblePaths.push_back(start);
			visited.clear();
			visited.insert(&start->data);
		}

		if (goalFound)
		{
			path = packPath(posiblePaths.back());
		}
		else
		{
			path = "";
		}

	}
	catch (std::bad_alloc)
	{
		path = "OOM";
	}

	ultimateMaxDepth = maxDepthLimit;
	actualRunningTime = ((float)(clock() - startTime) / CLOCKS_PER_SEC);

	return path;
}

/*
* Calculates the sum of mantattan distance of all tiles from their goal states based on the previous sum and the move taken.
*/
template<bool useManhattanDistance, class Node, class Map, class Goal>
typename std::enable_if<useManhattanDistance, void>::type calculateHeuristic(Node* currentNode, Map& currentState, Goal& goal, unsigned int oldPos, unsigned int newPos)
{
	unsigned int index = 0u;
	for (;; ++index)
	{
		if (currentNode->data.tiles[oldPos] == goal.tiles[index]) break;
	}

	unsigned int x1 = index % Map::width;
	unsigned int y1 = index / Map::width;
	unsigned int x2 = oldPos % Map::width;
	unsigned int y2 = oldPos / Map::width;
	int dx = (int)x1 - (int)x2;
	if (dx < 0) dx = -dx;
	int dy = (int)y1 - (int)y2;
	if (dy < 0) dy = -dy;
	int distance = dx + dy;

	unsigned int x3 = newPos % Map::width;
	unsigned int y3 = newPos / Map::width;
	int dx2 = (int)x1 - (int)x3;
	if (dx2 < 0) dx2 = -dx2;
	int dy2 = (int)y1 - (int)y3;
	if (dy2 < 0) dy2 = -dy2;
	int distance2 = dx2 + dy2;

	unsigned int g2 = currentState.g + distance - distance2;

	currentNode->data.f = currentState.f - currentState.g + 1u + g2;
	currentNode->data.g = g2;
}

/*
* Calculates the number of misplaced tiles from their goal states based on the previous number and the move taken.
*/
template<bool useManhattanDistance, class Node, class Map, class Goal>
typename std::enable_if<!useManhattanDistance, void>::type calculateHeuristic(Node* currentNode, Map& currentState, Goal& goal, unsigned int oldPos, unsigned int newPos)
{
	unsigned int g2 = currentState.g;
	if (currentNode->data.tiles[oldPos] == goal.tiles[newPos])
	{
		++g2;
	}
	else if (currentNode->data.tiles[oldPos] == goal.tiles[oldPos])
	{
		--g2;
	}
	currentNode->data.f = currentState.f - currentState.g + 1u + g2;
	currentNode->data.g = g2;
}

template<bool useManhattanDistance, class Queue, class Expanded, class Map, class Map2, class Allocator
#ifdef USE_HASH_MAP
	, class QueueLookup
#endif
	>
void expandAStarExpanded(const Map& currentState, ListNode<Map>* currentPath, size_t newPos, size_t oldPos, Queue& posiblePaths,
	Expanded& expanded, const Map2& goal, Allocator& allocator, int& numOfDeletionsFromMiddleOfHeap, int& numOfAttemptedNodeReExpansions
#ifdef USE_HASH_MAP
	, QueueLookup& queueLookup
#endif
	)
{
	ListNode<Map>* currentNode = (ListNode<Map>*)allocator.getNext();
	new(currentNode) ListNode<Map>{ currentState, currentPath };
	currentNode->data.emptyPos = newPos;
	currentNode->data.tiles[oldPos] = currentNode->data.tiles[currentNode->data.emptyPos];
	currentNode->data.tiles[currentNode->data.emptyPos] = 0;

	if (expanded.find(&currentNode->data) != expanded.end())
	{
		currentNode->~ListNode<Map>();
		allocator.replaceLast();
		++numOfAttemptedNodeReExpansions;
	}
	else
	{	
		calculateHeuristic<useManhattanDistance>(currentNode, currentState, goal, oldPos, newPos);

#ifdef USE_HASH_MAP
		auto elementInQueue = queueLookup.find(currentNode);
		if (elementInQueue != queueLookup.end())
		{
			auto element = (*elementInQueue);
			if (element->data.f > currentNode->data.f)
			{
				element->data.f = currentNode->data.f;
				element->data.g = currentNode->data.g;
				element->next = currentNode->next;
				posiblePaths.priorityIncreased(*element->data.positionInQueue);
			}
			

			currentNode->~ListNode<Map>();
			allocator.replaceLast();
			++numOfDeletionsFromMiddleOfHeap;
		}
		else
		{
			posiblePaths.push(LocationTracker<ListNode<Map>*>{currentNode});
			queueLookup.insert(currentNode);
		}
#else
		auto elementInQueue = posiblePaths.find(currentNode, ListNodeEqualityTester<Map>());
		if (elementInQueue != posiblePaths.end())
		{
			auto element = *elementInQueue;
			if (element->data.f > currentNode->data.f)
			{
				element->data.f = currentNode->data.f;
				element->data.g = currentNode->data.g;
				element->next = currentNode->next;
				posiblePaths.priorityIncreased(*elementInQueue);
			}


			currentNode->~ListNode<Map>();
			allocator.replaceLast();
			++numOfDeletionsFromMiddleOfHeap;
		}
		else
		{
			posiblePaths.push(currentNode);
		}
#endif
	}
}


template<bool useManhattanDistance>
string aStar_ExpandedList(string const& initialState, string const& goalState, int &numOfStateExpansions, int& maxQLength, float &actualRunningTime,
	int &numOfDeletionsFromMiddleOfHeap, int &numOfLocalLoopsAvoided, int &numOfAttemptedNodeReExpansions)
{
	string path;
	clock_t startTime;

	constexpr size_t width = 3, height = 3;
	using Map_t = MapWithHuristic<width, height>;
	Map<width, height> goal;
#ifdef USE_HASH_MAP
	using QueueElement = LocationTracker<ListNode<Map_t>*>;
	using QueueType = PriorityQueue<QueueElement, Vector<QueueElement>, GreaterHeuisticTracker<Map_t>>;
#else
	using QueueType = PriorityQueue<ListNode<Map_t>*, Vector<ListNode<Map_t>*>, GreaterHeuistic<Map_t>>;
#endif
	QueueType posiblePaths;
	VisitedList<Map_t*, Hasher<Map_t>, EqualityTester<Map_t>> expanded;
	ListNode<Map_t>* currentNode;
	StackSlabAllocator<sizeof(ListNode<Map_t>), alignof(ListNode<Map_t>), (sizeof(ListNode<Map_t>) + sizeof(void*) > 204u ? 20u :
		(4096u - sizeof(void*)) / sizeof(ListNode<Map_t>) * sizeof(ListNode<Map_t>))> allocator;

#ifdef USE_HASH_MAP
	HashMap<ListNode<Map_t>*, ListNodeHasher<Map_t, 10>, ListNodeEqualityTester<Map_t>> queueLookup;
#endif

	numOfDeletionsFromMiddleOfHeap = 0;
	numOfLocalLoopsAvoided = 0;
	numOfAttemptedNodeReExpansions = 0;
	numOfStateExpansions = 0;


	// cout << "------------------------------" << endl;
	// cout << "<<aStar_ExpandedList>>" << endl;
	// cout << "------------------------------" << endl;
	
	startTime = clock();

	try
	{

		for (size_t i = 0; i != goalState.size(); ++i)
		{
			if (goalState[i] == '0')
			{
				goal.tiles[i] = 0;
				goal.emptyPos = i;
			}
			else
			{
				goal.tiles[i] = goalState[i] - '0';
			}
		}

		currentNode = (ListNode<Map_t>*)allocator.getNext();
		new(currentNode) ListNode<Map_t>{ Map_t(), nullptr };
		for (size_t i = 0; i != initialState.size(); ++i)
		{
			if (initialState[i] == '0')
			{
				currentNode->data.tiles[i] = 0;
				currentNode->data.emptyPos = i;
			}
			else
			{
				currentNode->data.tiles[i] = initialState[i] - '0';
			}
		}

		
#ifdef USE_HASH_MAP
		posiblePaths.push(QueueElement{ currentNode });
		queueLookup.insert(posiblePaths.top().data);
#else
		posiblePaths.push(currentNode);
#endif
		maxQLength = 1;
		bool goalFound = false;

		while (!posiblePaths.empty())
		{
			
#ifdef USE_HASH_MAP
			ListNode<MapWithHuristic<width, height>>* currentPath = posiblePaths.top().data;
			queueLookup.erase(currentPath);
#else
			ListNode<MapWithHuristic<width, height>>* currentPath = posiblePaths.top();
#endif
			if (currentPath->data == goal)
			{
				goalFound = true;
				break;
			}
			posiblePaths.pop();
			MapWithHuristic<width, height>& currentState = currentPath->data;

			//expand path
			size_t emptyPos = currentState.emptyPos;
			size_t x = emptyPos % width;
			size_t y = emptyPos / width;
			if (y != 0u)
			{
#ifdef USE_HASH_MAP
				expandAStarExpanded<useManhattanDistance>(currentState, currentPath, emptyPos - width, emptyPos, posiblePaths,
					expanded, goal, allocator, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions, queueLookup);
#else
				expandAStarExpanded<useManhattanDistance>(currentState, currentPath, emptyPos - width, emptyPos, posiblePaths,
					expanded, goal, allocator, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions);
#endif
			}
			if (x != width - 1u)
			{
#ifdef USE_HASH_MAP
				expandAStarExpanded<useManhattanDistance>(currentState, currentPath, emptyPos + 1u, emptyPos, posiblePaths,
					expanded, goal, allocator, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions, queueLookup);
#else
				expandAStarExpanded<useManhattanDistance>(currentState, currentPath, emptyPos + 1u, emptyPos, posiblePaths,
					expanded, goal, allocator, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions);
#endif
			}
			if (y != height - 1u)
			{
#ifdef USE_HASH_MAP
				expandAStarExpanded<useManhattanDistance>(currentState, currentPath, emptyPos + width, emptyPos, posiblePaths,
					expanded, goal, allocator, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions, queueLookup);
#else
				expandAStarExpanded<useManhattanDistance>(currentState, currentPath, emptyPos + width, emptyPos, posiblePaths,
					expanded, goal, allocator, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions);
#endif
			}
			if (x != 0u)
			{
#ifdef USE_HASH_MAP
				expandAStarExpanded<useManhattanDistance>(currentState, currentPath, emptyPos - 1u, emptyPos, posiblePaths,
					expanded, goal, allocator, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions, queueLookup);
#else
				expandAStarExpanded<useManhattanDistance>(currentState, currentPath, emptyPos - 1u, emptyPos, posiblePaths,
					expanded, goal, allocator, numOfDeletionsFromMiddleOfHeap, numOfAttemptedNodeReExpansions);
#endif
			}

			if (posiblePaths.size() > (unsigned int)maxQLength)
			{
				maxQLength = (int)posiblePaths.size();
			}
			expanded.insert(&currentState);
			++numOfStateExpansions;
		}

		if (goalFound)
		{
#ifdef USE_HASH_MAP
			path = packPath(posiblePaths.top().data);
#else
			path = packPath(posiblePaths.top());
#endif
		}
		else
		{
			path = "";
		}

	}
	catch (std::bad_alloc)
	{
		path = "OOM";
	}

	actualRunningTime = ((float)(clock() - startTime) / CLOCKS_PER_SEC);

	return path;
}
	

string aStar_ExpandedList(string const& initialState, string const& goalState, int &numOfStateExpansions, int& maxQLength, float &actualRunningTime,
	int &numOfDeletionsFromMiddleOfHeap, int &numOfLocalLoopsAvoided, int &numOfAttemptedNodeReExpansions, heuristicFunction heuristic){
											 
	if (heuristic == heuristicFunction::manhattanDistance)
	{
		return aStar_ExpandedList<true>(initialState, goalState, numOfStateExpansions, maxQLength, actualRunningTime,
			numOfDeletionsFromMiddleOfHeap, numOfLocalLoopsAvoided, numOfAttemptedNodeReExpansions);
	}
	else
	{
		return aStar_ExpandedList<false>(initialState, goalState, numOfStateExpansions, maxQLength, actualRunningTime,
			numOfDeletionsFromMiddleOfHeap, numOfLocalLoopsAvoided, numOfAttemptedNodeReExpansions);
	}
}