#ifndef _PERMISSION_H_
#define _PERMISSION_H_


class Permission {
	char** filePermissions;
	Permission(long userID);
	// assume permissions file fits in one block for now
	char* loadBuffer(long userID);
	void processBuffer(char* buffer);

	// 0 for denied, 1 for granted
	unsigned int Access(char* fileName, unsigned int mode) const;
};



#endif
