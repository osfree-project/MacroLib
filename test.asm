INCLUDE	BIOS.INC
INCLUDE	DOS.INC

_DATA	SEGMENT 
HelloStr	DB	"Hello",0dh,0ah,"$"
pHelloStr	DW	HelloStr
fpHelloStr	DD	HelloStr

Welcome		DB	"osFree Macro Library test program v0.xx", 0dh, 0ah, 0dh, 0ah 
		DB	"1) BIOS tests", 0dh, 0ah
		DB	"2) DOS tests", 0dh, 0ah, 0dh, 0ah
		DB	"0) Exit", 0dh, 0ah
		DB	"$"
_DATA	ENDS

_STACK	SEGMENT STACK
	DB	255 DUP (?)
_STACK	ENDS

_TEXT	SEGMENT 
START:
	MOV	AX, _DATA
	MOV	DS, AX
	ASSUME DS:_DATA

	@SetMode 3
	@Cls
	@DispStr Welcome

	; MASM 5.x style
	@DispStr HelloStr
	@DispStr pHelloStr
	@DispStr fpHelloStr
	MOV	AX, OFFSET HelloStr
	@DispStr AX
	MOV	DX, OFFSET HelloStr
	@DispStr DX

	; MASM 6.x style
	DISPLAY HelloStr
	DISPLAY pHelloStr
	DISPLAY fpHelloStr
	MOV	AX, OFFSET HelloStr
	DISPLAY	AX
	MOV	DX, OFFSET HelloStr
	DISPLAY	DX

	; MASM 5.x style
	@Exit	0

	; MASM 6.x style
	END_PROCESS 0
_TEXT	ENDS

	END	START
