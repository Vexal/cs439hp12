#ifndef _NETWORK_H_
#define _NETWORK_H_

struct ARPEntry
{
	char ipAddress[4];
	char macAddress[6];
};

class ARPCache
{
public:
	void AddEntry(const char ipAddress[4], const char macAddress[6]);
	ARPEntry GetEntry(const char ipAddress[4]) const;
private:
	ARPEntry cache[100];
};

class Network
{
public:
	static constexpr unsigned int networkBufferSize = 16000;
	static Network* KernelNetwork;
	static constexpr unsigned int ioaddr = 0xc000;
	ARPCache arpCache;
	long netCount;
	long bufferPosition;
	int currentBufferIndex;

	unsigned char ReceiveBuffer[Network::networkBufferSize];
	unsigned char TransmitBuffer[Network::networkBufferSize];

	Network();
	void Init();
	void HandleNetworkInterrupt();

private:
	long getCurrentPacketLength() const;
	void endNetworkInterrupt();
	void sendPacket(const unsigned char* data, int length);

public:
	static void InitNetwork();
	static unsigned int pciConfigReadWord(unsigned char bus, unsigned char slot, unsigned
		char func, unsigned char offset);
	static void pciConfigWriteWord(unsigned char bus, unsigned char slot, unsigned
		char func, unsigned char offset, unsigned short val);
	static unsigned short pciCheckVendor(unsigned char bus, unsigned char slot);   
};

#endif
