#pragma once
#include "Vector.h"

/*
* Implements a set based on linear searching.
*/
template<class T, class EqualTo = std::equal_to<T>>
class ArrayMap
{
public:
	using value_type = T;
	using size_type = typename Vector<value_type>::size_type;
	using reference = typename Vector<value_type>::reference;
	using const_reference = typename  Vector<value_type>::const_reference;
	using iterator = typename Vector<value_type>::iterator;
	using const_iterator = typename Vector<value_type>::const_iterator;
private:
	Vector<value_type> mData;
	EqualTo equal;
public:
	ArrayMap() noexcept {}

	ArrayMap(ArrayMap&& other) : mData(std::move(other.mData)) {}

	ArrayMap(const ArrayMap& other) : mData(other.mData) {}

	void operator=(ArrayMap&& other)
	{
		this->~ArrayMap();
		new(this) ArrayMap(std::move(other));
	}

	void operator=(const ArrayMap& other)
	{
		this->~ArrayMap();
		new(this) ArrayMap(other);
	}

	iterator begin()
	{
		return mData.begin();
	}

	const_iterator begin() const
	{
		return mData.begin();
	}

	iterator end()
	{
		return mData.end();
	}

	const_iterator end() const
	{
		return mData.end();
	}

	bool empty() const noexcept
	{
		return mData.empty();
	}

	size_type size() const noexcept
	{
		return mData.size();
	}

	size_type max_size() const noexcept
	{
		return mData.max_size();
	}

	void insert(const_reference value)
	{
		mData.push_back(value);
	}

	void insert(value_type&& value)
	{
		mData.push_back(std::move(value));
	}

	iterator find(const_reference value)
	{
		const auto endVal = mData.end();
		for (auto current = mData.begin(); current != endVal; ++current)
		{
			if (equal(*current, value))
			{
				return current;
			}
		}
		return end();
	}

	const_iterator find(const_reference value) const
	{
		const auto endVal = mData.end();
		for (auto current = mData.begin(); current != endVal; ++current)
		{
			if (equal(*current, value))
			{
				return current;
			}
		}
		return end();
	}

	void clear()
	{
		mData.clear();
	}
};