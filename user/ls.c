#include "libc.h"

int main() {

	const long fileHandle = open(".");
	if(fileHandle < 0)
	{
		puts("didn't work\n");
	}

	const long fileLength = getlen(fileHandle);
	for(int a = 0; a < fileLength; a += 16)
	{
		char fileName[16];
		read(fileHandle, fileName, 16);
		fileName[12] = 0;
		puts(fileName);
		puts("\n");
	}
    return 0;
}
