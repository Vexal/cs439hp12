#ifndef _TEST_PROGRAM_H_
#define _TEST_PROGRAM_H_

#include "libcc.h"

class ChildWindow
{
private:
	int xPosition;
	int yPosition;
	int width;
	int height;

public:
	ChildWindow(int xPosition, int yPosition, int width, int height);
};

class DesktopWindowManager
{
private:
	long screenBufferId;
	unsigned int width;
	unsigned int height;
	unsigned char* buffer;
	unsigned int testCount;

	int defaultChildX;
	int defaultChildY;

	int defaultChildWidth;
	int defaultChildHeight;

	List<ChildWindow*> childWindows;
public:
	DesktopWindowManager(unsigned int width, unsigned int height);
	void Initialize();
	void Run();

private:
	void acquireNewChildProcesses();
	void sendBufferData();
};



#endif
