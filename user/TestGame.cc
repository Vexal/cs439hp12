extern "C" {
#include "libc.h"
#include "sys.h"
}


int main()
{
	const long screenBufferId = GetScreenBuffer();
	puts("testing background\n");

	unsigned char buf[320 * 200];

	for(int a = 0; a < 320 * 200; ++a)
	{
		buf[a] = a % 256;
	}

	WriteScreenBuffer(screenBufferId, buf);
}
