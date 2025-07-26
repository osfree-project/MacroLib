;/*!
;   @file
;
;   @brief DOS Macro Library
;
;   (c) osFree Project 2018-2022, <http://www.osFree.org>
;   for licence see licence.txt in root directory, or project website
;
;   @author Yuri Prokushev (yuri.prokushev@gmail.com)
;
;   This library based mostly on MASM 5.x and MASM 6.x documentation.
;
;   Documentation: 
;     Main reference: http://osfree.org/doku/en:docs:macrolib
;     RBIL (Ralf Brown's Interrupt List)
;
; This library provides macros for DOS API functions through INT 21h.
; Features:
; - Supports both standard (8.3) and LFN (long filename) operations
;
; Usage:
; 1. Include this file: INCLUDE dos.inc
; 2. Use macros instead of direct interrupt calls
;
; Naming Conventions:
; @ prefix  - Extended/helper macros
; LFN_ prefix - Long filename support
; @Vdm* - Universal macro for LFN and SFN support
;
; Define LFNAPI word variable to control LDN support on or off
;
;-----------------------------------------------------------------------------
;*/


IFNDEF DOS_DEFINED

DOS_DEFINED EQU 1

.8086

		INCLUDE	sysmaca.inc

;-----------------------------------------------------------------------------
; @DosCall - Core DOS function dispatcher
; Input: FUNC - Function number (constant or register)
;
; RBIL: INT 21h - DOS Function Dispatcher
;   AH/AX = function number. Preserves all registers except explicit returns.
;   Carry flag indicates error on most functions.
;-----------------------------------------------------------------------------
@DosCall	MACRO	FUNC
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
		INT	21H
		ENDM

;-----------------------------------------------------------------------------
; TERMINATE - Program termination (INT 20h)
;
; RBIL: INT 20h - TERMINATE PROGRAM
;   Exits to DOS. Requires CS = PSP segment. Prefer INT 21h/AH=4Ch for
;   greater flexibility. 
;   Reference: http://osfree.org/doku/en:docs:macrolib:terminate
;-----------------------------------------------------------------------------
TERMINATE	MACRO
		INT	20H
		ENDM

;-----------------------------------------------------------------------------
; FLUSH - Commit file to disk
; Input: HANDLE - File handle
;
; RBIL: INT 21h/AH=68h - COMMIT FILE (DOS 3.3+)
;   Forces write of buffered data to disk. 
;   Input: BX = file handle
;   Output: CF set on error, AX = error code
;   Reference: http://osfree.org/doku/en:docs:macrolib:flush
;-----------------------------------------------------------------------------
FLUSH		MACRO	HANDLE
		MOV	BX, HANDLE
		@DosCall 68H
		ENDM

;-----------------------------------------------------------------------------
; FILE_UNLOCK - Unlock file region
; Input: 
;   HANDLE - File handle
;   START  - Starting offset (32-bit)
;   BYTES  - Region size (32-bit)
;
; RBIL: INT 21h/AH=5Ch/AL=01h - UNLOCK FILE REGION
;   Releases a previously locked region. Requires SHARE.EXE loaded.
;   Input: BX=handle, CX:DX=offset, SI:DI=length
;   Output: CF set on error, AX = error code
;   Reference: http://osfree.org/doku/en:docs:macrolib:file_unlock
;-----------------------------------------------------------------------------
FILE_UNLOCK	MACRO	HANDLE,START,BYTES
		MOV	BX, HANDLE
		MOV	CX, WORD PTR START
		MOV	DX, WORD PTR START+2
		MOV	SI, WORD PTR BYTES
		MOV	DI, WORD PTR BYTES+2
		MOV	AL, 1
		@DosCall 5CH
		ENDM

;-----------------------------------------------------------------------------
; ABS_DISK_READ - Absolute disk read (INT 25h)
; Input:
;   DISK         - Drive (0=A, 1=B, ...)
;   BUFFER       - Data buffer
;   NUM_SECTORS  - Sectors to read
;   FIRST_SECTOR - Starting sector
;
; RBIL: INT 25h - ABSOLUTE DISK READ
;   Reads sectors directly. Pop flags after call!
;   Input: AL=drive, CX=count, DX=start, DS:BX=buffer
;   Output: CF set on error, error code in AL
;   Reference: http://osfree.org/doku/en:docs:macrolib:abs_disk_read
;-----------------------------------------------------------------------------
ABS_DISK_READ	MACRO	DISK,BUFFER,NUM_SECTORS,FIRST_SECTOR
		MOV	AL,DISK
		MOV	BX,OFFSET BUFFER
		MOV	CX,NUM_SECTORS
		MOV	DX,FIRST_SECTOR
		INT	25H
		POPF
		ENDM

;-----------------------------------------------------------------------------
; ABS_DISK_WRITE - Absolute disk write (INT 26h)
; Input: Same as ABS_DISK_READ
;
; RBIL: INT 26h - ABSOLUTE DISK WRITE
;   Writes sectors directly. Pop flags after call!
;   Input: AL=drive, CX=count, DX=start, DS:BX=buffer
;   Output: CF set on error, error code in AL
;   Reference: http://osfree.org/doku/en:docs:macrolib:abs_disk_write
;-----------------------------------------------------------------------------
ABS_DISK_WRITE	MACRO	DISK,BUFFER,NUM_SECTORS,FIRST_SECTOR
		MOV	AL,DISK
		MOV	BX,OFFSET BUFFER
		MOV	CX,NUM_SECTORS
		MOV	DX,FIRST_SECTOR
		INT	26H
		POPF
		ENDM


;-----------------------------------------------------------------------------
; STAY_RESIDENT - Terminate and stay resident (INT 27h)
; Input: LAST_INSTRUC - Last instruction of resident portion
;
; RBIL: INT 27h - TERMINATE AND STAY RESIDENT
;   Leaves program resident. DX = last resident address+1.
;   Prefer INT 21h/AH=31h for greater control.
;   Reference: http://osfree.org/doku/en:docs:macrolib:stay_resident
;-----------------------------------------------------------------------------
STAY_RESIDENT	MACRO	LAST_INSTRUC
		MOV	DX,OFFSET LAST_INSTRUC
		INC	DX
		INT	27H
		ENDM

;-----------------------------------------------------------------------------
; TERMINATE_PROGRAM - Terminate program (00h)
;
; RBIL: INT 21h/AH=00h - TERMINATE PROGRAM
;   Exits to DOS. Returns to parent process. Prefer AH=4Ch.
;   Reference: http://osfree.org/doku/en:docs:macrolib:terminate_program
;-----------------------------------------------------------------------------
TERMINATE_PROGRAM	MACRO
		@DosCall 00H
		ENDM

;-----------------------------------------------------------------------------
; READ_KBD_AND_ECHO - Read character with echo (01h)
; Output: AL = Character
;
; RBIL: INT 21h/AH=01h - READ CHARACTER FROM STANDARD INPUT WITH ECHO
;   Checks for Ctrl-C. Input is redirected if handle redirected.
;   Reference: http://osfree.org/doku/en:docs:macrolib:read_kbd_and_echo
;-----------------------------------------------------------------------------
READ_KBD_AND_ECHO	MACRO
		@DosCall 01H
		ENDM

;-----------------------------------------------------------------------------
; DISPLAY_CHAR - Display character (02h)
; Input: CHARACTER - Character to display
;
; RBIL: INT 21h/AH=02h - WRITE CHARACTER TO STANDARD OUTPUT
;   Outputs DL to stdout. Checks for Ctrl-C.
;   Reference: http://osfree.org/doku/en:docs:macrolib:display_char
;-----------------------------------------------------------------------------
DISPLAY_CHAR	MACRO	CHARACTER
		MOV	DL,CHARACTER
		@DosCall 02H
		ENDM

;-----------------------------------------------------------------------------
; @DispCh - Display one or more characters
; Input: CHARACTER - Character(s) to display
;-----------------------------------------------------------------------------
@DispCh		MACRO	CHARACTER: VARARG
		MOV	AH, 02H
		IFNB <CHARACTER>
			FOR	ARG, <CHARACTER>
			MOV	DL, ARG
			INT	21H
			ENDM
		ELSE
			@DosCall 02h
		ENDIF
		ENDM

;-----------------------------------------------------------------------------
; AUX_INPUT - Read from AUX device (03h)
; Output: AL = Character
;
; RBIL: INT 21h/AH=03h - AUXILIARY INPUT
;   Reads from first serial port (COM1). Can be redirected.
;   Reference: http://osfree.org/doku/en:docs:macrolib:aux_input
;-----------------------------------------------------------------------------
AUX_INPUT	MACRO
		@DosCall 03H
		ENDM

;-----------------------------------------------------------------------------
; AUX_OUTPUT - Write to AUX device (04h)
; Input: CHARACTER - Character to write
;
; RBIL: INT 21h/AH=04h - AUXILIARY OUTPUT
;   Writes DL to first serial port (COM1). Can be redirected.
;   Reference: http://osfree.org/doku/en:docs:macrolib:aux_output
;-----------------------------------------------------------------------------
AUX_OUTPUT	MACRO
		@DosCall 04H
		ENDM

;-----------------------------------------------------------------------------
; PRINT_CHAR - Print character (05h)
; Input: CHARACTER - Character to print
;
; RBIL: INT 21h/AH=05h - PRINTER OUTPUT
;   Outputs DL to printer (LPT1). Can be redirected.
;   Reference: http://osfree.org/doku/en:docs:macrolib:print_char
;-----------------------------------------------------------------------------
PRINT_CHAR	MACRO	CHARACTER
		MOV	DL,CHARACTER
		@DosCall 05H
		ENDM

;-----------------------------------------------------------------------------
; @PrtCh - Print one or more characters
; Input: CHARACTER - Character(s) to print
;-----------------------------------------------------------------------------
@PrtCh	MACRO	CHARACTER: VARARG
		MOV	AH, 05H
		FOR	ARG, <CHARACTER>
		MOV	DL, ARG
		INT	21H
		ENDM
		ENDM

;-----------------------------------------------------------------------------
; DIR_CONSOLE_IO - Direct console I/O (06h)
; Input: SWITCH - 0FFh for input, else character to output
; Output: ZF=1 if no char, AL=char if ZF=0 (on input)
;
; RBIL: INT 21h/AH=06h - DIRECT CONSOLE I/O
;   For input: DL=0FFh, returns char if available (does not wait)
;   For output: DL=char, does not check Ctrl-C
;   Reference: http://osfree.org/doku/en:docs:macrolib:dir_console_io
;-----------------------------------------------------------------------------
DIR_CONSOLE_IO	MACRO	SWITCH
		MOV	DL,SWITCH
		@DosCall 06H
		ENDM

;-----------------------------------------------------------------------------
; DIR_CONSOLE_INPUT - Direct console input without echo (07h)
; Output: AL = Character
;
; RBIL: INT 21h/AH=07h - DIRECT CHARACTER INPUT WITHOUT ECHO
;   Waits for character. No Ctrl-C check. Can be redirected.
;   Reference: http://osfree.org/doku/en:docs:macrolib:dir_console_input
;-----------------------------------------------------------------------------
DIR_CONSOLE_INPUT	MACRO
		@DosCall 07H
		ENDM

;-----------------------------------------------------------------------------
; READ_KBD - Read keyboard without echo (08h)
; Output: AL = Character
;
; RBIL: INT 21h/AH=08h - CONSOLE INPUT WITHOUT ECHO
;   Waits for character. Checks Ctrl-C. Can be redirected.
;   Reference: http://osfree.org/doku/en:docs:macrolib:read_kbd
;-----------------------------------------------------------------------------
READ_KBD	MACRO
		@DosCall 08H
		ENDM

;-----------------------------------------------------------------------------
; DISPLAY - Display string (09h)
; Input: 
;   STRING - '$'-terminated string
;   SEGMNT - Optional segment of string
;
; RBIL: INT 21h/AH=09h - DISPLAY STRING
;   Outputs string at DS:DX to stdout. '$' terminator required.
;   Reference: http://osfree.org/doku/en:docs:macrolib:display
;-----------------------------------------------------------------------------
DISPLAY		MACRO	STRING, SEGMNT
		IFDIFI	<STRING>, <DX>
			@LOADARG	DX, STRING
		ENDIF
		IFNB	<SEGMNT>
			MOV	AX, <SEGMNT>
			MOV	DS, AX
		ENDIF
		@DosCall 09H
		ENDM

;-----------------------------------------------------------------------------
; @DispStr - Alias for DISPLAY
;-----------------------------------------------------------------------------
@DispStr	MACRO STRING, SEGMNT
		IFDIFI	<STRING>, <DX>
			@LOADARG	DX, STRING
		ENDIF
		IFNB	<SEGMNT>
			MOV	AX, <SEGMNT>
			MOV	DS, AX
		ENDIF
		@DosCall 09H
		ENDM

;-----------------------------------------------------------------------------
; GET_STRING - Buffered keyboard input (0Ah)
; Input:
;   LIMIT  - Maximum length
;   STRING - Input buffer (format: [0]=max, [1]=actual, [2...]=chars)
;
; RBIL: INT 21h/AH=0Ah - BUFFERED INPUT
;   Reads from stdin. Buffer format: [0]=size, [1]=chars read, [2..]=data
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_string
;-----------------------------------------------------------------------------
GET_STRING	MACRO	LIMIT,STRING
		MOV	DX,OFFSET STRING
		MOV	STRING,LIMIT
		@DosCall 0AH
		ENDM

;-----------------------------------------------------------------------------
; @GetStr - Extended string input
; Input:
;   OFSET      - Buffer offset
;   TERMINATOR - String terminator
;   LIMIT      - Maximum length
;   SEGMNT     - Buffer segment
;-----------------------------------------------------------------------------
@GetStr		MACRO	OFSET, TERMINATOR, LIMIT, SEGMNT
		IFNB	<SEGMNT>
			MOV	AX, <SEGMNT>
			MOV	DS, AX
		ENDIF
		MOV	DX,OFFSET <OFSET>
		MOV	SI, DX
		IFNB	<LIMIT>
			MOV	STRING,LIMIT
		ENDIF
		@DosCall 0AH
		INC	SI
		MOV	BL, [SI]
		SUB	BH, BH
		INC	SI
		IFNB	TERMINATOR
			MOV	[BX+SI], TERMINATOR
		ENDIF
		ENDM

;-----------------------------------------------------------------------------
; CHECK_KBD_STATUS - Check keyboard status (0Bh)
; Output: AL = 0FFh if key available, 00h if no key
;
; RBIL: INT 21h/AH=0Bh - GET STDIN STATUS
;   Checks if character available. Can be redirected.
;   Reference: http://osfree.org/doku/en:docs:macrolib:check_kbd_status
;-----------------------------------------------------------------------------
CHECK_KBD_STATUS	MACRO
		@DosCall 0BH
		ENDM

;-----------------------------------------------------------------------------
; FLUSH_AND_READ_KBD - Flush buffer and read keyboard (0Ch)
; Input: SWITCH - Input mode (same as DIR_CONSOLE_IO)
;
; RBIL: INT 21h/AH=0Ch - FLUSH BUFFER AND READ STANDARD INPUT
;   Clears buffer then executes input function in AL
;   Reference: http://osfree.org/doku/en:docs:macrolib:flush_and_read_kbd
;-----------------------------------------------------------------------------
FLUSH_AND_READ_KBD	MACRO	SWITCH
		MOV	AL,SWITCH
		@DosCall 0CH
		ENDM

;-----------------------------------------------------------------------------
; RESET_DISK - Reset disk system (0Dh)
;
; RBIL: INT 21h/AH=0Dh - DISK RESET
;   Flushes file buffers. Does not update directory info.
;   Reference: http://osfree.org/doku/en:docs:macrolib:reset_disk
;-----------------------------------------------------------------------------
RESET_DISK	MACRO
		@DosCall 0DH
		ENDM

;-----------------------------------------------------------------------------
; LFN_RESET_DISK - Reset disk for LFN (710Dh)
; Input: DISK - Drive number
;
; RBIL: INT 21h/AX=710Dh - LONG FILENAME DISK RESET (DOS 7+)
;   CX=0002h: Reset all drives. DX=drive (0=default, 1=A, etc.)
;   Reference: http://osfree.org/doku/en:docs:macrolib:lfn_reset_disk
;-----------------------------------------------------------------------------
LFN_RESET_DISK	MACRO	DISK
		MOV	AX, 710DH
		MOV	CX, 2
		MOV	DX, DISK
		INT	21H
		ENDM

;-----------------------------------------------------------------------------
; SELECT_DISK - Set default drive (0Eh)
; Input: DISK - Drive letter ('A','B',etc)
; Output: AL = Number of logical drives
;
; RBIL: INT 21h/AH=0Eh - SELECT DEFAULT DRIVE
;   DL=drive (0=A, 1=B, ...). Returns AL=number of logical drives.
;   Reference: http://osfree.org/doku/en:docs:macrolib:select_disk
;-----------------------------------------------------------------------------
SELECT_DISK	MACRO	DISK
		MOV	DL,DISK
		SUB	DL, 65		; 'A'
		@DosCall 0EH
		INT	21H
		ENDM

;-----------------------------------------------------------------------------
; @SetDrv - Set default drive (0Eh)
; Input: DRIVE - Drive number (0=A, 1=B, ...)
; Output: AL = Number of logical drives
;-----------------------------------------------------------------------------
@SetDrv		MACRO	DRIVE
		MOV	DL,DRIVE
		@DosCall 0EH
		ENDM

;-----------------------------------------------------------------------------
; OPEN - Open file via FCB (0Fh)
; Input: FCB - File Control Block
; Output: AL = 0 on success, FFh on error
;
; RBIL: INT 21h/AH=0Fh - OPEN FILE USING FCB
;   Opens file matching FCB. DS:DX points to unopened FCB.
;   Reference: http://osfree.org/doku/en:docs:macrolib:open
;-----------------------------------------------------------------------------
OPEN		MACRO	FCB
		MOV	DX,OFFSET FCB
		@DosCall 0FH
		ENDM

;-----------------------------------------------------------------------------
; CLOSE - Close file via FCB (10h)
; Input: FCB - File Control Block
; Output: AL = 0 on success, FFh on error
;
; RBIL: INT 21h/AH=10h - CLOSE FILE USING FCB
;   Closes file associated with FCB. DS:DX points to opened FCB.
;   Reference: http://osfree.org/doku/en:docs:macrolib:close
;-----------------------------------------------------------------------------
CLOSE		MACRO	FCB
		MOV	DX,OFFSET FCB
		@DosCall 10H
		ENDM

;-----------------------------------------------------------------------------
; SEARCH_FIRST - Find first file via FCB (11h)
; Input: FCB - File Control Block
; Output: AL = 0 on success, FFh on error
;
; RBIL: INT 21h/AH=11h - FIND FIRST MATCHING FILE USING FCB
;   Searches for first match. DS:DX points to unopened FCB.
;   Reference: http://osfree.org/doku/en:docs:macrolib:search_first
;-----------------------------------------------------------------------------
SEARCH_FIRST	MACRO	FCB
		MOV	DX,OFFSET FCB
		@DosCall 11H
		ENDM

;-----------------------------------------------------------------------------
; SEARCH_NEXT - Find next file via FCB (12h)
; Input: FCB - File Control Block
; Output: AL = 0 on success, FFh on error
;
; RBIL: INT 21h/AH=12h - FIND NEXT MATCHING FILE USING FCB
;   Continues search from previous FIND FIRST. DS:DX points to FCB.
;   Reference: http://osfree.org/doku/en:docs:macrolib:search_next
;-----------------------------------------------------------------------------
SEARCH_NEXT	MACRO	FCB
		MOV	DX,OFFSET FCB
		@DosCall 12H
		ENDM

;-----------------------------------------------------------------------------
; DELETE - Delete file via FCB (13h)
; Input: FCB - File Control Block
; Output: AL = 0 on success, FFh on error
;
; RBIL: INT 21h/AH=13h - DELETE FILE USING FCB
;   Deletes files matching FCB. DS:DX points to unopened FCB.
;   Reference: http://osfree.org/doku/en:docs:macrolib:delete
;-----------------------------------------------------------------------------
DELETE		MACRO	FCB
		MOV	DX,OFFSET FCB
		@DosCall 13H
		ENDM

;-----------------------------------------------------------------------------
; READ_SEQ - Sequential read via FCB (14h)
; Input: FCB - File Control Block
; Output: AL = 0 success, 1 EOF, 3 partial record
;
; RBIL: INT 21h/AH=14h - SEQUENTIAL READ FROM FILE USING FCB
;   Reads next record to DTA. DS:DX points to opened FCB.
;   Reference: http://osfree.org/doku/en:docs:macrolib:read_seq
;-----------------------------------------------------------------------------
READ_SEQ	MACRO	FCB
		MOV	DX,OFFSET FCB
		@DosCall 14H
		ENDM

;-----------------------------------------------------------------------------
; WRITE_SEQ - Sequential write via FCB (15h)
; Input: FCB - File Control Block
; Output: AL = 0 success, 1 disk full, 2 DTA overflow
;
; RBIL: INT 21h/AH=15h - SEQUENTIAL WRITE TO FILE USING FCB
;   Writes next record from DTA. DS:DX points to opened FCB.
;   Reference: http://osfree.org/doku/en:docs:macrolib:write_seq
;-----------------------------------------------------------------------------
WRITE_SEQ	MACRO	FCB
		MOV	DX,OFFSET FCB
		@DosCall 15H
		ENDM

;-----------------------------------------------------------------------------
; CREATE - Create file via FCB (16h)
; Input: FCB - File Control Block
; Output: AL = 0 on success, FFh on error
;
; RBIL: INT 21h/AH=16h - CREATE OR TRUNCATE FILE USING FCB
;   Creates/truncates file matching FCB. DS:DX points to unopened FCB.
;   Reference: http://osfree.org/doku/en:docs:macrolib:create
;-----------------------------------------------------------------------------
CREATE		MACRO	FCB
		MOV	DX,OFFSET FCB
		@DosCall 16H
		ENDM

;-----------------------------------------------------------------------------
; RENAME - Rename file via FCB (17h)
; Input: 
;   FCB     - File Control Block
;   NEWNAME - New filename
; Output: AL = 0 on success, FFh on error
;
; RBIL: INT 21h/AH=17h - RENAME FILE USING FCB
;   Renames files matching FCB. DS:DX points to modified FCB.
;   Reference: http://osfree.org/doku/en:docs:macrolib:rename
;-----------------------------------------------------------------------------
RENAME		MACRO	FCB,NEWNAME
		MOV	DX,OFFSET FCB
		@DosCall 17H
		ENDM

;-----------------------------------------------------------------------------
; CURRENT_DISK - Get current drive (19h)
; Output: AL = Drive number (0=A, 1=B, ...)
;
; RBIL: INT 21h/AH=19h - GET CURRENT DEFAULT DRIVE
;   Returns current drive. 0=A, 1=B, etc.
;   Reference: http://osfree.org/doku/en:docs:macrolib:current_disk
;-----------------------------------------------------------------------------
CURRENT_DISK	MACRO
		@DosCall 19H
		ENDM

;-----------------------------------------------------------------------------
; @GetDrv - Get current drive (19h)
; Output: AL = Current drive number
;-----------------------------------------------------------------------------
@GetDrv		MACRO
		@DosCall 19H
		ENDM

;-----------------------------------------------------------------------------
; SET_DTA - Set Disk Transfer Area (1Ah)
; Input: BUFFER - DTA address
;
; RBIL: INT 21h/AH=1Ah - SET DISK TRANSFER AREA ADDRESS
;   Sets DS:DX as new DTA for FCB operations.
;   Reference: http://osfree.org/doku/en:docs:macrolib:set_dta
;-----------------------------------------------------------------------------
SET_DTA		MACRO	BUFFER
		MOV	DX,OFFSET BUFFER
		@DosCall 1AH
		ENDM

;-----------------------------------------------------------------------------
; @SetDTA - Set DTA with segment option
; Input:
;   BUFFER - Buffer address
;   SEGMNT - Buffer segment (optional)
;-----------------------------------------------------------------------------
@SetDTA		MACRO BUFFER, SEGMNT
		IFNB <SEGMNT>
			MOV AX, SEGMNT
			MOV DS, AX
		ENDIF
		IFNB <BUFFER>
			MOV	DX,OFFSET BUFFER
		ENDIF
		@DosCall 1AH
		ENDM

;-----------------------------------------------------------------------------
; DEF_DRIVE_DATA - Get default drive data (1Bh)
; Output: 
;   DS:BX - Drive parameter block address
;   DX    - Clusters per drive
;   AL    - Sectors per cluster
;   CX    - Bytes per sector
;
; RBIL: INT 21h/AH=1Bh - GET ALLOCATION INFORMATION FOR DEFAULT DRIVE
;   Returns info for current drive. Deprecated - use 36h instead.
;   Reference: http://osfree.org/doku/en:docs:macrolib:def_drive_data
;-----------------------------------------------------------------------------
DEF_DRIVE_DATA	MACRO
		@DosCall 1BH
		ENDM

;-----------------------------------------------------------------------------
; DRIVE_DATA - Get drive data (1Ch)
; Input: DRIVE - Drive number (0=default, 1=A, 2=B, ...)
; Output: Same as DEF_DRIVE_DATA
;
; RBIL: INT 21h/AH=1Ch - GET ALLOCATION INFORMATION FOR SPECIFIC DRIVE
;   Deprecated - use 36h instead.
;   Reference: http://osfree.org/doku/en:docs:macrolib:drive_data
;-----------------------------------------------------------------------------
DRIVE_DATA	MACRO	DRIVE
		MOV	DL,DRIVE
		@DosCall 1CH
		ENDM

;-----------------------------------------------------------------------------
; READ_RAN - Random read via FCB (21h)
; Input: FCB - File Control Block
; Output: AL = 0 success, 1 EOF, 3 partial record
;
; RBIL: INT 21h/AH=21h - RANDOM READ FROM FILE USING FCB
;   Reads record at current position. DS:DX points to opened FCB.
;   Reference: http://osfree.org/doku/en:docs:macrolib:read_ran
;-----------------------------------------------------------------------------
READ_RAN	MACRO	FCB
		MOV	DX,OFFSET FCB
		@DosCall 21H
		ENDM

;-----------------------------------------------------------------------------
; WRITE_RAN - Random write via FCB (22h)
; Input: FCB - File Control Block
; Output: AL = 0 success, 1 disk full, 2 DTA overflow
;
; RBIL: INT 21h/AH=22h - RANDOM WRITE TO FILE USING FCB
;   Writes record at current position. DS:DX points to opened FCB.
;   Reference: http://osfree.org/doku/en:docs:macrolib:write_ran
;-----------------------------------------------------------------------------
WRITE_RAN	MACRO	FCB
		MOV	DX,OFFSET FCB
		@DosCall 22H
		ENDM

;-----------------------------------------------------------------------------
; FILE_SIZE - Get file size via FCB (23h)
; Input: FCB - File Control Block
; Output: AL = 0 on success, FFh on error
;
; RBIL: INT 21h/AH=23h - GET FILE SIZE USING FCB
;   Sets random record field in FCB to file size. DS:DX points to unopened FCB.
;   Reference: http://osfree.org/doku/en:docs:macrolib:file_size
;-----------------------------------------------------------------------------
FILE_SIZE	MACRO	FCB
		MOV	DX,OFFSET FCB
		@DosCall 23H
		ENDM

;-----------------------------------------------------------------------------
; SET_RELATIVE_RECORD - Set relative record for FCB (24h)
; Input: FCB - File Control Block
;
; RBIL: INT 21h/AH=24h - SET RELATIVE RECORD FIELD
;   Sets current file position based on random record field. DS:DX=opened FCB.
;   Reference: http://osfree.org/doku/en:docs:macrolib:set_relative_record
;-----------------------------------------------------------------------------
SET_RELATIVE_RECORD	MACRO	FCB
		MOV	DX,OFFSET FCB
		@DosCall 24H
		ENDM

;-----------------------------------------------------------------------------
; SET_VECTOR - Set interrupt vector (25h)
; Input:
;   INTERRUPT     - Interrupt number
;   HANDLER_START - Handler address
;
; RBIL: INT 21h/AH=25h - SET INTERRUPT VECTOR
;   Sets vector for interrupt AL to DS:DX.
;   Reference: http://osfree.org/doku/en:docs:macrolib:set_vector
;-----------------------------------------------------------------------------
SET_VECTOR	MACRO	INTERRUPT,HANDLER_START
		MOV	AL,INTERRUPT
		MOV	DX,OFFSET HANDLER_START
		@DosCall 25H
		ENDM

;-----------------------------------------------------------------------------
; @SetInt - Set interrupt vector with segment option
; Input:
;   INTERRUPT - Interrupt number
;   VECTOR    - Handler address
;   SEGMNT    - Handler segment (optional)
;-----------------------------------------------------------------------------
@SetInt		MACRO	INTERRUPT, VECTOR, SEGMNT
		IFNB	<SEGMNT>
		MOV	AX, <SEGMNT>
		MOV	DS, AX
		ENDIF
		MOV	AL,INTERRUPT
		MOV	DX,OFFSET VECTOR
		@DosCall 25H
		ENDM

;-----------------------------------------------------------------------------
; CREATE_PSP - Create Program Segment Prefix (26h)
; Input: SEG_ADDR - Segment for new PSP
;
; RBIL: INT 21h/AH=26h - CREATE PROGRAM SEGMENT PREFIX
;   Creates PSP at DX. Deprecated - use EXEC instead.
;   Reference: http://osfree.org/doku/en:docs:macrolib:create_psp
;-----------------------------------------------------------------------------
CREATE_PSP	MACRO	SEG_ADDR
		MOV	DX,OFFSET SEG_ADDR
		@DosCall 26H
		ENDM

;-----------------------------------------------------------------------------
; RAN_BLOCK_READ - Random block read via FCB (27h)
; Input:
;   FCB      - File Control Block
;   COUNT    - Records to read
;   REC_SIZE - Record size
; Output: AL = error code, CX = actual records read
;
; RBIL: INT 21h/AH=27h - RANDOM BLOCK READ FROM FILE USING FCB
;   Reads multiple records. DS:DX=opened FCB, CX=record count.
;   Reference: http://osfree.org/doku/en:docs:macrolib:ran_block_read
;-----------------------------------------------------------------------------
RAN_BLOCK_READ	MACRO	FCB,COUNT,REC_SIZE
		MOV	DX,OFFSET FCB
		MOV	CX,COUNT
		MOV	WORD PTR FCB[14],REC_SIZE
		@DosCall 27H
		ENDM

;-----------------------------------------------------------------------------
; RAN_BLOCK_WRITE - Random block write via FCB (28h)
; Input: Same as RAN_BLOCK_READ
; Output: AL = error code, CX = actual records written
;
; RBIL: INT 21h/AH=28h - RANDOM BLOCK WRITE TO FILE USING FCB
;   Writes multiple records. DS:DX=opened FCB, CX=record count.
;   Reference: http://osfree.org/doku/en:docs:macrolib:ran_block_write
;-----------------------------------------------------------------------------
RAN_BLOCK_WRITE	MACRO	FCB,COUNT,REC_SIZE
		MOV	DX,OFFSET FCB
		MOV	CX,COUNT
		MOV	WORD PTR FCB[14],REC_SIZE
		@DosCall 28H
		ENDM

;-----------------------------------------------------------------------------
; PARSE - Parse filename into FCB (29h)
; Input:
;   STRING - Filename string
;   FCB    - File Control Block
;
; RBIL: INT 21h/AH=29h - PARSE FILENAME INTO FCB
;   Parses DS:SI string into FCB at ES:DI. AL=control flags.
;   Reference: http://osfree.org/doku/en:docs:macrolib:parse
;-----------------------------------------------------------------------------
PARSE		MACRO	STRING,FCB
		MOV	SI,OFFSET STRING
		MOV	DI,OFFSET FCB
		PUSH	ES
		PUSH	DS
		POP	ES
		MOV	AL,0FH
		@DosCall 29H
		POP	ES
		ENDM

;-----------------------------------------------------------------------------
; GET_DATE - Get system date (2Ah)
; Output: 
;   CX = Year (1980-2099)
;   DH = Month (1-12)
;   DL = Day (1-31)
;   AL = Day of week (0=Sunday, 1=Monday, ...)
;
; RBIL: INT 21h/AH=2Ah - GET SYSTEM DATE
;   Returns current system date.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_date
;-----------------------------------------------------------------------------
GET_DATE	MACRO
		@DosCall 2AH
		ENDM

;-----------------------------------------------------------------------------
; @GetDate - Get system date (2Ah)
; Output: Same as GET_DATE
;-----------------------------------------------------------------------------
@GetDate	MACRO
		@DosCall 2AH
		ENDM

;-----------------------------------------------------------------------------
; SET_DATE - Set system date (2Bh)
; Input:
;   YEAR  - Year (1980-2099)
;   MONTH - Month (1-12)
;   DAY   - Day (1-31)
; Output: AL = 0 on success, FFh on error
;
; RBIL: INT 21h/AH=2Bh - SET SYSTEM DATE
;   Sets new system date. CX=year, DH=month, DL=day.
;   Reference: http://osfree.org/doku/en:docs:macrolib:set_date
;-----------------------------------------------------------------------------
SET_DATE	MACRO	YEAR,MONTH,DAY
		MOV	CX,YEAR
		MOV	DH,MONTH
		MOV	DL,DAY
		@DosCall 2BH
		ENDM

;-----------------------------------------------------------------------------
; @SetDate - Set system date (2Bh)
; Input: Same as SET_DATE
;-----------------------------------------------------------------------------
@SetDate	MACRO MONTH, DAY, YEAR
		MOV	CX,YEAR
		MOV	DH,MONTH
		MOV	DL,DAY
		@DosCall 2BH
		ENDM

;-----------------------------------------------------------------------------
; GET_TIME - Get system time (2Ch)
; Output:
;   CH = Hours (0-23)
;   CL = Minutes (0-59)
;   DH = Seconds (0-59)
;   DL = Hundredths (0-99)
;
; RBIL: INT 21h/AH=2Ch - GET SYSTEM TIME
;   Returns current system time.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_time
;-----------------------------------------------------------------------------
GET_TIME	MACRO
		@DosCall 2CH
		ENDM

;-----------------------------------------------------------------------------
; @GetTime - Get system time (2Ch)
; Output: Same as GET_TIME
;-----------------------------------------------------------------------------
@GetTime	MACRO
		@DosCall 2CH
		ENDM

;-----------------------------------------------------------------------------
; SET_TIME - Set system time (2Dh)
; Input:
;   HOUR      - Hours (0-23)
;   MINUTES   - Minutes (0-59)
;   SECONDS   - Seconds (0-59)
;   HUNDREDTHS - Hundredths (0-99)
; Output: AL = 0 on success, FFh on error
;
; RBIL: INT 21h/AH=2Dh - SET SYSTEM TIME
;   Sets new system time. CH=hour, CL=min, DH=sec, DL=hundredths.
;   Reference: http://osfree.org/doku/en:docs:macrolib:set_time
;-----------------------------------------------------------------------------
SET_TIME	MACRO	HOUR,MINUTES,SECONDS,HUNDREDTHS
		MOV	CH,HOUR
		MOV	CL,MINUTES
		MOV	DH,SECONDS
		MOV	DL,HUNDREDTHS
		@DosCall 2DH
		ENDM

;-----------------------------------------------------------------------------
; @SetTime - Set system time (2Dh)
; Input: Same as SET_TIME
;-----------------------------------------------------------------------------
@SetTime	MACRO	HOUR, MINUTES, SECONDS, HUNDREDTHS
		MOV	CH,HOUR
		MOV	CL,MINUTES
		MOV	DH,SECONDS
		MOV	DL,HUNDREDTHS
		@DosCall 2DH
		ENDM

;-----------------------------------------------------------------------------
; VERIFY - Set verify flag (2Eh)
; Input: SWITCH - 00h off, 01h on
;
; RBIL: INT 21h/AH=2Eh - SET VERIFY FLAG
;   AL=00h disable, AL=01h enable write verification.
;   Reference: http://osfree.org/doku/en:docs:macrolib:verify
;-----------------------------------------------------------------------------
VERIFY		MACRO	SWITCH
		MOV	AL,SWITCH
		@DosCall 2EH
		ENDM

;-----------------------------------------------------------------------------
; GET_DTA - Get Disk Transfer Address (2Fh)
; Output: ES:BX = Current DTA address
;
; RBIL: INT 21h/AH=2Fh - GET DISK TRANSFER AREA ADDRESS
;   Returns current DTA in ES:BX.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_dta
;-----------------------------------------------------------------------------
GET_DTA		MACRO
		@DosCall 2FH
		ENDM

;-----------------------------------------------------------------------------
; @GetDTA - Get Disk Transfer Address (2Fh)
; Output: ES:BX = Current DTA
;-----------------------------------------------------------------------------
@GetDTA		MACRO
		@DosCall 2FH
		ENDM

;-----------------------------------------------------------------------------
; GET_VERSION - Get DOS version (3000h)
; Output: 
;   AL = Major version
;   AH = Minor version
;   BH = OEM flags
;   BL:CX = 24-bit serial number
;
; RBIL: INT 21h/AX=3000h - GET DOS VERSION
;   Returns DOS version information. AL=0 for DOS 5+ compatibility.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_version
;-----------------------------------------------------------------------------
GET_VERSION	MACRO
		@DosCall 3000H			; AL=0 FOR DOS 5+ COMPATIBILITY
		ENDM

;-----------------------------------------------------------------------------
; @GetVer - Get DOS version (3000h)
; Output: Same as GET_VERSION
;-----------------------------------------------------------------------------
@GetVer		MACRO
		@DosCall 3000H			; AL=0 FOR DOS 5+ COMPATIBILITY
		ENDM

;-----------------------------------------------------------------------------
; KEEP_PROCESS - Terminate and stay resident (31h)
; Input:
;   RETURN_CODE - Return code
;   LAST_BYTE   - Last byte of resident portion
;
; RBIL: INT 21h/AH=31h - TERMINATE AND STAY RESIDENT
;   AL=return code, DX=resident size in paragraphs.
;   Reference: http://osfree.org/doku/en:docs:macrolib:keep_process
;-----------------------------------------------------------------------------
KEEP_PROCESS	MACRO	RETURN_CODE,LAST_BYTE
		MOV	AL,RETURN_CODE
		MOV	DX,OFFSET LAST_BYTE
		MOV	CL,4
		SHR	DX,CL
		INC	DX
		@DosCall 31H
		ENDM

;-----------------------------------------------------------------------------
; @TSR - Terminate and stay resident (31h)
; Input:
;   PARAGRAPHS - Resident size in paragraphs
;   RETURN     - Return code
;-----------------------------------------------------------------------------
@TSR		MACRO	PARAGRAPHS, RETURN
		MOV	AL,RETURN
		MOV	DX,PARAGRAPHS
		@DosCall 31H
		ENDM

;-----------------------------------------------------------------------------
; CTRL_C_CK - Get/Set Ctrl-C check flag (33h)
; Input:
;   ACTION - 00h get state, 01h set state
;   STATE  - New state (for set)
; Output: On get: DL = current state
;
; RBIL: INT 21h/AH=33h - GET/SET BREAK FLAG
;   AL=00h get state (DL), AL=01h set state (DL=0/1).
;   Reference: http://osfree.org/doku/en:docs:macrolib:ctrl_c_ck
;-----------------------------------------------------------------------------
CTRL_C_CK	MACRO	ACTION,STATE
		IFNB <ACTION>
			MOV	AL,ACTION
		ENDIF
		IFNB <STATE>
			MOV	DL,STATE
		ENDIF
		@DosCall 33H
		ENDM

;-----------------------------------------------------------------------------
; GET_VECTOR - Get interrupt vector (35h)
; Input: INTERRUPT - Interrupt number
; Output: ES:BX = Vector address
;
; RBIL: INT 21h/AH=35h - GET INTERRUPT VECTOR
;   Returns vector for interrupt AL in ES:BX.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_vector
;-----------------------------------------------------------------------------
GET_VECTOR	MACRO	INTERRUPT
		MOV	AL,INTERRUPT
		@DosCall 35H
		ENDM

;-----------------------------------------------------------------------------
; @GetInt - Get interrupt vector (35h)
; Input: INTERRUPT - Interrupt number
; Output: ES:BX = Vector address
;-----------------------------------------------------------------------------
@GetInt		MACRO	INTERRUPT
		MOV	AL,INTERRUPT
		@DosCall 35H
		ENDM

;-----------------------------------------------------------------------------
; GET_DISK_SPACE - Get disk free space (36h)
; Input: DRIVE - Drive number (0=default, 1=A, 2=B, ...)
; Output:
;   AX = Sectors per cluster
;   BX = Free clusters
;   CX = Bytes per sector
;   DX = Total clusters
;
; RBIL: INT 21h/AH=36h - GET FREE DISK SPACE
;   Returns disk space information. DL=drive.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_disk_space
;-----------------------------------------------------------------------------
GET_DISK_SPACE	MACRO	DRIVE
		MOV	DL,DRIVE
		@DosCall 36H
		ENDM

;-----------------------------------------------------------------------------
; @ChkDrv - Check drive (36h)
; Input: DRIVE - Drive number
; Output: Same as GET_DISK_SPACE
;-----------------------------------------------------------------------------
@ChkDrv		MACRO	DRIVE
		MOV	DL,DRIVE
		@DosCall 36H
		ENDM


;-----------------------------------------------------------------------------
; GET_COUNTRY - Get country information (38h)
; Input:
;   COUNTRY - Country code (0FFh for current)
;   BUFFER  - Data buffer
; Output: BX = Country code
;
; RBIL: INT 21h/AH=38h - GET COUNTRY INFORMATION
;   Returns country-specific information. DS:DX=buffer.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_country
;-----------------------------------------------------------------------------
GET_COUNTRY	MACRO	COUNTRY,BUFFER
		LOCAL	GC_01
		MOV	DX,OFFSET BUFFER
		MOV	AX,COUNTRY
		CMP	AX,0FFH
		JL	GC_01
		MOV	AL,0FFH
		MOV	BX,COUNTRY
GC_01:		@DosCall 38H
		ENDM

;-----------------------------------------------------------------------------
; GET_UPPERCASE_TABLE - Get uppercase table (6602h)
; Input:
;   COUNTRY - Country code
;   CODEPAGE - Code page
;   BUFFER  - Buffer for table
;   BUFLEN  - Buffer length
;
; RBIL: INT 21h/AX=6602h - GET UPPERCASE TABLE (DOS 5+)
;   Returns uppercase table. BX=codepage, DX=country, CX=buflen, ES:DI=buffer.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_uppercase_table
;-----------------------------------------------------------------------------
GET_UPPERCASE_TABLE	MACRO COUNTRY, CODEPAGE, BUFFER, BUFLEN
		MOV	BX, CODEPAGE
		MOV	DX, COUNTRY
		MOV	CX, BUFLEN
		LES	DI, BUFFER
		@DosCall 6602H
		ENDM

;-----------------------------------------------------------------------------
; SET_COUNTRY - Set country information (38h)
; Input: COUNTRY - Country code
;
; RBIL: INT 21h/AH=38h - SET COUNTRY INFORMATION
;   Sets country-specific information.
;   Reference: http://osfree.org/doku/en:docs:macrolib:set_country
;-----------------------------------------------------------------------------
SET_COUNTRY	MACRO	COUNTRY
		LOCAL	SC_01
		MOV	DX,0FFFFH
		MOV	AX,COUNTRY
		CMP	AX,0FFH
		JL	@F
		MOV	AL,0FFH
		MOV	BX,COUNTRY
@@:		@DosCall 38H
		ENDM

;-----------------------------------------------------------------------------
; MAKE_DIR - Create directory (39h)
; Input: PATH - Directory path
;
; RBIL: INT 21h/AH=39h - CREATE SUBDIRECTORY
;   Creates new directory at DS:DX.
;   Reference: http://osfree.org/doku/en:docs:macrolib:make_dir
;-----------------------------------------------------------------------------
MAKE_DIR	MACRO	PATH
		LDS	DX, PATH
		@DosCall 39H
		ENDM

;-----------------------------------------------------------------------------
; @MkDir - Create directory with segment option
; Input:
;   PATH   - Directory path
;   SEGMNT - Path segment (optional)
;-----------------------------------------------------------------------------
@MkDir		MACRO	PATH, SEGMNT
		LDS	DX, PATH
		IFNB	<SEGMNT>
		MOV	AX, <SEGMNT>
		MOV	DS, AX
		ENDIF
		@DosCall 39H
		ENDM

;-----------------------------------------------------------------------------
; LFN_MAKE_DIR - Create directory with long name (7139h)
; Input: PATH - Directory path
;
; RBIL: INT 21h/AX=7139h - LONG FILENAME CREATE DIRECTORY (DOS 7+)
;   Creates directory with long name support. DS:DX=ASCIZ path.
;   Reference: http://osfree.org/doku/en:docs:macrolib:lfn_make_dir
;-----------------------------------------------------------------------------
LFN_MAKE_DIR	MACRO	PATH
		LDS	DX, PATH
		STC
		@DosCall 7139H
		ENDM

;-----------------------------------------------------------------------------
; REM_DIR - Remove directory (3Ah)
; Input: PATH - Directory path
;
; RBIL: INT 21h/AH=3Ah - REMOVE SUBDIRECTORY
;   Deletes empty directory at DS:DX.
;   Reference: http://osfree.org/doku/en:docs:macrolib:rem_dir
;-----------------------------------------------------------------------------
REM_DIR		MACRO	PATH
		LDS	DX, PATH
		@DosCall 3AH
		ENDM

;-----------------------------------------------------------------------------
; @RmDir - Remove directory with segment option
; Input:
;   PATH   - Directory path
;   SEGMNT - Path segment (optional)
;-----------------------------------------------------------------------------
@RmDir		MACRO	PATH, SEGMNT
		LDS	DX, PATH
		IFNB	<SEGMNT>
		MOV	AX, <SEGMNT>
		MOV	DS, AX
		ENDIF
		@DosCall 3AH
		ENDM

;-----------------------------------------------------------------------------
; LFN_REM_DIR - Remove directory with long name (713Ah)
; Input: PATH - Directory path
;
; RBIL: INT 21h/AX=713Ah - LONG FILENAME REMOVE DIRECTORY (DOS 7+)
;   Deletes directory with long name support. DS:DX=ASCIZ path.
;   Reference: http://osfree.org/doku/en:docs:macrolib:lfn_rem_dir
;-----------------------------------------------------------------------------
LFN_REM_DIR	MACRO	PATH
		LDS	DX, PATH
		STC
		@DosCall 713AH
		ENDM

;-----------------------------------------------------------------------------
; CHANGE_DIR - Change current directory (3Bh)
; Input: PATH - New directory path
; Output: AL = 0 on success, FFh on error
;
; RBIL: INT 21h/AH=3Bh - SET CURRENT DIRECTORY
;   Changes current directory to DS:DX.
;   Reference: http://osfree.org/doku/en:docs:macrolib:change_dir
;-----------------------------------------------------------------------------
CHANGE_DIR	MACRO	PATH
		LDS	DX, PATH
		@DosCall 3BH
		ENDM

;-----------------------------------------------------------------------------
; @ChDir - Change directory with segment option
; Input:
;   PATH   - Directory path
;   SEGMNT - Path segment (optional)
;-----------------------------------------------------------------------------
@ChDir		MACRO	PATH, SEGMNT
		LDS	DX, PATH
		IFNB	<SEGMNT>
		MOV	AX, <SEGMNT>
		MOV	DS, AX
		ENDIF
		@DosCall 03BH
		ENDM

;-----------------------------------------------------------------------------
; LFN_CHANGE_DIR - Change directory with long name (713Bh)
; Input: PATH - New directory path
;
; RBIL: INT 21h/AX=713Bh - LONG FILENAME SET CURRENT DIRECTORY (DOS 7+)
;   Changes current directory with long name support. DS:DX=ASCIZ path.
;   Reference: http://osfree.org/doku/en:docs:macrolib:lfn_change_dir
;-----------------------------------------------------------------------------
LFN_CHANGE_DIR	MACRO	PATH
		LDS	DX, PATH
		STC
		@DosCall 0713BH
		ENDM

;-----------------------------------------------------------------------------
; @VdmChDir - Universal change directory (detects LFN support)
; Input: PATH - Directory path
;-----------------------------------------------------------------------------
@VdmChDir	MACRO	PATH
		LDS	DX, PATH
		MOV	AX, 03B00H
		CMP	LFNAPI, 0FFFFH
		JNE	@F
		MOV	AX, 0713BH
		STC
@@:
		@DosCall AX
		ENDM
		
;-----------------------------------------------------------------------------
; CREATE_HANDLE - Create file via handle (3Ch)
; Input:
;   PATH    - File path
;   ATTRIB  - File attributes
; Output: AX = File handle
;
; RBIL: INT 21h/AH=3Ch - CREATE OR TRUNCATE FILE
;   Creates new file or truncates existing. DS:DX=ASCIZ path, CX=attributes.
;   Reference: http://osfree.org/doku/en:docs:macrolib:create_handle
;-----------------------------------------------------------------------------
CREATE_HANDLE	MACRO	PATH,ATTRIB
		LDS	DX,PATH
		MOV	CX,ATTRIB
		@DosCall 3CH
		ENDM

;-----------------------------------------------------------------------------
; @MakFil - Create file with options
; Input:
;   PATH    - File path
;   ATTRIB  - File attributes
;   SEGMNT  - Path segment
;   KIND    - Creation type (unused)
;-----------------------------------------------------------------------------
@MakFil		MACRO	PATH, ATTRIB, SEGMNT, KIND
		IFNB	<SEGMNT>
			MOV	AX, SEGMNT
			MOV	DS, AX
		ENDIF
		IFNB	<PATH>
			MOV	DX,PATH
		ENDIF
		IFNB	<ATTRIB>
			MOV	CX,ATTRIB
		ENDIF
		@DosCall 3CH
		ENDM

;-----------------------------------------------------------------------------
; OPEN_HANDLE - Open file via handle (3Dh)
; Input:
;   PATH   - File path
;   ACCESS - Access mode
; Output: AX = File handle
;
; RBIL: INT 21h/AH=3Dh - OPEN EXISTING FILE
;   Opens existing file. DS:DX=ASCIZ path, AL=access mode.
;   Reference: http://osfree.org/doku/en:docs:macrolib:open_handle
;-----------------------------------------------------------------------------
OPEN_HANDLE	MACRO	PATH,ACCESS
		LDS	DX,PATH
		MOV	AL,ACCESS
		@DosCall 3DH
		ENDM

;-----------------------------------------------------------------------------
; @OpenFil - Open file with options
; Input:
;   PATH   - File path
;   ACCESS - Access mode
;   SEGMNT - Path segment
;-----------------------------------------------------------------------------
@OpenFil	MACRO	PATH, ACCESS, SEGMNT
		IFNB	<SEGMNT>
			MOV	AX, SEGMNT
			MOV	DS, AX
		ENDIF
		IFNB	<PATH>
			MOV	DX,PATH
		ENDIF
		IFNB	<ACCESS>
			MOV	AL,ACCESS
		ENDIF
		@DosCall 3DH
		ENDM

;-----------------------------------------------------------------------------
; CLOSE_HANDLE - Close file via handle (3Eh)
; Input: HANDLE - File handle
;
; RBIL: INT 21h/AH=3Eh - CLOSE FILE
;   Closes file handle. BX=file handle.
;   Reference: http://osfree.org/doku/en:docs:macrolib:close_handle
;-----------------------------------------------------------------------------
CLOSE_HANDLE	MACRO	HANDLE
		IFNB	<HANDLE>
			MOV	BX,HANDLE
		ENDIF
		@DosCall 3EH
		ENDM

;-----------------------------------------------------------------------------
; @ClosFil - Close file (3Eh)
; Input: HANDLE - File handle
;-----------------------------------------------------------------------------
@ClosFil	MACRO	HANDLE
		IFNB	<HANDLE>
			MOV	BX,HANDLE
		ENDIF
		@DosCall 3EH
		ENDM

;-----------------------------------------------------------------------------
; READ_HANDLE - Read from file (3Fh)
; Input:
;   HANDLE - File handle
;   BUFFER - Data buffer
;   BYTES  - Bytes to read
; Output: AX = Actual bytes read
;
; RBIL: INT 21h/AH=3Fh - READ FROM FILE OR DEVICE
;   Reads data. BX=handle, CX=bytes, DS:DX=buffer.
;   Reference: http://osfree.org/doku/en:docs:macrolib:read_handle
;-----------------------------------------------------------------------------
READ_HANDLE	MACRO	HANDLE,BUFFER,BYTES
		MOV	BX,HANDLE
		MOV	DX,OFFSET BUFFER
		MOV	CX,BYTES
		@DosCall 3FH
		ENDM

;-----------------------------------------------------------------------------
; @Read - Extended file read
; Input:
;   OFSET   - Buffer offset
;   BYTES   - Bytes to read
;   HANDLE  - File handle
;   SEGMNT  - Buffer segment (optional)
; Output: AX = Actual bytes read
;-----------------------------------------------------------------------------
@Read		MACRO	OFSET, BYTES, HANDLE, SEGMNT
		IFNB	<SEGMNT>
			PUSH	AX
			MOV	AX, SEGMNT
			MOV	DS, AX
			POP		AX
		ENDIF

		IFNB	<HANDLE>
			MOV	BX,HANDLE
		ENDIF

		IFNB	<OFSET>
			MOV	DX,OFFSET OFSET
		ENDIF

		IFNB	<BYTES>
			MOV	CX,BYTES
		ENDIF
		@DosCall 3FH
		ENDM


;-----------------------------------------------------------------------------
; WRITE_HANDLE - Write to file (40h)
; Input:
;   HANDLE - File handle
;   BUFFER - Data buffer
;   BYTES  - Bytes to write
; Output: AX = Actual bytes written
;
; RBIL: INT 21h/AH=40h - WRITE TO FILE OR DEVICE
;   Writes data. BX=handle, CX=bytes, DS:DX=buffer.
;   Reference: http://osfree.org/doku/en:docs:macrolib:write_handle
;-----------------------------------------------------------------------------
WRITE_HANDLE	MACRO	HANDLE,BUFFER,BYTES
		MOV	BX,HANDLE
		LDS	DX,BUFFER
		MOV	CX,BYTES
		@DosCall 40H
		ENDM

;-----------------------------------------------------------------------------
; @Write - Extended file write
; Input:
;   OFSET   - Buffer offset
;   BYTES   - Bytes to write
;   HANDLE  - File handle
;   SEGMNT  - Buffer segment (optional)
; Output: AX = Actual bytes written
;-----------------------------------------------------------------------------
@Write		MACRO	OFSET, BYTES, HANDLE, SEGMNT
		IFNB	<HANDLE>
			MOV	BX,HANDLE
		ENDIF

		IFNB	<OFSET>
			LDS	DX,OFSET
		ENDIF

		IFNB	<SEGMNT>
			MOV	AX, <SEGMNT>
			MOV	DS, AX
		ENDIF

		IFNB	<BYTES>
			MOV	CX,BYTES
		ENDIF

		@DosCall 40H
		ENDM

;-----------------------------------------------------------------------------
; DELETE_ENTRY - Delete file (41h)
; Input: PATH - File path
;
; RBIL: INT 21h/AH=41h - DELETE FILE
;   Deletes file at DS:DX (ASCIZ path).
;   Reference: http://osfree.org/doku/en:docs:macrolib:delete_entry
;-----------------------------------------------------------------------------
DELETE_ENTRY	MACRO	PATH
		LDS	DX, PATH
		@DosCall 41H
		ENDM

;-----------------------------------------------------------------------------
; @DelFil - Delete file with segment option
; Input:
;   PATH   - File path
;   SEGMNT - Path segment (optional)
;-----------------------------------------------------------------------------
@DelFil		MACRO	PATH, SEGMNT
		LDS	DX, PATH
		IFNB	<SEGMNT>
			MOV	AX, <SEGMNT>
			MOV	DS, AX
		ENDIF
		@DosCall 41H
		ENDM

;-----------------------------------------------------------------------------
; LFN_DELETE_ENTRY - Delete file with long name (7141h)
; Input: PATH - File path
;
; RBIL: INT 21h/AX=7141h - LONG FILENAME DELETE FILE (DOS 7+)
;   Deletes file with long name support. DS:DX=ASCIZ path.
;   Reference: http://osfree.org/doku/en:docs:macrolib:lfn_delete_entry
;-----------------------------------------------------------------------------
LFN_DELETE_ENTRY	MACRO	PATH
		LDS	DX, PATH
		@DosCall 7141H
		ENDM

;-----------------------------------------------------------------------------
; MOVE_PTR - Move file pointer (42h)
; Input:
;   HANDLE - File handle
;   HIGH   - High word of offset
;   LOW    - Low word of offset
;   METHOD - Method:
;             0 = From beginning
;             1 = From current position
;             2 = From end
; Output: DX:AX = New file pointer
;
; RBIL: INT 21h/AH=42h - SEEK
;   Moves file pointer. AL=method, BX=handle, CX:DX=offset.
;   Reference: http://osfree.org/doku/en:docs:macrolib:move_ptr
;-----------------------------------------------------------------------------
MOVE_PTR	MACRO	HANDLE,HIGH,LOW,METHOD
		MOV	BX,HANDLE
		MOV	CX,HIGH
		MOV	DX,LOW
		MOV	AL,METHOD
		@DosCall 42H
		ENDM

;-----------------------------------------------------------------------------
; @MovePtr - Universal file pointer movement
; Input: Same as MOVE_PTR
;-----------------------------------------------------------------------------
@MovePtr	MACRO	HANDLE,HIGH,LOW,METHOD
		IFNB	<HANDLE>
			MOV	BX,HANDLE
		ENDIF
		IFNB	<HIGH>
			MOV	CX,HIGH
		ENDIF
		IFNB	<LOW>
			MOV	DX,LOW
		ENDIF
		IFNB	<METHOD>
			MOV	AL,METHOD
		ENDIF
		@DosCall 42H
		ENDM

;-----------------------------------------------------------------------------
; @MovePtrAbs - Move pointer from beginning of file
; Input:
;   HANDLE   - File handle
;   DISTANCE - Absolute offset
;-----------------------------------------------------------------------------
@MovePtrAbs	MACRO	HANDLE, DISTANCE
		MOV	BX,HANDLE
		MOV	CX,WORD PTR DISTANCE+2
		MOV	DX,WORD PTR DISTANCE
		@DosCall 4200H
		ENDM

;-----------------------------------------------------------------------------
; @MovePtrRel - Move pointer relative to current position
; Input:
;   HANDLE   - File handle
;   DISTANCE - Relative offset
;-----------------------------------------------------------------------------
@MovePtrRel	MACRO	HANDLE, DISTANCE
		MOV	BX,HANDLE
		MOV	CX,WORD PTR DISTANCE+2
		MOV	DX,WORD PTR DISTANCE
		@DosCall 4201H
		ENDM

;-----------------------------------------------------------------------------
; @GetFilSz - Get file size
; Input: HANDLE - File handle
; Output: DX:AX - File size
;-----------------------------------------------------------------------------
@GetFilSz	MACRO	HANDLE
		MOV	BX, HANDLE
		SUB	CX, CX
		MOV	DX, CX
		@DosCall 4201H
		PUSH	AX
		PUSH	DX
		MOV	DX, CX
		@DosCall 4202H
		POP	CX
		MOV	BX, DX
		POP	DX
		PUSH	AX
		PUSH	BX
		MOV	BX, HANDLE
		@DosCall 4200H
		POP	DX
		POP	AX
		ENDM

;-----------------------------------------------------------------------------
; CHANGE_MODE - Change file attributes (43h)
; Input:
;   PATH    - File path
;   ACTION  - Action:
;             0 = Get attributes
;             1 = Set attributes
;   ATTRIB  - New attributes (for set)
; Output: On get: CX = File attributes
;
; RBIL: INT 21h/AH=43h - GET/SET FILE ATTRIBUTES
;   Gets or sets file attributes. AL=action, CX=attributes, DS:DX=ASCIZ path.
;   Reference: http://osfree.org/doku/en:docs:macrolib:change_mode
;-----------------------------------------------------------------------------
CHANGE_MODE	MACRO	PATH,ACTION,ATTRIB
		LDS	DX,PATH
		IFNB	<ATTRIB>
			MOV	CX,ATTRIB
		ENDIF
		@DosCall 4300H + ACTION
		ENDM

;-----------------------------------------------------------------------------
; LFN_CHANGE_MODE - Change attributes for long filename (7143h)
; Input: Same as CHANGE_MODE
;
; RBIL: INT 21h/AX=7143h - LONG FILENAME GET/SET ATTRIBUTES (DOS 7+)
;   BL=action, CX=attributes, DS:DX=ASCIZ path.
;   Reference: http://osfree.org/doku/en:docs:macrolib:lfn_change_mode
;-----------------------------------------------------------------------------
LFN_CHANGE_MODE	MACRO	PATH,ACTION,ATTRIB
		LDS	DX,PATH
		MOV	BL,ACTION
		IFNB	<ATTRIB>
			MOV	CX,ATTRIB
		ENDIF
		STC
		@DosCall 7143H
		ENDM

;-----------------------------------------------------------------------------
; @VdmChangeMode - Universal change attributes (detects LFN support)
; Input: Same as CHANGE_MODE
;   Reference: http://osfree.org/doku/en:docs:macrolib:vdmchangemode
;-----------------------------------------------------------------------------
@VdmChangeMode	MACRO	PATH, ACTION, ATTRIB
		IFNB	<ATTRIB>
			MOV	CX,ATTRIB
		ENDIF
		LDS	DX,PATH
		MOV	AX, 4300H + ACTION
		CMP	LFNAPI, 0FFFFH
		JNE	@F
		MOV	BL, ACTION
		MOV	AX, 7143H
		STC
@@:		@DosCall AX
		ENDM

;-----------------------------------------------------------------------------
; IOCTL_DATA - Device I/O control data (44h)
; Input:
;   CODE    - Function code
;   HANDLE  - Device handle
;
; RBIL: INT 21h/AH=44h - IOCTL
;   Device control functions. AL=subfunction, BX=handle.
;   Reference: http://osfree.org/doku/en:docs:macrolib:ioctl_data
;-----------------------------------------------------------------------------
IOCTL_DATA	MACRO	CODE,HANDLE
		MOV	BX,HANDLE
		MOV	AL,CODE
		@DosCall 44H
		ENDM

;-----------------------------------------------------------------------------
; IOCTL_CHAR - Device I/O control character (44h)
; Input:
;   CODE    - Function code
;   HANDLE  - Device handle
;   BUFFER  - Data buffer
;-----------------------------------------------------------------------------
IOCTL_CHAR	MACRO	CODE,HANDLE,BUFFER
		MOV	BX,HANDLE
		MOV	DX,OFFSET BUFFER
		MOV	AL,CODE
		@DosCall 44H
		ENDM

;-----------------------------------------------------------------------------
; IOCTL_STATUS - Get device status (44h)
; Input:
;   CODE    - Function code
;   DRIVE   - Drive number
;   BUFFER  - Status buffer
;-----------------------------------------------------------------------------
IOCTL_STATUS	MACRO	CODE,DRIVE,BUFFER
		MOV	BL,DRIVE
		MOV	DX,OFFSET BUFFER
		MOV	AL,CODE
		@DosCall 44H
		ENDM

IOCTL_STATUS	MACRO	CODE,HANDLE
		MOV	BX,HANDLE
		MOV	AL,CODE
		@DosCall 44H
		ENDM

;-----------------------------------------------------------------------------
; IOCTL_CHANGE - Change drive (44h/08h)
; Input: DRIVE - Drive number
;-----------------------------------------------------------------------------
IOCTL_CHANGE	MACRO	DRIVE
		MOV	BL,DRIVE
		MOV	AL,08H
		@DosCall 44H
		ENDM

;-----------------------------------------------------------------------------
; IOCTL_RBLOCK - Check block device (44h/09h)
; Input: DRIVE - Drive number
;-----------------------------------------------------------------------------
IOCTL_RBLOCK	MACRO	DRIVE
		MOV	BL,DRIVE
		MOV	AL,09H
		@DosCall 44H
		ENDM

;-----------------------------------------------------------------------------
; IOCTL_RHANDLE - Check handle device (44h/0Ah)
; Input: HANDLE - Device handle
;-----------------------------------------------------------------------------
IOCTL_RHANDLE	MACRO	HANDLE
		MOV	BX,HANDLE
		MOV	AL,0AH
		@DosCall 44H
		ENDM

;-----------------------------------------------------------------------------
; IOCTL_RETRY - Set retry parameters (44h/0Bh)
; Input:
;   RETRIES - Retry count
;   WAIT    - Wait time
;-----------------------------------------------------------------------------
IOCTL_RETRY	MACRO	RETRIES,WAIT
		MOV	DX,RETRIES
		MOV	CX,WAIT
		MOV	AL,0BH
		@DosCall 44H
		ENDM

;-----------------------------------------------------------------------------
; GENERIC_IOCTL_HANDLES - Generic IOCTL for devices (44h/0Ch)
; Input:
;   HANDLE   - Device handle
;   FUNCTION - IOCTL function
;   CATEGORY - Device category
;   BUFFER   - Data buffer
;-----------------------------------------------------------------------------
GENERIC_IOCTL_HANDLES	MACRO HANDLE,FUNCTION,CATEGORY,BUFFER
		MOV    CH,05H
		MOV	CL,FUNCTION
		MOV	DX,OFFSET BUFFER
		MOV	BX,HANDLE
		MOV	AL,0CH
		@DosCall 44H
		ENDM

;-----------------------------------------------------------------------------
; GENERIC_IOCTL_BLOCK - Generic IOCTL for block devices (44h/0Dh)
; Input:
;   DRIVE_NUM - Drive number
;   FUNCTION  - IOCTL function
;   CATEGORY  - Device category
;   PARM_BLK  - Parameter block
;-----------------------------------------------------------------------------
GENERIC_IOCTL_BLOCK	MACRO	DRIVE_NUM,FUNCTION,CATEGORY,PARM_BLK
		MOV	CH,08H
		MOV	CL,FUNCTION
		MOV	DX,OFFSET PARM_BLK - 1
		MOV	BX,DRIVE_NUM
		MOV	AL,0DH
		@DosCall 44H
		ENDM

;-----------------------------------------------------------------------------
; IOCTL_GET_DRIVE_MAP - Get drive map (44h/0Eh)
; Input: LOGICAL_DRV - Logical drive number
; Output: AL = Physical drive number
;-----------------------------------------------------------------------------
IOCTL_GET_DRIVE_MAP	MACRO	LOGICAL_DRV
		MOV	BX,LOGICAL_DRV
		MOV	AL,0EH
		@DosCall 44H
		ENDM

;-----------------------------------------------------------------------------
; IOCTL_SET_DRIVE_MAP - Set drive map (44h/0Fh)
; Input: LOGICAL_DRV - Logical drive number
;-----------------------------------------------------------------------------
IOCTL_SET_DRIVE_MAP	MACRO	LOGICAL_DRV
		MOV	BX,LOGICAL_DRV
		MOV	AL,0FH
		@DosCall 44H
		ENDM

;-----------------------------------------------------------------------------
; XDUP - Duplicate file handle (45h)
; Input: HANDLE - Source handle
; Output: AX = New handle
;
; RBIL: INT 21h/AH=45h - DUPLICATE FILE HANDLE
;   Creates copy of file handle. BX=handle.
;   Reference: http://osfree.org/doku/en:docs:macrolib:xdup
;-----------------------------------------------------------------------------
XDUP		MACRO	HANDLE
		MOV	BX,HANDLE
		@DosCall 45H
		ENDM

;-----------------------------------------------------------------------------
; XDUP2 - Force duplicate file handle (46h)
; Input:
;   HANDLE1 - Source handle
;   HANDLE2 - Target handle
;
; RBIL: INT 21h/AH=46h - FORCE DUPLICATE FILE HANDLE
;   Makes handle2 refer to same file as handle1. BX=handle1, CX=handle2.
;   Reference: http://osfree.org/doku/en:docs:macrolib:xdup2
;-----------------------------------------------------------------------------
XDUP2		MACRO	HANDLE1,HANDLE2
		MOV	BX,HANDLE1
		MOV	CX,HANDLE2
		@DosCall 46H
		ENDM

;-----------------------------------------------------------------------------
; GET_DIR - Get current directory (47h)
; Input:
;   DRIVE   - Drive number (0=default, 1=A, 2=B, ...)
;   BUFFER  - Path buffer
;
; RBIL: INT 21h/AH=47h - GET CURRENT DIRECTORY
;   Returns current directory path. DL=drive, DS:SI=buffer.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_dir
;-----------------------------------------------------------------------------
GET_DIR		MACRO	DRIVE,BUFFER
		MOV	DL,DRIVE
		MOV	SI,BUFFER
		@DosCall 47H
		ENDM

;-----------------------------------------------------------------------------
; @GetDir - Get current directory with options
; Input:
;   BUFFER - Path buffer
;   DRIVE  - Drive number
;   SEGMNT - Buffer segment (optional)
;-----------------------------------------------------------------------------
@GetDir		MACRO	BUFFER, DRIVE, SEGMNT
		IFNB	<SEGMNT>
			MOV	AX, <SEGMNT>
			MOV	DS, AX
		ENDIF

		IFNB <DRIVE>
			MOV	DL,DRIVE
		ENDIF

		IFNB <BUFFER>
			MOV	SI,BUFFER
		ENDIF

		@DosCall 47H
		ENDM

;-----------------------------------------------------------------------------
; ALLOCATE_MEMORY - Allocate memory (48h)
; Input: BYTES - Bytes to allocate
; Output: AX = Segment of allocated block
;
; RBIL: INT 21h/AH=48h - ALLOCATE MEMORY BLOCK
;   Allocates memory. BX=paragraphs needed. Returns AX=segment.
;   Reference: http://osfree.org/doku/en:docs:macrolib:allocate_memory
;-----------------------------------------------------------------------------
ALLOCATE_MEMORY	MACRO	BYTES
		MOV	BX,BYTES
		MOV	CL,4
		SHR	BX,CL
		INC	BX
		@DosCall 48H
		ENDM

;-----------------------------------------------------------------------------
; @GetBlok - Allocate memory in paragraphs (48h)
; Input: PARAGRAPHS - Paragraphs to allocate
; Output: AX = Segment of allocated block
;    Documentation: http://osfree.org/doku/en:docs:macrolib:getblok
;-----------------------------------------------------------------------------
@GetBlok	MACRO	PARAGRAPHS
		IFNB	<PARAGRAPHS>
			MOV	BX, PARAGRAPHS
		ENDIF
		@DosCall 48H
		ENDM

;-----------------------------------------------------------------------------
; FREE_MEMORY - Free memory (49h)
; Input: SEG_ADDR - Segment of memory block
;
; RBIL: INT 21h/AH=49h - FREE MEMORY BLOCK
;   Releases memory block. ES=segment of block.
;   Reference: http://osfree.org/doku/en:docs:macrolib:free_memory
;-----------------------------------------------------------------------------
FREE_MEMORY	MACRO	SEG_ADDR
		MOV	ES,SEG_ADDR
;		MOV	ES,AX
		@DosCall 49H
		ENDM

;-----------------------------------------------------------------------------
; @FreeBlok - Free memory block (49h)
; Input: SEGMNT - Segment of block to free
;-----------------------------------------------------------------------------
@FreeBlok	MACRO	SEGMNT
		MOV	ES,SEGMNT
;		MOV	ES,AX
		@DosCall 49H
		ENDM

;-----------------------------------------------------------------------------
; SET_BLOCK - Resize memory block (4Ah)
; Input: LAST_BYTE - Last byte of resident portion
;
; RBIL: INT 21h/AH=4Ah - RESIZE MEMORY BLOCK
;   Modifies size of allocated block. ES=segment, BX=new size in paragraphs.
;   Reference: http://osfree.org/doku/en:docs:macrolib:set_block
;-----------------------------------------------------------------------------
SET_BLOCK	MACRO	LAST_BYTE
		MOV	BX,OFFSET LAST_BYTE
		MOV	CL,4
		SHR	BX,CL
		ADD	BX,17
		MOV	AH,4AH
		INT	21H
		MOV	AX,BX
		SHL	AX,CL
		DEC	AX
		MOV	SP,AX
		MOV	BP,SP
		ENDM

;-----------------------------------------------------------------------------
; @ModBlok - Resize memory block (4Ah)
; Input:
;   PARAGRAPHS - New size in paragraphs
;   SEGMNT     - Segment of block to modify
;    Documentation: http://osfree.org/doku/en:docs:macrolib:modblok
;-----------------------------------------------------------------------------
@ModBlok	MACRO	PARAGRAPHS, SEGMNT
		IFNB	<PARAGRAPHS>
			MOV	BX, PARAGRAPHS
		ENDIF
		IFNB	<SEGMNT>
			MOV	AX, <SEGMNT>
			MOV	ES, AX
		ENDIF
		@DosCall 4AH
		ENDM

;-----------------------------------------------------------------------------
; EXEC - Execute program (4Bh)
; Input:
;   PATH    - Path to executable
;   COMMAND - Command line
;   PARMS   - Parameter block
;
; RBIL: INT 21h/AH=4Bh - EXECUTE PROGRAM
;   Loads and executes program. AL=00h load and execute, DS:DX=ASCIZ path,
;   ES:BX=parameter block.
;   Reference: http://osfree.org/doku/en:docs:macrolib:exec
;-----------------------------------------------------------------------------
EXEC		MACRO	PATH, COMMAND, PARMS
		MOV	DX,OFFSET PATH
		MOV	BX,OFFSET PARMS
		MOV	WORD PTR PARMS[02H],OFFSET COMMAND
		MOV	WORD PTR PARMS[04H],CS
		MOV	WORD PTR PARMS[06H],5CH
		MOV	WORD PTR PARMS[08H],ES
		MOV	WORD PTR PARMS[0AH],6CH
		MOV	WORD PTR PARMS[0CH],ES
		MOV	AL,0
		@DosCall 4BH
		ENDM

;-----------------------------------------------------------------------------
; EXEC_OVL - Execute overlay (4Bh)
; Input:
;   PATH     - Path to overlay file
;   PARMS    - Parameter block
;   SEG_ADDR - Load segment
;
; RBIL: INT 21h/AH=4Bh/AL=03h - LOAD OVERLAY
;   Loads program overlay. DS:DX=ASCIZ path, ES:BX=parameter block.
;   Reference: http://osfree.org/doku/en:docs:macrolib:exec_ovl
;-----------------------------------------------------------------------------
EXEC_OVL	MACRO	PATH,PARMS,SEG_ADDR
		MOV	DX,OFFSET PATH
		MOV	BX,OFFSET PARMS
		MOV	PARMS,SEG_ADDR
		MOV	PARMS[02H],SEG_ADDR
		MOV	AL,3
		@DosCall 4BH
		ENDM

;-----------------------------------------------------------------------------
; @Exec - Universal program/overlay execution
; Input:
;   PATH      - File path
;   PARMS     - Command parameters
;   SEGPATH   - Path segment (optional)
;   SEGPARAMS - Parameter segment (optional)
;   OVERLAY   - If specified, overlay mode
;-----------------------------------------------------------------------------
@Exec		MACRO	PATH, PARMS, SEGPATH, SEGPARAMS, OVERLAY
		IFNB <PATH>
		MOV	DX,OFFSET PATH
		ENDIF

		IFNB <PARMS>
		MOV	BX,OFFSET PARMS
		ENDIF

		IFNB <SEGPATH>
		MOV	AX, SEGPATH
		MOV	DS, AX
		ENDIF

		IFNB <SEGPARAMS>
		MOV	AX, SEGPARAMS
		MOV	ES, AX
		ENDIF

		IFB	<OVERLAY>
		XOR	AX,AX
		ELSE
		MOV	AL,3
		ENDIF
		@DosCall 4BH
		ENDM

;-----------------------------------------------------------------------------
; @Exit - Terminate program with return code (4Ch)
; Input: RETURN - Return code (optional)
;
; RBIL: INT 21h/AH=4Ch - EXIT PROGRAM WITH RETURN CODE
;   Terminates program. AL=return code.
;   Reference: http://osfree.org/doku/en:docs:macrolib:exit
;-----------------------------------------------------------------------------
@Exit		MACRO	RETURN
		IFNB <RETURN>
		IF (.TYPE (RETURN)) AND 00000100b ; Constant
			IFNB	<RETURN>
				MOV	AX,4C00H+RETURN
			ELSE
				MOV	AH,4CH
			ENDIF
		ELSE
			IFDIFI	<RETURN>, <AL>
				IF TYPE (RETURN) EQ 2
					MOV	AX, RETURN
				ELSE
					MOV	AL, RETURN
				ENDIF
			ENDIF
			MOV	AH, 4CH
		ENDIF
		ELSE
			MOV	AH, 4CH
		ENDIF
		INT	21H
		ENDM

;-----------------------------------------------------------------------------
; END_PROCESS - Terminate program (4Ch)
; Alias for @Exit
;-----------------------------------------------------------------------------
END_PROCESS	MACRO	RETURN
		@Exit	RETURN
		ENDM

;-----------------------------------------------------------------------------
; RET_CODE - Get child return code (4Dh)
; Output: 
;   AL = Return code
;   AH = Exit reason:
;        0 = Normal
;        1 = Ctrl-C
;        2 = Critical error
;        3 = Terminate and stay resident
;
; RBIL: INT 21h/AH=4Dh - GET RETURN CODE OF SUBPROCESS
;   Returns exit code of child process.
;   Reference: http://osfree.org/doku/en:docs:macrolib:ret_code
;-----------------------------------------------------------------------------
RET_CODE	MACRO
		@DosCall 4DH
		ENDM

;-----------------------------------------------------------------------------
; @GetRet - Get child return code (4Dh)
; Output: Same as RET_CODE
;-----------------------------------------------------------------------------
@GetRet		MACRO
		@DosCall 4DH
		ENDM

;-----------------------------------------------------------------------------
; FIND_FIRST_FILE - Find first file (4Eh)
; Input:
;   PATH   - Search path
;   ATTRIB - File attributes
;
; RBIL: INT 21h/AH=4Eh - FIND FIRST MATCHING FILE
;   Finds first file matching spec. DS:DX=ASCIZ path, CX=attributes.
;   Reference: http://osfree.org/doku/en:docs:macrolib:find_first_file
;-----------------------------------------------------------------------------
FIND_FIRST_FILE	MACRO	PATH,ATTRIB
		MOV	DX,OFFSET PATH
		MOV	CX,ATTRIB
		@DosCall 4EH
		ENDM

;-----------------------------------------------------------------------------
; @GetFirst - Find first file with options
; Input:
;   PATH   - Search path
;   ATTRIB - File attributes
;   SEGMNT - Path segment (optional)
;-----------------------------------------------------------------------------
@GetFirst	MACRO	PATH, ATTRIB, SEGMNT
		IFNB <SEGMNT>
		MOV	AX, SEGMNT
		MOV	DS, AX
		ENDIF
		IFNB	<PATH>
		MOV	DX, OFFSET PATH
		ENDIF
		IFNB	<ATTRIB>
			MOV	CX, ATTRIB
		ELSE
			XOR	CX, CX
		ENDIF
		@DosCall 4EH
		ENDM

;-----------------------------------------------------------------------------
; FIND_NEXT_FILE - Find next file (4Fh)
;
; RBIL: INT 21h/AH=4Fh - FIND NEXT MATCHING FILE
;   Continues search from previous FIND FIRST.
;   Reference: http://osfree.org/doku/en:docs:macrolib:find_next_file
;-----------------------------------------------------------------------------
FIND_NEXT_FILE	MACRO
		@DosCall 4FH
		ENDM

;-----------------------------------------------------------------------------
; @GetNext - Find next file (4Fh)
;-----------------------------------------------------------------------------
@GetNext	MACRO
		@DosCall 4FH
		ENDM

;-----------------------------------------------------------------------------
; GET_VERIFY - Get verify flag state (54h)
; Output: AL = State (0 = off, 1 = on)
;
; RBIL: INT 21h/AH=54h - GET VERIFY FLAG STATE
;   Returns current verify setting.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_verify
;-----------------------------------------------------------------------------
GET_VERIFY	MACRO
		@DosCall 54H
		ENDM

;-----------------------------------------------------------------------------
; RENAME_FILE - Rename file (56h)
; Input:
;   OLD_PATH - Current filename
;   NEW_PATH - New filename
;
; RBIL: INT 21h/AH=56h - RENAME FILE
;   Renames file. DS:DX=old name, ES:DI=new name.
;   Reference: http://osfree.org/doku/en:docs:macrolib:rename_file
;-----------------------------------------------------------------------------
RENAME_FILE	MACRO	OLD_PATH,NEW_PATH
		LDS	DX, OLD_PATH
		LES	DI, NEW_PATH
		@DosCall 56H
		ENDM

;-----------------------------------------------------------------------------
; @MoveFil - Rename file with options
; Input:
;   OLD    - Current filename
;   NEW    - New filename
;   SEGOLD - Current name segment
;   SEGNEW - New name segment
;-----------------------------------------------------------------------------
@MoveFil	MACRO	OLD, NEW, SEGOLD, SEGNEW
		IFNB <SEGOLD>
		MOV	AX, SEGOLD
		MOV	DS, AX
		ENDIF
		MOV	DX, OLD
		IFNB <SEGNEW>
		MOV	AX, SEGNEW
		MOV	ES, AX
		ENDIF
		MOV	DI, NEW
		@DosCall 56H
		ENDM

;-----------------------------------------------------------------------------
; LFN_RENAME_FILE - Rename file with long name (7156h)
; Input: Same as RENAME_FILE
;
; RBIL: INT 21h/AX=7156h - LONG FILENAME RENAME FILE (DOS 7+)
;   Renames file with long name support. DS:DX=old, ES:DI=new.
;   Reference: http://osfree.org/doku/en:docs:macrolib:lfn_rename_file
;-----------------------------------------------------------------------------
LFN_RENAME_FILE	MACRO	OLD_PATH,NEW_PATH
		LDS	DX, OLD_PATH
		LES	DI, NEW_PATH
		@DosCall 7156H
		ENDM

;-----------------------------------------------------------------------------
; GET_SET_DATE_TIME - Get/Set file date-time (57h)
; Input:
;   HANDLE - File handle
;   ACTION - 0 = Get, 1 = Set
;   TIME   - New time (for set)
;   DATE   - New date (for set)
; Output: On get: CX = Time, DX = Date
;
; RBIL: INT 21h/AH=57h - GET/SET FILE DATE AND TIME
;   Gets or sets file timestamp. AL=action, BX=handle.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_set_date_time
;-----------------------------------------------------------------------------
GET_SET_DATE_TIME	MACRO	HANDLE,ACTION,TIME,DATE
		MOV	BX,HANDLE
		MOV	AL,ACTION
		MOV	CX,WORD PTR TIME
		MOV	DX,WORD PTR DATE
		@DosCall 57H
		ENDM

;-----------------------------------------------------------------------------
; ALLOC_STRAT - Set memory allocation strategy (58h)
; Input:
;   CODE     - 0 = Get, 1 = Set
;   STRATEGY - New strategy:
;              0 = First fit
;              1 = Best fit
;              2 = Last fit
; Output: On get: AX = Current strategy
;
; RBIL: INT 21h/AH=58h - GET/SET MEMORY ALLOCATION STRATEGY
;   AL=00h get strategy, AL=01h set strategy (BX=strategy).
;   Reference: http://osfree.org/doku/en:docs:macrolib:alloc_strat
;-----------------------------------------------------------------------------
ALLOC_STRAT	MACRO	CODE,STRATEGY
		MOV	BX,STRATEGY
		MOV	AL,CODE
		@DosCall 58H
		ENDM

;-----------------------------------------------------------------------------
; GET_ERROR - Get extended error information (59h)
; Output:
;   AX = Error code
;   BH = Error class
;   BL = Suggested action
;   CH = Locus
;
; RBIL: INT 21h/AH=59h - GET EXTENDED ERROR INFORMATION
;   Returns extended error info. BX=0000h.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_error
;-----------------------------------------------------------------------------
GET_ERROR	MACRO
		XOR	BX, BX
		@DosCall 59H
		ENDM

;-----------------------------------------------------------------------------
; CREATE_TEMP - Create temporary file (5Ah)
; Input:
;   PATHNAME - Directory path
;   ATTRIB   - File attributes
; Output: AX = File handle
;
; RBIL: INT 21h/AH=5Ah - CREATE TEMPORARY FILE
;   Creates unique file in specified directory. DS:DX=path, CX=attributes.
;   Reference: http://osfree.org/doku/en:docs:macrolib:create_temp
;-----------------------------------------------------------------------------
CREATE_TEMP	MACRO	PATHNAME,ATTRIB
		MOV	CX,ATTRIB
		MOV	DX,OFFSET PATHNAME
		@DosCall 5AH
		ENDM

;-----------------------------------------------------------------------------
; CREATE_NEW - Create new file (5Bh)
; Input:
;   PATHNAME - File path
;   ATTRIB   - File attributes
; Output: AX = File handle
;
; RBIL: INT 21h/AH=5Bh - CREATE NEW FILE
;   Creates file only if it doesn't exist. DS:DX=path, CX=attributes.
;   Reference: http://osfree.org/doku/en:docs:macrolib:create_new
;-----------------------------------------------------------------------------
CREATE_NEW	MACRO	PATHNAME,ATTRIB
		MOV	CX,ATTRIB
		MOV	DX,OFFSET PATHNAME
		@DosCall 5BH
		ENDM

;-----------------------------------------------------------------------------
; LOCK_FILE - Lock file region (5Ch)
; Input:
;   HANDLE - File handle
;   START  - Starting offset
;   BYTES  - Region size
;
; RBIL: INT 21h/AH=5Ch/AL=00h - LOCK FILE REGION
;   Locks file region. Requires SHARE.EXE. BX=handle, CX:DX=offset, SI:DI=length.
;   Reference: http://osfree.org/doku/en:docs:macrolib:lock_file
;-----------------------------------------------------------------------------
LOCK_FILE	MACRO	HANDLE,START,BYTES
		MOV	BX,HANDLE
		MOV	CX,WORD PTR START
		MOV	DX,WORD PTR START+2
		MOV	SI,WORD PTR BYTES
		MOV	DI,WORD PTR BYTES+2
		MOV	AL,0
		@DosCall 5CH
		ENDM

;-----------------------------------------------------------------------------
; UNLOCK - Unlock file region (5Ch)
; Input: Same as LOCK_FILE
;-----------------------------------------------------------------------------
UNLOCK		MACRO	HANDLE,START,BYTES
		MOV	BX,HANDLE
		MOV	CX,WORD PTR START
		MOV	DX,WORD PTR START+2
		MOV	SI,WORD PTR BYTES
		MOV	DI,WORD PTR BYTES+2
		MOV	AL,1
		@DosCall 5CH
		ENDM

;-----------------------------------------------------------------------------
; GET_MACHINE_NAME - Get machine name (5Eh)
; Input: BUFFER - Name buffer
;
; RBIL: INT 21h/AH=5Eh/AL=00h - GET MACHINE NAME
;   Returns network name. DS:DX=buffer.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_machine_name
;-----------------------------------------------------------------------------
GET_MACHINE_NAME	MACRO	BUFFER
		MOV	DX,OFFSET BUFFER
		MOV	AL,0
		@DosCall 5EH
		ENDM

;-----------------------------------------------------------------------------
; PRINTER_SETUP - Set printer setup string (5Eh)
; Input:
;   INDEX   - Printer number (0-2)
;   LGTH    - String length
;   STRING  - Setup string
;
; RBIL: INT 21h/AH=5Eh/AL=02h - PRINTER SETUP
;   Sets control string for printer. BX=printer, CX=length, DS:DX=string.
;   Reference: http://osfree.org/doku/en:docs:macrolib:printer_setup
;-----------------------------------------------------------------------------
PRINTER_SETUP	MACRO	INDEX,LGTH,STRING
		MOV	BX,INDEX
		MOV	CX,LGTH
		MOV	DX,OFFSET STRING
		MOV	AL,2
		@DosCall 5EH
		ENDM

;-----------------------------------------------------------------------------
; GET_LIST - Get printer list (5Fh)
; Input:
;   INDEX   - Printer number
;   LOCAL   - Local name buffer
;   REMOTE  - Remote name buffer
;
; RBIL: INT 21h/AH=5Fh/AL=02h - GET PRINTER LIST
;   Returns printer assignments. BX=index, DS:SI=local, ES:DI=remote.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_list
;-----------------------------------------------------------------------------
GET_LIST	MACRO	INDEX,LOCAL,REMOTE
		MOV	BX,INDEX
		MOV	SI,OFFSET LOCAL
		MOV	DI,OFFSET REMOTE
		MOV	AL,2
		@DosCall 5FH
		ENDM

;-----------------------------------------------------------------------------
; REDIR - Redirect device (5Fh)
; Input:
;   DEVICE      - Device type (3=LPT, 4=DRIVE)
;   VALUE       - Device value
;   SOURCE      - Local name
;   DESTINATION - Network name
;-----------------------------------------------------------------------------
REDIR		MACRO	DEVICE,VALUE,SOURCE,DESTINATION
		MOV	BL,DEVICE
		MOV	CX,VALUE
		MOV	SI,OFFSET SOURCE
		MOV	ES,SEG DESTINATION
		MOV	DI,OFFSET DESTINATION
		MOV	AL,03H
		@DosCall 5FH
		ENDM

;-----------------------------------------------------------------------------
; CANCEL_REDIR - Cancel redirection (5Fh)
; Input: LOCAL - Local device name
;-----------------------------------------------------------------------------
CANCEL_REDIR	MACRO	LOCAL
		MOV	SI,OFFSET LOCAL
		MOV	AL,4
		@DosCall 5FH
		ENDM

;-----------------------------------------------------------------------------
; GET_PSP - Get Program Segment Prefix (62h)
; Output: BX = PSP segment address
;
; RBIL: INT 21h/AH=62h - GET PROGRAM SEGMENT PREFIX ADDRESS
;   Returns PSP segment of current process in BX.
;   Reference: http://osfree.org/doku/en:docs:macrolib:get_psp
;-----------------------------------------------------------------------------
GET_PSP		MACRO
		@DosCall 62H
		ENDM

;-----------------------------------------------------------------------------
; DISPLAY_ASCIZ - Display ASCIZ string with newline
; Input: ASCIZ_STRING - Null-terminated string
;-----------------------------------------------------------------------------
DISPLAY_ASCIZ	MACRO	ASCIZ_STRING
		LOCAL	SEARCH,FOUND_IT
		MOV	BX,OFFSET ASCIZ_STRING

		SEARCH:
		CMP	BYTE PTR [BX],0
		JE	FOUND_IT
		INC	BX
		JMP SHORT SEARCH

FOUND_IT:
		MOV	BYTE PTR [BX],"$"
		DISPLAY	ASCIZ_STRING
		MOV	BYTE PTR [BX],0
		DISPLAY_CHAR	0DH
		DISPLAY_CHAR	0AH
		ENDM

;-----------------------------------------------------------------------------
; MOVE_STRING - Copy string
; Input:
;   SOURCE      - Source string
;   DESTINATION - Destination string
;   COUNT       - Character count
;-----------------------------------------------------------------------------
MOVE_STRING	MACRO	SOURCE,DESTINATION,COUNT
		PUSH	ES
		PUSH	DS
		POP	ES
		ASSUME	ES:CODE
		MOV	SI,OFFSET SOURCE
		MOV	DI,OFFSET DESTINATION
		MOV	CX,COUNT
	REP	MOVS	ES:DESTINATION,SOURCE
		ASSUME	ES:NOTHING
		POP	ES
		ENDM

;-----------------------------------------------------------------------------
; CONVERT - Convert number to string
; Input:
;   VALUE        - Number to convert
;   BASE         - Number base
;   DESTINATION  - String buffer (2 characters)
;-----------------------------------------------------------------------------
CONVERT		MACRO	VALUE,BASE,DESTINATION
		LOCAL	TABLE,START
		JMP	START
TABLE		DB	"0123456789ABCDEF"

START:
		PUSH	AX
		PUSH	BX
		PUSH	DX
		MOV	AL,VALUE
		XOR	AH,AH
		XOR	BX,BX
		DIV	BASE
		MOV	BL,AL
		MOV	AL,CS:TABLE[BX]
		MOV	DESTINATION,AL
		MOV	BL,AH
		MOV	AL,CS:TABLE[BX]
		MOV	DESTINATION[1],AL
		POP	DX
		POP	BX
		POP	AX
		ENDM

;-----------------------------------------------------------------------------
; CONVERT_TO_BINARY - Convert string to number
; Input:
;   STRING - Number string
;   NUMBER - Digit count
;   VALUE  - Result variable
;-----------------------------------------------------------------------------
CONVERT_TO_BINARY	MACRO  STRING,NUMBER,VALUE
		LOCAL	TEN,START,CALC,MULT,NO_MULT
		JMP	START
TEN		DB	10

START:
		MOV	VALUE,0
		XOR	CX,CX
		MOV	CL,NUMBER
		XOR	SI,SI

CALC:
		XOR	AX,AX
		MOV	AL,STRING[SI]
		SUB	AL,48
		CMP	CX,2
		JL	NO_MULT
		PUSH	CX
		DEC	CX

MULT:
		MUL	CS:TEN
		LOOP	MULT
		POP	CX

NO_MULT:
		ADD	VALUE,AX
		INC	SI
		LOOP	CALC
		ENDM

;-----------------------------------------------------------------------------
; CONVERT_DATE - Convert date from DOS format
; Input: DIR_ENTRY - Directory entry
; Output:
;   CX = Year
;   DH = Month
;   DL = Day
;-----------------------------------------------------------------------------
CONVERT_DATE	MACRO	DIR_ENTRY
		MOV	DX,WORD PTR DIR_ENTRY[24]
		MOV	CL,5
		SHR	DL,CL
		MOV	DH,DIR_ENTRY[24]
		AND	DH,1FH
		XOR	CX,CX
		MOV	CL,DIR_ENTRY[25]
		SHR	CL,1
		ADD	CX,1980
		ENDM

;-----------------------------------------------------------------------------
; PACK_DATE - Pack date to DOS format
; Input:
;   DH = Day
;   DL = Month
;   CX = Year (1980-2099)
; Output: DATE - Packed date
;-----------------------------------------------------------------------------
PACK_DATE	MACRO	DATE
		LOCAL	SET_BIT
;
; ON ENTRY: DH=DAY, DL=MONTH, CX=(YEAR-1980)
;
		SUB	CX,1980
		PUSH	CX
		MOV	DATE,DH
		MOV	CL,5
		SHL	DL,CL
		POP	CX
		JNC	SET_BIT
		OR	CL,80H

SET_BIT:
		OR	DATE,DL
		ROL	CL,1
		MOV	DATE[1],CL
		ENDM

;-----------------------------------------------------------------------------
; @GetKey - Extended keyboard input
; Input:
;   ECHO  - Echo character (1/0)
;   CC    - Check Ctrl-C (1/0)
;   CLEAR - Clear keyboard buffer (1/0)
; Output: AL = Input character
;-----------------------------------------------------------------------------
@GetKey		MACRO	ECHO, CC, CLEAR
		IF	ECHO
			IF	CC
			MOV	AH, 01H
			ELSE
			MOV	AH, 07H
			ENDIF
		ELSE
			IF	CC
			MOV	AH, 08H
			ELSE
			MOV	AH, 07H
			ENDIF
		ENDIF
		IF	CLEAR
		MOV	AL, AH
		MOV	AH, 0CH
		ENDIF
		INT	21H
		IF	ECHO
			IF	NOT CC
			MOV	DL, AL
			@DosCall 02H
			ENDIF
		ENDIF
		ENDM


ENDIF
