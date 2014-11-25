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
