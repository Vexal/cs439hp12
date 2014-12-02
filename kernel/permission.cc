extern "C" {
	#include "libc.h"
	#include "sys.h"
}

#include "libcc.h"

/*
 * Mode numbers: 0 read; 1 write; 2 execute.
 * UserID: order of appearance in both passwords and permissions files
 * No space or ";" allowed in file names
 * Each filename and permission entry is separated by a space
 * Each user capability list is separated by a newline character
 */

class Permission {
	char* filePermissions;
	unsigned int len;
	Permission(long userID) {
		LoadBuffer(userID);
	}
	// assume permissions file fits in one block for now
	void LoadBuffer(long userID) {
		// put the loaded buffer on heap and return the address
		int userNum = 0;
		char buf[512];
		int start;
		int end;
		int seeking = 0;
		long fd = open("permissions");
		long length = read(fd, buf, 512);
		while (userNum != userID + 1) {
			for (int i = seeking; seeking < length; seeking++) {
				if (buf[i] == '\n') {
					userNum++;
					if (userNum == userID) start = i;
					if (userNum == userID + 1) end = i;
					seeking = i;
					break;
				}
			}
		}
		int len = end - start;
		this->len = len;
		filePermissions = new char[len];
		for (int i = 0; i < len; i++) {
			filePermissions[i] = buf[i + start];
		}
	}

	// 0 for denied, 1 for granted
	unsigned int Access(char* fileName, unsigned int mode) const {
		for(unsigned int i = 0; i < len; i++) {

		}
		return 0;
	}
};

// Need to implement check for spaces in filename when implementing adding/writing to files
