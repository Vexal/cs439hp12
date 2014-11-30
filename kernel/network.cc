#include "network.h"
#include "machine.h"

Network* Network::KernelNetwork = nullptr;
const unsigned char Network::myMac[6] = {0x52,0x54,0x00,0x12,0x34,0x56};

void Network::HandleNetworkInterrupt() 
{
    ++this->netCount;

	unsigned short interruptType = inw(ioaddr + 0x3E);
	Debug::printf("Received interrupt of type: %x from the network card.\n", interruptType);

    
    if((interruptType & 1) > 0)
    {
        this->handlePacketReceiveInterrupt();
    }
    if((interruptType & 2) > 0)
    {
        Debug::printf("Network interrupt error: %d\n", interruptType);
    }
    if((interruptType & 4) > 0)
    {
        //handle transmission complete
        this->endTxOKInterrupt();
    }
    if((interruptType & 16) > 0)
    {
    	//buffer full.
    	Debug::printf("Our network buffer is full.\n");
    }
}

Network::Network() :
	netCount(0),
	currentBufferPosition(0),
	currentBufferIndex(0)
{

}

void Network::handlePacketReceiveInterrupt()
{
	const unsigned char* const rcvBuffer = this->ReceiveBuffer + this->currentBufferPosition;
    const unsigned short packetLength = this->getCurrentPacketLength();
    const unsigned short realPacketLength = packetLength + 4;
    const PacketType etherType = getCurrentPacketType();
    if(this->isCurrentPacketForUs())    
    {
        Debug::printf("current packet length is %d\n", packetLength);
        unsigned char sender[6];
        this->getCurrentPacketSender(sender);
        Debug::printf("sender is ");
        for (int i = 0; i < 5; ++i) {
            Debug::printf("%x:", sender[i]);
        }
        Debug::printf("%x\n", sender[5]);

        //Debug::printf("type is %x\n", etherType);

        switch(etherType)
        {
            case PacketType::ARP:
            {
                Debug::printf("ARP PACKET");
                ARPPacket request;
                memcpy(&request, &this->ReceiveBuffer[this->currentBufferPosition + 18], 28);
                //request.printPacket();

                //this->arpCache.AddEntry(request.srcIP, request.srcMac);

                unsigned char packet[42] = {0};
                memcpy(packet, sender, 6);
                memcpy(packet+6, myMac, 6);
                packet[12] = 0x08;
                packet[13] = 0x06;

                ARPPacket reply;
                memcpy(reply.hardwareType, request.hardwareType, 2);
                memcpy(reply.protocolType, request.protocolType, 2);
                reply.hardwareLen = request.hardwareLen;
                reply.protocolLen = request.protocolLen;
                reply.operationCode[0] = 0;
                reply.operationCode[1] = 2;
                memcpy(reply.srcMac, myMac, 6);
                memcpy(reply.srcIP, request.destIP, 4);
                memcpy(reply.destMac, request.srcMac, 6);
                memcpy(reply.destIP, request.srcIP, 4);

                //reply.printPacket();

                memcpy(packet+14, &reply, 28);
                this->sendPacket(packet, 42);
                break;
            }

            case PacketType::IPv4:
            {
                Debug::printf("IPV4 PACKET");
                IPv4Header ipv4Header;
                memcpy(&ipv4Header, rcvBuffer + 18, sizeof(IPv4Header));
                /*for(int a = 0; a < packetLength; ++a)
                {     
                    Debug::printf("%d: %02x (%03d) \n", a, rcvBuffer[a],
                    		rcvBuffer[a]);
                }*/
                ipv4Header.print();

                switch(ipv4Header.protocol)
                {
					case 1: //ICMP
					{
						Debug::printf("Received ICMP packet.\n");
						ICMPHeader icmpHeader;
						memcpy(&icmpHeader, rcvBuffer + 18 + sizeof(IPv4Header), sizeof(ICMPHeader));
						icmpHeader.print();
						switch(icmpHeader.type)
						{
						case 0:
						{
							Debug::printf("Received ICMP echo reply.\n");
							break;
						}
						case 8:
						{
							Debug::printf("Received ICMP echo request.\n");
							break;
						}
						}
					}
					break;
                }
                break;
            }

            case PacketType::IPv6:
            {
            	Debug::printf("IPV6 PACKET");
                break;
            }

            case PacketType::UNKNOWN:
            {
            	Debug::printf("UNKNOWN PACKET TYPE.");
                break;
            }
        }
    }
    else
    {
        Debug::printf("Found somebody else's packet: %x %x %x %x %x %x\n",
        		rcvBuffer[4],
        		rcvBuffer[5],
        		rcvBuffer[6],
        		rcvBuffer[7],
        		rcvBuffer[8],
        		rcvBuffer[9]);
    }

    this->currentBufferPosition+= realPacketLength;
    this->currentBufferPosition = (this->currentBufferPosition + 3) & ~0x3;
    Debug::printf("\nCurrent network receive buffer position: %d\n", this->currentBufferPosition);
    this->endRxOKInterrupt();
}
void Network::sendPacket(const unsigned char* data, int length)
{
	//hd((unsigned long)data, (unsigned long)data +100);
    //while(1) {}
    memcpy((void*)this->TransmitBuffer, data, length);	//copy the packet into the buffer

    //set addr and size of tx buffer
    outl(ioaddr + 0x20 + 4 * currentBufferIndex,  (unsigned long)this->TransmitBuffer);
    outl(ioaddr + 0x10 + 4 * currentBufferIndex, length);

    currentBufferIndex++;
    currentBufferIndex %= 4;
}

bool Network::isCurrentPacketForUs() const
{
    int i = 0;
    for (; i < 6; ++i) 
    {
        //Debug::printf("Checking rcv %x vs our %x\n", this->ReceiveBuffer[this->currentBufferPosition + i + 4], myMac[i]);
        if(this->ReceiveBuffer[this->currentBufferPosition + i + 4] != myMac[i])
        {      
            break;
        }
    }

    if(i == 6)
        return true;

    i = 0;
    for (; i < 6; ++i) 
    {
        //Debug::printf("Checking rcv %x vs broadcast %x\n", this->ReceiveBuffer[this->currentBufferPosition + i + 4], 0xff);
        if(this->ReceiveBuffer[this->currentBufferPosition + i + 4] != 0xFF)
        {
            break;
        }
    }

    return i == 6;
}

//Retrieves the length of the packet currently
//pointed to by bufferPosition.
unsigned short Network::getCurrentPacketLength() const
{
    return *((short *) (this->ReceiveBuffer + this->currentBufferPosition + 2));
}

//Retrieves the sender of the packet currently
//pointed to by bufferPosition.
void Network::getCurrentPacketSender(unsigned char sender[6])
{
    for (int i = 0; i < 6; ++i) 
    {
        sender[i] = this->ReceiveBuffer[this->currentBufferPosition + i + 10];
    }
}

//Retrieves the sender of the packet currently
//pointed to by bufferPosition.
PacketType Network::getCurrentPacketType() const
{
    const unsigned char firstByte = this->ReceiveBuffer[this->currentBufferPosition + 16];
    const unsigned char secondByte = this->ReceiveBuffer[this->currentBufferPosition + 17];
    const unsigned short type =  (firstByte << 8) + secondByte;

    switch(type)
    {
        case 0x0806:
            return PacketType::ARP;
        case 0x0800:
            return PacketType::IPv4;
        case 0x86dd:
            return PacketType::IPv6;
    }

    return PacketType::UNKNOWN;
}

void Network::endRxOKInterrupt()
{
	 // Interrupt Status - Clears the Rx OK bit, acknowledging a packet has been received,
     // and is now in rx_buffer
	outw(ioaddr + 0x3E, 0x1);
}

void Network::endTxOKInterrupt()
{
     // Interrupt Status - Clears the Tx OWN bit, acknowledging a packet has been transmitted
    outl(ioaddr + 0x10, 0x0);
    outw(ioaddr + 0x3E, 0x4);
}

void Network::Init()
{
	const unsigned short vendor = pciCheckVendor(0, 3);
	Debug::printf("The vendor id is %x\n", vendor);
	
	for(int a = 0; a < 64; a+=2)
	{
		const unsigned short BAR0 = pciConfigReadWord(0, 3, 0, a);
	
		 Debug::printf("The thing at %x  is %x\n", a, BAR0);
	}	
	pciConfigWriteWord(0, 3, 0, 0xa, 6);
    outb( ioaddr + 0x52, 0x0);

    const long mac = inl(ioaddr);
    Debug::printf("Found mac0-5 %x\n", mac);

    outb( ioaddr + 0x37, 0x10);
    while( (inb(ioaddr + 0x37) & 0x10) != 0) { }

    const long transmitStatus = inl(ioaddr + 0x10);
	Debug::printf("Transmit status: %x\n", transmitStatus);

	const long transmitBufferAddress = inl(ioaddr + 0x20);
    outl(ioaddr + 0x20, (long)TransmitBuffer);
    const long transmitBufferAddress2 = inl(ioaddr + 0x20);
    Debug::printf("Transmit buffer: %x, then after setting: %x\n", transmitBufferAddress, transmitBufferAddress2);

    const long receiveBufferAddress = inl(ioaddr + 0x30);
    outl(ioaddr + 0x30, (long)ReceiveBuffer);
    const long receiveBufferAddress2 = inl(ioaddr + 0x30);
    Debug::printf("Receive buffer: %x, then after setting: %x\n", receiveBufferAddress, receiveBufferAddress2);

    const long imrMask = inw(ioaddr + 0x3C);
    outw(ioaddr + 0x3C, 0x0005);
    const long imrMask2 = inw(ioaddr + 0x3C);
    Debug::printf("IMR mask: %x, then after setting: %x\n", imrMask, imrMask2);

    const long isrMask = inw(ioaddr + 0x3e);
    //outw(ioaddr + 0x3E, 0x0005);
    const long isrMask2 = inw(ioaddr + 0x3E);
    Debug::printf("ISR mask: %x, then after setting: %x\n", isrMask, isrMask2);

    const long rcvConfig =  0xf | (1 << 7);
    const long rcvConfigInitial = inl(ioaddr + 0x44);
    outl(ioaddr + 0x44,rcvConfig); // (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP
    const long rcvConfigSet = inl(ioaddr + 0x44);

    Debug::printf("Receive Config: %x, then after setting: %x\n", rcvConfigInitial, rcvConfigSet);


    const long reAndTe = inb(ioaddr + 0x37);
    outb(ioaddr + 0x37, 0x0C);
    const long reAndTe2 = inb(ioaddr + 0x37);
    Debug::printf("Receive / Transmit enable: %x, then after setting: %x\n", reAndTe, reAndTe2);

}

void Network::InitNetwork()
{
	Network::KernelNetwork = new Network();
	Network::KernelNetwork->Init();
}

unsigned int Network::pciConfigReadWord(unsigned char bus, unsigned char slot, unsigned
		char func, unsigned char offset)
{
	unsigned int address;
	unsigned int lbus = (unsigned int)bus;
	unsigned int lslot = (unsigned int)slot;
	unsigned int lfunc = (unsigned int)func;
	unsigned short  tmp = 0;

	address = (unsigned int)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
			(offset & 0xfc) | ((unsigned int)0x80000000));

	outl(0xCF8, address);

	//tmp = inl(0xfc);
	tmp = (unsigned short)((inl(0xcfc) >> ((offset & 2) * 8)) & 0xFFFF);
	return tmp;
}

void Network::pciConfigWriteWord(unsigned char bus, unsigned char slot, unsigned
		char func, unsigned char offset, unsigned short val)
{
	offset = 0x6;
	const int sz = 0;
	unsigned int address;
	unsigned int lbus = (unsigned int)bus;
	unsigned int lslot = (unsigned int)slot;
	unsigned int lfunc = (unsigned int)func;
	unsigned long  tmp = 0;

	address = (unsigned int)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
			(offset & 0xfc) | ((unsigned int)0x80000000));

	//address = (lbus << 16) | (lslot << 11) | (lfunc << 8) | offset | 0x80000000;
	outl(0xCF8, address);

	tmp = inl(0xcfc + sz);
	Debug::printf("Read in %x\n", tmp);
	//tmp = (unsigned short)((inl(0xcfc) >> ((offset & 2) * 8)) & 0xFFFF);
	//return tmp;

	outl(0xcfc + sz, (unsigned int)0xFFFF0107);

	tmp = inl(0xcfc + sz);
	Debug::printf("Read in %x\n", tmp);
}

unsigned short Network::pciCheckVendor(unsigned char bus, unsigned char slot)
{
	unsigned short vendor;// device;

	if((vendor = pciConfigReadWord(bus, slot, 0, 0)) != 0xFFFF)
	{
	}

	return vendor;
}
