#ifndef _LIBCC_H
#define _LIBCC_H

typedef unsigned int size_t;

extern "C" {
#include "heap.h"
}

void* operator new(size_t size)
{
	return malloc(size);
}

void operator delete(void* p)
{
	return free(p);
}

void* operator new[](size_t size)
{
	return malloc(size);
}

void operator delete[](void* p)
{
	return free(p);
}

#endif
