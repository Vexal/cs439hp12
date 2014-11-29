extern "C" {
#include "libc.h"
#include "sys.h"
}

#include "libcc.h"
#include "md5.h"

bool foundUser(const char* buf, const char* username) {
	for (long i = 0; i < 8; i++) {
		char x = buf[i];
        if (x != username[i]) {
        	return false;
        }
        if (x == 0) return true;
	}
	return true;
}

bool passwordMatches(const char* buf, const char* password) {
	for (long i = 8; i < 21; i++) {
		char x = buf[i];
        if (x != password[i - 8]) return false;
    }
	return true;
}

long strlen(const char* str) {
    long n = 0;
    while (*str++ != 0) n++;
    return n;
}

int main() {
	puts("Logging in...\n");
	const long fd = open("passwords");
	puts("Username: ");
	const char* username = gets();
	puts("Password: ");
	const char* password = gets();

   password = "grape";

   long len = strlen(password);
   puts("password: ");
   puts(password);
   puts("\n");
   puts("strlen: ");
   putdec(len);
   puts("\n");

   MD5 md5 = MD5();
   md5.update(password, len);
   md5.finalize();
   char* hashed = md5.hexdigest();

   puts("hashed: ");
   puts(hashed);
   puts("\n");

	char buf[21];
	for (long i = 0; i < getlen(fd); i+=21) {
		read(fd, buf, 21);
		if (foundUser(buf, username)) {
			if (passwordMatches(buf, password)) {
				puts("Logged in as ");
				puts(username);
				puts(".\n");
				return 0;
			} else {
				puts("Authentication failed\n");
				return -1;
			}
			break;
		}
	}
	puts("Username not found\n");

	return -1;
	//const char* hashedPassword = md5(password);
}



// MD5 source: http://www.zedwood.com/article/cpp-md5-function
// memcpy and memset check
// took out overloading shortcut which takes std::string
