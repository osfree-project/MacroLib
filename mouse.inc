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
;   Documentation: http://osfree.org/doku/en:docs:macrolib
;
;*/

@mou		MACRO	FUNC
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
;    Documentation: 
;

@MouInit	MACRO
		@mou	0
		ENDM

;-----------------------------------------------------------------------------
;
;    AL = 01h    Show Mouse Cursor
;
;    Documentation: 
;

@MouShowPointer	MACRO
		@mou	1
		ENDM

;-----------------------------------------------------------------------------
;
;    AL = 03h    Mouse Cursor Status
;
;    Documentation: 
;

@MouStatus	MACRO
		@mou	3
		ENDM

;-----------------------------------------------------------------------------
;
;    AL = 04h    Set Mouse Cursor Position
;
;    Documentation: 
;

@MouSetPos	MACRO	X, Y
		MOV	CX, X
		MOV	DX, Y
		@mou	4
		ENDM

;-----------------------------------------------------------------------------
;
;    AL = 0Fh    Set Mouse Mickey/Pixel ratio
;
;    Documentation: 
;

@MouSetMickey	MACRO	X, Y
		MOV	CX, X
		MOV	DX, Y
		@mou	4
		ENDM

;-----------------------------------------------------------------------------
;
;    AL = 10h    Mouse Cursor Region
;
;    Documentation: 
;

@MouRegion	MACRO UCX, UCY, LCX, LCY
		MOV CX, UCX
		MOV DX, UCY
		MOV SI, LCX
		MOV DI, LCY
		@mou	010h
		ENDM