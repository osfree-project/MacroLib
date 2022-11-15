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
;   @author Yuri Prokushev (yuri.prokushev@gmail.com)
;
;*/

@DEFINE		MACRO	CALLNAME
		IFNDEF	CALLNAME
			EXTERN	CALLNAME: FAR
		ENDIF
		ENDM

@PUSHC		MACRO op				;; 80186 or higher
			IF  @Cpu AND 00000010B
		            push op
			ELSE			    	;; 8088/8086
		            mov  ax, op
		            push ax
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
