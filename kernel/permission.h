#ifndef _PERMISSION_H_
#define _PERMISSION_H_

extern "C" {
	#include "libc.h"
	#include "sys.h"
}

#include "libcc.h"

struct FilePermission
{
	bool readPermission;
	bool writePermission;
	bool executePermission;
};

class Permission {
	//List<FilePermissions>
	char* buffer;
	unsigned int len;
	// assume permissions file fits in one block for now
	void loadBuffer(long userID);
	void processBuffer();
public:
	Permission(long userID);
	// 0 for denied, 1 for granted
	unsigned int Access(char* fileName, unsigned int mode) const;
};

#endif
