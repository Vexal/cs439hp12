#ifndef _PERMISSION_H_
#define _PERMISSION_H_
#include "libcc.h"

struct FilePermission
{
	bool readPermission;
	bool writePermission;
	bool executePermission;
};

class Permission {
	//List<FilePermissions>
	char** filePermissions;
	Permission(long userID);
	// assume permissions file fits in one block for now
	char* loadBuffer(long userID);
	void processBuffer(char* buffer);

	// 0 for denied, 1 for granted
	unsigned int Access(char* fileName, unsigned int mode) const;
};



#endif
