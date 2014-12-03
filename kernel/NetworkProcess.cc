#include "NetworkProcess.h"

NetworkProcess::NetworkProcess() :
	Process("network",nullptr)
{

}

//dispatch packets to destination processes;
//send packets bbbbbbbbbbbbbbbbbbbbbbbbbbbb;
long NetworkProcess::run()
{
	Process::disable();
	while(!this->networkSending.isEmpty())
	{
		Network::KernelNetwork->SendPacket(this->networkSending.removeHead());
	}
	Process::enable();
}

void NetworkProcess::QueueNetworkSend(const Packet& packet)
{
	this->networkSending.addTail(packet);
}

void NetworkProcess::QueueNetworkReceive(const Packet& packet)
{
	this->networkReceiving.addTail(packet);
}
