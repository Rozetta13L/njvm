#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "njvm3.h"

unsigned int globalVarZahl, instrZahl;
int framePointer, stackPointer, programmCounter = 0;
unsigned int stack[STACKSIZE];
FILE *binFile;
unsigned int *staticDataArea;
unsigned int *programmSpeicher;
unsigned char opcode;
int immediateWert, instruction;

// Versuchen zu offnen der File der im Kommandozeile gegeben wird
void binFileOffnen(char *file)
{
    char formatIdentifier[4];
    unsigned int versionCheck;
    binFile = fopen(file, "r");
    if (binFile == NULL) //ueberpruefen ob der file erfolgreich geoefnnet wird
    {
        printf("Error: cannot open code file '%s'\n", file);
        exit(-1);
    }
    fread(formatIdentifier, sizeof(char), 4, binFile); // die erste 4 bits lesen (Program-Format)
    if (strncmp(formatIdentifier, "NJBF", 4) != 0)     // ueberpruefen ob die Format richtig ist
    {
        printf("Die Format-Identifier ist nicht 'NJBF' !!\n");
        exit(-1);
    }
    fread(&versionCheck, 1, sizeof(int), binFile); // dei naechste 4 bits lesen (version)
    if (versionCheck != version)                   // ueberpruefen ob die Version richtig ist
    {
        printf("Program-Version ist nicht das gleiche Version wie die VM !!\n");
        exit(-1);
    }
    fread(&instrZahl, 1, sizeof(int), binFile); // die naechste 4 bits lesen (intsruktions-zahl bestimmen)
    programmSpeicher = malloc(instrZahl * 4);   // programspeicher allocieren
    if (programmSpeicher == NULL)
    {
        printf("Problem beim Speicher allocating !!!\n");
        exit(-1);
    }
    fread(&globalVarZahl, 1, sizeof(int), binFile); // die naechste 4 bits lesen (globalvariablen-zahl bestimmen)
    staticDataArea = malloc(globalVarZahl * 4);     // SDA allocieren
    if (staticDataArea == NULL)
    {
        printf("Problem beim SDA-Speicher allocating !!!\n");
        exit(-1);
    }
    fread(programmSpeicher, instrZahl, sizeof(int), binFile); // die restliche bits in programspeicher lesen
    framePointer = 0;
    stackPointer = 0;
    programmCounter = 0;
}

// Der geoeffnete File schliessen
void binFileSchliessen()
{
    int pruefen;
    pruefen = fclose(binFile);
    if (pruefen == 0) // File-Schliessung ist erfoelgt
    {
        printf("Ninja Virtual Machine stopped\n");
        exit(0);
    }
    else
    {
        printf("Problem beim File schliessen\n");
        exit(-1);
    }
}

// Debugger
void debugger()
{
    stackPointer = 0;    // SP auf 0 setzen
    framePointer = 0;    // FP auf 0 setzen
    programmCounter = 0; // PC auf 0 setzen
    int breakPoint = -1;
    printf("DEBUG: file 'prog1_2.bin' loaded (code size = %d, data size = %d)\n", instrZahl, globalVarZahl);
    printf("Ninja Virtual Machine started\n");
    listen(programmSpeicher);
    char debugInput[10];
    int prevPC;
    while (1)
    {
        printf("DEBUG: inspect, list, breakpoint, step, run, quit?\n");
        scanf("%s", debugInput);
        if (strncmp(debugInput, "i", 1) == 0) // ispect
        {
            printf("inspect: stack, data?\n");
            scanf("%s", debugInput);
            if (strncmp(debugInput, "s", 1) == 0) // stack
            {
                int durchStack;
                // Schleife, um durch den Stack zu laufen, aber von obe nach unten(ruckwaerts)
                for (durchStack = stackPointer; durchStack >= 0; durchStack--)
                {
                    if (durchStack == stackPointer && durchStack == framePointer)
                    {
                        printf("sp,fp\t-->\t\t %04d \t\t xxxx\n", durchStack);
                    }
                    else if (durchStack == stackPointer)
                    {
                        printf("sp   \t-->\t\t %04d \t\t xxxx\n", durchStack);
                    }
                    else if (durchStack == framePointer)
                    {
                        printf("fp   \t-->\t\t %04d \t\t %d  \n", durchStack, stack[durchStack]);
                    }
                    else
                    {
                        printf("               \t\t %04d \t\t %d  \n", durchStack, stack[durchStack]);
                    }
                }
                printf("\t\t\t   ---Bottom of Stack---   \t\t\t\n");
            }
            else if (strncmp(debugInput, "d", 1) == 0) // data
            {
                int count;
                for (count = 0; count < globalVarZahl; count++)
                {
                    printf("data[%04d]:\t\t %d\n", count, staticDataArea[count]);
                }
                printf("\t\t\t   ---End of Data---   \t\t\t\n");
            }
            listen(programmSpeicher);
        }
        else if (strncmp(debugInput, "l", 1) == 0) // list
        {
            prevPC = programmCounter;
            programmCounter = 0;
            for (int i = 0; i < instrZahl; i++)
            {
                listen(programmSpeicher);
                programmCounter++;
            }
            printf("\t\t\t   ---End of Code---   \t\t\t\n");
            programmCounter = prevPC;
            listen(programmSpeicher);
        }
        else if (strncmp(debugInput, "b", 1) == 0) // breakpoint
        {
            int breakWert;
            if (breakPoint == -1)
            {
                printf("DEBUG [breakpoint}: Cleared\n");
            }
            else if (breakPoint > 0)
            {
                printf("DEBUG [breakpoint}: set at %d\n", breakPoint);
            }
            printf("DEBUG [breakpoint}: address to set. -1 to clear, 0 for no change\n");
            scanf("%d", &breakWert);
            if (breakWert == -1)
            {
                breakPoint = breakWert;
                printf("DEBUG [breakpoint}: now cleared\n");
            }
            else if (breakWert > 0)
            {
                breakPoint = breakWert;
                printf("DEBUG [breakpoint}: now set at %d\n", breakPoint);
            }
            listen(programmSpeicher);
        }
        else if (strncmp(debugInput, "s", 1) == 0) // step
        {
            ausfuehrung(programmSpeicher);
            programmCounter++;
            listen(programmSpeicher);
        }
        else if (strncmp(debugInput, "r", 1) == 0) // run
        {
            while (1)
            {
                if (breakPoint > 0)
                {
                    ausfuehrung(programmSpeicher);
                    programmCounter++;
                    if (breakPoint - 1 == programmCounter)
                    {
                        ausfuehrung(programmSpeicher);
                        programmCounter++;
                        listen(programmSpeicher);
                        break;
                    }
                }
                else
                {
                    ausfuehrung(programmSpeicher);
                    programmCounter++;
                }
            }
        }
        else if (strncmp(debugInput, "q", 1) == 0) //quit
        {
            binFileSchliessen();
        }
    }
}

// Wert in dem nachsten freien Platz auf dem Stack speichern
void push(int wert)
{
    if (stackPointer > STACKSIZE)
    {
        printf("STACKUEBERLAUF !!!\n");
        exit(-1);
    }
    else
    {
        stack[stackPointer] = wert;      // Der Wert in dem freien Platz speichern
        stackPointer = stackPointer + 1; // Stackpointer zeigt auf dem naechsten freien Platz
    }
}

// Der Wert aus dem Stack nehemen
int pop()
{
    if (stackPointer < 0)
    {
        printf("STACKUNTERLAUF !!!\n");
        exit(-1);
    }
    else
    {
        stackPointer = stackPointer - 1; // Stackpointer zeigt  auf dem letzt gespeicherten Wert
        int wert = stack[stackPointer];  // Der Wert nehmen
        stack[stackPointer] = 0;         // Der Wert zuruecksetzen
        return wert;
    }
}

// Der Program listen und ausgeben
void listen(unsigned int programSpeicher[])
{
    instruction = programSpeicher[programmCounter];                           // Die naechste Instruktion in dem Program lesen
    opcode = instruction >> 24;                                               // Der Opcode durch rechts schieben Operator kriegen, weil er in dem 8 Oebersten Bits stehet
    immediateWert = SIGN_EXTEND(IMMEDIATE(programSpeicher[programmCounter])); // Der Wert, der gepusht werden soll. sigh_extend ist benutzt im falle von negativen Zahlen
    if (opcode == halt)
    {
        printf("%04d\t halt\n", programmCounter);
    }
    else if (opcode == pushc)
    {
        printf("%04d\t pushc\t %d\n", programmCounter, immediateWert);
    }
    else if (opcode == add)
    {
        printf("%04d\t add\n", programmCounter);
    }
    else if (opcode == sub)
    {
        printf("%04d\t sub\n", programmCounter);
    }
    else if (opcode == mul)
    {
        printf("%04d\t mul\n", programmCounter);
    }
    else if (opcode == div)
    {
        printf("%04d\t div\n", programmCounter);
    }
    else if (opcode == mod)
    {
        printf("%04d\t mod\n", programmCounter);
    }
    else if (opcode == rdint)
    {
        printf("%04d\t rdint\n", programmCounter);
    }
    else if (opcode == wrint)
    {
        printf("%04d\t wrint\n", programmCounter);
    }
    else if (opcode == rdchr)
    {
        printf("%04d\t rdchr\n", programmCounter);
    }
    else if (opcode == wrchr)
    {
        printf("%04d\t wrchr\n", programmCounter);
    }
    else if (opcode == pushg)
    {
        printf("%04d\t pushg\t %d\n", programmCounter, immediateWert);
    }
    else if (opcode == popg)
    {
        printf("%04d\t popg\t %d\n", programmCounter, immediateWert);
    }
    else if (opcode == asf)
    {
        printf("%04d\t asf\t %d\n", programmCounter, immediateWert);
    }
    else if (opcode == rsf)
    {
        printf("%04d\t rsf\n", programmCounter);
    }
    else if (opcode == pushl)
    {
        printf("%04d\t pushl\t %d\n", programmCounter, immediateWert);
    }
    else if (opcode == popl)
    {
        printf("%04d\t popl\t %d\n", programmCounter, immediateWert);
    }
    else if (opcode == eq)
    {
        printf("%04d\t eq\n", programmCounter);
    }
    else if (opcode == ne)
    {
        printf("%04d\t ne\n", programmCounter);
    }
    else if (opcode == lt)
    {
        printf("%04d\t lt\n", programmCounter);
    }
    else if (opcode == le)
    {
        printf("%04d\t le\n", programmCounter);
    }
    else if (opcode == gt)
    {
        printf("%04d\t gt\n", programmCounter);
    }
    else if (opcode == ge)
    {
        printf("%04d\t ge\n", programmCounter);
    }
    else if (opcode == jmp)
    {
        printf("%04d\t jmp\t %d\n", programmCounter, immediateWert);
    }
    else if (opcode == brf)
    {
        printf("%04d\t brf\t %d\n", programmCounter, immediateWert);
    }
    else if (opcode == brt)
    {
        printf("%04d\t brt\t %d\n", programmCounter, immediateWert);
    }
}

// Die Program-Instruktionen ausfuehren und das Ergebnis rechnen
void ausfuehrung(unsigned int programSpeicher[])
{
    int wert1, wert2, ergebnis;

    // Das Gleiche wie Methode 'listen'
    instruction = programSpeicher[programmCounter];
    opcode = instruction >> 24;
    immediateWert = SIGN_EXTEND(IMMEDIATE(programSpeicher[programmCounter]));
    if (opcode == halt)
    {
        binFileSchliessen();
    }
    else if (opcode == pushc)
    {
        push(immediateWert); // Wert auf dem Stack legen
    }
    else if (opcode == add)
    {
        wert2 = pop();       // letzte gepushte Wert nehmen
        wert1 = pop();       // vorletzte gepushte Wert nehmen
        push(wert1 + wert2); // Das Ergebnis pushen
    }
    else if (opcode == sub)
    { // wie add
        wert2 = pop();
        wert1 = pop();
        push(wert1 - wert2);
    }
    else if (opcode == mul)
    { // wie add
        wert2 = pop();
        wert1 = pop();
        push(wert1 * wert2);
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
        }
    }
    else if (opcode == rdint)
    {
        int input;
        scanf("%d", &input); // Integer von stdin lesen
        push(input);         // Der Wert pushen
    }
    else if (opcode == wrint)
    {
        wert1 = pop();
        printf("%d\n", wert1); // Wert aus dem Stack nehmen, und auf stdout ausgeben
    }
    else if (opcode == rdchr)
    {
        int input;
        input = getchar(); // nur erste Character vom was der Benutzer schreibt in input speichern
        push(input);
    }
    else if (opcode == wrchr)
    {
        char ausgabe;
        wert1 = pop();
        ausgabe = (char)wert1; // der Wert zur Datentyp Character wandeln
        printf("%c", ausgabe);
    }
    else if (opcode == pushg) // Mit hilfe von Global-VAriablen in SDA arbeiten:
    {
        wert1 = staticDataArea[immediateWert]; // wer aus dem SDA holen
        push(wert1);                           // der Wert auf dem Stack pushen
    }
    else if (opcode == popg) // Mit hilfe von Global-VAriablen in SDA arbeiten:
    {
        wert1 = pop();                         // Wert von Stack holen
        staticDataArea[immediateWert] = wert1; // der Wert in dem gewunschten platz im SDA speichern
    }
    else if (opcode == asf) // allocating Stack-Frame:
    {
        push(framePointer);
        framePointer = stackPointer;
        stackPointer = stackPointer + immediateWert;
    }
    else if (opcode == rsf) // reset Stack-Frame
    {
        stackPointer = framePointer;
        framePointer = pop();
    }
    else if (opcode == pushl) // Mit hilfe von aloocated Stack-Frame arbeiten:
    {
        int gewunPos;
        gewunPos = framePointer + immediateWert; // gewunschte Position ereichen
        wert1 = stack[gewunPos];                 // der Wert aus dem Stack-Fframe nehmen
        push(wert1);                             // auf dem Stack pushen
    }
    else if (opcode == popl) // Mit hilfe von aloocated Stack-Frame arbeiten:
    {
        int gewunPos;
        gewunPos = framePointer + immediateWert; // gewunschte Position ereichen
        wert1 = pop();                           // der Wert von dem Stack nehmen
        stack[gewunPos] = wert1;                 // der Wert in dem gewunschten Platz im Stack-Frame
    }
    else if (opcode == eq)
    {
        wert2 = pop();
        wert1 = pop();

        ergebnis = wert1 == wert2;
        push(ergebnis);
    }
    else if (opcode == ne)
    {
        wert2 = pop();
        wert1 = pop();

        ergebnis = wert1 != wert2;
        push(ergebnis);
    }
    else if (opcode == lt)
    {
        wert2 = pop();
        wert1 = pop();

        ergebnis = wert1 < wert2;
        push(ergebnis);
    }
    else if (opcode == le)
    {
        wert2 = pop();
        wert1 = pop();

        ergebnis = wert1 <= wert2;
        push(ergebnis);
    }
    else if (opcode == gt)
    {
        wert2 = pop();
        wert1 = pop();

        ergebnis = wert1 > wert2;
        push(ergebnis);
    }
    else if (opcode == ge)
    {
        wert2 = pop();
        wert1 = pop();

        ergebnis = wert1 >= wert2;
        push(ergebnis);
    }
    else if (opcode == jmp)
    {
        programmCounter = immediateWert - 1;
    }
    else if (opcode == brf)
    {
        wert1 = pop();
        if (wert1 == 0)
        {
            programmCounter = immediateWert - 1;
        }
        else
        {
        }
    }
    else if (opcode == brt)
    {
        wert1 = pop();
        if (wert1 == 0)
        {
            programmCounter = immediateWert - 1;
        }
        else
        {
        }
    }
}

// Main-Methode
int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        int i, j;
        for (i = 1; i < argc; i++)
        {
            if (strcmp(argv[i], "--help") == 0) // Hilfe ausdrucken (wie die VM funktioniert bzw- benutzt werden soll)
            {
                printf("usage: ./njvm [options] <code file>\n");
                printf(" --debug          start virtual machine in debug mode\n");
                printf(" --version        show version and exit\n");
                printf(" --help           show this help and exit\n");
                exit(0);
            }
            else if (strcmp(argv[i], "--version") == 0) // VM-Version ausdrucken und wann ist die compiliert war
            {
                printf("Ninja Virtual Machine Version %d (compiled Oct 14 2020, 22:54:23)\n", version);
                exit(0);
            }
        }
        for (j = 1; j < argc; j++)
        {
            if (strcmp(argv[j], "--debug") == 0)
            {
                if (argc > 3)
                {
                    printf("Error: more than one code file specified\n");
                    exit(0);
                }
                if (j == 2)
                {
                    binFileOffnen(argv[1]);
                }
                else if (j == 1)
                {
                    binFileOffnen(argv[2]);
                }
                debugger();
            }
            else if ((strncmp(argv[j], "--", 2) == 0) || (strncmp(argv[j], "--", 1) == 0)) // ueberpruefen ob ein unbekanntes Befehl gegeben wird
            {
                printf("unknown command line argument '%s', try './njvm --help'\n", argv[i]);
                exit(-1);
            }
        }
        if (argc > 2)
        {
            printf("Error: more than one code file specified\n");
        }
        else
        {

            stackPointer = 0;    // SP auf 0 setzen
            framePointer = 0;    // FP auf 0 setzen
            programmCounter = 0; // PC auf 0 setzen
            binFileOffnen(argv[1]);
            printf("Ninja Virtual Machine started\n");
            while (1)
            {
                ausfuehrung(programmSpeicher);
                programmCounter++;
            }
        }
    }
    else
    {
        printf("Error: no code file specified\n");
    }

    return 0;
}