extern "C" {
#include "libc.h"
#include "sys.h"
}

#include "libcc.h"


int main()
{
	puts((const char*)"this works\n");
	puts("this still works\n");
	char* testNew = new char[25];
	testNew[0] = 'a';
	testNew[1] = 'c';
	testNew[2] = '\n';
	testNew[3] = 0;
	
	puts(testNew);
	testdraw('a');
	puts("Finished calling testdraw\n");
	delete[] testNew;

	const long screenBufferId = GetScreenBuffer();
	if(screenBufferId < 0)
	{
		puts("Failed to acquire screen buffer.\n");
	}

	puts("Successfully acquired screen buffer.\n");

	unsigned char* buf = new unsigned char[320*200];

	for(int a = 0; a < 320; ++a)
	{
		for(int b = 0; b < 200; ++b)
		{
			buf[b*320 + a] = 2;
		}
	}
	
	WriteScreenBuffer(screenBufferId, buf);
	return 0;
}
