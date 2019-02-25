#pragma once
#include <memory>
#include <limits>
#include <type_traits>

/*
* Implements a set using hashing.
* Inserting or removing an item from a HashMap invalidates all pointers, references and iterators into it.
*/
template<class T, class Hasher = std::hash<T>, class EqualTo = std::equal_to<T>>
class HashMap
{
	Hasher hasher;
	EqualTo equal;
public:
	using value_type = T;
	using size_type = size_t;
	using reference = value_type&;
	using const_reference = const value_type&;
private:
	template<size_t size, size_t alignment>
	class Memory
	{
		alignas(alignment) char data[size];
	};

	class Node
	{
		size_type mDistanceFromIdealPosition;
		Memory<sizeof(value_type), alignof(value_type)> mData;
	public:
		

		~Node()
		{
			if (mDistanceFromIdealPosition != 0u)
			{
				data().~value_type();
			}
		}

		void operator=(const Node& other)
		{
			mDistanceFromIdealPosition = other.mDistanceFromIdealPosition;
			if (mDistanceFromIdealPosition != 0u)
			{
				data() = other.data();
			}
		}

		value_type& data()
		{
			return *reinterpret_cast<value_type*>(&mData);
		}

		const value_type& data() const
		{
			return *reinterpret_cast<const value_type*>(&mData);
		}

		size_type& distanceFromIdealPosition()
		{
			return mDistanceFromIdealPosition;
		}
	};

	template<class T2>
	class Iterator
	{
		template<class T3, class Hasher3, class EqualTo3>
		friend class HashMap;
		Node* ptr;
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T2;
		using pointer = value_type*;
		using reference = value_type&;
		using iterator_category = std::bidirectional_iterator_tag;

		Iterator(Node* ptr1) : ptr(ptr1) {}

		template<class OtherType, class Tag = typename std::enable_if<std::is_same<OtherType, T2>::value || std::is_same<OtherType, typename std::remove_const<T2>::type>::value>::type>
		Iterator(const Iterator<OtherType>& other) : ptr(other.ptr) {}

		Iterator operator++()
		{
			do
			{
				++ptr;

			} while (ptr->distanceFromIdealPosition() == 0u);
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator ret(ptr);
			do
			{
				++ptr;
			} while (ptr->distanceFromIdealPosition() == 0u);
			return ret;
		}

		Iterator operator--()
		{
			do
			{
				--ptr;
			} while (ptr->distanceFromIdealPosition() == 0u);
			return *this;
		}

		Iterator operator--(int)
		{
			Iterator ret(ptr);
			do
			{
				--ptr;
			} while (ptr->distanceFromIdealPosition() == 0u);
			return ret;
		}

		bool operator==(const Iterator& other) const noexcept
		{
			return ptr == other.ptr;
		}

		bool operator!=(const Iterator& other) const noexcept
		{
			return ptr != other.ptr;
		}

		reference operator*()
		{
			return ptr->data();
		}

		pointer operator->()
		{
			return *ptr->data();
		}
	};

	Node* data = nullptr;
	size_type capacity = 0u;
	size_type mSize = 0u;
	size_type loadThreshold = 0u;
	float loadFactor = 0.5f;


	void rehashNoChecks(size_type newCapacity)
	{
		if (capacity == 0u)
		{
			newCapacity = 8u;
		}

		size_type oldCapacity = capacity;
		Node* oldData = data;

		capacity = newCapacity;
		data = std::allocator<Node>().allocate(newCapacity + 1u);
		data[newCapacity].distanceFromIdealPosition() = 1u;
		mSize = 0u;
		loadThreshold = (size_type)(newCapacity * (double)loadFactor);

		const Node* const dataEnd = data + capacity;
		for (auto i = data; i != dataEnd; ++i)
		{
			i->distanceFromIdealPosition() = 0u;
		}

		const Node* const endData = oldData + oldCapacity;
		for (auto i = oldData; i != endData; ++i)
		{
			if (i->distanceFromIdealPosition() != 0u)
			{
				insert(i->data());
			}
		}

		if (oldCapacity != 0u)
		{
			const auto dataEnd2 = oldData + oldCapacity;
			for (Node* n = oldData; n != dataEnd2; ++n)
			{
				n->~Node();
			}
			std::allocator<Node>().deallocate(oldData, oldCapacity + 1u);
		}
	}
public:
	using iterator = Iterator<value_type>;
	using const_iterator = Iterator<const value_type>;

	HashMap() noexcept {}

	HashMap(HashMap&& other) noexcept
	{
		data = other.data;
		capacity = other.capacity;
		mSize = other.mSize;
		loadThreshold = other.loadThreshold;
		loadFactor = other.loadFactor;
		other.capacity = 0u;
	}

	HashMap(const HashMap& other)
	{
		if (other.size != 0)
		{
			data = std::allocator<Node>().allocate(other.capacity);
			capacity = other.capacity;
			mSize = other.mSize;
			loadThreshold = other.loadThreshold;
			loadFactor = other.loadFactor;
			for (size_type i = 0u; i != capacity; ++i)
			{
				data[i] = other.data[i];
			}
		}
		else
		{
			new(this) HashMap();
		}
	}

	~HashMap()
	{
		if (capacity != 0u)
		{
			const auto dataEnd = data + capacity;
			for (Node* n = data; n != dataEnd; ++n)
			{
				n->~Node();
			}
			std::allocator<Node>().deallocate(data, capacity + 1u);
		}
	}

	void operator=(HashMap&& other)
	{
		this->~HashMap();
		new(this) HashMap(std::move(other));
	}

	void operator=(const HashMap& other)
	{
		this->~HashMap();
		new(this) HashMap(other);
	}

	iterator begin()
	{
		if (data != nullptr)
		{
			Node* current = data;
			while (current->distanceFromIdealPosition() == 0u)
			{
				++current;
			}
			return iterator(current);
		}
		return iterator(nullptr);
	}

	const_iterator begin() const
	{
		if (data != nullptr)
		{
			Node* current = data;
			while (current->distanceFromIdealPosition() == 0u)
			{
				++current;
			}
			return iterator(current);
		}
		return iterator(nullptr);
	}

	iterator end()
	{
		return iterator(data + capacity);
	}

	const_iterator end() const
	{
		return const_iterator(data + capacity);
	}

	bool empty() const noexcept
	{
		return size == 0u;
	}

	size_type size() const noexcept
	{
		return mSize;
	}

	size_type max_size() const noexcept
	{
		return loadFactor * std::numeric_limits<size_type>::max();
	}

	void insert(value_type value)
	{
		if (mSize == loadThreshold) rehashNoChecks(capacity * 2u);
		++mSize;
		auto hash = hasher(value);
		Node* bucket = data + (hash & (capacity - 1));
		size_type distanceFromIdealBucket = 1u;
		Node* const endData = data + capacity;
		while (true)
		{
			if (bucket->distanceFromIdealPosition() == 0)
			{
				bucket->data() = std::move(value);
				bucket->distanceFromIdealPosition() = distanceFromIdealBucket;
				break;
			}
			else if (bucket->distanceFromIdealPosition() < distanceFromIdealBucket)
			{
				std::swap(value, bucket->data());
				std::swap(bucket->distanceFromIdealPosition(), distanceFromIdealBucket);
			}

			++bucket;
			if (bucket == endData) bucket = data;
			++distanceFromIdealBucket;
		}
	}

	iterator find(const_reference value)
	{
		if (capacity == 0u) return iterator(nullptr);
		auto hash = hasher(value);
		Node* bucket = data + (hash & (capacity - 1));
		size_type distanceFromIdealBucket = 1u;
		Node* const endData = data + capacity;
		while (bucket->distanceFromIdealPosition() >= distanceFromIdealBucket)
		{
			if(bucket->distanceFromIdealPosition() == distanceFromIdealBucket && equal(bucket->data(), value)) return iterator(bucket);
			++bucket;
			if (bucket == endData) bucket = data;
			++distanceFromIdealBucket;
		}
		
		return iterator(endData);
	}

	void clear()
	{
		const auto dataEnd = data + capacity;
		for (auto current = data; current != dataEnd; ++current)
		{
			if (current->distanceFromIdealPosition() != 0u)
			{
				current->~Node();
				current->distanceFromIdealPosition() = 0u;
			}
		}
	}

	void erase(const_iterator pos)
	{
		/**
		* Shift items back into the new empty space until we reach an empty space or an item in its ideal position.
		*/
		std::size_t previousBucket = pos.ptr - data;
		std::size_t bucket = (previousBucket + 1u) & (capacity - 1u);

		data[previousBucket].data().~value_type();
		--mSize;

		while (data[bucket].distanceFromIdealPosition() > 1u)
		{
			const size_type newDistance = data[bucket].distanceFromIdealPosition() - 1u;
			data[previousBucket].distanceFromIdealPosition() = newDistance;
			new(&data[previousBucket].data()) value_type(std::move(data[bucket].data()));
			data[bucket].data().~value_type();

			previousBucket = bucket;
			bucket = (previousBucket + 1u) & (capacity - 1u);
		}
		data[previousBucket].distanceFromIdealPosition() = 0u;
	}

	void erase(const_reference value)
	{
		erase(find(value));
	}
};