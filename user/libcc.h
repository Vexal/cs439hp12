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

class String
{
public:
	char* c_string;
	int length;

	bool operator<(const String& rhs) const
	{
		for(int a = 0; a < this->length && a < rhs.length; ++a)
		{
			if(rhs.c_string[a] >= this->c_string[a])
			{
				return false;
			}
		}

		return this->length < rhs.length;
	}
};

template<typename KEY_TYPE, typename VALUE_TYPE> class Map
{
public:
	class MapNode
	{
	public:
		KEY_TYPE key;
		VALUE_TYPE value;
		MapNode* leftChild;
		MapNode* rightChild;

		MapNode(KEY_TYPE key) :
			key(key),
			value(value),
			leftChild(nullptr),
			rightChild(nullptr)
		{

		}
	};

	MapNode* root;

	Map() :
		root(nullptr)
	{

	}

	//always creates a key if does not exist.  use find() to check first.
	VALUE_TYPE& operator[](const KEY_TYPE& key)
	{
		if(this->root == nullptr)
		{
			MapNode* newNode = new MapNode(key);
			return newNode->value;
		}

		MapNode* foundNode = this->Find(this->root, key);

		if(foundNode != nullptr)
		{
			return foundNode->value;
		}

		foundNode = new MapNode(key);
		if(this->root == nullptr)
		{
			this->root = foundNode;
		}
		else
		{
			this->addNode(this->root, foundNode);
		}

		return foundNode->value;
	}

	//to check whether a key exists, call this function directly
	MapNode* Find(MapNode const * const node, const KEY_TYPE& target)
	{
		if(node == nullptr)
		{
			return nullptr;
		}

		if(target < node->key)
		{
			return Find(node->leftChild, target);
		}
		else if(target > node->key)
		{
			return Find(node->rightChild, target);
		}
		else
		{
			return node;
		}

		return nullptr;
	}

private:
	//root is set elsewhere.
	void addNode(MapNode* node, MapNode* newChild)
	{
		if(newChild->key < node->key)
		{
			if(node->leftChild == nullptr)
			{
				node->leftChild = newChild;
			}
			else if(newChild->key > node->leftChild->key)
			{
				MapNode* previousLeftChild = node->leftChild;
				node->leftChild = newChild;
				newChild->leftChild = previousLeftChild;
				if(previousLeftChild->rightChild != nullptr && previousLeftChild->rightChild->key > newChild->key)
				{
					newChild->rightChild = previousLeftChild->rightChild;
					previousLeftChild->rightChild = nullptr;
				}
			}
			else
			{
				this->addNode(node->leftChild, newChild);
			}
		}
		else
		{
			if(node->rightChild == nullptr)
			{
				node->rightChild = newChild;
			}
			else if(newChild->key < node->rightChild->key)
			{
				MapNode* previousRightChild = node->rightChild;
				node->rightChild = newChild;
				newChild->rightChild = previousRightChild;
				if(previousRightChild->leftChild != nullptr && previousRightChild->leftChild->key > newChild->key)
				{
					newChild->leftChild = previousRightChild->leftChild;
					previousRightChild->leftChild = nullptr;
				}
			}
			else
			{
				this->addNode(node->leftChild, newChild);
			}
		}
	}
};

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
