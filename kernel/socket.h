#ifndef _SOCKET_H_H_
#define _SOCKET_H_H_
#include "resource.h"
#include "network.h"

class Socket : public Resource
{
	const PacketProtocol protocol;
	const int port;
	const int owningProcess;

public:

	Socket(PacketProtocol protocol, int port, int owningProcess);

	inline PacketProtocol GetProtocol() const {return this->protocol;}
	inline int GetPort() const {return this->port;}
	inline int GetOwningProcess() const {return this->owningProcess;}
};





#endif
