#ifndef _DEBUGGER_H
#define _DEBUGGER_H
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define version 4
#define STACKSIZE 10000

void listen(int instruktion);
void ausfuehrung(int instruktion);
void binFileSchliessen(void);
void debugger(void);

#endif