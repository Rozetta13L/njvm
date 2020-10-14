#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "njvm1.h"

int stackPointer, programmCounter = 0;
unsigned int stack[10000];
//unsigned int programmSpeicher[20];

void push(int wert)
{
    stack[stackPointer] = wert;
    stackPointer = stackPointer + 1;
}

int pop()
{
    stackPointer = stackPointer - 1;
    int wert = stack[stackPointer];
    stack[stackPointer] = 0;
    return wert;
}

void listen(unsigned int *programSpeicher)
{
    int opcode, immediateWert, instruction;
    for (int i = 0; i <= ARRAYLENGTH(programSpeicher); i++)
    {
        instruction = programSpeicher[programmCounter];
        opcode = instruction >> 24;
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        for (int i = 2; i < argc; i++)
        {
            if (strcmp(argv[i], "--help") == 0)
            {
                printf("usage: ./njvm [option] [option] ...\n");
                printf("--prog1          select program 1 to execute\n");
                printf("--prog2          select program 2 to execute\n");
                printf("--prog3          select program 3 to execute\n");
                printf("--version        show version and exit\n");
                printf("--help           show this help and exit\n");
            }
            else if (strcmp(argv[i], "--version") == 0)
            {
                printf("Ninja Virtual Machine Version %d (compiled Oct 13 2020, 01:30:23)\n", version);
            }
            else
            {
                printf("unknown command line argument '%s', try './njvm --help'\n", argv[i]);
                exit(-1);
            }
        }
        if (strcmp(argv[1], "--prog1") == 0)
        {
            /* code */
        }
        else if (strcmp(argv[1], "--prog2") == 0)
        {
            /* code */
        }
        else if (strcmp(argv[1], "--prog3") == 0)
        {
            /* code */
        }
    }

    return 0;
}