;/*!
;   @file
;
;   @ingroup fapi
;
;   @brief Family API implementation
;
;   (c) osFree Project 2018-2022, <http://www.osFree.org>
;   for licence see licence.txt in root directory, or project website
;
;   This is Family API implementation for DOS, used with BIND tools
;   to link required API
;
;   This is small subset of OS/2 API macros which was shipped with IBM Macro Assembler/2 1.0
;   Full subset can be found in OS/2 Developer's Toolkit (sysmac.inc/doscalls.inc/subcalls.inc)
;
;   @author Yuri Prokushev (yuri.prokushev@gmail.com)
;
;*/

		INCLUDE MACROS.INC

@DEFINE		MACRO	CALLNAME
		IFNDEF	CALLNAME
			EXTERN	CALLNAME: FAR
		ENDIF
		ENDM


@PUSHW		MACRO	ARG
			@PUSHC	ARG
		ENDM

@PUSHS		MACRO	ARG
		.ERRB	<ARG>
		a=.type ARG
		IF (A EQ 62h)
			@PUSHC	WORD PTR ARG
			@PUSHC	WORD PTR ARG+2
		else
			MOV		AX,SEG ARG
			PUSH	AX
			LEA		AX,ARG
			PUSH	AX
		endif
		ENDM

@PUSHD		MACRO	ARG
		.ERRB	<ARG>
		IF(OPATTR(ARG) EQ 35h)	; Immediate and defined
			@PUSHC	LOWWORD ARG
			@PUSHC	HIGHWORD ARG
		ELSE
			PUSH	DS		;SAVE DATA SEGMENT
			PUSH	BX		;SAVE CURRENT BX VALUE
			MOV	AX, SEG ARG
			MOV	DS, AX
			MOV	BX, OFFSET ARG	;LOAD BX WITH OFFSET VALUE
			PUSH	WORD PTR [BX]	;PUSH LOW-ORDER OF 32 BIT VAL STACK
			MOV	AX,[BX+2]	;PUT HIGH ORDER 32-BIT VALUE IN AX
			PUSH	BP		;SAVE REGISTER
			MOV	BP,SP
			XCHG	[BP+6],AX	;
			POP	BP		;RESTORE
			MOV	DS,AX		;RESTORE DS TO SAVED VALUE.
			POP	AX		;POP OFF LOW ORDER 32 BIT VAL
			POP	BX		;RESTORE BX TO SAVED VALUE.
			PUSH	AX		;PUT LOW ORDER 32 BIT VALUE ON STACK
		ENDIF
		ENDM

@LOADARG	MACRO	REG, ADR
		IF (.TYPE (ADR)) AND 00010000b		; REGISTER
			MOV	REG, ADR
		ELSEIF TYPE (ADR) EQ 1			; BYTES (1 BYTE)
			MOV	REG, OFFSET ADR
		ELSEIF TYPE (ADR) EQ 2			; NEAR POINTER (2 BYTE)
			MOV	REG, ADR
		ELSEIF TYPE (ADR) EQ 4			; FAR POINTER (4 BYTE)
			LDS	REG, ADR
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		ENDM


@VioScrollUp	MACRO	top,left,bottom,right,lines,cell,handle
			@DEFINE  VioScrollUp
			@PUSHW	top
			@PUSHW	left
			@PUSHW	bottom
			@PUSHW	right
			@PUSHW	lines
			@PUSHS	cell
			@PUSHW	handle
			CALL	FAR PTR VioScrollUp
		ENDM

@VioSetCurPos	MACRO	row,column,handle
			@DEFINE	VioSetCurPos
			@PUSHW  row
			@PUSHW	column
			@PUSHW   handle
			CALL	FAR PTR VioSetCurPos
		ENDM

@KbdStringIn	MACRO	buffer,bufflng,iowait,handle
			@DEFINE	KbdStringIn
			@PUSHS	buffer
			@PUSHS	bufflng
			@PUSHW	iowait
			@PUSHW	handle
			CALL	FAR PTR KbdStringIn
		ENDM

@DosOpen	MACRO	name,handle,action,size,attrib,flag,mode,rsrvd
			@DEFINE	DosOpen
			@PUSHS	name
			@PUSHS	handle
			@PUSHS	action
		 	@PUSHD	size
			@PUSHW	attrib
			@PUSHW	flag
			@PUSHW	mode
			@PUSHD	rsrvd
			CALL	FAR PTR DosOpen
		ENDM

@DosRead	MACRO	handle,buffer,bufflng,read
			@DEFINE	DosRead
			@PUSHW	handle
			@PUSHS	buffer
			@PUSHW	bufflng
			@PUSHS	read
			CALL	FAR PTR DosRead
		ENDM

@DosWrite	MACRO	handle,buffer,bufflng,write
			@DEFIN	DosWrite
			@PUSHW	handle
			@PUSHS	buffer
			@PUSHW	bufflng
			@PUSHS	write
			CALL	FAR PTR	DosWrite
		ENDM

@DosClose	MACRO	handle
			@DEFINE DosClose
			@PUSHW	handle
			CALL FAR PTR DosClose
		ENDM

@DosExit	MACRO	action,result
			@DEFINE DosExit
			@PUSHW	action
			@PUSHW	result
			CALL	FAR PTR DosExit
		ENDM
