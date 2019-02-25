#pragma once
#include <memory>
/*
* An array that dynamically grows when needed.
* Currently doesn't support reverse iteration.
*/
template<class T, class Allocator = std::allocator<T>>
class Vector : Allocator
{
public:
	using value_type = T;
	using iterator = value_type*;
	using const_iterator = const value_type*;
	//using reverse_iterator = ;
	//using const_reverse_iterator = ;
	using pointer = value_type*;
	using reference = value_type&;
	using const_pointer = const value_type*;
	using const_reference = const value_type&;
	using size_type = std::size_t;
private:
	value_type* mData;
	pointer mEnd;
	pointer mCapacityEnd;

	void increaseCapacity()
	{
		auto oldSize = size();
		size_type newSize;
		if (oldSize == 0u)
		{
			newSize = 8u;
		}
		else
		{
			newSize = oldSize + (oldSize >> 1u);
		}

		auto newData = this->allocate(newSize);
		for (size_type i = 0u; i != oldSize; ++i)
		{
			new(&newData[i]) value_type(std::move(mData[i]));
			mData[i].~value_type();
		}
		if (oldSize != 0u)
		{
			this->deallocate(mData, oldSize);
		}
		mData = newData;
		mEnd = newData + oldSize;
		mCapacityEnd = newData + newSize;
	}
public:
	Vector() noexcept : mData(nullptr), mEnd(nullptr), mCapacityEnd(nullptr) {}

	Vector(Vector&& other) : mData(std::move(other.mData)), mEnd(other.mEnd), mCapacityEnd(other.mCapacityEnd) {}

	Vector(const Vector& other)
	{
		auto otherSize = other.mEnd - other.mData;
		if (otherSize != 0u)
		{
			auto otherCapacity = other.mCapacityEnd - other.mData;
			mData = this->allocate(otherCapacity);
			for (size_type i = 0u; i != otherSize; ++i)
			{
				mData[i] = other.mData[i];
			}
			mEnd = mData + otherSize;
			mCapacityEnd = nullptr;
		}
		else
		{
			mData = nullptr;
			mEnd = nullptr;
			mCapacityEnd = nullptr;
		}
	}

	void operator=(Vector&& other)
	{
		this->~Vector();
		new(this) Vector(std::move(other));
	}

	void operator=(const Vector& other)
	{
		this->~Vector();
		new(this) Vector(other);
	}

	reference operator[](const size_type pos) noexcept
	{
		return mData[pos];
	}

	const_reference operator[](const size_type pos) const noexcept
	{
		return mData[pos];
	}

	void pop_back()
	{
		--mEnd;
		mEnd->~value_type();
	}

	void push_back(const_reference value)
	{
		if (mEnd == mCapacityEnd) increaseCapacity();
		new(mEnd) value_type(value);
		++mEnd;
	}

	void push_back(value_type&& value)
	{
		if (mEnd == mCapacityEnd) increaseCapacity();
		new(mEnd) value_type(std::move(value));
		++mEnd;
	}

	template<class... Args>
	void emplace_back(Args&&... args)
	{
		++mEnd;
		if (mEnd = mCapacityEnd) increaseCapacity();
		new(mEnd - 1u) value_type(std::forward<Args>(args)...);
	}

	size_type size() const noexcept
	{
		return mEnd - mData;
	}

	constexpr size_type max_size() const noexcept
	{
		return std::numeric_limits<size_type>::max;
	}

	iterator begin() noexcept
	{
		return mData;
	}

	iterator end() noexcept
	{
		return mEnd;
	}

	const_iterator begin() const noexcept
	{
		return mData;
	}

	const_iterator end() const noexcept
	{
		return mEnd;
	}

	bool empty() const noexcept
	{
		return mEnd == mData;
	}

	void clear()
	{
		for (auto& value : *this)
		{
			value.~value_type();
		}
		mEnd = mData;
	}
};