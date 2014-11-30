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
	defaultChildX(30),
	defaultChildY(30),
	defaultChildWidth(80),
	defaultChildHeight(60),
	defaultBorderWidth(3),
	backgroundBorderColor(3),
	foregroundBorderColor(4),
	foregroundWindow(nullptr)

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
		Sleep(1000);
		//puts("rendering.\n");
		LockScreenBuffer(this->screenBufferId);
		this->acquireNewChildProcesses();
		this->renderChildren();
		this->sendBufferData();
		UnlockScreenBuffer(this->screenBufferId);
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
			const int newId = newProcessIds[a];
			putdec(newId);
			puts("\n");
			ChildWindow* newWindow = new ChildWindow(this->defaultChildX, this->defaultChildY, 80, 60, newId, this->defaultBorderWidth);
			this->foregroundWindow = newWindow;
			this->defaultChildX += 95;
			this->defaultChildY += 15;
			this->childWindows.Push(newWindow);
			//puthex((long)this->childWindows.first->value);
		}

		puts("initialized new process windows.\n");
	}
}

void DesktopWindowManager::renderChildren()
{
	List<ChildWindow*>::ListNode* first = this->childWindows.GetHead();

	while(first != nullptr)
	{
		//render the child window at the correct position.
		const ChildWindow* const child = first->value;
		this->renderWindowBorder(child);

		unsigned char* childWindowBuffer = new unsigned char[child->GetWidth() * child->GetHeight()];
		GetChildBuffer(childWindowBuffer, child->GetProcessId());
		int px = 0;

		for(int x = child->GetX(); x < child->GetX() + child->GetWidth(); ++x)
		{
			if(x >= this->width)
				continue;
			for(int y = child->GetY(); y < child->GetY() + child->GetHeight() && y < this->height; ++y)
			{
				const int pixelLocation = x + this->width * y;
				this->buffer[pixelLocation] = childWindowBuffer[px];

				++px;
			}
		}

		delete[] childWindowBuffer;
		first = first->next;
	}
}

void DesktopWindowManager::sendBufferData() const
{
	WriteScreenBuffer(screenBufferId, this->buffer);
}

void DesktopWindowManager::renderWindowBorder(const ChildWindow* const child)
{
	const int borderXLeftStart = child->GetX() - child->GetBorderWidth();
	for(int x = borderXLeftStart; x < borderXLeftStart + child->GetBorderWidth(); ++x)
	{
		if(x >= this->width)
			continue;

		for(int y = child->GetY() - child->GetBorderWidth(); y < child->GetY() + child->GetBorderWidth() + child->GetHeight() && y < this->height; ++y)
		{
			const int pixelLocation = x + this->width * y;
			this->buffer[pixelLocation] = this->foregroundWindow == child ? this->foregroundBorderColor : this->backgroundBorderColor;
		}
	}

	const int borderXRightStart = child->GetX() + child->GetWidth();
	for(int x = borderXRightStart; x < borderXRightStart + child->GetBorderWidth(); ++x)
	{
		if(x >= this->width)
			continue;

		for(int y = child->GetY() - child->GetBorderWidth(); y < child->GetY() + child->GetBorderWidth() + child->GetHeight() && y < this->height; ++y)
		{
			const int pixelLocation = x + this->width * y;
			this->buffer[pixelLocation] = this->foregroundWindow == child ? this->foregroundBorderColor : this->backgroundBorderColor;
		}
	}


	for(int x = borderXLeftStart; x < borderXLeftStart + child->GetWidth() + child->GetBorderWidth(); ++x)
	{
		if(x >= this->width)
			continue;

		for(int y = child->GetY() - child->GetBorderWidth(); y < child->GetY() + child->GetBorderWidth() && y < this->height; ++y)
		{
			const int pixelLocation = x + this->width * y;
			this->buffer[pixelLocation] = this->foregroundWindow == child ? this->foregroundBorderColor : this->backgroundBorderColor;
		}
	}

	for(int x = borderXLeftStart; x < borderXLeftStart + child->GetWidth() + child->GetBorderWidth(); ++x)
	{
		if(x >= this->width)
			continue;

		for(int y = child->GetY() + child->GetHeight(); y < child->GetY() + child->GetBorderWidth() + child->GetHeight() && y < this->height; ++y)
		{
			const int pixelLocation = x + this->width * y;
			this->buffer[pixelLocation] = this->foregroundWindow == child ? this->foregroundBorderColor : this->backgroundBorderColor;
		}
	}
}

ChildWindow::ChildWindow(int xPosition, int yPosition, int width, int height, int processId, int borderWidth) :
		xPosition(xPosition),
		yPosition(yPosition),
		width(width),
		height(height),
		processId(processId),
		borderWidth(borderWidth)
{

}



