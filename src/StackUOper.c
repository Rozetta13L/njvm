#include "Operationen.h"

extern int framePointer, stackPointer, programmCounter;
extern StackSlot stack[STACKSIZE];
extern ObjRef *staticDataArea;
extern unsigned int *programmSpeicher;
unsigned char opcode;
int immediateWert;
ObjRef valueRegister;

// Wert in dem nachsten freien Platz auf dem Stack speichern
void push(int wert, bool isObject)
{
    if (stackPointer > STACKSIZE) // Der Stack ist voll, keine Werte können mehr drin gespeichert werden
    {
        printf("STACKUEBERLAUF !!!\n");
        exit(-1);
    }
    else
    {
        if (isObject)
        {
            stack[stackPointer].u.isObjRef = true;
            stack[stackPointer].u.ObjRef = wert;
            stackPointer = stackPointer + 1; // Stackpointer zeigt auf dem naechsten freien Platz
        }
        else
        {
            stack[stackPointer].u.isObjRef = false;
            stack[stackPointer].u.number = wert;
            stackPointer = stackPointer + 1; // Stackpointer zeigt auf dem naechsten freien Platz
        }
    }
}

// Der Wert aus dem Stack nehemen
int pop(void)
{
    if (stackPointer < 0) // es gibt keine Werte mehr im Stack
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
}

// Die Program-Instruktionen ausfuehren und das Ergebnis rechnen
void ausfuehrung(int instruktion)
{
    int wert1, wert2, ergebnis;

    // Das Gleiche wie Methode 'listen'

    opcode = instruktion >> 24;
    immediateWert = SIGN_EXTEND(IMMEDIATE(instruktion));
    if (opcode == halt)
    {
        binFileSchliessen(); // Der File schliessen und die njvm beenden
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
        scanf("%d", &input); // Integer von stdin lesen (User-Input)
        push(input);         // Der gelesene Wert pushen
    }
    else if (opcode == wrint)
    {
        wert1 = pop();
        printf("%d", wert1); // Wert aus dem Stack nehmen, und auf dem Bildschirm (Terminal) ausgeben
    }
    else if (opcode == rdchr)
    {
        int input;
        input = getchar(); // nur erste Character vom, was der Benutzer schreibt, speichern
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
        wert1 = staticDataArea[immediateWert]; // wert aus dem SDA (gewünschten Platz) holen
        push(wert1);                           // der Wert auf dem Stack pushen
    }
    else if (opcode == popg) // Mit hilfe von Global-VAriablen in SDA arbeiten:
    {
        wert1 = pop();                         // Wert von Stack holen
        staticDataArea[immediateWert] = wert1; // der Wert in dem gewunschten platz im SDA speichern
    }
    else if (opcode == asf) // allocating Stack-Frame:
    {
        push(framePointer);                          // der jetztige framePointer in dem Stack speichern
        framePointer = stackPointer;                 // framePointer auf dem Stackpointer-Position legen
        stackPointer = stackPointer + immediateWert; // Stackpointer setzen auf wie viele Plätze,die von dem Programm gelesen werden, plus den Platz der Stackpointer
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
        wert1 = stack[gewunPos];                 // der Wert aus dem Stack-Frame nehmen
        push(wert1);                             // auf dem Stack pushen
    }
    else if (opcode == popl) // Mit hilfe von aloocated Stack-Frame arbeiten:
    {
        int gewunPos;
        gewunPos = framePointer + immediateWert; // gewunschte Position ereichen
        wert1 = pop();                           // der Wert von dem Stack nehmen
        stack[gewunPos] = wert1;                 // der Wert in dem gewunschten Platz im Stack-Frame speichern
    }
    else if (opcode == eq) // die 2 Werte sind gleich
    {
        wert2 = pop();
        wert1 = pop();

        ergebnis = wert1 == wert2;
        push(ergebnis);
    }
    else if (opcode == ne) // die 2 Werte nicht gleich
    {
        wert2 = pop();
        wert1 = pop();

        ergebnis = wert1 != wert2;
        push(ergebnis);
    }
    else if (opcode == lt) // der 2 gepopte-Wert ist kleiner als 1 gepopte-Wert
    {
        wert2 = pop();
        wert1 = pop();

        ergebnis = wert1 < wert2;
        push(ergebnis);
    }
    else if (opcode == le) // der 2 gepopte-Wert ist kleiner oder gleich als 1 gepopte-Wert
    {
        wert2 = pop();
        wert1 = pop();

        ergebnis = wert1 <= wert2;
        push(ergebnis);
    }
    else if (opcode == gt) // der 2 gepopte-Wert ist grosser als 1 gepopte-Wert
    {
        wert2 = pop();
        wert1 = pop();

        ergebnis = wert1 > wert2;
        push(ergebnis);
    }
    else if (opcode == ge) // der 2 gepopte-Wert ist grosser oder gleich als 1 gepopte-Wert
    {
        wert2 = pop();
        wert1 = pop();

        ergebnis = wert1 >= wert2;
        push(ergebnis);
    }
    else if (opcode == jmp) // springen zum gewahlten PLatz in Intsruktionen-Liste
    {
        programmCounter = immediateWert; // der Programmcounter auf dem Wert der von dem Programm gelesen wird setzen
    }
    else if (opcode == brf) // springen zum gewahlten PLatz in Intsruktionen-Liste falls der wert gleich 0 ist(das geht durch die gleichheit Instruktionen)
    {
        wert1 = pop();
        if (wert1 == 0)
        {
            programmCounter = immediateWert; // der Programmcounter auf dem Wert der von dem Programm gelesen wird setzen
        }
        else
        {
        }
    }
    else if (opcode == brt) // springen zum gewahlten PLatz in Intsruktionen-Liste falls der wert gleich 1 ist(das geht durch die gleichheit Instruktionen)
    {
        wert1 = pop();
        if (wert1 == 1)
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
        wert1 = pop();
        programmCounter = wert1; // der Programmcounter auf dem Wert der von dem Stack gepopt wird setzen, der soll das gleiche sein der beim "call" gepusht ist
    }
    else if (opcode == drop) // n Werte vom Stack popen
    {
        for (int i = 0; i < immediateWert; i++) // n-mal werte aus dem Stack popen (n = Der wert der von dem Programm gelesen wird)
        {
            pop();
        }
    }
    else if (opcode == pushr) // der gespeicherten Wert im Return Value Register auf dem Stack pushen
    {
        push(valueRegister);
    }
    else if (opcode == popr) // der aus dem Stack gepopten Wert im Return Value Register speichern
    {
        valueRegister = pop();
    }
    else if (opcode == dup) // der letzten Wert im Stack duplitizieren
    {
        wert1 = pop();
        wert2 = wert1 * wert1;
        push(wert2);
    }
}