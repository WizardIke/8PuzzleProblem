#pragma once
#include <memory>
/*
* Items can be added end removed from both ends.
* If adding an item to the Deque causes its capacity to increase all references and pointers into the Deque will be invalidated.
* Doesn't currently support iteration.
*/
template<class T, class Allocater = std::allocator<T>>
class Deque : Allocater
{
	T* mData;
	size_t readPos;
	size_t writePos;
	size_t capacity;

	void resize()
	{
		size_t oldCap = capacity;
		capacity = (oldCap >> 1) + oldCap;
		T* newData = this->allocate(capacity);
		for (size_t i = 0u, j = readPos; i != oldCap; ++i)
		{
			new(&newData[i]) T(std::move(mData[j]));
			mData[j].~T();
			++j;
			if (j == oldCap) j = 0u;
		}
		this->deallocate(mData, oldCap);
		mData = newData;
		readPos = 0u;
		writePos = oldCap;
	}
public:
	using value_type = T;
	using size_type = size_t;
	using reference = value_type&;
	using const_reference = const value_type&;

	Deque() 
	{
		mData = this->allocate(8u);
		readPos = 0u;
		writePos = 0u;
		capacity = 8u;
	}

	Deque(const Deque& other)
	{
		size_t cap = other.capacity;
		T* newData = this->allocate(cap);
		for (size_t i = other.readPos; i != other.writePos; ++i)
		{
			new(&newData[i]) T(other.mData[i]);
			++i;
			if (i == cap) i = 0u;
		}
		mData = newData;
		readPos = other.readPos;
		writePos = other.writePos;
		capacity = cap;
	}

	Deque(Deque&& other)
	{
		mData = other.mData;
		readPos = other.readPos;
		writePos = other.writePos;
		capacity = other.capacity;
		other.capacity = 0u;
	}

	~Deque()
	{
		if (capacity != 0u)
		{
			for (size_t i = readPos; i != writePos;)
			{
				mData[i].~T();
				++i;
				if (i == capacity) i = 0u;
			}
			this->deallocate(mData, capacity);
		}
	}

	void operator=(const Deque& other)
	{
		this->~Deque();
		new(this) Deque(other);
	}

	void operator=(Deque&& other)
	{
		this->~Deque();
		new(this) Deque(std::move(other));
	}

	reference front()
	{
		return mData[readPos];
	}

	reference back()
	{
		return mData[writePos - 1u];
	}

	bool empty()
	{
		return readPos == writePos;
	}

	size_t size()
	{
		if (writePos >= readPos)
		{
			return writePos - readPos;
		}
		else
		{
			return capacity - readPos + writePos;
		}
	}

	void push_back(T& value)
	{
		new(&mData[writePos]) T(value);
		++writePos;
		if (writePos == capacity) writePos = 0u;
		if (writePos == readPos) resize();
	}

	void push_front(reference value)
	{
		if (readPos == 0) readPos = capacity;
		--readPos;
		new(&mData[readPos]) T(value);
		if (writePos == readPos) resize();
	}

	template<class... Args>
	void emplace_back(Args&&... args)
	{
		new(&mData[writePos]) T(std::forward<Args>(args)...);
		++writePos;
		if (writePos == capacity) writePos = 0u;
		if (writePos == readPos) resize();
	}

	void pop_front()
	{
		++readPos;
		if (readPos == capacity) readPos = 0u;
	}

	void pop_back()
	{
		if (writePos == 0u) writePos = capacity;
		--writePos;
	}

	void swap(Deque& other)
	{
		using std::swap;

		swap(mData, other.mData);
		swap(readPos, other.readPos);
		swap(writePos, other.writePos);
		swap(capacity, other.capacity);
	}
};

template<class T>
void swap(Deque<T>& rhs, Deque<T>& lhs)
{
	rhs.swap(lhs);
}