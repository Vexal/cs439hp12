#ifndef _SCREENBUFFER_H_
#define _SCREENBUFFER_H_
#include "resource.h"

class ScreenBuffer : public Resource
{
private:
	const int width;
	const int height;
	char* buffer;

public:
	ScreenBuffer(int width, int height);
	inline void WriteBuffer(const char* buffer, unsigned int len) {memcpy(this->buffer, buffer, len);}
	inline void WriteBuffer(int x, int y, unsigned char val) {this->buffer[this->width * y + x] = val;}

	inline int GetWidth() const {return this->width;}
	inline int GetHeight() const {return this->height;}

};




#endif
