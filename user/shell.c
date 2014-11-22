#include "libc.h"

void notFound(char* cmd) {
    puts(cmd);
    puts(": command not found\n");
}

int main() {
    while (1) {
        puts("shell> ");
        char* in = gets();
        int ind = 0;
        int argc = 0;
        char** args;

        while(in[ind] != 0 && (in[ind] == ' ' || in[ind] == '\r'))
        {
        	++in;
        }

        while(in[ind] != 0)
        {
        	while(in[ind] != 0 && in[ind] != ' ')
			{
				++ind;
			}

        	while(in[ind] == ' ')
        	{
            	++ind;
        	}

        	++argc;
        }


        if(argc == 0)
        {
        	continue;
        }

        args = malloc(4 * (argc + 1));

        int ind2 = 0;
        for(int a = 0; a < argc && in[ind2] != 0; ++a)
        {
        	int argumentLength = 0;
        	while(in[ind2] != ' ' && in[ind2] != 0)
        	{
        		++argumentLength;
        		++ind2;
        	}

        	args[a] = malloc(argumentLength + 1);
        	memcpy(args[a], in + ind2 - argumentLength, argumentLength);
        	args[a][argumentLength] = 0;

        	while(in[ind2] == ' ' && in[ind2] != 0)
        	{
        		++ind2;
        	}
        }

        args[argc] = 0;

        const long id = fork();

        if(id != 0)
        {
        	join(id);

        }
        else
        {
        	const long err = execv(args[0], args);
        	if(err < 0)
        	{
                puts(args[0]);
                puts(": command not found\n");
        	}

            if (in) free(in);
        	exit(5);
        }

        if (in) free(in);
    }
    return 0;
}
