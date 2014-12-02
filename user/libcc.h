#ifndef _LIBCC_H
#define _LIBCC_H

typedef unsigned int size_t;

extern "C" {
#include "heap.h"
}

inline void* operator new(size_t size)
{
	return malloc(size);
}

inline void operator delete(void* p)
{
	return free(p);
}

inline void* operator new[](size_t size)
{
	return malloc(size);
}

inline void operator delete[](void* p)
{
	return free(p);
}

template<typename T> class List
{
public:
	class ListNode
	{
	public:
		ListNode* next;
		ListNode* previous;
		T value;

		ListNode(const T& val) :
			next(nullptr),
			previous(nullptr),
			value(val)
		{

		}
	};

	ListNode* first;
	ListNode* last;
	List() :
		first(nullptr),
		last(nullptr)
	{

	}

	inline void Push(const T& v)
	{
		ListNode* newNode = new ListNode(v);

		if(this->first == nullptr)
		{
			this->first = newNode;
			this->last = newNode;
		}
		else
		{
			this->first->previous = newNode;
			newNode->next = this->first;
			this->first = newNode;
		}
	}

	inline ListNode* GetHead() {return this->first;}
	inline ListNode* GetLast() {return this->last;}

	inline void MoveFrontToEnd()
	{
		if(this->first == nullptr || this->first->next == nullptr)
			return;

		ListNode* oldFirst = this->first;
		this->first->next->previous = nullptr;
		this->first = this->first->next;

		this->last->next = oldFirst;
		oldFirst->previous = this->last;
		this->last = oldFirst;
	}
};

#endif
