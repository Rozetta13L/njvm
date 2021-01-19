#include "Debugger.h"

extern typedef int Object;
extern typedef Object *ObjRef;
extern typedef struct
{
    bool isObjRef;
    union
    {
        ObjRef ObjRef;
        int number;
    } u;

} StackSlot;
extern int framePointer, stackPointer, programmCounter;
extern ObjRef *staticDataArea;
extern unsigned int *programmSpeicher;
extern unsigned int globalVarZahl, instrZahl;
StackSlot stack[STACKSIZE];
extern char *fileName;
int instruction;
// Debugger
void debugger(void)
{
    int breakPoint = -1;
    printf("DEBUG: file '%s' loaded (code size = %d, data size = %d)\n", fileName, instrZahl, globalVarZahl);
    printf("Ninja Virtual Machine started\n");
    instruction = programmSpeicher[programmCounter]; // die zu ausführen inrtuction speichern
    listen(instruction);                             // die zu ausführen Schritt von dem Programm drucken
    char debugInput[10];
    int prevPC;
    while (1)
    {
        printf("DEBUG: inspect, list, breakpoint, step, run, quit?\n");
        scanf("%s", debugInput);              // Entscheidungs-Wahl
        if (strncmp(debugInput, "i", 1) == 0) // der Stack oder SDA ispectieren und die gespeicherte Daten dadrin lesen
        {
            printf("inspect: stack, data?\n");
            scanf("%s", debugInput);
            if (strncmp(debugInput, "s", 1) == 0) // stack ist beim User gewählt
            {
                int durchStack;
                // Schleife, um durch den Stack zu laufen, aber von oben nach unten(ruckwaerts)
                for (durchStack = stackPointer; durchStack >= 0; durchStack--)
                {
                    if (durchStack == stackPointer && durchStack == framePointer) // wenn  die SP und FP auf dem glecihen Platz auf dem Stack zeigen
                    {
                        printf("sp,fp\t-->\t\t %04d \t\t xxxx\n", durchStack);
                    }
                    else if (durchStack == stackPointer) // wo der SP ist (nachste freie platz im Stack)
                    {
                        printf("sp   \t-->\t\t %04d \t\t xxxx\n", durchStack);
                    }
                    else if (durchStack == framePointer) // wo der gewahlte frame ist
                    {
                        printf("fp   \t-->\t\t %04d \t\t %d  \n", durchStack, stack[durchStack]);
                    }
                    else // alle andere platze im Stack zeigen und die gespeicherte werte drin
                    {
                        printf("               \t\t %04d \t\t %d  \n", durchStack, stack[durchStack]);
                    }
                }
                printf("\t\t\t   ---Bottom of Stack---   \t\t\t\n");
            }
            else if (strncmp(debugInput, "d", 1) == 0) // SDA
            {
                int count;
                for (count = 0; count < globalVarZahl; count++) // SDA schleife fur zeigen mit den Werte drin
                {
                    printf("data[%04d]:\t\t %d\n", count, staticDataArea[count]);
                }
                printf("\t\t\t   ---End of Data---   \t\t\t\n");
            }
            instruction = programmSpeicher[programmCounter]; // die zu nächst ausführen inrtuction speichern
            listen(instruction);                             // die zu ausführen Schritt von dem Programm drucken
        }
        else if (strncmp(debugInput, "l", 1) == 0) // INstruktionen listen
        {
            prevPC = programmCounter; // jetztige PC speichern
            programmCounter = 0;      // PC auf 0 setzen um durch den BinFile Instruktionen durchzugehen un es listen
            for (int i = 0; i < instrZahl; i++)
            {
                instruction = programmSpeicher[programmCounter]; // die zu ausführen inrtuction speichern
                programmCounter++;                               // PC erhöhen
                listen(instruction);                             // die zu ausführen Schritt von dem Programm drucken
            }
            printf("\t\t\t   ---End of Code---   \t\t\t\n");
            programmCounter = prevPC;                        // den PC zuruck setzen auf was er war
            instruction = programmSpeicher[programmCounter]; // die zu nächst ausführen inrtuction speichern
            listen(instruction);
        }
        else if (strncmp(debugInput, "b", 1) == 0) // breakpoint setzen
        {
            int breakWert;
            if (breakPoint == -1) // wenn die breakpoint nicht gesetzt ist
            {
                printf("DEBUG [breakpoint}: Cleared\n");
            }
            else if (breakPoint > 0) // wenn eine Breakpoint schon gesetzt ist
            {
                printf("DEBUG [breakpoint}: set at %d\n", breakPoint);
            }
            printf("DEBUG [breakpoint}: address to set. -1 to clear, 0 for no change\n");
            scanf("%d", &breakWert); // breakpointe von User-Angabe nehmen
            if (breakWert == -1)
            {
                breakPoint = breakWert; // breakpoint cleared
                printf("DEBUG [breakpoint}: now cleared\n");
            }
            else if (breakWert > 0)
            {
                breakPoint = breakWert; // breakpoint gesetzt
                printf("DEBUG [breakpoint}: now set at %d\n", breakPoint);
            }
            instruction = programmSpeicher[programmCounter]; // die zu nächst ausführen inrtuction speichern
            listen(instruction);
        }
        else if (strncmp(debugInput, "s", 1) == 0) // step
        {
            instruction = programmSpeicher[programmCounter]; // die zu ausführen inrtuction speichern
            programmCounter++;
            ausfuehrung(instruction);                        // eine Instruktion ausfuehren
            instruction = programmSpeicher[programmCounter]; // die zu nächst ausführen inrtuction speichern
            listen(instruction);                             // die nächste zur Ausfuehrung-Instruktion listen
        }
        else if (strncmp(debugInput, "r", 1) == 0) // run
        {
            while (1)
            {
                if (breakPoint > 0) // uberprufen ob eine breakpoint gesetzt ist
                {
                    instruction = programmSpeicher[programmCounter]; // die zu ausführen inrtuction speichern
                    programmCounter++;
                    ausfuehrung(instruction);              // eine Instruktion ausfuehren
                    if (breakPoint - 1 == programmCounter) // wenn die gewunschte breakpoint platz erreicht ist dann abbrechen
                    {
                        instruction = programmSpeicher[programmCounter]; // die zu ausführen inrtuction speichern
                        programmCounter++;
                        ausfuehrung(instruction);                        // eine Instruktion ausfuehren
                        instruction = programmSpeicher[programmCounter]; // die zu ausführen inrtuction speichern
                        listen(instruction);                             // die nächste zur Ausfuehrung-Instruktion listen
                        break;
                    }
                }
                else // ausführen ohne anzuhalten
                {
                    instruction = programmSpeicher[programmCounter]; // die zu ausführen inrtuction speichern
                    programmCounter++;
                    ausfuehrung(instruction); // den Instruktion ausfuehren
                }
            }
        }
        else if (strncmp(debugInput, "q", 1) == 0) //quit
        {
            binFileSchliessen(); // File schlissen und damit wird die njvm beendet
        }
    }
}