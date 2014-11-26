extern "C" {
#include "libc.h"
#include "sys.h"
}

#include "libcc.h"
#include "DesktopWindowManager.h"

int main()
{
	puts((const char*)"this works\n");
	puts("this still works\n");
	char* testNew = new char[25];
	testNew[0] = 'a';
	testNew[1] = 'c';
	testNew[2] = '\n';
	testNew[3] = 0;
	
	puts(testNew);
	testdraw('a');
	puts("Finished calling testdraw\n");
	delete[] testNew;

	DesktopWindowManager windowManager(320, 200);
	windowManager.Initialize();
	windowManager.Run();
	return 0;
}
