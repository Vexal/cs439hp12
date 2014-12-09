extern "C" {
#include "libc.h"
#include "sys.h"
}

#include "libcc.h"
#include "smile.h"

int main(int argc, char** args)
{
	//get server ip address
	const char* addrStr = args[1];

	int i = 0;
	unsigned char addr[4];
	int j = 0;
	unsigned char byte = 0;
	while (addrStr[i] != 0)
	{
		if (addrStr[i] == '.')
		{
			addr[j] = byte;
			j++;
			byte = 0;
		}
		else
		{
			byte = byte * 10 + ((unsigned char)addrStr[i]-48);
		}

		++i;
	}
	addr[3] = byte;

	const long screenBufferId = GetScreenBuffer();

	char smileBuffer[512];
	const long smileId = open("smile.pic");
	if(smileId < 0)
	{
		puts("failed to load smile.\n");
		return -1;
	}

	unsigned char buf[80 * 60];
	int count = 0;
	int color = 2;
	for(int a = 0; a < 80 * 60; ++a)
	{
		buf[a] = 2;
	}
	const long bytesRead = read(smileId, (void*)smileBuffer, 512);
	char smile[256];

	smileBuffer[bytesRead] = 0;
	for(int y = 0; y < 16; ++y)
	{
		for(int x = 0; x < 16; ++x)
		{
			smile[x * 16 + y] = smileBuffer[y * 17 + x];
		}
	}

	Smile mySmile;
	mySmile.x = 15;
	mySmile.y = 18;

	const int socketDescriptor = OpenSocket(1, 3);
	if(socketDescriptor < 0)
	{
		puts("Failed to open socket.\n");
	}

	puts("Successfully opened socket descriptor on port 16.\n");

	while(1)
	{

		unsigned char buffer[512];
		unsigned char ip[4];
		const int bytesFromNetwork = ReadSocket(socketDescriptor, ip, buffer, 512);

		if (bytesFromNetwork > 0)
		{
			putdec(bytesFromNetwork);
			puts("\n");
			puts((const char*)buffer);
			puts("\n");

			if(buffer[0] == 'b' && buffer[1] == ':')
			{
				puts("Received ball position update: ");
				int ballX;
				int ballY;
				memcpy(&ballX, buffer + 2, 4);
				memcpy(&ballY, buffer + 6, 4);

				putdec(ballX); puts(", "); putdec(ballY); puts("\n");
				mySmile.x = ballX;
				mySmile.y = ballY;
			}
		}


		for(int a = 0; a < 80 * 60; ++a)
		{
			buf[a] = 2;
		}

		++count;
		const int keyPressCount = GetQueuedKeyPressCount();

		if(keyPressCount > 0)
		{
			char* keyBuffer = new char[keyPressCount];
			GetQueuedKeyPresses(keyBuffer, keyPressCount);
			for(int a = 0; a < keyPressCount; ++a)
			{
				switch(keyBuffer[a])
				{
				/*case 'a':
					mySmile.x-= 1;
					break;
				case 'd':
					mySmile.x+= 1;
					break;
				case 'w':
					mySmile.y-= 1;
					break;
				case 's':
					mySmile.y+= 1;
					break;*/
				case 't':
				{
					const char* data = "c:";
					WriteSocket(socketDescriptor, addr, (unsigned char*)data, strlen(data) + 1, 2);
				}
				break;
				}
			}

			delete[] keyBuffer;
		}

		for(int y = 0; y < 16; ++y)
		{
			for(int x = 0; x < 16; ++x)
			{
				buf[(x + mySmile.x) * 60 + y + mySmile.y] = smile[x*16 + y] != '0' ? 4 : 0;
			}
		}

		LockScreenBuffer(screenBufferId);
		if(WriteScreenBuffer(screenBufferId, buf) < 0)
		{
			puts("Failed to write Test Game screenbuffer.\n");
		}
		UnlockScreenBuffer(screenBufferId);

		Sleep(16);
	}

	return 0;
}
