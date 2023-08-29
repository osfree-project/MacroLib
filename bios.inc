;/*!
;   @file
;
;   @brief BIOS Macro Library
;
;   (c) osFree Project 2002-2022, <http://www.osFree.org>
;   for licence see licence.txt in root directory, or project website
;
;   @author Yuri Prokushev (yuri.prokushev@gmail.com)
;
;   This library based on IBM 5150 Technical Reference #6322507 of Apr 1984, MACRO.DOC from MASM 5.x,
;   Ralf Brown Interrupt list.
;
;   Functions, found in later BIOSes mostly not covered, but can be added in future. Macro names mostly constructed
;   from MASM 5.x, MASM 6.x and ALP documentation.
;
;   Documentation: http://osfree.org/doku/en:docs:macrolib
;
;*/

IFNDEF BIOS_DEFINED

BIOS_DEFINED EQU 1

;=============================================================================
; Video BIOS Interrupts
;=============================================================================

@VBIOS_CALL	MACRO	FUNC
		IF (OPATTR (FUNC)) AND 00000100B ; Constant
			IF FUNC LT 100H	; BYTE
				MOV	AH,FUNC
			ELSE		; WORD
				MOV	AX,FUNC
			ENDIF
		ELSEIF (OPATTR (FUNC)) AND 00010000B ; Register
			IF TYPE (FUNC) EQ 1	; BYTES (1 BYTE)
			MOV	AH,FUNC
			ELSEIF TYPE (FUNC) EQ 2	; WORD (2 BYTE)
				IFDIFI	<FUNC>, <AX>
					MOV	AX,FUNC
				ENDIF
			ENDIF
		ELSE
			.ERR 1 Unsupported argument type
		ENDIF
		INT	10H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 00h    Set Video Mode
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:setmode
;

@SetMode	MACRO	MODE
		IFNB	<MODE>
			xor     ax,ax
			mov     al,mode
		ELSE
			MOV	AX, 3
		ENDIF
		@VBIOS_CALL	AX
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 01h    Set Cursor Shape Size
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:setcursz
;

@SetCurSz	MACRO	FIRST,LAST
		IFDIFI	<FIRST>, <CH>
			IF TYPE (FIRST) EQ 1	; BYTES (1 BYTE)
				MOV	CH,FIRST
			ELSE
				ECHO Bad argument
				.ERR
			ENDIF
		ENDIF
		IFDIFI	<LAST>, <CL>
			IF TYPE (LAST) EQ 1	; BYTES (1 BYTE)
				MOV	CL,LAST
			ELSE
				ECHO Bad argument
				.ERR
			ENDIF
		ENDIF
		PUSH	BX		; BIOS bug
		@GetMode		; BIOS bug
		POP	BX		; BIOS bug
		@VBIOS_CALL	01H
		ENDM

@SetCsrSize	MACRO	FIRST,LAST
		@SetCurSz	FIRST,LAST
		ENDM


;-----------------------------------------------------------------------------
;
;    AH = 02h    Set Cursor Position
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:setcurpos
;

@SetCurPos	MACRO	COLUMN,ROW,PAGE

		IFNB	<COLUMN>
		IFDIFI	<COLUMN>, <DL>
			IF TYPE (COLUMN) EQ 1	; BYTES (1 BYTE)
				MOV	DL,COLUMN
			ELSE
				ECHO Bad argument
				.ERR
			ENDIF
		ENDIF
		ELSE
			SUB	DL, DL
		ENDIF

		IFNB	<ROW>
		IFDIFI	<ROW>, <DH>
			IF TYPE (ROW) EQ 1	; BYTES (1 BYTE)
				MOV	DH,ROW
			ELSE
				ECHO Bad argument
				.ERR
			ENDIF
		ENDIF
		ELSE
			SUB	DH, DH
		ENDIF

		IFNB	<PAGE>
			IF TYPE (PAGE) EQ 1	; BYTES (1 BYTE)
				MOV	BH,PAGE
			ELSE
				ECHO Bad argument
				.ERR
			ENDIF
		ELSE
			SUB	BH, BH
		ENDIF
		@VBIOS_CALL	02H
		ENDM

@SetCsrPos	MACRO	COLUMN,ROW,PAGE
		@SetCurPos	COLUMN,ROW,PAGE
		ENDM
		
;-----------------------------------------------------------------------------
;
;    AH = 03h    Get Current Cursor Position and Size
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:getcur
;

@GetCur	MACRO	PAGE
		IFNB	<PAGE>
			IF TYPE (PAGE) EQ 1	; BYTES (1 BYTE)
				MOV	BH,PAGE
			ELSE
				ECHO Bad argument
				.ERR
			ENDIF
		ELSE
			SUB	BH, BH
		ENDIF
		@VBIOS_CALL	03H
		ENDM

@GetCsr	MACRO	PAGE
		@GetCur PAGE
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 05h    Set Active Video Page
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:setpage
;

@SetPage	MACRO	PAGE
		IFNB	<PAGE>
			IF TYPE (PAGE) EQ 1	; BYTES (1 BYTE)
				MOV	AX, 0500H+PAGE
				@VBIOS_CALL	AX
			ELSE
				ECHO Bad argument
				.ERR
			ENDIF
		ELSE
			@VBIOS_CALL	0500H
		ENDIF
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 06h    Scroll Screen Area Up
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:scrollup
;

@ScrollUp	MACRO	DISTANCE,ATTRIB,UPCOL,UPROW,DOWNCOL,DOWNROW
		PUSH	BP		; BIOS bug
		PUSH	DS		; BIOS bug
		IF TYPE (UPCOL) EQ 1	; BYTES (1 BYTE)
			MOV	CL,UPCOL
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (UPROW) EQ 1	; BYTES (1 BYTE)
			MOV	CH,UPROW
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (DOWNCOL) EQ 1	; BYTES (1 BYTE)
			MOV	DL,DOWNCOL
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (DOWNROW) EQ 1	; BYTES (1 BYTE)
			MOV	DH,DOWNROW
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (ATTRIB) EQ 1	; BYTES (1 BYTE)
			MOV	BH,ATTRIB
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (ATTRIB) EQ 1	; BYTES (1 BYTE)
			MOV	AX, 0600h+DISTANCE
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		@VBIOS_CALL	AX
		POP	DS		; BIOS bug
		POP	BP		; BIOS bug
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 07h    Scroll Screen Area Down
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:scrolldn
;

@ScrollDn	MACRO	DISTANCE,ATTRIB,UPCOL,UPROW,DOWNCOL,DOWNROW
		PUSH	BP		; BIOS bug
		PUSH	DS		; BIOS bug
		IF ((.TYPE (UPCOL)) AND 00000100b) AND (UPCOL LE 0FFH); Constant
			MOV	CL,UPCOL
		ELSEIF TYPE (UPCOL) EQ 1	; BYTES (1 BYTE)
			MOV	CL,UPCOL
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF ((.TYPE (UPROW)) AND 00000100b) AND (UPROW LE 0FFH); Constant
			MOV	CH,UPROW
		ELSEIF TYPE (UPROW) EQ 1	; BYTES (1 BYTE)
			MOV	CH,UPROW
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF ((.TYPE (DOWNCOL)) AND 00000100b) AND (DOWNCOL LE 0FFH); Constant
			MOV	DL,DOWNCOL
		ELSEIF TYPE (DOWNCOL) EQ 1	; BYTES (1 BYTE)
			MOV	DL,DOWNCOL
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF ((.TYPE (DOWNROW)) AND 00000100b) AND (DOWNROW LE 0FFH); Constant
			MOV	DH,DOWNROW
		ELSEIF TYPE (DOWNROW) EQ 1	; BYTES (1 BYTE)
			MOV	DH,DOWNROW
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF ((.TYPE (ATTRIB)) AND 00000100b) AND (ATTRIB LE 0FFH); Constant
			MOV	BH,ATTRIB
		ELSEIF TYPE (ATTRIB) EQ 1	; BYTES (1 BYTE)
			MOV	BH,ATTRIB
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF ((.TYPE (DISTANCE)) AND 00000100b) AND (DISTANCE LE 0FFH); Constant
			MOV	AX, 0700H+DISTANCE
		ELSEIF TYPE (DISTANCE) EQ 1	; BYTES (1 BYTE)
			MOV	AH, 07
			MOV	AL, DISTANCE
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		@VBIOS_CALL	AX
		POP	DS		; BIOS bug
		POP	BP		; BIOS bug
		ENDM


@Scroll		MACRO	DISTANCE,ATRIB,UPCOL,UPROW,DOWNCOL,DOWNROW
		IF	DISTANCE LE 0
			@ScrollUp -DISTANCE,ATRIB,UPCOL,UPROW,DOWNCOL,DOWNROW
		ELSE
			@ScrollDn DISTANCE,ATRIB,UPCOL,UPROW,DOWNCOL,DOWNROW
		ENDIF
		ENDM

;-----------------------------------------------------------------------------
;
;    Clear Screen
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:cls
;

@Cls	MACRO
		@Scroll 25,07h,0,0,79,24
		@SetCurPos
		ENDM
;-----------------------------------------------------------------------------
;
;    AH = 08h    Get Character and Attribute
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:getchatr
;

@GetChAtr	MACRO	PAGE
		PUSH	SI		; BIOS bug
		PUSH	DI		; BIOS bug
		PUSH	BP		; BIOS bug
		IFNB	<PAGE>
		IF TYPE (PAGE) EQ 1	; BYTES (1 BYTE)
			MOV	BH,PAGE
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		ELSE
			SUB	BH, BH
		ENDIF
		@VBIOS_CALL	08H
		POP	BP		; BIOS bug
		POP	DI		; BIOS bug
		POP	SI		; BIOS bug
		ENDM

@GetCharAtr	MACRO	PAGE
		@GetChAtr	PAGE
		ENDM
;-----------------------------------------------------------------------------
;
;    AH = 09h    Print Char With Attribute
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:putchatr
;

@PutChAtr	MACRO	CHAR,ATRIB,PAGE,REPEAT
		IFDIFI	<CHAR>,<AL>
		IF TYPE (CHAR) EQ 1	; BYTES (1 BYTE)
			MOV	AL,CHAR
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		ENDIF
		IF TYPE (ATRIB) EQ 1	; BYTES (1 BYTE)
			MOV	BL,ATRIB
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IFNB	<PAGE>
		IF TYPE (PAGE) EQ 1	; BYTES (1 BYTE)
			MOV	BH,PAGE
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		ELSE
			SUB	BH, BH
		ENDIF
		IFDIFI	<REPEAT>,<CX>
			MOV	CX,REPEAT
		ENDIF
		@VBIOS_CALL	09H
		ENDM

@PutCharAtr	MACRO	CHAR,ATRIB,PAGE,REPEAT
		@PutChAtr	CHAR,ATRIB,PAGE,REPEAT
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Ah    Print Char
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:putch
;

@PutCh		MACRO	CHAR,ATTRIB,PAGE,REPEAT
		IFNB	<CHAR>
		IFDIFI	<CHAR>, <AL>
		IF TYPE (CHAR) EQ 1	; BYTES (1 BYTE)
			MOV	AL,CHAR
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		ENDIF
		ENDIF
		IFNB	<ATTRIB>
		IF TYPE (ATTRIB) EQ 1	; BYTES (1 BYTE)
			MOV	BL,ATTRIB
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		ENDIF
		IFNB	<PAGE>
		IF TYPE (PAGE) EQ 1	; BYTES (1 BYTE)
			MOV	BH,PAGE
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		ELSE
			SUB	BH, BH
		ENDIF
		IFDIFI	<REPEAT>, <CX>
			MOV	CX,REPEAT
		ENDIF
		@VBIOS_CALL	0AH
		ENDM

@PutChar	MACRO	CHAR,ATTRIB,PAGE,REPEAT
		@PutCh	CHAR,ATTRIB,PAGE,REPEAT
		ENDM
;-----------------------------------------------------------------------------
;
;    AH = 0Bh    Set Palette
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:setpalet
;

@SetPalet	MACRO	COLOR
		MOV	BH,1
		IF TYPE (COLOR) EQ 1	; BYTES (1 BYTE)
			MOV	BL,COLOR
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		@VBIOS_CALL	0BH
		ENDM

@SetPalette	MACRO	COLOR
		@SetPalet COLOR
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Bh    Set Color
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:setcolor
;

@SetColor	MACRO	COLOR
		SUB	BH,BH
		IF TYPE (COLOR) EQ 1	; BYTES (1 BYTE)
			MOV	BL,COLOR
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		@VBIOS_CALL	0BH
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Ch    Draw Pixel
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:setdot
;

@SetDot		MACRO	COLOR, ROW, COL, PAGE
		IFNB	<PAGE>
		IF TYPE (PAGE) EQ 1	; BYTES (1 BYTE)
			MOV	BH,PAGE
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		ELSE
			SUB	BH, BH
		ENDIF
		MOV	DX, ROW
		MOV	CX, COL
		MOV	AX, 0C00H+COLOR
		@VBIOS_CALL	AX
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Dh    Get Pixel
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:getdot
;

@GetDot		MACRO	COL, ROW, PAGE
		IFNB	<PAGE>
		IF TYPE (PAGE) EQ 1	; BYTES (1 BYTE)
			MOV	BH,PAGE
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		ELSE
			SUB	BH, BH
		ENDIF
		MOV	DX, ROW
		MOV	CX, COL
		@VBIOS_CALL	0DH
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Eh    Write char in TTY mode
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:wrttty
;

@WrtTTY		MACRO	CHAR, PAGE
		IFNB	<PAGE>
		IF TYPE (PAGE) EQ 1	; BYTES (1 BYTE)
			MOV	BH,PAGE
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		ELSE
			SUB	BH, BH
		ENDIF
		PUSH	SI		; BIOS bug
		PUSH	DI		; BIOS bug
		PUSH	BP		; BIOS bug
		IFDIFI	<CHAR>, <AL>
		IF TYPE (CHAR) EQ 1	; BYTES (1 BYTE)
			MOV	AL, CHAR
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		ENDIF
		@VBIOS_CALL	0EH
		POP	BP		; BIOS bug
		POP	DI		; BIOS bug
		POP	SI		; BIOS bug
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Fh    Return Video State
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:videostate
;

@VideoState	MACRO
		@VBIOS_CALL	0FH
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Fh    Get Current Video Mode
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:getmode
;

@GetMode	MACRO
		@VBIOS_CALL	0Fh
		ENDM
 
;-----------------------------------------------------------------------------
;
;    AH = 1Ah    Get Display Configuration
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:getdisplay
;
 
@GetDisplay	MACRO
		XOR	AX, AX
		@VBIOS_CALL	1Ah
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 1Ch    Get Display Configuration
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:getvideostate
;

@GetVideoState	MACRO
		XOR	AX, AX
		@VBIOS_CALL	1Ch
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 12h    Alternate functions
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:getegainfo
;

@GetEGAInfo	MACRO
		MOV	AH,12H
		MOV	BL,10H
		INT	10H
		ENDM

;=============================================================================
; Serial Port BIOS Interrupts
;=============================================================================
		
@AUX_CALL	MACRO	FUNC
		IF (.TYPE (FUNC)) AND 00000100b ; Constant
			MOV	AH,FUNC
		ELSEIF TYPE (FUNC) EQ 1	; BYTES (1 BYTE)
			MOV	AH,FUNC
		ELSEIF TYPE (FUNC) EQ 2	; WORD (2 BYTE)
		IFDIFI	<FUNC>, <AX>
			MOV	AX,FUNC
		ENDIF
		ENDIF
		INT	14H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 00h    Initialize port
;
;            Parameters:
;                Entry:  AL = Baud rate code
;                        DX = Port number
;                Exit:   AX = Port status (see function 03h)
;
;    Baud rates is passed in the high order 3 bits of AL
;    as follows:
;
;                000 =   110 baud
;                001 =   150  ''
;                010 =   300  ''
;                011 =   600  ''
;                100 =  1200  ''
;                101 =  2400  ''
;                110 =  4800  ''
;                111 =  9600  ''
;

;    Bits 4-3 define parity:     0 0       no parity
;                                1 0       no parity
;                                0 1      odd parity
;                                1 1     even parity
;
;    Bit 2 defines stop bits:      0        1 stop bit;
;                                  1        2 
;
;    Bits 1-0 character length:  1 0        7 bits
;                                1 1        8 bits
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:auxinit
;

@AuxInit	MACRO PORT, RATE
		MOV	DX, PORT
		IF TYPE (RATE) EQ 1	; BYTES (1 BYTE)
			MOV	AX, 0000H+RATE
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		@AUX_CALL	AX
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 01h    Transmit character with wait
;
;            Parameters:
;                Entry:  AL = Character
;                        DX = Port number
;                Exit:   AX = Port status (see function 03h)
;
;    AL contains the character to be sent.   If there is room in the transmit
;    buffer the return will be immediate,  otherwise it will wait until there
;    is room to store the character in the transmit buffer.  On return, AX is
;    set as in a status request (see function 03h).
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:auxsendchar
;

@AuxSendChar	MACRO	PORT, CHAR
		MOV	DX, PORT
		IF TYPE (CHAR) EQ 1	; BYTES (1 BYTE)
			MOV	AX, 0100H+CHAR
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		@AUX_CALL	AX
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 02h    Receive character with wait
;
;            Parameters:
;                Entry:  DX = Port number
;                Exit:   AH = 00h
;                        AL = Input character
;
;    If there is a character  available in the  receive buffer,  returns with
;    the next character in AL.  It will wait until a character is received if
;    none is available.
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:auxrecievechar
;

@AuxRecieveChar	MACRO	PORT
		MOV	DX, PORT
		@AUX_CALL	02H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 03h    Request status
;
;            Parameters:
;                Entry:  DX = Port number
;                Exit:   AX = Status bit mask (see below)
;
;    Returns with the line and modem status in AX.  Status bits returned are:
;
;            In AH:
;                Bit 0 = RDA  - input data is available in buffer
;                Bit 1 = OVRN - the input buffer has been overrun.  All
;                               characters received after the buffer is
;                               full should be discarded.
;                Bit 5 = THRE - room is available in output buffer
;                Bit 6 = TSRE - output buffer is empty
;
;            In AL:
;                Bit 3 = Always 1 (always return with this bit set to 1)
;                Bit 7 = DCD  - carrier detect
;
; AH CONTAINS THE LINE STATUS
; BIT 7 1: TIME OUT
; BIT 6 = TRANS SHIFT REGISTER EMPTY
; BIT 5 = TRAN HOLDING REGISTER EMPTY
; BIT 4 = BREAK DETECT
; BIT 3 = FRAMING ERROR
; BIT 2 = PARITY ERROR
; BIT 1 = OVERRUN ERROR
; BIT 0 = DATA READY
; Al CONTAINS THE HeDEH STATUS
; BIT 7 1: RECEIVED LINE SIGNAL DETECT
; BIT 6 1: RING INDICATOR
; BIT 5 1: QATA SET READY
; BIT 4 1: CLEAR TO SEND
; BIT 3 =DELTA RECEIVE LINE SIGNAL DETECT
; BIT 2 = TRAILING EDGE RWG DETECTOR
; BIT 1 1: DELTA DATA SET READY
; BIT 0 1: DELTA CLEAR TO SEND
;
;    This can be used by the application to determine  whether carrier detect
;    (CD) is set,  signifying the presence/absence of a remote connection, as
;    well as monitoring both the input and output buffer status.  Bit 3 of AL
;    is always returned set to enable programs to use it as a carrier detect
;    bit on hardwired (null modem) links.
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:auxstatus
;

@AuxStatus	MACRO	PORT
		MOV	DX, PORT
		@AUX_CALL	03H
		ENDM

;=============================================================================
; Printer port BIOS Interrupts
;=============================================================================

@PRN_CALL	MACRO	FUNC
		IF (.TYPE (FUNC)) AND 00000100b ; Constant
			MOV	AH,FUNC
		ELSEIF TYPE (FUNC) EQ 1	; BYTES (1 BYTE)
			MOV	AH,FUNC
		ELSEIF TYPE (FUNC) EQ 2	; WORD (2 BYTE)
		IFDIFI	<FUNC>, <AX>
			MOV	AX,FUNC
		ENDIF
		ENDIF
		INT	17H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 00h    Print Char
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:prnprint
;

@PrnPrint	MACRO	PR, CHAR
		IF TYPE (CHAR) EQ 1	; BYTES (1 BYTE)
			MOV	AX, 0000H+CHAR
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		MOV	DX, PR
		PUSH	BX		; BIOS bug
		@PRN_CALL	AX
		POP	BX		; BIOS bug
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 01h    Initialize Printer
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:prninit
;

@PrnInit	MACRO	PR
		MOV	DX, PR
		@PRN_CALL	1
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 02h    Get printer status
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:prnstatus
;

@PrnStatus	MACRO	PR
		MOV	DX, PR
		@PRN_CALL	2
		ENDM

;=============================================================================
; Keyboard BIOS Interrupts
;=============================================================================

@KBD_CALL	MACRO	FUNC
		IF (.TYPE (FUNC)) AND 00000100b ; Constant
			MOV	AH,FUNC
		ELSEIF TYPE (FUNC) EQ 1	; BYTES (1 BYTE)
			MOV	AH,FUNC
		ELSEIF TYPE (FUNC) EQ 2	; WORD (2 BYTE)
		IFDIFI	<FUNC>, <AX>
			MOV	AX,FUNC
		ENDIF
		ENDIF
		INT	16H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 00h    Input char from keyboard
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:charin
;

@CharIn		MACRO
		@KBD_CALL	0
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 01h    Peek char from keyboard buffer
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:charpeek
;

@CharPeek	MACRO
		@KBD_CALL	1
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 02h    Keyboard Status
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:kbdstatus
;

@KbdStatus	MACRO
		@KBD_CALL	2
		ENDM


;-----------------------------------------------------------------------------
;
;    AH = 09h    Get Keyboard functions
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:getkbdfuncs
;

@GetKbdFuncs	MACRO
		@KBD_CALL	9
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Ah    Get Keyboard ID
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:getkbdid
;

@GetKbdId	MACRO
		@KBD_CALL	0AH
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 10h    Input char from keyboard (enhanced keyboard only)
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:charinenh
;

@CharInEnh	MACRO
		@KBD_CALL	10H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 11h    Peek char from keyboard buffer (enhanced keyboard only)
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:charpeekenh
;

@CharPeekEnh	MACRO
		@KBD_CALL	11H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 12h    Keyboard Status (enhanced keyboard only)
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:kbdstatusenh
;

@KbdStatusEnh	MACRO
		@KBD_CALL	12H
		ENDM


;=============================================================================
; Disk I/O BIOS Interrupts
;=============================================================================

@DSK_CALL	MACRO	FUNC
		IF (.TYPE (FUNC)) AND 00000100b ; Constant
			MOV	AH,FUNC
		ELSEIF TYPE (FUNC) EQ 1	; BYTES (1 BYTE)
			MOV	AH,FUNC
		ELSEIF TYPE (FUNC) EQ 2	; WORD (2 BYTE)
		IFDIFI	<FUNC>, <AX>
			MOV	AX,FUNC
		ENDIF
		ENDIF
		INT	13H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 00h    Reset disk
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:dskreset
;

@DskReset	MACRO
		@DSK_CALL	0
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 01h    Get disk status
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:dskstatus
;

@DskStatus	MACRO
		@DSK_CALL	1
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 02h    Read Sector
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:dskread
;

@DskRead	MACRO	DISK, HEAD, TRACK, SECTOR, SECTS, BUF
		IF TYPE (DISK) EQ 1	; BYTES (1 BYTE)
			MOV	DL, DISK
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (HEAD) EQ 1	; BYTES (1 BYTE)
			MOV	DH, HEAD
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (TRACK) EQ 1	; BYTES (1 BYTE)
			MOV	CH, TRACK
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (SECTOR) EQ 1	; BYTES (1 BYTE)
			MOV	CL, SECTOR
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (SECTS) EQ 1	; BYTES (1 BYTE)
			MOV	AX, 0200H+SECTS
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		LES	BX, BUF
		PUSH	DX		; BIOS bug
		STC			; BIOS bug
		@DSK_CALL	AX
		STI			; BIOS bug
		POP	DX		; BIOS bug
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 03h    Write Sector
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:dskwrite
;

@DskWrite	MACRO	DISK, HEAD, TRACK, SECTOR, SECTS, BUF
		IF TYPE (DISK) EQ 1	; BYTES (1 BYTE)
			MOV	DL, DISK
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (HEAD) EQ 1	; BYTES (1 BYTE)
			MOV	DH, HEAD
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (TRACK) EQ 1	; BYTES (1 BYTE)
			MOV	CH, TRACK
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (SECTOR) EQ 1	; BYTES (1 BYTE)
			MOV	CL, SECTOR
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (SECTS) EQ 1	; BYTES (1 BYTE)
			MOV	AX, 0300H+SECTS
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		LES	BX, BUF
		@DSK_CALL	AX
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 04h    Verify disk sector
;
; @todo: old bioses expect also buffer
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:dskverify
;

@DskVerify	MACRO	DISK, HEAD, TRACK, SECTOR, SECTS
		@DskReset		; BIOS bug
		IF TYPE (DISK) EQ 1	; BYTES (1 BYTE)
			MOV	DL, DISK
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (HEAD) EQ 1	; BYTES (1 BYTE)
			MOV	DH, HEAD
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (TRACK) EQ 1	; BYTES (1 BYTE)
			MOV	CH, TRACK
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (SECTOR) EQ 1	; BYTES (1 BYTE)
			MOV	CL, SECTOR
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (SECTS) EQ 1	; BYTES (1 BYTE)
			MOV	AX, 0400H+SECTS
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		@DSK_CALL	AX
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 05h    Format Track
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:formattrack
;

@DskFormat	MACRO	DISK, HEAD, TRACK, BUF
		IF TYPE (DISK) EQ 1	; BYTES (1 BYTE)
			MOV	DL, DISK
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (HEAD) EQ 1	; BYTES (1 BYTE)
			MOV	DH, HEAD
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		IF TYPE (TRACK) EQ 1	; BYTES (1 BYTE)
			MOV	CH, TRACK
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		LES	BX, BUF
		@DSK_CALL	5
		ENDM

;-----------------------------------------------------------------------------
;
;    Return Memory Size
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:memsize
;

@MemSize	MACRO
		INT	12H
		ENDM

;-----------------------------------------------------------------------------
;
;    Return Equipment Info 
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:equipment
;

@Equipment	MACRO
		INT	11H
		ENDM


@Tape		MACRO	FUNC
		IF (.TYPE (FUNC)) AND 00000100b ; Constant
			MOV	AH,FUNC
		ELSEIF TYPE (FUNC) EQ 1	; BYTES (1 BYTE)
			MOV	AH,FUNC
		ELSEIF TYPE (FUNC) EQ 2	; WORD (2 BYTE)
		IFDIFI	<FUNC>, <AX>
			MOV	AX,FUNC
		ENDIF
		ENDIF
		INT	15H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 00h    Turn Tape Motor On
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:tapeon
;

@TapeOn		MACRO
		@TAPE	0H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 01h    Turn Tape Motor Off
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:tapeoff
;

@TapeOff	MACRO
		@TAPE	1H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 02h    Read Block From Tape
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:taperead
;

@TapeRead	MACRO BUF, LEN
		LES	BX, BUF
		MOV	CX, LEN
		@TAPE	02H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 03h    Write Block To Tape
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:tapewrite
;

@TapeWrite	MACRO BUF, LEN
		LES	BX, BUF
		MOV	CX, LEN
		@TAPE	03H
		ENDM


;-----------------------------------------------------------------------------
;
;    AH = C0h    Get ROM Configuration table
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:getromconfig
;

@GetROMConfig	MACRO
		@TAPE	0C0H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 00h    Get Current Time
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:gettime
;

@GetTime	MACRO
		MOV	AH, 0H
		INT	1AH
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 01h    Set Current Time
;
;    Documentation: http://osfree.org/doku/en:docs:macrolib:settime
;

@SetTime	MACRO	COUNT
		MOV	CX, HIGHWORD COUNT
		MOV	DX, LOWWORD COUNT
		MOV	AH, 1H
		INT	1AH
		ENDM

ENDIF
