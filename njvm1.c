#include <stdio.h>
#include <time.h>
#include <string.h>
#include "njvm1.h"
int main(int argc, char *argv[])
{
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
            printf("Ninja Virtual Machine Version %d, Compiled %s\n", version, ctime(&t));
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