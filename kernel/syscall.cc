#include "syscall.h"
#include "machine.h"
#include "idt.h"
#include "process.h"
#include "child.h"
#include "fs.h"
#include "err.h"
#include "u8250.h"
#include "libk.h"
#include "screenbuffer.h"

unsigned char* networkBuffer = nullptr;

void Syscall::init(void) {
    IDT::addTrapHandler(100,(uint32_t)syscallTrap,3);
}

unsigned int pciConfigReadWord(unsigned char bus, unsigned char slot, unsigned
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

void pciConfigWriteWord(unsigned char bus, unsigned char slot, unsigned
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

unsigned short pciCheckVendor(unsigned char bus, unsigned char slot)
{
	unsigned short vendor;// device;

	if((vendor = pciConfigReadWord(bus, slot, 0, 0)) != 0xFFFF)
	{
	}

	return vendor;
}
extern "C" long syscallHandler(uint32_t* context, long num, long a0, long a1) {

    switch (num) {
    case 0: /* exit */
        Process::exit(a0);
        return -1;
    case 1: /* putchar */
        Debug::printf("%c",a0);
        return 0;
    case 2: /* fork */
        {
            uint32_t userPC = context[8];
            uint32_t userESP = context[11];
            Child *child = new Child(Process::current);
            child->pc = userPC;
            child->esp = userESP;
            child->eax = 0;
            long id = Process::current->resources->open(child);
            child->start();

            return id;
        }
    case 3: /* semaphore */
        {
            Semaphore *s = new Semaphore(a0);
            return Process::current->resources->open(s);
        }
    case 4: /* down */
        {
            Semaphore* s = (Semaphore*) Process::current->resources->get(
                 a0,ResourceType::SEMAPHORE);
            if (s == nullptr) return ERR_INVALID_ID;
            s->down();
            return 0;
        }
    case 5 : /* up */
        {
            Semaphore* s = (Semaphore*) Process::current->resources->get(a0,
                    ResourceType::SEMAPHORE);
            if (s == nullptr) return ERR_INVALID_ID;
            s->up();
            return 0;
        }
    case 6 : /* join */
        {
            Process *proc = (Process*) Process::current->resources->get(a0,
                 ResourceType::PROCESS);
            if (proc == nullptr) return ERR_INVALID_ID;
            proc->doneEvent.wait();
            long code = proc->exitCode;
            Process::current->resources->close(a0);
            return code;
        }
    case 7 : /* shutdown */
        {
            Debug::shutdown("");
            return 0;
        }
    case 8 : /* open */
        {
            File* f = FileSystem::rootfs->rootdir->lookupFile((char*) a0);
            if (f == nullptr) return ERR_NOT_FOUND;
            else return Process::current->resources->open(f);
        }
    case 9 : /* getlen */
        {
             File* f = (File*) Process::current->resources->get(a0,ResourceType::FILE);
             if (f == nullptr) {
                 return ERR_INVALID_ID;
             }
             return f->getLength();
        }
    case 10: /* read */
        {
             long *args = (long*) a0;
             File* f = (File*) Process::current->resources->get(args[0],ResourceType::FILE);
             if (f == nullptr) {
                 return ERR_INVALID_ID;
             }
             void* buf = (void*) args[1];
             long len = (long) args[2];
             return f->read(buf,len);
        }
    case 11 : /* seek */
        {
             File* f = (File*) Process::current->resources->get(a0,ResourceType::FILE);
             if (f == nullptr) {
                 return ERR_INVALID_ID;
             }
             f->seek(a1);
             return 0;
        }
    case 12 : /* close */
        {
             return Process::current->resources->close(a0);
        }
    case 13: /* execv */
        {
             /* find the security exposures in this code */
             char* name = (char*) a0;
             char** userArgs = (char**) a1;
//
             SimpleQueue<const char*> args;

             int i = 0;

             while(true) {
                 char* s = K::strdup(userArgs[i]);
                 if (s == 0) break;
                 args.addTail(s);
                 i++;
             }

             long rc = Process::current->execv(name,&args,i);

             /* execv failed, cleanup */
             while (!args.isEmpty()) {
                 const char* s = args.removeHead();
                 delete[] s;
             }
             return rc;
        }
    case 14: /* getchar */
        {
              return U8250::it->get();
        }
    case 15: //test draw
	{
		Debug::printf("Writing %c\n", (char)a0);
		const unsigned short vendor = pciCheckVendor(0, 3);
		Debug::printf("The vendor id is %x\n", vendor);
		
		for(int a = 0; a < 128; a+=2)
		{
			const unsigned short BAR0 = pciConfigReadWord(0, 3, 0, a);
		
			 Debug::printf("The thing at %x  is %x\n", a, BAR0);
		}	
		pciConfigWriteWord(0, 3, 0, 0xa, 6);
		const int ioaddr = 0xc000;
        outb( ioaddr + 0x52, 0x0);

        const long mac = inl(ioaddr);
        Debug::printf("Found mac0-5 %x\n", mac);

        outb( ioaddr + 0x37, 0x10);
         while( (inb(ioaddr + 0x37) & 0x10) != 0) { }

        const long receiveBufferAddress = inl(ioaddr + 0x30);
        networkBuffer = new unsigned char[16000]();

        outl(ioaddr + 0x30, (long)networkBuffer);
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
	return 0;
	
	case 16: //get screen buffer
	{
		//we have to know whether there is already a process controlling the global buffer.
		//we need to fool subsequent programs into thinking they have access to a real buffer.



		if(ScreenBuffer::globalBuffer == nullptr)
		{
			ScreenBuffer* screenBuffer = new ScreenBuffer(320, 200, Process::current->getId());
			ScreenBuffer::globalBuffer = screenBuffer;
			const long resourceId = Process::current->resources->open(screenBuffer);

			return resourceId;
		}
		else
		{
			/*BufferRequest* bufferRequest = new BufferRequest(Process::current->getId());
			const long resourceId = Process::current->resources->open(bufferRequest);
			bufferRequest->resourceId = resourceId;*/
			ScreenBuffer* screenBuffer = new ScreenBuffer(80, 60, Process::current->getId());
			const long resourceId = Process::current->resources->open(screenBuffer);
			ScreenBuffer::globalBuffer->AddBufferRequest(screenBuffer);

			return resourceId;
		}


	}
	return 0;	

	case 17: //write screen buffer
	{
		ScreenBuffer* screenBuffer = static_cast<ScreenBuffer*>(Process::current->resources->get(a0, SCREEN_BUFFER));
		if(screenBuffer == nullptr)
		{
			return -1;
		}


		const unsigned char* buf = (const unsigned char*)(a1);
		screenBuffer->WriteBuffer(buf);
	}
	return 0;

	case 18: //GetNewWindowRequests()
	{
		//we would like to return to the requesting process a list of new processes requesting windows so that
		//the requesting process can do things for them.
		//we only want to grant this privilege to the process owning the global screen buffer.

		ScreenBuffer* screenBuffer = ScreenBuffer::globalBuffer;
		if(screenBuffer == nullptr || screenBuffer->GetOwnerProcessId() != Process::current->getId())
		{
			Debug::printf("Fail\n");
			return -1;
		}

		const int requestCount = screenBuffer->GetBufferRequestCount();

		int* processIds = (int*)a0;
		Debug::printf("Sending %d new process requests to window manager for result buffer %x.\n", requestCount, (long)processIds);
		//int* processIds = new int[requestCount];

		for(int a = 0; a < requestCount; ++a)
		{
			processIds[a] = screenBuffer->GetNextChildBuffer();
		}

		Debug::printf("Finished getting new process windows now.\n");
	}

	return 0;

	case 19: //GetBufferRequestCount()
	{
		ScreenBuffer* screenBuffer = ScreenBuffer::globalBuffer;
		if(screenBuffer == nullptr || screenBuffer->GetOwnerProcessId() != Process::current->getId())
		{
			return -1;
		}

		return screenBuffer->GetBufferRequestCount();
	}
    default:
        Process::trace("syscall(%d,%d,%d)",num,a0,a1);
        return -1;
    }
}
