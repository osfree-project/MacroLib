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

@MouInit	MACRO	MODE
		@mou	0
		ENDM

;-----------------------------------------------------------------------------
;
;    AL = 01h    Show Mouse Cursor
;
;    Documentation: 
;

@MouShowPointer	MACRO	MODE
		@mou	1
		ENDM

;-----------------------------------------------------------------------------
;
;    AL = 03h    Mouse Cursor Status
;
;    Documentation: 
;

@MouStatus	MACRO	MODE
		@mou	3
		ENDM

