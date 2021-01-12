#include "Filehandlung.h"

unsigned int globalVarZahl, instrZahl;
int framePointer, stackPointer, programmCounter = 0;
FILE *binFile;
unsigned int *staticDataArea;
unsigned int *programmSpeicher;

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
    fread(&versionCheck, 1, sizeof(int), binFile); // die naechste 4 bits lesen (version)
    if (versionCheck != version)                   // ueberpruefen ob die Version richtig ist
    {
        printf("Program-Version ist nicht das gleiche Version wie die VM !!\n");
        exit(-1);
    }
    fread(&instrZahl, 1, sizeof(int), binFile); // die naechste 4 bits lesen (intsruktions-zahl bestimmen)
    programmSpeicher = malloc(instrZahl * 4);   // programspeicher allocieren
    if (programmSpeicher == NULL)               // ueberpruefen ob der programspeicher erfolgreich Memory bekommen hat
    {
        printf("Problem beim Speicher allocating !!!\n");
        exit(-1);
    }
    fread(&globalVarZahl, 1, sizeof(int), binFile); // die naechste 4 bits lesen (globalvariablen-zahl bestimmen)
    staticDataArea = malloc(globalVarZahl * 4);     // SDA allocieren
    if (staticDataArea == NULL)                     // ueberpruefen ob die SDA erfolgreich Memory bekommen hat
    {
        printf("Problem beim SDA-Speicher allocating !!!\n");
        exit(-1);
    }
    fread(programmSpeicher, instrZahl, sizeof(int), binFile); // die restliche bits (Instruktionen) in programspeicher lesen
    // stackPointer, programmCounter, framePointer alle auf 0 setzen
    stackPointer = 0;
    programmCounter = 0;
    framePointer = 0;
}

// Der geoeffnete File schliessen
void binFileSchliessen(void)
{
    int pruefen;
    free(programmSpeicher); // die benutzte Memory für programmSpeicher freilassen
    free(staticDataArea);   // die benutzte Memory für SDA freilassen
    pruefen = fclose(binFile);
    if (pruefen == 0) // File-Schliessung ist erfoelgt
    {
        printf("Ninja Virtual Machine stopped\n");
        exit(0);
    }
    else // es gab Probleme beim Schliessen
    {
        printf("Problem beim File schliessen\n");
        exit(-1);
    }
}