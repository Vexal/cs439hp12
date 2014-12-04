#include "socket.h"

Socket::Socket(PacketProtocol protocol, int port, int owningProcess) :
	protocol(protocol),
	port(port),
	owningProcess(owningProcess)
{

}
