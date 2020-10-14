#ifndef njvm2
#define njvm2
#define version 2
#define halt 0
#define pushc 1
#define add 2
#define sub 3
#define mul 4
#define div 5
#define mod 6
#define rdint 7
#define wrint 8
#define rdchr 9
#define wrchr 10
#define pushg 11
#define popg 12
#define asf 13
#define rsf 14
#define pushl 15
#define popl 16
#define IMMEDIATE(x) ((x)&0x00FFFFFF)
#define SIGN_EXTEND(i) ((i)&0x00800000 ? (i) | 0xFF000000 : (i))

void push(int wert);
int pop();
void listen(unsigned int programSpeicher[], int arrayLength);
void ausfuehrung(unsigned int programSpeicher[]);
void binFileOffnen();
void binFileSchliessen();

#endif