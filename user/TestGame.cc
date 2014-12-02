extern "C" {
#include "libc.h"
#include "sys.h"
}

#include "libcc.h"

int main()
{
	const long screenBufferId = GetScreenBuffer();
	puts("testing background with buffer id ");
	putdec(screenBufferId);
	puts(".\n");

	unsigned char buf[320 * 200];



	int count = 0;
	int color = 2;
	while(1)
	{
		++count;
		for(int a = 0; a < 320 * 200; ++a)
		{
			buf[a] = a % 4 == 0 ? a % 256 : color;
		}
		Sleep(16);
		const int keyPressCount = GetQueuedKeyPressCount();

		if(keyPressCount > 0)
		{
			char* keyBuffer = new char[keyPressCount];
			GetQueuedKeyPresses(keyBuffer, keyPressCount);
			for(int a = 0; a < keyPressCount; ++a)
			{
				color += 10;
				color %= 256;
				char c[3] = {keyBuffer[a], '\n', 0};
				puts(c);
			}
			delete[] keyBuffer;
		}
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
