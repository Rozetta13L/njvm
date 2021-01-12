#include "njvm.h"

extern int programmCounter;
extern unsigned int *programmSpeicher;
char *fileName = NULL;
extern int instruction;
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
            if (strcmp(argv[j], "--debug") == 0) // debugger starten
            {
                if (argc > 3) // wenn mehr als ein file als parameter gegeben ist
                {
                    printf("Error: more than one code file specified\n");
                    exit(0);
                }
                if (j == 2) // wenn --debug als zweite parameter ist
                {
                    fileName = argv[1];
                    binFileOffnen(fileName);
                }
                else if (j == 1) // wenn --debug als erste parameter ist
                {
                    fileName = argv[2];
                    binFileOffnen(fileName);
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
            binFileOffnen(argv[1]); // der als parameter gegebene Programm öffnen
            printf("Ninja Virtual Machine started\n");
            while (1)
            {
                instruction = programmSpeicher[programmCounter];
                programmCounter++;
                ausfuehrung(instruction);
            }
        }
    }
    else
    {
        printf("Error: no code file specified\n");
    }

    return 0;
}
