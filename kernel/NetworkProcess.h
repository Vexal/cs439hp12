#ifndef _NETWORK_PROCESS_H_
#define _NETWORK_PROCESS_H_
#include "process.h"
#include "network.h"

class NetworkProcess : public Process
{
public:
	SimpleQueue<Packet> networkSending;
	SimpleQueue<Packet> networkReceiving;

	NetworkProcess();
    virtual long run();

    void QueueNetworkSend(const Packet& packet);
    void QueueNetworkReceive(const Packet& packet);
};


#endif
