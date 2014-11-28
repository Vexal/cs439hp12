/*
 * DesktopWindowManager.cc

 *
 *  Created on: Nov 25, 2014
 *      Author: vexal
 */
extern "C" {
#include "libc.h"
#include "sys.h"
}

#include "libcc.h"

#include "DesktopWindowManager.h"

DesktopWindowManager::DesktopWindowManager(unsigned int width, unsigned int height) :
	screenBufferId(-1),
	width(width),
	height(height),
	buffer(new unsigned char[width * height]),
	testCount(0),
	defaultChildX(0),
	defaultChildY(0),
	defaultChildWidth(80),
	defaultChildHeight(50)

{

}

void DesktopWindowManager::Initialize()
{
	this->screenBufferId = GetScreenBuffer();
	if(this->screenBufferId < 0)
	{
		puts("Failed to acquire screen buffer.\n");
	}

	puts("Successfully acquired screen buffer.\n");

	for(int a = 0; a < 320; ++a)
	{
		for(int b = 0; b < 200; ++b)
		{
			this->buffer[b*320 + a] = 0;
		}
	}
}

void DesktopWindowManager::Run()
{
	while(1)
	{
		for(int a = 0; a < 320; ++a)
		{
			for(int b = 0; b < 200; ++b)
			{
				this->buffer[b*320 + a] = testCount % 256;
			}
		}

		this->acquireNewChildProcesses();
		this->sendBufferData();
		++testCount;
	}
}

//query the OS for a list of new child processes requesting a window since the last update
//maybe this can be replaced later with some sort of signal from OS to window manager process.
void DesktopWindowManager::acquireNewChildProcesses()
{
	const int newProcessCount = GetBufferRequestCount();

	if(newProcessCount > 0)
	{
		puts("found new process for window: ");
		putdec(newProcessCount);
		puts("\n");
		int* newProcessIds = new int[newProcessCount];
		GetNewWindowRequests(newProcessIds);
		for(int a = 0; a < newProcessCount; ++a)
		{
			puts("Initializing window for process: ");
			putdec(newProcessIds[a]);
			puts("\n");
			this->childWindows.Push(new ChildWindow(30, 30, 80, 60, newProcessIds[a]));
		}

		puts("initialized new process windows.\n");
	}
}

void DesktopWindowManager::sendBufferData()
{
	WriteScreenBuffer(screenBufferId, this->buffer);
}

ChildWindow::ChildWindow(int xPosition, int yPosition, int width, int height, int processId) :
		xPosition(xPosition),
		yPosition(yPosition),
		width(width),
		height(height),
		processId(processId)
{

}



