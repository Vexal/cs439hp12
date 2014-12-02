extern "C" {
#include "libc.h"
#include "sys.h"
}

#include "libcc.h"
#include "md5.h"

bool foundUser(const char* buf, const char* username) {
	for (long i = 0; i < 10; i++) {
		char x = buf[i];
        if (x != username[i]) {
        	return false;
        }
        if (x == 0) return true;
	}
	return true;
}

bool passwordMatches(const char* buf, const char* password) {
	for (long i = 10; i < 42; i++) {
		char x = buf[i];
		if (x != password[i - 10]) return false;
    }
	return true;
}

int main() {
	while(1) {
		const long fd = open("passwords");
		puts("Username: ");
		const char* username = gets();
		puts("Password: ");
		const char* password = gets();

 	   	long len = strlen(password);

	    // hash the input password
	    MD5 md5 = MD5();
 	    md5.update(password, len);
	    md5.finalize();
	    const char* hashed = md5.hexdigest();

		char buf[42];
		for (long i = 0; i < getlen(fd); i+=42) {
			read(fd, buf, 42);
			if (foundUser(buf, username)) {
				if (passwordMatches(buf, hashed)) {
					puts("Logged in as ");
					puts(username);
					puts(" (uid: ");
					putdec(i / 42);
					puts(")\n");
					SetUserPermissions(i / 42);
					return 0;
				}
			}
		}
		puts("Authentication failed\n");
	}
}

// MD5 source: http://www.zedwood.com/article/cpp-md5-function
// took out overloading shortcut which takes std::string
// exactly 10 characters for each username
// exactly 32 characters for hashed password
