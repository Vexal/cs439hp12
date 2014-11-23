#include "screenbuffer.h"


ScreenBuffer::ScreenBuffer(int width, int height) :
		Resource(SCREEN_BUFFER),
		width(width),
		height(height),
		buffer((unsigned char*)ScreenBuffer::vgaMemory)
{
}
