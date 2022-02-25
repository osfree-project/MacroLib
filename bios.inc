;/*!
;   @file
;
;   @ingroup fapi
;
;   @brief BIOS Macro Library
;
;   (c) osFree Project 2018-2022, <http://www.osFree.org>
;   for licence see licence.txt in root directory, or project website
;
;   This is Family API implementation for DOS, used with BIND tools
;   to link required API
;
;   @author Yuri Prokushev (yuri.prokushev@gmail.com)
;
;   This library based on IBM 5150 Technical Reference #6322507 of Apr 1984.
;   Functions, found in later BIOSes not covered. Macro names mostly constructed
;   from MASM 5.x documentation.
;
;*/

@vid		macro	func
		mov     ah,func
		int	10h
		endm

@SetMode	macro	mode
		mov     al,mode
		@vid	0h
		endm

@GetMode	macro
		@vid	15
		endm

@SetCurSz	MACRO	first,last
		mov	ch,first
		mov	cl,last
		@vid	01h
		ENDM

@SetCurPos	MACRO	column,row,page
		mov	dl,column
		mov	dh,row
		mov	bh,page
		@vid	02h
		ENDM

@GetCur		MACRO	page
		mov	bh,page
		@vid	03h
		ENDM

@SetPage	MACRO	page
		mov	al,page
		@vid	05h
		ENDM

@ScrollUp	MACRO	distance,atrib,upcol,uprow,downcol,downrow
		mov	cl,upcol
		mov	ch,uprow
		mov	dl,downcol
		mov	dh,downrow
		mov	bh,atrib
		mov	al,distance
		@vid	06h
		ENDM

@ScrollDn	MACRO	distance,atrib,upcol,uprow,downcol,downrow
		mov	cl,upcol
		mov	ch,uprow
		mov	dl,downcol
		mov	dh,downrow
		mov	bh,atrib
		mov	al,distance
		@vid	07h
		ENDM

@GetChAtr	MACRO	page
		mov	bh,page
		@vid	08h
		ENDM

@PutChAtr	MACRO	char,atrib,page,repeat
		mov	al,char
		mov	bl,atrib
		mov	bh,page
		mov	cx,repeat
		@vid	09h
		ENDM

@PutCh		MACRO	char,atrib,page,repeat
		mov	al,char
		mov	bl,atrib
		mov	bh,page
		mov	cx,repeat
		@vid	0Ah
		ENDM

@SetPalet	MACRO	color
		mov	bh,1
		mov	bl,color
		@vid	0Bh
		ENDM

@SetColor	MACRO	color
		sub	bh,bh
		mov	bl,color
		@vid	0Bh
		ENDM

@SetDot		MACRO	color, Row, Col
		mov	dx, Row
		mov	cx, Col
		mov	al,color
		@vid	0Ch
		ENDM

@GetDot		MACRO	color, Row
		mov	dx, Row
		mov	cx, Col
		@vid	0dh
		ENDM

@WrtTTY		Macro	Char
		mov	al, char
		@vid	0eH
		EndM

@VideoState	Macro
		@vid	0fh
		EndM


COM			MACRO	FUNC
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

COM_STATUS		MACRO	PORT
			MOV	DX, PORT
			COM	03H
			ENDM

@Prn		MACRO	FUNC
		MOV	AH, FUNC
		INT	17H
		ENDM

@PrnPrint	Macro	Pr, Char
		Mov	Al, Char
		Mov	Dx, Pr
		@Prn	0
		EndM

@PrnInit	Macro	Pr
		Mov	Dx, Pr
		@Prn	1
		EndM

@PrnStatus	Macro	Pr
		Mov	Dx, Pr
		@Prn	2
		EndM

@Kbd		MACRO	FUNC
		MOV	AH, FUNC
		INT	16H
		ENDM

@KbdCharIn	Macro
		@Kbd	0
		EndM

@KbdPeek	Macro
		@Kbd	1
		EndM

@KbdStatus	Macro
		@Kbd	2
		EndM

@Dsk		MACRO	FUNC
		MOV	AH, FUNC
		INT	13H
		ENDM

@DskReset	Macro
		@Dsk	0
		EndM

@DskStatus	Macro
		@Dsk	1
		EndM

@DskRead	Macro	Disk, Head, Track, Sector, Sects, Buf
		Mov	Dl, Disk
		Mov	Dh, Head
		Mov	Ch, Track
		Mov	Cl, Sector
		Mov	Al, Sects
		Les	Bx, Buf
		@Dsk	2
		EndM

@DskWrite	Macro	Disk, Head, Track, Sector, Sects, Buf
		Mov	Dl, Disk
		Mov	Dh, Head
		Mov	Ch, Track
		Mov	Cl, Sector
		Mov	Al, Sects
		Les	Bx, Buf
		@Dsk	3
		EndM

@DskVerify	Macro	Disk, Head, Track, Sector, Sects
		Mov	Dl, Disk
		Mov	Dh, Head
		Mov	Ch, Track
		Mov	Cl, Sector
		Mov	Al, Sects
		@Dsk	4
		EndM

@DskFormat	Macro	Disk, Head, Track, Buf
		Mov	Dl, Disk
		Mov	Dh, Head
		Mov	Ch, Track
		Les	Bx, Buf
		@Dsk	2
		EndM

@MemSize	Macro
		Int	12H
		EndM

@Equipment	Macro
		Int	11h
		EndM


@Tape	Macro	func
	Mov	ah, func
	Int	15h
	EndM

@TapeOn		Macro
		@Tape	0h
		EndM

@TapeOff	Macro
		@Tape	1h
		EndM

@TapeRead	Macro Buf, Len
		Les	Bx, Buf
		Mov	Cx, Len
		@Tape	02h
		EndM

@TapeWrite	Macro Buf, Len
		Les	Bx, Buf
		Mov	Cx, Len
		@Tape	03h
		EndM

@GetTime	Macro
		Mov	Ah, 0h
		Int	1ah
		EndM

@SetTime	Macro	Count
		Mov	CX, HighWord Count
		Mov	DX, LowWord Count
		Mov	Ah, 1h
		Int	1ah
		EndM

