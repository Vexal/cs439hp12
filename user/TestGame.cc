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



	int count = 0;
	while(1)
	{
		++count;
		for(int a = 0; a < 320 * 200; ++a)
		{
			buf[a] = count % 256;
		}
		Sleep(16);
		//puts("attemping to lock buffer for writing: ");
		//putdec(screenBufferId);
		LockScreenBuffer(screenBufferId);
		//puts(" ...done.\n");
		if(WriteScreenBuffer(screenBufferId, buf) < 0)
		{
			puts("Failed to write Test Game screenbuffer.\n");
		}
		UnlockScreenBuffer(screenBufferId);

		//puts("finished writing buffer.\n");

	}

	return 0;
}
