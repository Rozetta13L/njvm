#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "njvm2.h"

int stackPointer, programmCounter = 0;
unsigned int stack[10000];
FILE *binFile;

void binFileOffnen(char *file)
{
    binFile = fopen(file, "r");
}

// Der geoeffnete File schliessen
void binFileSchliessen()
{
    int pruefen;
    pruefen = fclose(binFile);
    if (pruefen == 0) // File-Schliessung ist erfoelgt
    {
        printf("Ninja Virtual Machine stopped\n");
    }
    else
    {
        printf("Problem beim File schliessen\n");
        exit(-1);
    }
}

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
        int abbruch = 1;
        for (int i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "--help") == 0)
            {
                printf("usage: ./njvm [options] <code file>\n");
                printf(" --version        show version and exit\n");
                printf(" --help           show this help and exit\n");
                exit(0);
            }
            else if (strcmp(argv[i], "--version") == 0)
            {
                printf("Ninja Virtual Machine Version %d (compiled Oct 14 2020, 22:54:23)\n", version);
                exit(0);
            }
            else if ((strncmp(argv[i], "--", 2) == 0) || (strncmp(argv[i], "--", 1) == 0))
            {
                printf("unknown command line argument '%s', try './njvm --help'\n", argv[i]);
                exit(-1);
            }

            if (abbruch >= 2)
            {
                break;
            }

            abbruch++;
        }
        if (argc > 2)
        {
            printf("Error: more than one code file specified\n");
        }
        else
        {
            binFileOffnen(argv[1]);
            binFileSchliessen();
        }
    }
    else
    {
        printf("Error: no code file specified\n");
    }

    return 0;
}