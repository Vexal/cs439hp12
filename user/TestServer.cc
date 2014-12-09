extern "C" {
#include "libc.h"
#include "sys.h"
}

#include "libcc.h"

struct Vector
{
	int x;
	int y;
	Vector operator+(const Vector &vec)
	{
		return Vector(this->x+vec.x, this->y+vec.y);
	}

	Vector operator+=(const Vector &vec)
	{
		this->x += vec.x;
		this->y += vec.y;
		return *this;
	}

	Vector(int x, int y) : x(x), y(y)
	{}
};

class Ball
{
public:
	Vector position;
	Vector velocity;

public:
	void Move()
	{
		position += velocity;
		if (position.x < 0)
		{
			position.x = -position.x;
			velocity.x = -velocity.x;
		}
		if (position.y < 0)
		{
			position.y = -position.y;
			velocity.y = -velocity.y;
		}
		if ((position.x + 16) > 80)
		{
			position.x = position.x - (position.x + 16 - 80)*2;
			velocity.x = -velocity.x;
		}
		if ((position.y + 16) > 60)
		{
			position.y = position.y - (position.y + 16 - 60)*2;
			velocity.y = -velocity.y;
		}
	}

	Ball(Vector position, Vector velocity) : position(position), velocity(velocity)
	{}

};

class Game
{
public:
	Ball ball;

public:
	void Update()
	{
		ball.Move();
	}

	void Start()
	{
		Vector pos(32,22);
		Vector vel(5,5);
		ball = Ball(pos, vel);
	}

	Game() : ball(Vector(0,0), Vector(0,0))
	{

	}
};

struct Client
{
	unsigned char ip[4];
	int playerNum;
};

int main(int argc, char** args)
{
	int listenPort = 2;
	int sendPort = 3;
	puts("Listen port is: "); putdec(listenPort); puts(" send port is: "); putdec(sendPort); puts(".\n");
	

	const int socketDescriptor = OpenSocket(1, listenPort);
	if(socketDescriptor < 0)
	{
		puts("Failed to open socket.\n");
	}

	puts("Successfully opened socket descriptor on port 17.\n");

	Game game;

	Client* connections[256] = {nullptr};
	List<Client*> clients;

	while(1)
	{

		unsigned char buffer[512];
		unsigned char ip[4];
		const int bytesFromNetwork = ReadSocket(socketDescriptor, ip, buffer, 512);

		if (bytesFromNetwork > 0)
		{
			puts((const char*)buffer);
			puts("\n");

			//check for connections;

			if(buffer[0] == 'c' && buffer[1] == ':')
			{
				if (connections[ip[3]] == nullptr)
				{
					Client* client = new Client();
					memcpy(client->ip, ip, 4);
					client->playerNum = clients.size;
					connections[ip[3]] = client;
					clients.Push(client);
					puts("Received a new connection request ");
					putdec(ip[3]);
					puts("\n");
					if(clients.size >= 2)
					{
						game.Start();
					}
				}
			}
			
		}

		if(clients.size >= 2)
		{
			game.Update();
		}

		List<Client*>::ListNode* node = clients.GetHead();
		while(node != nullptr)
		{
			unsigned char data[10];
			data[0] = 'b'; data[1] = ':';
			memcpy(data + 2, &game.ball.position.x, 4);
			memcpy(data + 6, &game.ball.position.y, 4);

	 		WriteSocket(socketDescriptor, node->value->ip, (unsigned char*)data, 10, sendPort);
			node = node->next;
		}
		const int keyPressCount = GetQueuedKeyPressCount();

		// if(keyPressCount > 0)
		// {
		// 	char* keyBuffer = new char[keyPressCount];
		// 	GetQueuedKeyPresses(keyBuffer, keyPressCount);
		// 	for(int a = 0; a < keyPressCount; ++a)
		// 	{
		// 		const char data[2] = {keyBuffer[a], 0};

		// 		WriteSocket(socketDescriptor, addr, (unsigned char*)data, strlen(data) + 1, sendPort);
		// 	}

		// 	delete[] keyBuffer;
		// }

		//unsigned char* buffer;
		//buffer[0] = 'b';
		//memcpy(buffer + 1, game)
		Sleep(16);
	}

	return 0;
}
