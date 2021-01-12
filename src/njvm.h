#ifndef _NJVM_H
#define _NJVM_H
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define version 4

void ausfuehrung(int instruktion);
void binFileOffnen(char *file);
void debugger(void);

#endif
