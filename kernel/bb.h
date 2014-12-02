#ifndef _BB_H_
#define _BB_H_

#include "semaphore.h"
#include "io.h"

//
// Bounded buffer using semaphores
//
template<typename T> class BB : public InputStream<T>, OutputStream<T> {
    T *arr;
    int n;
    int head;
    int tail;
    Semaphore nFull;
    Semaphore nEmpty;
    Semaphore mutex;
    int counts;
public:
    BB(int n) : arr(new T[n]),n(n),head(0),tail(0),nFull(0),nEmpty(n),mutex(1), counts(0) {
    	counts = 0;
    }

    void put(T v) {
        nEmpty.down();
        mutex.down();
        arr[tail++]  = v;
        ++counts;
        if (tail == n) tail = 0;
        mutex.up();
        nFull.up();
    }    

    T get() {
        nFull.down();
        mutex.down();
        --counts;
        T v = arr[head++];
        if (head == n) head = 0;
        mutex.up();
        nEmpty.up();
        return v;
    }

    int GetCount() const
    {
    	return counts;
    }
        
};

#endif
