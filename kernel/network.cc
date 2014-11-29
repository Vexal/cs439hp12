#include "network.h"
#include "machine.h"
#include "debug.h"

Network* Network::KernelNetwork = nullptr;

void Network::HandleNetworkInterrupt() 
{
	unsigned char interruptType = inl(ioaddr + 0x3E);

	if(interruptType != 0)
	Debug::printf("Received interrupt of type: %x from the network card.\n", interruptType);
	if(netCount > 1)
	{
		this->endNetworkInterrupt();
	   	return;
	}
        
    for(int a = 0; a < 200; ++a)
    {     
    	Debug::printf("%02x (%03d) ", this->ReceiveBuffer[a], this->ReceiveBuffer[a]);
    }

    ++this->netCount;
    this->endNetworkInterrupt();
    unsigned char data[64] = {0};
    data[0] = 0x5e;
    data[1] = 0xb4;
    data[2] = 0x75;
    data[3] = 0xba;
    data[4] = 0xfa;
    data[5] = 0x9a;
    data[6] = 0x52;
    data[7] = 0x54;
    data[8] = 0x00;
    data[9] = 0x12;
    data[10] = 0x34;
    data[11] = 0x56;
    data[12] = 0x08;
    data[13] = 0x06;
    data[14] = 0x00;
    data[15] = 0x01;
    data[16] = 0x08;
    data[17] = 0x00;
    data[18] = 0x06;
    data[19] = 0x04;
    data[20] = 0x00;
    data[21] = 0x02;
    data[22] = 0x52;
    data[23] = 0x54;
    data[24] = 0x00;
    data[25] = 0x12;
    data[26] = 0x34;
    data[27] = 0x56;
    data[28] = 0xc0;
    data[29] = 0xa8;
    data[30] = 0x07;
    data[31] = 0x02;
    data[32] = 0x5e;
    data[33] = 0xb4;
    data[34] = 0x75;
    data[35] = 0xba;
    data[36] = 0xfa;
    data[37] = 0x9a;
    data[38] = 0xc0;
    data[39] = 0xa8;
    data[40] = 0x07;
    data[41] = 0x01;

    //this->sendPacket(data, 64);
    Debug::printf("\n\n\n");
}

Network::Network() :
	netCount(0),
	bufferPosition(0),
	currentBufferIndex(0)
{

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

//Retrieves the length of the packet currently
//pointed to by bufferPosition.
long Network::getCurrentPacketLength() const
{

}

void Network::endNetworkInterrupt()
{
	 // Interrupt Status - Clears the Rx OK bit, acknowledging a packet has been received,
     // and is now in rx_buffer
	outw(ioaddr + 0x3E, 0x1);
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

    unsigned char data[64] = {0xAA};
    for(int a = 0; a < 64; ++a)
    {
    	data[a] = a % 16;
    }
    data[0] = 0x5e;
    data[1] = 0xb4;
    data[2] = 0x75;
    data[3] = 0xba;
    data[4] = 0xfa;
    data[5] = 0x9a;
    data[6] = 0x52;
    data[7] = 0x54;
    data[8] = 0x00;
    data[9] = 0x12;
    data[10] = 0x34;
    data[11] = 0x56;
    data[12] = 0x08;
    data[13] = 0x06;
    data[14] = 0x00;
    data[15] = 0x01;
    data[16] = 0x08;
    data[17] = 0x00;
    data[18] = 0x06;
    data[19] = 0x04;
    data[20] = 0x00;
    data[21] = 0x02;
    data[22] = 0x52;
    data[23] = 0x54;
    data[24] = 0x00;
    data[25] = 0x12;
    data[26] = 0x34;
    data[27] = 0x56;
    data[28] = 0xc0;
    data[29] = 0xa8;
    data[30] = 0x07;
    data[31] = 0x02;
    data[32] = 0x5e;
    data[33] = 0xb4;
    data[34] = 0x75;
    data[35] = 0xba;
    data[36] = 0xfa;
    data[37] = 0x9a;
    data[38] = 0xc0;
    data[39] = 0xa8;
    data[40] = 0x07;
    data[41] = 0x01;

    this->sendPacket(data, 64);
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