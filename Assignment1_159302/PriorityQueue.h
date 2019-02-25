#pragma once
#include <functional>

/*
* Implements a PriorityQueue using a heap.
* top() returns a refernce an item for which Compare()(item, otherItem) returns false for all values of otherItem in the PriorityQueue. 
*	This means that Compare can be std::less<T> for a max PriorityQueue and std::greater<T> for a min PriorityQueue.
* top() always returns a refernce to the same item unless an item is added, removed or has its priority changed.
* Compare must provide a strict weak ordering.
*/
template<class T, class Container, class Compare = std::less<T>>
class PriorityQueue
{
public:
	using container_type = Container;
	using value_compare = Compare;
	using value_type = typename Container::value_type;
	using size_type = typename Container::size_type;
	using reference = typename Container::reference;
	using const_reference = typename Container::const_reference;
	using iterator = typename Container::iterator;
	using const_iterator = typename Container::const_iterator;
private:
	Container data;
	Compare comparer;

	void moveUp(size_type index)
	{
		using std::swap;
		const size_type parentIndex = (index - 1u) / 2u;
		if (index != 0u)
		{
			if (comparer(data[parentIndex], data[index]))
			{
				swap(data[parentIndex], data[index]);
				moveUp(parentIndex);
			}
		}
	}

	void moveDown(size_type index = 0u)
	{
		using std::swap;
		const size_type child1 = 2u * index + 1u;
		const size_type child2 = 2u * index + 2u;
		if (child1 < data.size())
		{
			if (child2 < data.size())
			{
				if (comparer(data[index], data[child1]) || comparer(data[index], data[child2]))
				{
					if (comparer(data[child2], data[child1]))
					{
						swap(data[child1], data[index]);
						moveDown(child1);
					}
					else
					{
						swap(data[child2], data[index]);
						moveDown(child2);
					}
				}
			}
			else
			{
				if (comparer(data[index], data[child1]))
				{
					swap(data[child1], data[index]);
				}
			}
		}
	}
public:
	PriorityQueue()
	{
		static_assert(std::is_same<T, value_type>(), "Container value_type must be the same as the PriorityQueue value_type");
	}

	PriorityQueue& operator=(const PriorityQueue& other)
	{
		data = other.data;
		return *this;
	}

	PriorityQueue& operator=(PriorityQueue&& other)
	{
		data = std::move(other.data);
		return *this;
	}

	reference top()
	{
		return data[0];
	}

	bool empty()
	{
		return data.empty();
	}

	size_t size()
	{
		return data.size();
	}

	void push(const_reference value)
	{
		auto index = data.size();
		data.push_back(value);
		moveUp(index);
	}

	template<class... Args>
	void emplace(Args&&... args)
	{
		auto index = data.size();
		data.emplace_back(std::forward<Args>(args)...);
		moveUp(index);
	}

	void pop()
	{
		if (data.size() != 1u)
		{
			data[0] = *(data.end() - 1u);
		}
		data.pop_back();
		moveDown(0u);
	}

	void swap(PriorityQueue& other)
	{
		using std::swap;
		swap(data, other.data);
	}

	iterator begin()
	{
		return data.begin();
	}

	const_iterator begin() const
	{
		return data.begin();
	}

	iterator end()
	{
		return data.end();
	}

	const_iterator end() const
	{
		return data.end();
	}

	void priorityIncreased(reference value)
	{
		auto index = &value - &data[0];
		moveUp(index);
	}

	void erase(iterator pos)
	{
		auto dataEnd = data.end();
		--dataEnd;
		if (pos != dataEnd)
		{
			*pos = std::move(*dataEnd);
			data.pop_back();
			auto index = size_type(pos - data.begin());
			if(index == 0u) moveDown(index);
			else
			{
				auto parentIndex = (index - 1u) / 2u;
				if(comparer(data[index], data[parentIndex])) moveDown(index);
				else moveUp(index);
			}
		}
		else
		{
			data.pop_back();
		}
	}

	template<class EqualTo>
	iterator find(const_reference value, EqualTo equal = std::equal_to<value_type>()) //Needs a specialization for when find is definded for Container
	{
		const auto dataEnd = data.end();
		for (auto start = data.begin(); start != dataEnd; ++start)
		{
			if (equal(*start, value))
			{
				return start;
			}
		}
		return dataEnd;
	}
};

template<class... Ts>
void swap(PriorityQueue<Ts...>& rhs, PriorityQueue<Ts...>& lhs)
{
	rhs.swap(lhs);
}