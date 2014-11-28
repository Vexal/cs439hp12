extern "C" {
#include "libc.h"
#include "sys.h"
}


int main()
{
	const long screenBufferId = GetScreenBuffer();
	puts("testing background with buffer id ");
	putdec(screenBufferId);
	puts(".\n");

	unsigned char buf[320 * 200];

	for(int a = 0; a < 320 * 200; ++a)
	{
		buf[a] = a % 256;
	}

	//while(1)
	{
		if(WriteScreenBuffer(screenBufferId, buf) < 0)
		{
			puts("Failed to write Test Game screenbuffer.\n");
		}
	}

	return 0;
}
