#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "njvm1.h"

int stackPointer, programmCounter = 0;
unsigned int stack[10000];

// Wert in dem nachsten freien Platz auf dem Stack speichern
void push(int wert)
{
    stack[stackPointer] = wert;      // Der Wert in dem freien Platz speichern
    stackPointer = stackPointer + 1; // Stackpointer zeigt auf dem naechsten freien Platz
}

// Der Wert aus dem Stack nehemen
int pop()
{
    stackPointer = stackPointer - 1; // Stackpointer zeigt  auf dem letzt gespeicherten Wert
    int wert = stack[stackPointer];  // Der Wert nehmen
    stack[stackPointer] = 0;         // Der Wert zuruecksetzen
    return wert;
}

// Der Program listen und ausgeben
void listen(unsigned int programSpeicher[], int arrayLength)
{
    programmCounter = 0; // Programcounter auf 0 zuruecksetzen
    unsigned char opcode;
    int immediateWert, instruction;
    for (unsigned int i = 0; i < arrayLength; i++) //bricht ab, wenn alle Instruktionen ausgelistet sind
    {
        instruction = programSpeicher[programmCounter];                           // Die naechste Instruktion in dem Program lesen
        opcode = instruction >> 24;                                               // Der Opcode durch rechts schieben Operator kriegen, weil er in dem 8 Oebersten Bits stehet
        immediateWert = SIGN_EXTEND(IMMEDIATE(programSpeicher[programmCounter])); // Der Wert, der gepusht werden soll. sigh_extend ist benutzt im falle von negativen Zahlen
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

// Die Program-Instruktionen ausfuehren und das Ergebnis rechnen
void ausfuehrung(unsigned int programSpeicher[])
{
    programmCounter = 0;
    int wert1, wert2;
    unsigned char opcode;
    int immediateWert, instruction;
    while (1) // bricht ab, wenn halt kommt
    {         // Das Gleiche wie Methode 'listen'
        instruction = programSpeicher[programmCounter];
        opcode = instruction >> 24;
        immediateWert = SIGN_EXTEND(IMMEDIATE(programSpeicher[programmCounter]));
        if (opcode == halt)
        {
            break; // Abbruch-Bedingung
        }
        else if (opcode == pushc)
        {
            push(immediateWert); // Wert auf dem Stack legen
            programmCounter++;   // Programcounter erhoehen
        }
        else if (opcode == add)
        {
            wert2 = pop();       // letzte gepushte Wert nehmen
            wert1 = pop();       // vorletzte gepushte Wert nehmen
            push(wert1 + wert2); // Das Ergebnis pushen
            programmCounter++;   // Programcounter erhoehen
        }
        else if (opcode == sub)
        { // wie add
            wert2 = pop();
            wert1 = pop();
            push(wert1 - wert2);
            programmCounter++;
        }
        else if (opcode == mul)
        { // wie add
            wert2 = pop();
            wert1 = pop();
            push(wert1 * wert2);
            programmCounter++;
        }
        else if (opcode == div)
        { // wie add
            wert2 = pop();
            wert1 = pop();
            if (wert2 == 0) // ueberpruefen ob der Nenner gleich 0, bricht der Program ab
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
        { // wie div
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
            scanf("%d", &input); // Integer von stdin lesen
            push(input);         // Der Wert pushen
            programmCounter++;
        }
        else if (opcode == wrint)
        {
            wert1 = pop();
            printf("%d", wert1); // Wert aus dem Stack nehmen, und auf stdout ausgeben
            programmCounter++;
        }
        else if (opcode == rdchr)
        {
            int input;
            input = getchar(); // nur erste Character vom was der Benutzer schreibt in input speichern
            push(input);
            programmCounter++;
        }
        else if (opcode == wrchr)
        {
            char ausgabe;
            wert1 = pop();
            ausgabe = (char)wert1; // der Wert zur Datentyp Character wandeln
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
            int arrayLength = sizeof(programmSpeicher) / sizeof(programmSpeicher[0]); // Instruktionenzahl rechnen
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
            int arrayLength = sizeof(programmSpeicher) / sizeof(programmSpeicher[0]); // Instruktionenzahl rechnen
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
            int arrayLength = sizeof(programmSpeicher) / sizeof(programmSpeicher[0]); // Instruktionenzahl rechnen
            listen(programmSpeicher, arrayLength);
            ausfuehrung(programmSpeicher);
            printf("Ninja Virtual Machine stopped\n");
        }
    }

    return 0;
}