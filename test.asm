INCLUDE	DOS.INC

_DATA	SEGMENT 
HelloStr	DB	"Hello$"
pHelloStr	DW	HelloStr
fpHelloStr	DD	HelloStr
_DATA	ENDS

_STACK	SEGMENT STACK
	DB	255 DUP (?)
_STACK	ENDS

_TEXT	SEGMENT 
START:
	MOV	AX, _DATA
	MOV	ES, AX
	ASSUME DS:_DATA

	; MASM 5.x style
	@DispStr HelloStr
	@DispStr pHelloStr
	@DispStr fpHelloStr

	; MASM 6.x style
	DISPLAY HelloStr
	DISPLAY pHelloStr
	DISPLAY fpHelloStr

_TEXT	ENDS

	END	START
