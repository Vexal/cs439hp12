#include "socket.h"

Socket::Socket(PacketProtocol protocol, int port, int owningProcess) :
	Resource(ResourceType::SOCKET),
	protocol(protocol),
	port(port),
	owningProcess(owningProcess)
{

}
