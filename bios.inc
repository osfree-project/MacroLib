;/*!
;   @file
;
;   @brief BIOS Macro Library
;
;   (c) osFree Project 2018-2022, <http://www.osFree.org>
;   for licence see licence.txt in root directory, or project website
;
;   @author Yuri Prokushev (yuri.prokushev@gmail.com)
;
;   This library based on IBM 5150 Technical Reference #6322507 of Apr 1984.
;   Functions, found in later BIOSes not covered. Macro names mostly constructed
;   from MASM 5.x documentation.
;
;*/

@vid		MACRO	FUNC
		MOV	AH,FUNC
		INT	10H
		ENDM

@SetMode	MACRO	MODE
		MOV	AL,MODE
		@VID	0H
		ENDM

@GetMode	MACRO
		@VID	15
		ENDM

@SetCurSz	MACRO	FIRST,LAST
		MOV	CH,FIRST
		MOV	CL,LAST
		@VID	01H
		ENDM

@SetCurPos	MACRO	COLUMN,ROW,PAGE
		MOV	DL,COLUMN
		MOV	DH,ROW
		MOV	BH,PAGE
		@VID	02H
		ENDM

@GetCur		MACRO	PAGE
		MOV	BH,PAGE
		@VID	03H
		ENDM

@SetPage	MACRO	PAGE
		MOV	AL,PAGE
		@VID	05H
		ENDM

@ScrollUp	MACRO	DISTANCE,ATRIB,UPCOL,UPROW,DOWNCOL,DOWNROW
		MOV	CL,UPCOL
		MOV	CH,UPROW
		MOV	DL,DOWNCOL
		MOV	DH,DOWNROW
		MOV	BH,ATRIB
		MOV	AL,DISTANCE
		@VID	06H
		ENDM

@ScrollDn	MACRO	DISTANCE,ATRIB,UPCOL,UPROW,DOWNCOL,DOWNROW
		MOV	CL,UPCOL
		MOV	CH,UPROW
		MOV	DL,DOWNCOL
		MOV	DH,DOWNROW
		MOV	BH,ATRIB
		MOV	AL,DISTANCE
		@VID	07H
		ENDM

@GetChAtr	MACRO	PAGE
		MOV	BH,PAGE
		@VID	08H
		ENDM

@PutChAtr	MACRO	CHAR,ATRIB,PAGE,REPEAT
		MOV	AL,CHAR
		MOV	BL,ATRIB
		MOV	BH,PAGE
		MOV	CX,REPEAT
		@VID	09H
		ENDM

@PutCh		MACRO	CHAR,ATRIB,PAGE,REPEAT
		MOV	AL,CHAR
		MOV	BL,ATRIB
		MOV	BH,PAGE
		MOV	CX,REPEAT
		@VID	0AH
		ENDM

@SetPalet	MACRO	COLOR
		MOV	BH,1
		MOV	BL,COLOR
		@VID	0BH
		ENDM

@SetColor	MACRO	COLOR
		SUB	BH,BH
		MOV	BL,COLOR
		@VID	0BH
		ENDM

@SetDot		MACRO	COLOR, ROW, COL
		MOV	DX, ROW
		MOV	CX, COL
		MOV	AL,COLOR
		@VID	0CH
		ENDM

@GetDot		MACRO	COLOR, ROW
		MOV	DX, ROW
		MOV	CX, COL
		@VID	0DH
		ENDM

@WrtTTY		MACRO	CHAR
		MOV	AL, CHAR
		@VID	0EH
		ENDM

@VideoState	MACRO
		@VID	0FH
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

COM_INIT	MACRO PORT, RATE
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

COM_SEND_CHAR	MACRO	PORT, CHAR
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

COM_RECIEVE_CHAR	MACRO	PORT
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

COM_STATUS	MACRO	PORT
		MOV	DX, PORT
		COM	03H
		ENDM

@Prn		MACRO	FUNC
		MOV	AH, FUNC
		INT	17H
		ENDM

@PrnPrint	MACRO	PR, CHAR
		MOV	AL, CHAR
		MOV	DX, PR
		@PRN	0
		ENDM

@PrnInit	MACRO	PR
		MOV	DX, PR
		@PRN	1
		ENDM

@PrnStatus	MACRO	PR
		MOV	DX, PR
		@PRN	2
		ENDM

@Kbd		MACRO	FUNC
		MOV	AH, FUNC
		INT	16H
		ENDM

@KbdCharIn	MACRO
		@KBD	0
		ENDM

@KbdPeek	MACRO
		@KBD	1
		ENDM

@KbdStatus	MACRO
		@KBD	2
		ENDM

@Dsk		MACRO	FUNC
		MOV	AH, FUNC
		INT	13H
		ENDM

@DskReset	MACRO
		@DSK	0
		ENDM

@DskStatus	MACRO
		@DSK	1
		ENDM

@DskRead	MACRO	DISK, HEAD, TRACK, SECTOR, SECTS, BUF
		MOV	DL, DISK
		MOV	DH, HEAD
		MOV	CH, TRACK
		MOV	CL, SECTOR
		MOV	AL, SECTS
		LES	BX, BUF
		@DSK	2
		ENDM

@DskWrite	MACRO	DISK, HEAD, TRACK, SECTOR, SECTS, BUF
		MOV	DL, DISK
		MOV	DH, HEAD
		MOV	CH, TRACK
		MOV	CL, SECTOR
		MOV	AL, SECTS
		LES	BX, BUF
		@DSK	3
		ENDM

@DskVerify	MACRO	DISK, HEAD, TRACK, SECTOR, SECTS
		MOV	DL, DISK
		MOV	DH, HEAD
		MOV	CH, TRACK
		MOV	CL, SECTOR
		MOV	AL, SECTS
		@DSK	4
		ENDM

@DskFormat	MACRO	DISK, HEAD, TRACK, BUF
		MOV	DL, DISK
		MOV	DH, HEAD
		MOV	CH, TRACK
		LES	BX, BUF
		@DSK	2
		ENDM

@MemSize	MACRO
		INT	12H
		ENDM

@Equipment	MACRO
		INT	11H
		ENDM


@Tape		MACRO	FUNC
		MOV	AH, FUNC
		INT	15H
		ENDM

@TapeOn		MACRO
		@TAPE	0H
		ENDM

@TapeOff	MACRO
		@TAPE	1H
		ENDM

@TapeRead	MACRO BUF, LEN
		LES	BX, BUF
		MOV	CX, LEN
		@TAPE	02H
		ENDM

@TapeWrite	MACRO BUF, LEN
		LES	BX, BUF
		MOV	CX, LEN
		@TAPE	03H
		ENDM

@GetTime	MACRO
		MOV	AH, 0H
		INT	1AH
		ENDM

@SetTime	MACRO	COUNT
		MOV	CX, HIGHWORD COUNT
		MOV	DX, LOWWORD COUNT
		MOV	AH, 1H
		INT	1AH
		ENDM

