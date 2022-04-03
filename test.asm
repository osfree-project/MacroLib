INCLUDE	BIOS.INC
INCLUDE	DOS.INC

_DATA	SEGMENT 
HelloStr	DB	"Hello",0dh,0ah,"$"
pHelloStr	DW	HelloStr
fpHelloStr	DD	HelloStr
_DATA	ENDS

_STACK	SEGMENT STACK
	DB	255 DUP (?)
_STACK	ENDS

_TEXT	SEGMENT 
START:
	MOV	AX, _DATA
	MOV	DS, AX
	ASSUME DS:_DATA

	@Cls

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
