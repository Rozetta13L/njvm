//
// version
//
	.vers	8

//
// execution framework
//
__start:
	call	_main
	call	_exit
__stop:
	jmp	__stop

//
// Integer readInteger()
//
_readInteger:
	asf	0
	rdint
	popr
	rsf
	ret

//
// void writeInteger(Integer)
//
_writeInteger:
	asf	0
	pushl	-3
	wrint
	rsf
	ret

//
// Character readCharacter()
//
_readCharacter:
	asf	0
	rdchr
	popr
	rsf
	ret

//
// void writeCharacter(Character)
//
_writeCharacter:
	asf	0
	pushl	-3
	wrchr
	rsf
	ret

//
// Integer char2int(Character)
//
_char2int:
	asf	0
	pushl	-3
	popr
	rsf
	ret

//
// Character int2char(Integer)
//
_int2char:
	asf	0
	pushl	-3
	popr
	rsf
	ret

//
// void exit()
//
_exit:
	asf	0
	halt
	rsf
	ret

//
// void writeString(String)
//
_writeString:
	asf	1
	pushc	0
	popl	0
	jmp	_writeString_L2
_writeString_L1:
	pushl	-3
	pushl	0
	getfa
	call	_writeCharacter
	drop	1
	pushl	0
	pushc	1
	add
	popl	0
_writeString_L2:
	pushl	0
	pushl	-3
	getsz
	lt
	brt	_writeString_L1
	rsf
	ret

//
// void main()
//
_main:
	asf	7
	pushc	1
	popl	0
	pushc	1
	popl	1
	pushc	1
	popl	2
	pushc	2
	popl	3
	jmp	__2
__1:
	pushl	0
	pushl	3
	mul
	pushl	2
	pushl	1
	mul
	add
	popl	4
	pushl	1
	pushl	3
	mul
	popl	5
	pushl	4
	pushl	5
	call	_gcd
	drop	2
	pushr
	popl	6
	pushl	4
	pushl	6
	div
	popl	4
	pushl	5
	pushl	6
	div
	popl	5
	pushl	3
	pushc	1
	add
	popl	3
	pushl	4
	popl	0
	pushl	5
	popl	1
	pushl	4
	call	_writeInteger
	drop	1
	pushc	32
	call	_writeCharacter
	drop	1
	pushc	47
	call	_writeCharacter
	drop	1
	pushc	32
	call	_writeCharacter
	drop	1
	pushl	5
	call	_writeInteger
	drop	1
	pushc	10
	call	_writeCharacter
	drop	1
__2:
	pushl	3
	pushc	100
	le
	brt	__1
__3:
	pushc	1
	call	_writeInteger
	drop	1
	pushc	47
	call	_writeCharacter
	drop	1
	pushc	105
	call	_writeCharacter
	drop	1
	pushc	44
	call	_writeCharacter
	drop	1
	pushc	105
	call	_writeCharacter
	drop	1
	pushc	61
	call	_writeCharacter
	drop	1
	pushc	1
	call	_writeInteger
	drop	1
	pushc	44
	call	_writeCharacter
	drop	1
	pushc	46
	call	_writeCharacter
	drop	1
	pushc	46
	call	_writeCharacter
	drop	1
	pushc	46
	call	_writeCharacter
	drop	1
	pushc	44
	call	_writeCharacter
	drop	1
	pushc	100
	call	_writeInteger
	drop	1
	pushc	10
	call	_writeCharacter
	drop	1
	pushc	115
	call	_writeCharacter
	drop	1
	pushc	117
	call	_writeCharacter
	drop	1
	pushc	109
	call	_writeCharacter
	drop	1
	pushc	109
	call	_writeCharacter
	drop	1
	pushc	101
	call	_writeCharacter
	drop	1
	pushc	32
	call	_writeCharacter
	drop	1
	pushc	61
	call	_writeCharacter
	drop	1
	pushc	32
	call	_writeCharacter
	drop	1
	pushl	4
	call	_writeInteger
	drop	1
	pushc	32
	call	_writeCharacter
	drop	1
	pushc	47
	call	_writeCharacter
	drop	1
	pushc	32
	call	_writeCharacter
	drop	1
	pushl	5
	call	_writeInteger
	drop	1
	pushc	10
	call	_writeCharacter
	drop	1
__0:
	rsf
	ret

//
// Integer gcd(Integer, Integer)
//
_gcd:
	asf	1
	pushl	-4
	pushl	-3
	mod
	pushc	0
	eq
	brf	__5
	pushl	-3
	popr
	jmp	__4
__5:
	pushl	-3
	pushl	-4
	pushl	-3
	mod
	call	_gcd
	drop	2
	pushr
	popr
	jmp	__4
__4:
	rsf
	ret
