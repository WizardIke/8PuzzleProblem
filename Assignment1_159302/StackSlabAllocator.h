#pragma once
#include <memory>

/*
* Allocates and Deallocates memory in fifi order.
* Frees all allocated memory when destructed.
*/


template<size_t element_size, size_t element_align, size_t block_size_in_elements>
struct StackSlabAllocatorNode
{
	alignas(element_align) unsigned char data[element_size * block_size_in_elements];
	StackSlabAllocatorNode<element_size, element_align, block_size_in_elements>* next;
};

template<size_t element_size, size_t element_align, size_t block_size_in_elements,
	class Allocator = std::allocator<StackSlabAllocatorNode<element_size, element_align, block_size_in_elements>>>
class StackSlabAllocator : Allocator
{
	StackSlabAllocatorNode<element_size, element_align, block_size_in_elements>* mData;
	unsigned char* mEnd;
	unsigned char* mCurrent;

	void increaseSize()
	{
		auto newData = this->allocate(1u);
		newData->next = mData;
		mData = newData;
		mEnd = &newData->data[element_size * block_size_in_elements];
		mCurrent = &newData->data[0u];
	}

	void decreasedSize()
	{
		auto temp = mData;
		mData = mData->next;
		this->deallocate(temp, 1u);
		mEnd = &mData->data[element_size * block_size_in_elements];
		mCurrent = mEnd + element_size;
	}
public:
	StackSlabAllocator()
	{
		mData = nullptr;
		mEnd = nullptr;
		mCurrent = nullptr;
	}

	~StackSlabAllocator()
	{
		for (auto current = mData; current != nullptr;)
		{
			auto temp = current;
			current = current->next;
			this->deallocate(temp, 1u);
		}
	}

	void* getNext()
	{
		if (mCurrent == mEnd) increaseSize();
		void* ret = mCurrent;
		mCurrent += element_size;
		return ret;
	}

	void replaceLast()
	{
		if (mCurrent == &mData->data[0u]) decreasedSize();
		mCurrent -= element_size;
	}
};