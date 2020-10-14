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

void listen(unsigned int programSpeicher[], int arrayLength)
{
    programmCounter = 0;
    unsigned char opcode;
    int immediateWert, instruction;
    for (unsigned int i = 0; i < arrayLength; i++)
    {
        instruction = programSpeicher[programmCounter];
        opcode = instruction >> 24;
        immediateWert = SIGN_EXTEND(IMMEDIATE(programSpeicher[programmCounter]));
        if (opcode == halt)
        {
            printf("%03d\t halt\n", programmCounter);
        }
        else if (opcode == pushc)
        {
            printf("%03d\t pushc\t %d\n", programmCounter, immediateWert);
            programmCounter++;
        }
        else if (opcode == add)
        {
            printf("%03d\t add\n", programmCounter);
            programmCounter++;
        }
        else if (opcode == sub)
        {
            printf("%03d\t sub\n", programmCounter);
            programmCounter++;
        }
        else if (opcode == mul)
        {
            printf("%03d\t mul\n", programmCounter);
            programmCounter++;
        }
        else if (opcode == div)
        {
            printf("%03d\t div\n", programmCounter);
            programmCounter++;
        }
        else if (opcode == mod)
        {
            printf("%03d\t mod\n", programmCounter);
            programmCounter++;
        }
        else if (opcode == rdint)
        {
            printf("%03d\t rdint\n", programmCounter);
            programmCounter++;
        }
        else if (opcode == wrint)
        {
            printf("%03d\t wrint\n", programmCounter);
            programmCounter++;
        }
        else if (opcode == rdchr)
        {
            printf("%03d\t rdchr\n", programmCounter);
            programmCounter++;
        }
        else if (opcode == wrchr)
        {
            printf("%03d\t wrchr\n", programmCounter);
            programmCounter++;
        }
    }
}

void ausfuehrung(unsigned int programSpeicher[])
{
    programmCounter = 0;
    int wert1, wert2;
    unsigned char opcode;
    int immediateWert, instruction;
    while (1)
    {
        instruction = programSpeicher[programmCounter];
        opcode = instruction >> 24;
        immediateWert = SIGN_EXTEND(IMMEDIATE(programSpeicher[programmCounter]));
        if (opcode == halt)
        {
            break;
        }
        else if (opcode == pushc)
        {
            push(immediateWert);
            programmCounter++;
        }
        else if (opcode == add)
        {
            wert2 = pop();
            wert1 = pop();
            push(wert1 + wert2);
            programmCounter++;
        }
        else if (opcode == sub)
        {
            wert2 = pop();
            wert1 = pop();
            push(wert1 - wert2);
            programmCounter++;
        }
        else if (opcode == mul)
        {
            wert2 = pop();
            wert1 = pop();
            push(wert1 * wert2);
            programmCounter++;
        }
        else if (opcode == div)
        {
            wert2 = pop();
            wert1 = pop();
            if (wert2 == 0)
            {
                printf("können durch 0 nicht teilen !!");
                exit(-1);
            }
            else
            {
                push(wert1 / wert2);
                programmCounter++;
            }
        }
        else if (opcode == mod)
        {
            wert2 = pop();
            wert1 = pop();
            if (wert2 == 0)
            {
                printf("können durch 0 nicht teilen !!");
                exit(-1);
            }
            else
            {
                push(wert1 % wert2);
                programmCounter++;
            }
        }
        else if (opcode == rdint)
        {
            int input;
            scanf("%d", &input);
            push(input);
            programmCounter++;
        }
        else if (opcode == wrint)
        {
            wert1 = pop();
            printf("%d", wert1);
            programmCounter++;
        }
        else if (opcode == rdchr)
        {
            int input;
            input = getchar();
            push(input);
            programmCounter++;
        }
        else if (opcode == wrchr)
        {
            char ausgabe;
            wert1 = pop();
            ausgabe = (char)wert1;
            printf("%c", ausgabe);
            programmCounter++;
        }
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
            printf("Ninja Virtual Machine started\n");
            unsigned int programmSpeicher[] = {
                (pushc << 24) | IMMEDIATE(3),
                (pushc << 24) | IMMEDIATE(4),
                (add << 24),
                (pushc << 24) | IMMEDIATE(10),
                (pushc << 24) | IMMEDIATE(6),
                (sub << 24),
                (mul << 24),
                (wrint << 24),
                (pushc << 24) | IMMEDIATE(10),
                (wrchr << 24),
                (halt << 24)};
            int arrayLength = sizeof(programmSpeicher) / sizeof(programmSpeicher[0]);
            listen(programmSpeicher, arrayLength);
            ausfuehrung(programmSpeicher);
            printf("Ninja Virtual Machine stopped\n");
        }
        else if (strcmp(argv[1], "--prog2") == 0)
        {
            printf("Ninja Virtual Machine started\n");
            unsigned int programmSpeicher[] = {
                (pushc << 24) | IMMEDIATE((-2)),
                (rdint << 24),
                (mul << 24),
                (pushc << 24) | IMMEDIATE(3),
                (add << 24),
                (wrint << 24),
                (pushc << 24) | IMMEDIATE('\n'),
                (wrchr << 24),
                (halt << 24)};
            int arrayLength = sizeof(programmSpeicher) / sizeof(programmSpeicher[0]);
            listen(programmSpeicher, arrayLength);
            ausfuehrung(programmSpeicher);
            printf("Ninja Virtual Machine stopped\n");
        }
        else if (strcmp(argv[1], "--prog3") == 0)
        {
            printf("Ninja Virtual Machine started\n");
            unsigned int programmSpeicher[] = {
                (rdchr << 24),
                (wrint << 24),
                (pushc << 24) | IMMEDIATE('\n'),
                (wrchr << 24),
                (halt << 24)};
            int arrayLength = sizeof(programmSpeicher) / sizeof(programmSpeicher[0]);
            listen(programmSpeicher, arrayLength);
            ausfuehrung(programmSpeicher);
            printf("Ninja Virtual Machine stopped\n");
        }
    }

    return 0;
}