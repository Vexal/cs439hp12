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
		//enableGraphics();
		
		for(int a = 0; a < 128; a+=2)
		{
			const unsigned short BAR0 = pciConfigReadWord(0, 3, 0, a);
		
			Debug::printf("The thing at %x  is %x\n", a, BAR0);
		}	
		Debug::printf("Finished enabling graphics\n");
		//short* gr = (short*)0xb8000;
		//gr[0] = 0xFFFF;
		/*unsigned char* pixels = (unsigned char*)0xA0000;
		for(int a = 0; a < 10000; ++a)
		{
			pixels[a] = a%256;
		}*/

		const char* buf = (const char*)a0;
		const unsigned int len = a1;

	}
	return 0;
	
	case 16: //get screen buffer
	{
		//we have to know whether there is already a process controlling the global buffer.
		//we need to fool subsequent programs into thinking they have access to a real buffer.

		ScreenBuffer* screenBuffer = new ScreenBuffer(320, 200, Process::current->getId());

		if(ScreenBuffer::globalBuffer == nullptr)
		{
			ScreenBuffer::globalBuffer = screenBuffer;
		}

		const long resourceId = Process::current->resources->open(screenBuffer);

		return resourceId;
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
    default:
        Process::trace("syscall(%d,%d,%d)",num,a0,a1);
        return -1;
    }
}
