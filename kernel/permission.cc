#include "permission.h"
#include "machine.h"
#include "fs.h"
/*
 * Mode numbers: 0 read; 1 write; 2 execute.
 * UserID: order of appearance in both passwords and permissions files
 * No space or ";" allowed in file names
 * Each filename and permission entry is separated by a space
 * Each user capability list is separated by a newline character
 */

	Permission::Permission(long userId) :
		userId(userId)
	{
		loadBuffer();
		processBuffer();
	}
	// assume permissions file fits in one block for now
	void Permission::loadBuffer() {
		// put the loaded buffer on heap and return the address
		int userNum = 0;
		char buf[512];
		int start;
		int end;
		int seeking = 0;
		File* f = FileSystem::rootfs->rootdir->lookupFile("permissions");
		long length = f->readFully(buf, 512);
		while (userNum != userId + 1) {
			for (int i = seeking; seeking < length; seeking++) {
				if (buf[i] == '\n') {
					userNum++;
					if (userNum == userId) start = i;
					if (userNum == userId + 1) end = i;
					seeking = i;
					break;
				}
			}
		}
		int len = end - start;
		this->len = len;
		this->buffer = new char[len];
		for (int i = 0; i < len; i++) {
			this->buffer[i] = buf[i + start];
		}
	}

	void Permission::processBuffer() {
		while(this->buffer[0] != '\n')
		{
			unsigned int index = 0;
			FilePermission perm;
			while(this->buffer[index] != ' ') {
				++index;
			}

			char* fileName = new char[index + 1];
			fileName[index] = 0;

			memcpy(fileName, this->buffer, index);
			this->filePermissions[fileName] = perm;

			buffer+= index + 1;

			perm.readPermission = buffer[0] == '1';
			perm.writePermission = buffer[1] == '1';
			perm.executePermission = buffer[2] == '1';

			buffer+= 4;
		}

	}

	// 0 for denied, 1 for granted
	unsigned int Permission::Access(char* fileName, unsigned int mode) const {
		return 0;
	}

// Need to implement check for spaces in filename when implementing adding/writing to files
