#include <stdio.h>
#include <time.h>
#include <string.h>
int main(int argc, char *argv[])
{
    // Auf 0.2
    // for (int i = 1; i < argc; i++)
    // {
    //     printf("%s\n", argv[i]);
    // }
    // printf("Ninja Virtual Machine started\n");
    // printf("Ninja Virtual Machine stopped\n");

    // Auf 0.3
    time_t t;
    time(&t);
    if (argc > 1)
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            printf("usage: ./njvm [option] [option] ...\n  --version        show version and exit\n  --help           show this help and exit\n");
        }
        else if (strcmp(argv[1], "--version") == 0)
        {
            printf("Ninja Virtual Machine Version 0, Compiled %s\n", ctime(&t));
        }
        else
        {
            printf("unknown command line argument '%s', try './njvm --help'\n", argv[1]);
        }
    }
    else
    {
        printf("Ninja Virtual Machine started\n");
        printf("Ninja Virtual Machine stopped\n");
    }

    return 0;
}