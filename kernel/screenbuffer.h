#ifndef _SCREENBUFFER_H_
#define _SCREENBUFFER_H_
#include "resource.h"
#include "machine.h"
#include "queue.h"

//The buffer request resource keeps track of processes who request
//a window buffer when a former process already owns the rights to render it.
//the owning process of the global buffer should find these requests so that
//it may handle the itself.

class BufferRequest : public Resource
{
public:
	int processId;
	int resourceId;

	BufferRequest(int processId) : Resource(BUFFER_REQUEST), processId(processId), resourceId(-1) {}
};

class ScreenBuffer : public Resource
{
public:
	static ScreenBuffer* globalBuffer;
private:

	static constexpr unsigned int vgaMemory = 0xA0000;
	const int width;
	const int height;
	unsigned char* const buffer;
	unsigned int ownerProcessId;
	SimpleQueue<ScreenBuffer*> bufferRequests; // a list of processes requesting to create a window
	List<ScreenBuffer*> childBuffers;

public:
	ScreenBuffer(int width, int height, unsigned int ownerProcessId);
	inline void AddBufferRequest(ScreenBuffer* request) {this->bufferRequests.addTail(request);}
	inline void WriteBuffer(const unsigned char* buffer) {memcpy(this->buffer, buffer, this->width * this->height);}
	inline void WriteBuffer(int x, int y, unsigned char val) {this->buffer[this->width * y + x] = val;}

	inline int GetWidth() const {return this->width;}
	inline int GetHeight() const {return this->height;}
	inline unsigned int GetOwnerProcessId() const {return this->ownerProcessId;}
	inline const ScreenBuffer* const GetChildBuffer(int processId) {List<ScreenBuffer*>::ListNode* first = this->childBuffers.GetHead(); while(first != nullptr && first->value->ownerProcessId != processId) {first = first->next;} return first->value;}
	inline void CopyChildBuffer(int processId, unsigned char* buf) {const ScreenBuffer* const child = this->GetChildBuffer(processId); memcpy(buf, child->buffer, child->width * child->height);}
	int GetNextChildBuffer();
	inline int GetBufferRequestCount() const {return this->bufferRequests.GetSize();}
};




#endif
