#ifndef _KBD_H_
#define _KBD_H_

#include "bb.h"
#include "semaphore.h"
class Keyboard {
    
public:
	/*static char inputBuffer[128];
	static int inputBufferPosition;
	static int inputGetPosition;
	static Mutex mutex;
	static char GetNextKey()
	{
		mutex.lock();
		const char c = inputBuffer[inputGetPosition++];
		if(inputGetPosition >)
		mutex.unlock();
		return c;
	}

	static void PutKey(char c)
	{
		mutex.lock();
		inputBuffer[inputBufferPosition++] = c;
		if(inputBufferPosition >= 128)
		{
			inputBufferPosition = 0;
		}
		mutex.unlock();
	}*/
    static InputStream<char> *is;
    static void handler(void);
    static void init(void);
};

#endif
