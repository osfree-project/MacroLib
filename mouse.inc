;/*!
;   @file
;
;   @brief Mouse driver Macro Library
;
;   (c) osFree Project 2002-2022, <http://www.osFree.org>
;   for licence see licence.txt in root directory, or project website
;
;   @author Yuri Prokushev (yuri.prokushev@gmail.com)
;
;   This is macros for Mouse INT 33H. This file is new, not found in MASM or related assemblers
;
;   Documentation: http://osfree.org/doku/en:docs:macrolib
;
;*/

IFNDEF MOUSE_DEFINED

MOUSE_DEFINED EQU 1

@MOU_CALL	MACRO	FUNC
		IF (.TYPE (FUNC)) AND 00000100b ; Constant
			MOV	AX,FUNC
		ELSEIF TYPE (FUNC) EQ 1	; BYTES (1 BYTE)
			MOV	AX,FUNC
		ELSEIF TYPE (FUNC) EQ 2	; WORD (2 BYTE)
			IFDIFI	<FUNC>, <AX>
				MOV	AX,FUNC
			ENDIF
		ELSE
			.ERR Unsupported argument type
		ENDIF
		INT	33H
		ENDM

;-----------------------------------------------------------------------------
;
;    AL = 00h    Init driver
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:mouinit
;

@MouInit	MACRO
		@MOU_CALL	0
		ENDM

;-----------------------------------------------------------------------------
;
;    AL = 01h    Show Mouse Cursor
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:moushowpointer
;

@MouShowPointer	MACRO
		@MOU_CALL	1
		ENDM

;-----------------------------------------------------------------------------
;
;    AL = 03h    Mouse Cursor Status
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:moustatus
;

@MouStatus	MACRO
		@MOU_CALL	3
		ENDM

;-----------------------------------------------------------------------------
;
;    AL = 04h    Set Mouse Cursor Position
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:mousetpos
;

@MouSetPos	MACRO	X, Y
		IFDIFI	<X>, <CX>
			MOV	CX, X
		ENDIF
		IFDIFI	<Y>, <DX>
			MOV	DX, Y
		ENDIF
		@MOU_CALL	4
		ENDM

;-----------------------------------------------------------------------------
;
;    AL = 0Fh    Set Mouse Mickey/Pixel ratio
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:mousetmickey
;

@MouSetMickey	MACRO	X, Y
		IFDIFI	<X>, <CX>
			MOV	CX, X
		ENDIF
		IFDIFI	<Y>, <DX>
			MOV	DX, Y
		ENDIF
		@MOU_CALL	4
		ENDM

;-----------------------------------------------------------------------------
;
;    AL = 10h    Mouse Cursor Region
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:mouregion
;

@MouRegion	MACRO UCX, UCY, LCX, LCY
		IFDIFI	<UCX>, <CX>
			MOV	CX, X
		ENDIF
		IFDIFI	<UCY>, <DX>
			MOV	DX, Y
		ENDIF
		IFDIFI	<LCX>, <SI>
			MOV SI, LCX
		ENDIF
		IFDIFI	<LCY>, <DI>
			MOV DI, LCY
		ENDIF
		@MOU_CALL	010h
		ENDM

ENDIF
