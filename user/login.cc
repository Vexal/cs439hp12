extern "C" {
#include "libc.h"
#include "sys.h"
}

#include "libcc.h"

int main() {
	puts("Username: ");
	const char* username = gets();
	puts("Password: ");
	const char* password = gets();
	puts("Thankyou.\n");
	//bool authenticated = login();
}
