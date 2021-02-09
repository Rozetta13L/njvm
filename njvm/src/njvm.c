#include "njvm.h"
#include "bigint/build/include/support.h"
#include "bigint/build/include/bigint.h"

char *fileName = NULL;
unsigned char opcode;
int immediateWert, instruction;
unsigned int globalVarZahl, instrZahl;
int framePointer, stackPointer, programmCounter = 0;
FILE *binFile;
unsigned int *programmSpeicher;
bool debugFlag;
void *heap;
unsigned int stackSize = 64;
unsigned int heapSize = 8192;
typedef struct
{
    bool isObjRef;
    union
    {
        ObjRef ObjRef;
        int number;
    } u;

} StackSlot;
ObjRef valueRegister;
ObjRef *staticDataArea;
StackSlot *stack;

// Versuchen zu offnen der File der im Kommandozeile gegeben wird
void binFileOffnen(char *file)
{
    char formatIdentifier[4];
    unsigned int versionCheck;
    // den File offnen und uberpruefen ob es erfolgreich geoffnet ist
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
    fread(&globalVarZahl, 1, sizeof(int), binFile);            // die naechste 4 bits lesen (globalvariablen-zahl bestimmen)
    staticDataArea = malloc(globalVarZahl * (sizeof(ObjRef))); // SDA allocieren
    if (staticDataArea == NULL)                                // ueberpruefen ob die SDA erfolgreich Memory bekommen hat
    {
        printf("Problem beim SDA-Speicher allocating !!!\n");
        exit(1);
    }
    // alle werte in SDA NULL setzen
    for (int i = 0; i < globalVarZahl; i++)
    {
        staticDataArea[i] = NULL;
    }

    fread(programmSpeicher, instrZahl, sizeof(int), binFile); // die restliche bits (Instruktionen) in programspeicher lesen
    stack = malloc(stackSize * 1024);                         // stack size allocieren
    if (stack == NULL)                                        // ueberpruefen ob die SDA erfolgreich Memory bekommen hat
    {
        printf("Problem beim Stack allocating !!!\n");
        exit(1);
    }
    heap = malloc(heapSize * 1024); // heap size allocieren
    if (heap == NULL)               // ueberpruefen ob die SDA erfolgreich Memory bekommen hat
    {
        printf("Problem beim Heap allocating !!!\n");
        exit(1);
    }
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

// Wert in dem nachsten freien Platz auf dem Stack speichern, es ist kein Reference auf ein Object
void push(int wert)
{
    if (stackPointer > (stackSize * stackSize)) // Der Stack ist voll, keine Werte können mehr drin gespeichert werden
    {
        printf("STACKUEBERLAUF !!!\n");
        exit(-1);
    }
    else
    {
        stack[stackPointer].isObjRef = false; // setze dass das Wert kein Reference auf ein Object ist
        stack[stackPointer].u.number = wert;  // das Wert als int setzen
        stackPointer = stackPointer + 1;      // Stackpointer zeigt auf dem naechsten freien Platz
    }
}

// Reference auf ein Object, der auf dem Heap steht
void pushObj(ObjRef obj)
{
    if (stackPointer > (stackSize * stackSize)) // Der Stack ist voll, keine Werte können mehr drin gespeichert werden
    {
        printf("STACKUEBERLAUF !!!\n");
        exit(-1);
    }
    else
    {
        stack[stackPointer].isObjRef = true; // wert ist ein reference auf Object
        stack[stackPointer].u.ObjRef = obj;  // der umgewandelte int zu big wert auf dem Stack
        stackPointer = stackPointer + 1;     // Stackpointer zeigt auf dem naechsten freien Platz
    }
}

// Der Wert aus dem Stack nehmen, ein int wert kein reference auf Object
int pop(void)
{
    if (stackPointer < 0) // es gibt keine Werte mehr im Stack
    {
        printf("STACKUNTERLAUF !!!\n");
        exit(-1);
    }
    else
    {
        int wert;
        stackPointer = stackPointer - 1;     // stackpointer auf Top of Stack stellen
        wert = stack[stackPointer].u.number; // der int wert aus dem Stack nehmen
        return wert;
    }
}

//Der Wert aus dem Stack nehmen, ein ObjRef kein direct integer
ObjRef popObj(void)
{
    if (stackPointer < 0) // es gibt keine Werte mehr im Stack
    {
        printf("STACKUNTERLAUF !!!\n");
        exit(-1);
    }
    else
    {
        stackPointer = stackPointer - 1; // stackpointer auf Top of Stack legen
        return stack[stackPointer].u.ObjRef;
    }
}

// neues Primitive Object erzeugen ein Pointer auf int werte
ObjRef newPrimObject(int dataSize)
{
    ObjRef primObj;
    unsigned int objSize = sizeof(unsigned int) + dataSize; // der zu speichern Size berechnen (4 Byte für size und dataSize (als Parameter) hat wie viel Bytes fur data braucht)
    if ((primObj = malloc(objSize)) == NULL)                // hier es allocieren
    {
        printf("Problem beim newPrimObj allocieren\n");
        exit(-1);
    }
    primObj->size = (unsigned int)dataSize; // 4 Bytes setzen fur die size -> die restliche Bytes bleiben fur die
    return primObj;
}

// neues Compound Object erzeugen, ein Pointer auf ObjRefs
ObjRef newCmpObj(int dataSize)
{
    ObjRef cmpObj;
    /*
    Size fur der ObjRef der alle andere ObjRefs enhalt allocieren
    4 Bytes int fur das size
    und dann dataSize fur data allocieren und weil jeder data ein ObjRef ist dann multiplizieren es mit size von ObjRef
    */
    if ((cmpObj = malloc(sizeof(unsigned int) + (dataSize * sizeof(ObjRef)))) == NULL)
    {
        printf("Problem beim newCmpObj allocieren\n");
        exit(-1);
    }
    // der Size enhalt die anzahl wie viele Objekte drin gibt und die MSB (die hindeuted das es ein CmpObj ist)
    cmpObj->size = (unsigned int)(dataSize | MSB);
    //alle referencen in es als null zuerst initialisieren
    for (int i = 0; i < dataSize; i++)
    {
        GET_REFS_PTR(cmpObj)
        [i] = NULL;
    }
    return cmpObj;
}
// Debugger
void debugger(void)
{
    int breakPoint = -1;
    printf("DEBUG: file\t:\t'%s'\n", fileName);
    printf("\t code\t:\t%d instructions\n", instrZahl);
    printf("\t data\t:\t%d objects\n", globalVarZahl);
    printf("\t stack\t:\t%d slots\n", stackSize * stackSize);
    printf("\t heap\t:\t 2 *%d bytes\n", heapSize * 512);
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
            printf("inspect: stack, data oder object?\n");
            scanf("%s", debugInput);
            if (strncmp(debugInput, "s", 1) == 0) // stack ist beim User gewählt
            {
                int durchStack;
                // Schleife, um durch den Stack zu laufen, aber von oben nach unten(ruckwaerts)
                for (durchStack = stackPointer; durchStack >= 0; durchStack--)
                {
                    if (durchStack == stackPointer && durchStack == framePointer) // wenn  die SP und FP auf dem glecihen Platz auf dem Stack zeigen
                    {
                        printf("sp,fp\t-->\t\t %04d (xxxxxx)\t\t xxxxxx\n", durchStack);
                    }
                    else if (durchStack == stackPointer) // wo der SP ist (nachste freie platz im Stack)
                    {
                        printf("sp   \t-->\t\t %04d (xxxxxx)\t\t xxxxxx\n", durchStack);
                    }
                    else if (durchStack == framePointer) // wo der gewahlte frame ist
                    {
                        if (stack[durchStack].isObjRef)
                        {
                            printf("fp   \t-->\t\t %04d (objref)\t\t %p  \n", durchStack, (void *)stack[durchStack].u.ObjRef); // die addresse zur void pointer umwandeln und es ausgeben
                        }
                        else
                        {
                            printf("fp   \t-->\t\t %04d (number)\t\t %d  \n", durchStack, stack[durchStack].u.number);
                        }
                    }
                    else // alle andere platze im Stack zeigen und die gespeicherte werte drin
                    {
                        if (stack[durchStack].isObjRef)
                        {
                            printf("               \t\t %04d (objref)\t\t %p  \n", durchStack, (void *)stack[durchStack].u.ObjRef);
                        }
                        else
                        {
                            printf("               \t\t %04d (number)\t\t %d  \n", durchStack, stack[durchStack].u.number);
                        }
                    }
                }
                printf("\t\t\t   ---Bottom of Stack---   \t\t\t\n");
            }
            else if (strncmp(debugInput, "d", 1) == 0) // SDA
            {
                int count;
                for (count = 0; count < globalVarZahl; count++) // SDA schleife fur zeigen mit den Werte drin
                {
                    if (staticDataArea[count] == NULL) //uberpruefen ob die werte in SDA NULL sind
                    {
                        printf("data[%04d]:\t (objref)\t nil\n", count);
                    }
                    else
                    {
                        printf("data[%04d]:\t (objref)\t %p\n", count, (void *)staticDataArea[count]->data);
                    }
                }
                printf("\t\t\t   ---End of Data---   \t\t\t\n");
            }
            else if (strncmp(debugInput, "o", 1) == 0) // objref wert sehen
            {
                int value;
                ObjRef reference;
                printf("object reference?\n");
                scanf("%p", (void **)&reference); // die address von User Eingabe lesen
                if (IS_PRIMITIVE(reference))      // die reference ist ein Primitives Object
                {
                    printf("< Primitiv Object >\n");
                    bip.op1 = reference; //reference in bip.op1 weil es in bigToInt benutzt wird
                    value = bigToInt();  //die umgewandelte ObjRef zu Int speichern
                    printf("Value =\t %d\n", value);
                    printf("\t\t--- End of Object ---\t\t\n");
                }
                else if (reference == NULL)
                {
                    printf("kein Object (nil)\n");
                }
                else
                {
                    printf("< Compound Object >\n");
                    int size;
                    size = GET_ELEMENT_COUNT(reference); // wie viel referencen auf ObjRefs gibt es (Elemente)
                    for (int i = 0; i < size; i++)
                    {
                        printf("field[%04d]:\t(objref)  %p\n", i, (void *)GET_REFS_PTR(reference)[i]);
                    }
                    printf("\t\t--- End of Object ---\t\t\n");
                }
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

// Der Program listen und ausgeben
void listen(int instruktion)
{

    opcode = instruktion >> 24;                          // Der Opcode durch rechts schieben Operator kriegen, weil er in dem 8 Oebersten Bits stehet
    immediateWert = SIGN_EXTEND(IMMEDIATE(instruktion)); // Der Wert, der gepusht werden soll. sigh_extend ist benutzt im falle von negativen Zahlen
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
    else if (opcode == call)
    {
        printf("%04d\t call\t %d\n", programmCounter, immediateWert);
    }
    else if (opcode == ret)
    {
        printf("%04d\t ret\n", programmCounter);
    }
    else if (opcode == drop)
    {
        printf("%04d\t drop\t %d\n", programmCounter, immediateWert);
    }
    else if (opcode == pushr)
    {
        printf("%04d\t pushr\n", programmCounter);
    }
    else if (opcode == popr)
    {
        printf("%04d\t popr\n", programmCounter);
    }
    else if (opcode == dup)
    {
        printf("%04d\t dup\n", programmCounter);
    }
    else if (opcode == new)
    {
        printf("%04d\t new\t %d\n", programmCounter, immediateWert);
    }
    else if (opcode == getf)
    {
        printf("%04d\t getf\t %d\n", programmCounter, immediateWert);
    }
    else if (opcode == putf)
    {
        printf("%04d\t putf\t %d\n", programmCounter, immediateWert);
    }
    else if (opcode == newa)
    {
        printf("%04d\t newa\n", programmCounter);
    }
    else if (opcode == getfa)
    {
        printf("%04d\t getfa\n", programmCounter);
    }
    else if (opcode == putfa)
    {
        printf("%04d\t putfa\n", programmCounter);
    }
    else if (opcode == getsz)
    {
        printf("%04d\t getsz\n", programmCounter);
    }
    else if (opcode == pushn)
    {
        printf("%04d\t pushn\n", programmCounter);
    }
    else if (opcode == refeq)
    {
        printf("%04d\t refeq\n", programmCounter);
    }
    else if (opcode == refne)
    {
        printf("%04d\t refne\n", programmCounter);
    }
}

// Die Program-Instruktionen ausfuehren und das Ergebnis rechnen
void ausfuehrung(int instruktion)
{
    int ergebnis;

    // Das Gleiche wie Methode 'listen'

    opcode = instruktion >> 24;
    immediateWert = SIGN_EXTEND(IMMEDIATE(instruktion));
    if (opcode == halt)
    {
        binFileSchliessen(); // Der File schliessen und die njvm beenden
    }
    else if (opcode == pushc)
    {
        bigFromInt(immediateWert); //von int zu big umwandeln, das umgewandelte WErt steht im bip.res
        pushObj(bip.res);          // Wert auf dem Stack legen
    }
    else if (opcode == add)
    {
        bip.op2 = popObj(); // letzte gepushte Object nehmen
        bip.op1 = popObj(); // vorletzte gepushte Object nehmen
        bigAdd();           // beide Werte addieren, das Ergebnis ist in bip.res gespeichert
        pushObj(bip.res);
    }
    else if (opcode == sub)
    { // wie add
        bip.op2 = popObj();
        bip.op1 = popObj();
        bigSub();
        pushObj(bip.res);
    }
    else if (opcode == mul)
    { // wie add
        bip.op2 = popObj();
        bip.op1 = popObj();
        bigMul();
        pushObj(bip.res);
    }
    else if (opcode == div)
    {
        bip.op2 = popObj(); // Nenner poppen
        bip.op1 = popObj(); // zahler poppen
        // in die bigDiv wird geprüft ob der nenner = 0 ist
        bigDiv(); //division das Ergebnis in bip.res gespeichert
        pushObj(bip.res);
    }
    else if (opcode == mod)
    {
        // wie div
        bip.op2 = popObj();
        bip.op1 = popObj();
        // in die bigDiv wird geprüft ob der nenner = 0 ist
        bigDiv(); //division ausfuhren, und beim Modulo brauchen das Rest von Division, das wird in bip.rem gespeichert
        pushObj(bip.rem);
    }
    else if (opcode == rdint)
    {
        bigRead(stdin);   // das Ergebnis wird in bip.res gespeichert
        pushObj(bip.res); // Der gelesene Object pushen
    }
    else if (opcode == wrint)
    {
        bip.op1 = popObj(); // Object in bip.op1 speichern weil bigPrint benutzt es zu ausdrucken
        bigPrint(stdout);   // die Ausgabe auf stdout ausgeben
    }
    else if (opcode == rdchr)
    {
        int input;
        input = getchar(); // nur erste Character vom, was der Benutzer schreibt, als int speichern
        bigFromInt(input); // zu big wandeln, das Ergebis in bip.res gespeichert
        pushObj(bip.res);
    }
    else if (opcode == wrchr)
    {
        char ausgabe;
        bip.op1 = popObj();
        ausgabe = (char)bigToInt(); // der umgewandelte Int-Wert zur Datentyp Character wandeln
        printf("%c", ausgabe);
    }
    else if (opcode == pushg) // Mit hilfe von Global-VAriablen in SDA arbeiten:
    {
        ObjRef sdaObj;
        sdaObj = staticDataArea[immediateWert]; // ObjectRef aus dem SDA (gewünschten Platz) holen
        pushObj(sdaObj);                        // der Wert auf dem Stack pushen
    }
    else if (opcode == popg) // Mit hilfe von Global-VAriablen in SDA arbeiten:
    {
        ObjRef sdaObj;
        sdaObj = popObj();                      // Wert von Stack holen
        staticDataArea[immediateWert] = sdaObj; // der Wert in dem gewunschten platz im SDA speichern
    }
    else if (opcode == asf) // allocating Stack-Frame:
    {
        push(framePointer);                          // der jetztige framePointer in dem Stack speichern
        framePointer = stackPointer;                 // framePointer auf dem Stackpointer-Position legen
        stackPointer = stackPointer + immediateWert; // Stackpointer setzen auf wie viele Plätze,die von dem Programm gelesen werden, plus den Platz der Stackpointer
        //alle slots zwischen fp und sp(Stack-Frame) mussen mit nill gesetzt werden, um falsche zugrief zu vermeiden
        for (int i = framePointer; i < stackPointer; i++)
        {
            stack[i].isObjRef = true;
            stack[i].u.ObjRef = NULL;
        }
    }
    else if (opcode == rsf) // reset Stack-Frame
    {
        stackPointer = framePointer; // der Stackpointer zurücksetzen, wo er früher war (das ist der Framepointer platz jetzt)
        framePointer = pop();        // der altere Wert der Framepointer aus dem Stack nehmen
    }
    else if (opcode == pushl) // Mit hilfe von die bereits allocated Stack-Frame arbeiten:
    {

        int gewunPos;
        gewunPos = framePointer + immediateWert; // gewunschte Position ereichen
        pushObj(stack[gewunPos].u.ObjRef);       // auf dem Stack pushen
    }
    else if (opcode == popl) // Mit hilfe von aloocated Stack-Frame arbeiten:
    {
        int gewunPos;
        gewunPos = framePointer + immediateWert; // gewunschte Position ereichen
        stack[gewunPos].isObjRef = true;
        stack[gewunPos].u.ObjRef = popObj(); // der Wert in dem gewunschten Platz im Stack-Frame speichern
    }
    else if (opcode == eq) // die 2 Werte sind gleich
    {
        bip.op2 = popObj();
        bip.op1 = popObj();
        ergebnis = bigCmp();       //beide Objects vergleichen und die ruckgabe ist ein int wert
        bigFromInt(ergebnis == 0); // wenn die ObjRef gleich sind dann bekommen wir 0 zuruck (beim true wird ObjRef auf 1)
        pushObj(bip.res);
    }
    else if (opcode == ne) // die 2 Werte nicht gleich
    {
        bip.op2 = popObj();
        bip.op1 = popObj();
        ergebnis = bigCmp();
        bigFromInt(ergebnis != 0); // wenn die ObjRef ungleich sind dann bekommen wir wert ungleich 0 zuruck (beim true wird ObjRef auf 1)
        pushObj(bip.res);
    }
    else if (opcode == lt) // der 2 gepopte-Wert ist kleiner als 1 gepopte-Wert
    {
        bip.op2 = popObj();
        bip.op1 = popObj();
        ergebnis = bigCmp();
        bigFromInt(ergebnis < 0); // wenn die ObjRef ungleich sind dann bekommen wir wert kleiner 0 zuruck (beim true wird ObjRef auf 1)
        pushObj(bip.res);
    }
    else if (opcode == le) // der 2 gepopte-Wert ist kleiner oder gleich als 1 gepopte-Wert
    {
        bip.op2 = popObj();
        bip.op1 = popObj();
        ergebnis = bigCmp();
        bigFromInt(ergebnis <= 0); // wenn die ObjRef ungleich sind dann bekommen wir wert kleiner oder gleich 0 zuruck (beim true wird ObjRef auf 1)
        pushObj(bip.res);
    }
    else if (opcode == gt) // der 2 gepopte-Wert ist grosser als 1 gepopte-Wert
    {
        bip.op2 = popObj();
        bip.op1 = popObj();
        ergebnis = bigCmp();
        bigFromInt(ergebnis > 0); // wenn die ObjRef ungleich sind dann bekommen wir wert grosser 0 zuruck (beim true wird ObjRef auf 1)
        pushObj(bip.res);
    }
    else if (opcode == ge) // der 2 gepopte-Wert ist grosser oder gleich als 1 gepopte-Wert
    {
        bip.op2 = popObj();
        bip.op1 = popObj();
        ergebnis = bigCmp();
        bigFromInt(ergebnis >= 0); // wenn die ObjRef ungleich sind dann bekommen wir wert grosser oder gleich 0 zuruck (beim true wird ObjRef auf 1)
        pushObj(bip.res);
    }
    else if (opcode == jmp) // springen zum gewahlten PLatz in Intsruktionen-Liste
    {
        programmCounter = immediateWert; // der Programmcounter auf dem Wert der von dem Programm gelesen wird setzen
    }
    else if (opcode == brf) // springen zum gewahlten PLatz in Intsruktionen-Liste falls der wert gleich 0 ist(das geht durch die gleichheit Instruktionen)
    {
        bip.op1 = popObj();
        int wert = bigToInt();
        if (wert == 0)
        {
            programmCounter = immediateWert; // der Programmcounter auf dem Wert der von dem Programm gelesen wird setzen
        }
        else
        {
        }
    }
    else if (opcode == brt) // springen zum gewahlten PLatz in Intsruktionen-Liste falls der wert gleich 1 ist(das geht durch die gleichheit Instruktionen)
    {
        bip.op1 = popObj();
        int wert = bigToInt();
        if (wert == 1)
        {
            programmCounter = immediateWert; // der Programmcounter auf dem Wert der von dem Programm gelesen wird setzen
        }
        else
        {
        }
    }
    else if (opcode == call) // pushen der nachste Instruktions PLatz und springen zu der gewahlten Instruktion
    {
        push(programmCounter);           // der Programmcounter auf dem Stack pushen um es später nochmal zu haben
        programmCounter = immediateWert; // der Programmcounter auf dem Wert der von dem Programm gelesen wird setzen
    }
    else if (opcode == ret) // popen der Wert vom Stack und es im Programmcounter speichern
    {
        int wert;
        wert = pop();
        programmCounter = wert; // der Programmcounter auf dem Wert der von dem Stack gepopt wird setzen, der soll das gleiche sein der beim "call" gepusht ist
    }
    else if (opcode == drop) // n Werte vom Stack popen
    {
        stackPointer = stackPointer - immediateWert;
    }
    else if (opcode == pushr) // der gespeicherten ObjRef im Return Value Register auf dem Stack pushen
    {
        pushObj(valueRegister);
    }
    else if (opcode == popr) // der aus dem Stack gepopten ObjRef im Return Value Register speichern
    {
        valueRegister = popObj(); // ObjRef in Return Value Register speichern
    }
    else if (opcode == dup) // der letzten Wert im Stack duplitizieren
    {
        ObjRef objWert;
        objWert = popObj(); // Top of Stack ObjRef nehmen
        // der ObjRef zweimal auf dem Stack pushen
        pushObj(objWert);
        pushObj(objWert);
    }
    else if (opcode == new) // new Record erzeugen
    {
        pushObj(newCmpObj(immediateWert)); //neues Compound Object auf dem Stack liegen
    }

    else if (opcode == getf) // Die im Verbundobjekt object an Position <n> enthaltene Referenz auf ein Objekt value wird auf dem Stack pushen
    {
        ObjRef verbundObj;
        verbundObj = popObj(); // ein Compound Object aus dem Stack nehmen
        // überprufen ob es ungleich NULL ist (kein nil wert)
        if (verbundObj == NULL)
        {
            printf("NULL Verbundobject!!\n");
            exit(-1);
        }
        // überpruefen dass es ein Compound Object ist und kein Primitives Object ist
        if (IS_PRIMITIVE(verbundObj))
        {
            printf("Es ist ein primitives Wert !!!\n");
            exit(-1);
        }
        // uberpruefen ob es genug ObjReferenzen in das verbungobj gibt damit wir darauf zugriefen konnen mit <n>
        if (GET_ELEMENT_COUNT(verbundObj) < immediateWert)
        {
            printf("Keine genuge ObjRef in das verbundobj !!!\n");
            exit(-1);
        }
        // das gewunschte ObjRef in das Compound Object auf dem Stack pushen
        pushObj(GET_REFS_PTR(verbundObj)[immediateWert]);
    }
    else if (opcode == putf) // Die Referenz auf ein Objekt value wird im Verbundobjekt object an Position <n> speichern
    {
        // der Primitives ObjRef in dem Compound Object gespeichert werden soll poppen
        ObjRef valueVonStack;
        valueVonStack = popObj();
        // der Compound Object in dem, der Primitives Object gespeichert werden soll, aus dem Stack nehmen
        ObjRef verbundObj;
        verbundObj = popObj();
        // überprufen ob es ungleich NULL ist (kein nil wert)
        if (verbundObj == NULL)
        {
            printf("NULL Verbundobject!!\n");
            exit(-1);
        }
        // überpruefen dass es ein Compound Object ist und kein Primitives Object ist
        if (IS_PRIMITIVE(verbundObj))
        {
            printf("Es ist ein primitives Wert !!!\n");
            exit(-1);
        }
        // uberpruefen ob es genug ObjReferenzen in das verbungobj gibt damit wir darauf zugriefen konnen mit <n>
        if (GET_ELEMENT_COUNT(verbundObj) < immediateWert)
        {
            printf("Keine genuge ObjRef in das verbundobj !!!\n");
            exit(-1);
        }
        // das Primitives Object in das gewunschte Position in das Compound Object speichern
        GET_REFS_PTR(verbundObj)
        [immediateWert] = valueVonStack;
    }
    else if (opcode == newa) //Ein neues Array erzeugen
    {
        //Array-size von Stack nehmen (lokale var) und es zu int umwandeln
        bip.op1 = popObj();
        int aSize = bigToInt();
        ObjRef aObj;
        aObj = newCmpObj(aSize);
        // das Array auf dem Stack
        pushObj(aObj);
    }
    else if (opcode == getfa) //Die im Verbundobjekt array an Position index enthaltene Referenz auf ein Objekt value wird auf dem Stack pushen
    {
        // das gleiches verhalten beim getf, nur das gewunschte index ist vom Stack genommen und nicht als Parameter beim Instruktion
        bip.op1 = popObj();
        int aIdx = bigToInt();
        ObjRef arrObj;
        arrObj = popObj();
        // überprufen ob es ungleich NULL ist (kein nil wert)
        if (arrObj == NULL)
        {
            printf("NULL Verbundobject!!\n");
            exit(-1);
        }
        // überpruefen dass es ein Compound Object ist und kein Primitives Object ist
        if (IS_PRIMITIVE(arrObj))
        {
            printf("Es ist ein primitives Wert !!!\n");
            exit(-1);
        }
        // uberpruefen ob es genug ObjReferenzen in das verbungobj gibt damit wir darauf zugriefen konnen mit <n>
        if (GET_ELEMENT_COUNT(arrObj) < aIdx)
        {
            printf("Keine genuge ObjRef in das verbundobj !!!\n");
            exit(-1);
        }

        pushObj(GET_REFS_PTR(arrObj)[aIdx]);
    }
    else if (opcode == putfa) //Die Referenz auf ein Objekt value wird im Verbundobjekt array an Position index speichern
    {
        // das gleiches verhalten beim putf, nur das gewunschte index ist vom Stack genommen und nicht als Parameter beim Instruktion
        ObjRef valueVonStack;
        valueVonStack = popObj();
        bip.op1 = popObj();
        int aIdx = bigToInt();
        ObjRef arrObj;
        arrObj = popObj();
        // überprufen ob es ungleich NULL ist (kein nil wert)
        if (arrObj == NULL)
        {
            printf("NULL Verbundobject!!\n");
            exit(-1);
        }
        // überpruefen dass es ein Compound Object ist und kein Primitives Object ist
        if (IS_PRIMITIVE(arrObj))
        {
            printf("Es ist ein primitives Wert !!!\n");
            exit(-1);
        }
        // uberpruefen ob es genug ObjReferenzen in das verbungobj gibt damit wir darauf zugriefen konnen mit <n>
        if (GET_ELEMENT_COUNT(arrObj) < aIdx)
        {
            printf("Keine genuge ObjRef in das verbundobj !!!\n");
            exit(-1);
        }
        GET_REFS_PTR(arrObj)
        [aIdx] = valueVonStack;
    }
    else if (opcode == getsz) // ObjRef-Size berechnen und das Ergebnis auf dem Stack pushen
    {
        ObjRef objWert;
        objWert = popObj();
        int ergebnis;
        // uberpruefen ob es NULL ist
        if (objWert == NULL)
        {
            printf("NULL Verbundobject!!\n");
            exit(-1);
        }
        // wenn dass das wert drin ein Primitives ObjRef ist
        if (IS_PRIMITIVE(objWert))
        {
            ergebnis = -1;
            bigFromInt(ergebnis);
            pushObj(bip.res);
        }
        // wenn dass das wert drin ein Compound ObjRef ist
        else
        {
            // die Elemente Anzahl in das Compound Object kriegen
            ergebnis = GET_ELEMENT_COUNT(objWert);
            // das int zu big umwandeln und auf dem Stack pushen
            bigFromInt(ergebnis);
            pushObj(bip.res);
        }
    }
    else if (opcode == pushn) // NULL ObjRef pushen
    {
        // ein NULL ObjRef auf dem Stack pushen
        pushObj(NULL);
    }
    else if (opcode == refeq) // uberpruefen ob die Reference gleich sind
    {
        int ergebnis = (popObj() == popObj()); // die referencen vergleichen, wenn sie gleich sind -> true, ungleich -> false
        bigFromInt(ergebnis);
        pushObj(bip.res);
    }
    else if (opcode == refne) // uberpruefen ob die Reference ungleich sind
    {
        int ergebnis = (popObj() != popObj()); // die referencen vergleichen, wenn sie ungleich sind -> true, gleich -> false
        bigFromInt(ergebnis);
        pushObj(bip.res);
    }
}
// Main-Methode
int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        int i, fileCount;
        fileCount = 0;
        for (i = 1; i < argc; i++)
        {

            if (strcmp(argv[i], "--help") == 0) // Hilfe ausdrucken (wie die VM funktioniert bzw- benutzt werden soll)
            {
                printf("usage: ./njvm [options] <code file>\n");
                printf(" --stack <n>      set stack size to n KBytes (default: n = 64)\n");
                printf(" --heap <n>       set heap size to n KBytes (default: n = 8192)\n");
                printf(" --gcstats        show garbage collection statistics\n");
                printf(" --gcpurge        purge old objects after collection\n");
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
            else if (strcmp(argv[i], "--stack") == 0) // hier nicht implementiert
            {
                i++;
            }
            else if (strcmp(argv[i], "--heap") == 0) // hier nicht implementiert
            {
                i++;
            }
            else if (strcmp(argv[i], "--gcstats") == 0) // hier nicht implementiert
            {
            }
            else if (strcmp(argv[i], "--gcpurge") == 0) // hier nicht implementiert
            {
            }
            else if (strcmp(argv[i], "--debug") == 0) // debugger zum start stellen
            {
                debugFlag = true;
            }
            else if ((strncmp(argv[i], "--", 2) == 0) || (strncmp(argv[i], "--", 1) == 0)) // ueberpruefen ob ein unbekanntes Befehl gegeben wird
            {
                printf("unknown command line argument '%s', try './njvm --help'\n", argv[i]);
                exit(-1);
            }
            else // filename speichern, wenn mehr als ein name gegeben ist, dann das Programm stoppen
            {
                fileCount = fileCount + 1;
                if (fileCount > 1)
                {
                    printf("Error: more than one code file specified\n");
                    exit(-1);
                }

                fileName = argv[i];
            }
        }
    }
    else
    {
        printf("Error: no code file specified\n");
    }
    if (debugFlag) // wenn Debugger zum start gestellt ist
    {
        binFileOffnen(fileName);
        // debugger starten
        debugger();
    }
    else
    {
        binFileOffnen(fileName); // der als parameter gegebene Programm öffnen
        printf("Ninja Virtual Machine started\n");
        // das Programm laufen lassen
        while (1)
        {
            instruction = programmSpeicher[programmCounter];
            programmCounter++;
            ausfuehrung(instruction);
        }
    }

    return 0;
}

void fatalError(char *msg)
{
    printf("%s\n", msg);
    exit(-1);
}