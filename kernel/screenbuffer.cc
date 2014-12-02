#include "screenbuffer.h"

ScreenBuffer* ScreenBuffer::globalBuffer = nullptr;

ScreenBuffer::ScreenBuffer(int width, int height, unsigned int ownerProcessId, unsigned int bufferLocation) :
		Resource(SCREEN_BUFFER),
		width(width),
		height(height),
		buffer((unsigned char*)bufferLocation),
		ownerProcessId(ownerProcessId)
{
	Debug::printf("Creating new screen buffer for process: %d with width: %d and height: %d.\n", this->ownerProcessId, this->width, this->height);
}

int ScreenBuffer::GetNextChildBuffer()
{
	if(this->bufferRequests.isEmpty())
		return -1;

	ScreenBuffer*  nextBuffer = this->bufferRequests.removeHead();

	const int newId = nextBuffer->ownerProcessId;
	int widt = nextBuffer->width;
	Debug::printf("buffer %x process id: %d width: %d.\n", (long)nextBuffer, newId, widt);
	widt = nextBuffer->width;
	Debug::printf("new width: %d.\n", widt);
	this->childBuffers.Push(nextBuffer);
	return newId;
}

void ScreenBuffer::AddBufferRequest(ScreenBuffer* request)
{
	this->bufferRequests.addTail(request);
	Debug::printf("just added for p: %d.\n", request->GetOwnerProcessId());
}

 ScreenBuffer* ScreenBuffer::GetChildBuffer(int processId)
{
	//Debug::printf("About to find child buffers.\n");
	List<ScreenBuffer*>::ListNode* first = this->childBuffers.GetHead();
	if(first == nullptr)
		return nullptr;

	while(first != nullptr && first->value->ownerProcessId != processId)
	{
		first = first->next;
	}

	//Debug::printf("Found buffer %x with pid: %d.\n", (long)first->value, first->value->GetOwnerProcessId());
	return first->value;
}

ScreenBuffer::~ScreenBuffer()
{
	Debug::printf("DELETING SCREEN BUFFER!!\n\n\n\n!!!!!!!!!!");
	delete[] this->buffer;
}

