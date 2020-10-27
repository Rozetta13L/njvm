#ifndef njvm3
#define njvm3
#define version 4
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
#define eq 17
#define ne 18
#define lt 19
#define le 20
#define gt 21
#define ge 22
#define jmp 23
#define brf 24
#define brt 25
#define call 26
#define ret 27
#define drop 28
#define pushr 29
#define popr 30
#define dup 31
#define IMMEDIATE(x) ((x)&0x00FFFFFF)
#define SIGN_EXTEND(i) ((i)&0x00800000 ? (i) | 0xFF000000 : (i))
#define STACKSIZE 10000

void push(int wert);
int pop(void);
void listen(unsigned int programSpeicher[]);
void ausfuehrung(unsigned int programSpeicher[]);
void binFileOffnen(char *file);
void binFileSchliessen(void);
void debugger(void);

#endif