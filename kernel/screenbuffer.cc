#include "screenbuffer.h"

ScreenBuffer* ScreenBuffer::globalBuffer = nullptr;

ScreenBuffer::ScreenBuffer(int width, int height, unsigned int ownerProcessId) :
		Resource(SCREEN_BUFFER),
		width(width),
		height(height),
		buffer((unsigned char*)ScreenBuffer::vgaMemory),
		ownerProcessId(ownerProcessId)
{
}

int ScreenBuffer::GetNextChildBuffer()
{
	Debug::printf("asdfasdf\n");
	if(this->bufferRequests.isEmpty())
		return -1;

	ScreenBuffer const * const nextBuffer = this->bufferRequests.removeHead();

	Debug::printf("buffer %x process id: ", nextBuffer);
	const int newId = nextBuffer->ownerProcessId;
	Debug::printf("%d...\n", newId);
	return newId;
}
