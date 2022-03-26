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

@vid		MACRO	FUNC
		IF TYPE (FUNC) EQ 1	; BYTES (1 BYTE)
			MOV	AH,FUNC
		ELSEIF TYPE (FUNC) EQ 2	; WORD (2 BYTE)
			MOV	AX,FUNC
		ENDIF
		INT	10H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 00h    Set Video Mode
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:00
;

@SetMode	MACRO	MODE
		IF TYPE (MODE) EQ 1	; BYTES (1 BYTE)
			MOV	AX, WORD MODE
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		@VID	AX
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 01h    Set Cursor Shape Size
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:01
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
		@VID	01H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 02h    Set Cursor Position
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:02
;

@SetCurPos	MACRO	COLUMN,ROW,PAGE
		IFDIFI	<COLUMN>, <DL>
		IF TYPE (COLUMN) EQ 1	; BYTES (1 BYTE)
		MOV	DL,COLUMN
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		ENDIF
		IFDIFI	<ROW>, <DH>
		IF TYPE (ROW) EQ 1	; BYTES (1 BYTE)
		MOV	DH,ROW
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
		@VID	02H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 03h    Get Current Cursor Position and Size
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:03
;

@GetCur		MACRO	PAGE
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
		@VID	03H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 05h    Set Active Video Page
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:05
;

@SetPage	MACRO	PAGE
		IFNB	<PAGE>
		IF TYPE (PAGE) EQ 1	; BYTES (1 BYTE)
			MOV	AL,PAGE
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		ELSE
		SUB	AL, AL
		ENDIF
		@VID	05H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 06h    Scroll Screen Area Up
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:06
;

@ScrollUp	MACRO	DISTANCE,ATRIB,UPCOL,UPROW,DOWNCOL,DOWNROW
		PUSH	BP		; BIOS bug
		PUSH	DS		; BIOS bug
		MOV	CL,UPCOL
		MOV	CH,UPROW
		MOV	DL,DOWNCOL
		MOV	DH,DOWNROW
		MOV	BH,ATRIB
		MOV	AL,DISTANCE
		@VID	06H
		POP	DS		; BIOS bug
		POP	BP		; BIOS bug
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 07h    Scroll Screen Area Down
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:07
;

@ScrollDn	MACRO	DISTANCE,ATRIB,UPCOL,UPROW,DOWNCOL,DOWNROW
		PUSH	BP		; BIOS bug
		PUSH	DS		; BIOS bug
		MOV	CL,UPCOL
		MOV	CH,UPROW
		MOV	DL,DOWNCOL
		MOV	DH,DOWNROW
		MOV	BH,ATRIB
		MOV	AL,DISTANCE
		@VID	07H
		POP	DS		; BIOS bug
		POP	BP		; BIOS bug
		ENDM


@Scroll		MACRO	DISTANCE,ATRIB,UPCOL,UPROW,DOWNCOL,DOWNROW
		IF	distance LE 0
		@ScrollUp -DISTANCE,ATRIB,UPCOL,UPROW,DOWNCOL,DOWNROW
		ELSE
		@ScrollDn DISTANCE,ATRIB,UPCOL,UPROW,DOWNCOL,DOWNROW
		ENDIF
		ENDM

@Cls		MACRO
		@Scroll 24,07h,0,0,79,24
		ENDM
;-----------------------------------------------------------------------------
;
;    AH = 08h    Get Character and Attribute
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:08
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
		@VID	08H
		POP	BP		; BIOS bug
		POP	DI		; BIOS bug
		POP	SI		; BIOS bug
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 09h    Print Char With Attribute
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:09
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
		MOV	BL,ATRIB
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
		@VID	09H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Ah    Print Char
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:0A
;

@PutCh		MACRO	CHAR,ATRIB,PAGE,REPEAT
		IFDIFI	<CHAR>, <AL>
		MOV	AL,CHAR
		ENDIF
		IFNB	<ATRIB>
		MOV	BL,ATRIB
		ENDIF
		IFNB	<PAGE>
		MOV	BH,PAGE
		ELSE
		SUB	BH, BH
		ENDIF
		IFDIFI	<REPEAT>, <CX>
		MOV	CX,REPEAT
		ENDIF
		@VID	0AH
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Bh    Set Palette
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:0B
;

@SetPalet	MACRO	COLOR
		MOV	BH,1
		IF TYPE (COLOR) EQ 1	; BYTES (1 BYTE)
			MOV	BL,COLOR
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		@VID	0BH
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Bh    Set Color
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:0B
;

@SetColor	MACRO	COLOR
		SUB	BH,BH
		IF TYPE (COLOR) EQ 1	; BYTES (1 BYTE)
			MOV	BL,COLOR
		ELSE
			ECHO Bad argument
			.ERR
		ENDIF
		@VID	0BH
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Ch    Draw Pixel
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:0C
;

@SetDot		MACRO	COLOR, ROW, COL, PAGE
		IFNB	<PAGE>
		MOV	BH,PAGE
		ELSE
		SUB	BH, BH
		ENDIF
		MOV	DX, ROW
		MOV	CX, COL
		MOV	AL, COLOR
		@VID	0CH
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Dh    Get Pixel
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:0D
;

@GetDot		MACRO	COL, ROW, PAGE
		IFNB	<PAGE>
		MOV	BH,PAGE
		ELSE
		SUB	BH, BH
		ENDIF
		MOV	DX, ROW
		MOV	CX, COL
		@VID	0DH
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Eh    Write char in TTY mode
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:0E
;

@WrtTTY		MACRO	CHAR, PAGE
		IFNB	<PAGE>
		MOV	BH, PAGE
		ELSE
		SUB	BH, BH
		ENDIF
		PUSH	SI		; BIOS bug
		PUSH	DI		; BIOS bug
		PUSH	BP		; BIOS bug
		IFDIFI	<CHAR>, <AL>
		MOV	AL, CHAR
		ENDIF
		@VID	0EH
		POP	BP		; BIOS bug
		POP	DI		; BIOS bug
		POP	SI		; BIOS bug
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Fh    Return Video State
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:0F
;

@VideoState	MACRO
		@VID	0FH
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 0Fh    Get Current Video Mode
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:0F
;

@GetMode	MACRO
		@VID	0Fh
		ENDM
 
;-----------------------------------------------------------------------------
;
;    AH = 1Ah    Get Display Configuration
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:1A
;
 
@GetDisplay	MACRO
		XOR	AX, AX
		@VID	1Ah
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 1Ch    Get Display Configuration
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:1C
;

@GetVideoState	MACRO
		XOR	AX, AX
		@VID	1Ch
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 12h    Alternate functions
;
;    Documentation: http://osfree.org/doku/en:docs:bios:api:int10:12
;

@GetEGAInfo	MACRO
		MOV	AH,12H
		MOV	BL,10H
		INT	10H
		ENDM
		
COM		MACRO	FUNC
		MOV	AH, FUNC
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
;-----------------------------------------------------------------------------

@AuxInit	MACRO PORT, RATE
		MOV	DX, PORT
		MOV	AL, RATE
		COM	00H
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
;-----------------------------------------------------------------------------

@AuxSendChar	MACRO	PORT, CHAR
		MOV	DX, PORT
		MOV	AL, CHAR
		COM	01H
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
;-----------------------------------------------------------------------------

@AuxRecieveChar	MACRO	PORT
		MOV	DX, PORT
		COM	02H
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
;-----------------------------------------------------------------------------

@AuxStatus	MACRO	PORT
		MOV	DX, PORT
		COM	03H
		ENDM

@Prn		MACRO	FUNC
		MOV	AH, FUNC
		INT	17H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 00h    Print Char
;
;

@PrnPrint	MACRO	PR, CHAR
		MOV	AL, CHAR
		MOV	DX, PR
		@PRN	0
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 01h    Initialize Printer
;
;

@PrnInit	MACRO	PR
		MOV	DX, PR
		@PRN	1
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 02h    Get printer status
;
;
@PrnStatus	MACRO	PR
		MOV	DX, PR
		@PRN	2
		ENDM

@Kbd		MACRO	FUNC
		MOV	AH, FUNC
		INT	16H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 00h    Input char from keyboard
;
;

@CharIn		MACRO
		@KBD	0
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 01h    Peek char from keyboard buffer
;
;

@CharPeek	MACRO
		@KBD	1
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 02h    Keyboard Status
;
;

@KbdStatus	MACRO
		@KBD	2
		ENDM

@Dsk		MACRO	FUNC
		MOV	AH, FUNC
		INT	13H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 00h    Reset disk
;
;


@DskReset	MACRO
		@DSK	0
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 01h    Get disk status
;
;
@DskStatus	MACRO
		@DSK	1
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 02h    Read Sector
;
;
@DskRead	MACRO	DISK, HEAD, TRACK, SECTOR, SECTS, BUF
		MOV	DL, DISK
		MOV	DH, HEAD
		MOV	CH, TRACK
		MOV	CL, SECTOR
		MOV	AL, SECTS
		LES	BX, BUF
		@DSK	2
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 03h    Write Sector
;
;

@DskWrite	MACRO	DISK, HEAD, TRACK, SECTOR, SECTS, BUF
		MOV	DL, DISK
		MOV	DH, HEAD
		MOV	CH, TRACK
		MOV	CL, SECTOR
		MOV	AL, SECTS
		LES	BX, BUF
		@DSK	3
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 04h    Verify disk sector
;
;
@DskVerify	MACRO	DISK, HEAD, TRACK, SECTOR, SECTS
		MOV	DL, DISK
		MOV	DH, HEAD
		MOV	CH, TRACK
		MOV	CL, SECTOR
		MOV	AL, SECTS
		@DSK	4
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 05h    Format Track
;
;
@DskFormat	MACRO	DISK, HEAD, TRACK, BUF
		MOV	DL, DISK
		MOV	DH, HEAD
		MOV	CH, TRACK
		LES	BX, BUF
		@DSK	2
		ENDM

;-----------------------------------------------------------------------------
;
;    Return Memory Size
;
;

@MemSize	MACRO
		INT	12H
		ENDM

;-----------------------------------------------------------------------------
;
;    Return Equipment Info 
;
;
@Equipment	MACRO
		INT	11H
		ENDM


@Tape		MACRO	FUNC
		MOV	AH, FUNC
		INT	15H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 00h    Turn Tape Motor On
;
;

@TapeOn		MACRO
		@TAPE	0H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 01h    Turn Tape Motor Off
;
;
@TapeOff	MACRO
		@TAPE	1H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 02h    Read Block From Tape
;
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
;
@TapeWrite	MACRO BUF, LEN
		LES	BX, BUF
		MOV	CX, LEN
		@TAPE	03H
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 00h    Get Current Time
;
;

@GetTime	MACRO
		MOV	AH, 0H
		INT	1AH
		ENDM

;-----------------------------------------------------------------------------
;
;    AH = 01h    Set Current Time
;
;
@SetTime	MACRO	COUNT
		MOV	CX, HIGHWORD COUNT
		MOV	DX, LOWWORD COUNT
		MOV	AH, 1H
		INT	1AH
		ENDM

;@todo: @Scroll, @Cls
