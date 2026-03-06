;/*!
;   @file
;
;   @brief SVC Macro Library for MVM (Multiple Virtual Machine)
;
;   (c) osFree Project 2018-2026, <http://www.osFree.org>
;   for licence see licence.txt in root directory, or project website
;
;   This library provides macros for SVC (Supervisor Call) API
;   used in osFree MVM (Multiple Virtual Machine) environment.
;
;   Documentation:
;     Main reference: https://osfree.org/doku/en:docs:mvm:api
;
;   Usage:
;     1. Include this file: INCLUDE svc.inc
;     2. Use macros instead of direct HLT sequences
;
;   Naming Conventions:
;     @Svc prefix - SVC API macros
;     SVC_ prefix - constants (defined in svc.inc or this file)
;
;   Each macro emits: HLT, DB <SVC_constant>, DB (NOT <SVC_constant>) & 0FFh
;   via SVCCALL macro.
;-----------------------------------------------------------------------------
;*/

IFNDEF SVC_DEFINED
SVC_DEFINED EQU 1

;==============================================================================
; Generic SVC call dispatcher
;==============================================================================
SVCCALL MACRO func
        HLT
        DB  func, (NOT func) AND 0FFh
ENDM

;==============================================================================
; SVC function constants (from https://osfree.org/doku/en:docs:mvm:api)
;==============================================================================
SVC_DUPHANDLE       equ 0    ; INT 21h AH=45h
SVC_ERROREXIT       equ 1    ; no DOS equivalent
SVC_EXIT            equ 2    ; INT 21h AH=4Ch
SVC_CLOSE           equ 3    ; INT 21h AH=3Eh
SVC_RESETDISK       equ 4    ; INT 21h AH=0Dh
SVC_GETPSP          equ 5    ; INT 21h AH=62h
SVC_SELECTDRIVE     equ 6    ; INT 21h AH=0Eh
SVC_SETDTA          equ 7    ; INT 21h AH=1Ah
SVC_GETVERSION      equ 8    ; INT 21h AH=30h
SVC_WRITE           equ 9    ; INT 21h AH=40h
SVC_SETDATE         equ 10   ; INT 21h AH=2Bh
SVC_SETTIME         equ 11   ; INT 21h AH=2Dh
SVC_GETDATE         equ 12   ; INT 21h AH=2Ah
SVC_GETTIME         equ 13   ; INT 21h AH=2Ch
SVC_READ            equ 14   ; INT 21h AH=3Fh
SVC_CHGFILEPTR      equ 15   ; INT 21h AH=42h
SVC_RMDIR           equ 16   ; INT 21h AH=3Ah
SVC_MKDIR           equ 17   ; INT 21h AH=39h
SVC_QCURDIR         equ 18   ; INT 21h AH=47h
SVC_CHDIR           equ 19   ; INT 21h AH=3Bh
SVC_GETSETFILETIME  equ 20   ; INT 21h AH=57h
SVC_COMMITFILE      equ 21   ; INT 21h AH=68h
SVC_FINDFIRSTFCB    equ 22   ; INT 21h AH=11h
SVC_FINDNEXTFCB     equ 23   ; INT 21h AH=12h
SVC_FINDFIRST       equ 25   ; INT 21h AH=4Eh
SVC_OPEN            equ 28   ; INT 21h AH=3Dh
SVC_DELETEFCB       equ 29   ; INT 21h AH=13h
SVC_RENAMEFCB       equ 30   ; INT 21h AH=17h
SVC_DELETE          equ 31   ; INT 21h AH=41h
SVC_MOVE            equ 32   ; INT 21h AH=56h
SVC_OPENFCB         equ 37   ; INT 21h AH=0Fh
SVC_CLOSEFCB        equ 39   ; INT 21h AH=10h
SVC_SETVERIFY       equ 44   ; INT 21h AH=2Eh
SVC_FINDNEXT        equ 45   ; INT 21h AH=4Fh
SVC_CREATE          equ 46   ; INT 21h AH=3Ch
SVC_CREATETEMP      equ 47   ; INT 21h AH=5Ah
SVC_PARSEFILENAME   equ 49   ; INT 21h AH=29h
SVC_ABSDISKREAD     equ 61   ; INT 25h
SVC_ABSDISKWRITE    equ 62   ; INT 26h

;==============================================================================
; SVC Macros
;==============================================================================

;-----------------------------------------------------------------------------
; @SvcDupHandle - Duplicate file handle (SVC 0, INT 21h AH=45h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:0
; Input: HANDLE - handle to duplicate
; Output: AX = new handle
;-----------------------------------------------------------------------------
@SvcDupHandle MACRO HANDLE
        IFNB <HANDLE>
            MOV BX, HANDLE
        ENDIF
        SVCCALL SVC_DUPHANDLE
ENDM

;-----------------------------------------------------------------------------
; @SvcErrorExit - Terminate MVM with error message (SVC 1)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:1
; Input: MSG - far pointer to ASCIIZ message (must be pushed before call)
; Note: The far pointer must be on top of stack before invoking this macro.
;       Example: PUSH DS, PUSH OFFSET msg, @SvcErrorExit
;-----------------------------------------------------------------------------
@SvcErrorExit MACRO
        SVCCALL SVC_ERROREXIT
ENDM

;-----------------------------------------------------------------------------
; @SvcExit - Terminate MVM and return code (SVC 2, INT 21h AH=4Ch)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:2
; Input: RETURN - return code in AL (optional, default 0)
;-----------------------------------------------------------------------------
@SvcExit MACRO RETURN
        IFNB <RETURN>
            MOV AL, RETURN
        ELSE
            XOR AL, AL
        ENDIF
        SVCCALL SVC_EXIT
ENDM

;-----------------------------------------------------------------------------
; @SvcClose - Close file handle (SVC 3, INT 21h AH=3Eh)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:3
; Input: HANDLE - file handle
;-----------------------------------------------------------------------------
@SvcClose MACRO HANDLE
        IFNB <HANDLE>
            MOV BX, HANDLE
        ENDIF
        SVCCALL SVC_CLOSE
ENDM

;-----------------------------------------------------------------------------
; @SvcResetDisk - Reset disk buffers (SVC 4, INT 21h AH=0Dh)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:4
;-----------------------------------------------------------------------------
@SvcResetDisk MACRO
        SVCCALL SVC_RESETDISK
ENDM

;-----------------------------------------------------------------------------
; @SvcGetPSP - Get current PSP (SVC 5, INT 21h AH=62h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:5
; Output: BX = PSP segment
;-----------------------------------------------------------------------------
@SvcGetPSP MACRO
        SVCCALL SVC_GETPSP
ENDM

;-----------------------------------------------------------------------------
; @SvcSelectDrive - Select default drive (SVC 6, INT 21h AH=0Eh)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:6
; Input: DRIVE - drive number (0=A, 1=B, ...)
; Output: AL = number of logical drives
;-----------------------------------------------------------------------------
@SvcSelectDrive MACRO DRIVE
        IFNB <DRIVE>
            MOV DL, DRIVE
        ENDIF
        SVCCALL SVC_SELECTDRIVE
ENDM

;-----------------------------------------------------------------------------
; @SvcSetDTA - Set Disk Transfer Address (SVC 7, INT 21h AH=1Ah)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:7
; Input: DTA - address of DTA buffer (DS:DX)
;        SEGMNT - optional segment for DTA (default DS)
;-----------------------------------------------------------------------------
@SvcSetDTA MACRO DTA, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <DTA>
            MOV DX, OFFSET DTA
        ENDIF
        SVCCALL SVC_SETDTA
ENDM

;-----------------------------------------------------------------------------
; @SvcGetVersion - Get DOS version (SVC 8, INT 21h AH=30h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:8
; Output: AL = major, AH = minor, BH = OEM, BL:CX = serial
;-----------------------------------------------------------------------------
@SvcGetVersion MACRO
        SVCCALL SVC_GETVERSION
ENDM

;-----------------------------------------------------------------------------
; @SvcWrite - Write to file handle (SVC 9, INT 21h AH=40h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:9
; Input: HANDLE - file handle
;        BUFFER - data buffer
;        BYTES  - number of bytes to write
;        SEGMNT - optional segment of buffer (default DS)
; Output: AX = actual bytes written
;-----------------------------------------------------------------------------
@SvcWrite MACRO HANDLE, BUFFER, BYTES, SEGMNT
        IFNB <HANDLE>
            MOV BX, HANDLE
        ENDIF
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <BUFFER>
            MOV DX, OFFSET BUFFER
        ENDIF
        IFNB <BYTES>
            MOV CX, BYTES
        ENDIF
        SVCCALL SVC_WRITE
ENDM

;-----------------------------------------------------------------------------
; @SvcSetDate - Set system date (SVC 10, INT 21h AH=2Bh)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:10
; Input: YEAR - year (1980-2099)
;        MONTH - month (1-12)
;        DAY - day (1-31)
; Output: AL = 0 if success, FFh if error
;-----------------------------------------------------------------------------
@SvcSetDate MACRO YEAR, MONTH, DAY
        IFNB <YEAR>
            MOV CX, YEAR
        ENDIF
        IFNB <MONTH>
            MOV DH, MONTH
        ENDIF
        IFNB <DAY>
            MOV DL, DAY
        ENDIF
        SVCCALL SVC_SETDATE
ENDM

;-----------------------------------------------------------------------------
; @SvcSetTime - Set system time (SVC 11, INT 21h AH=2Dh)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:11
; Input: HOUR - hour (0-23)
;        MINUTE - minute (0-59)
;        SECOND - second (0-59)
;        HUNDREDTH - hundredths (0-99)
; Output: AL = 0 if success, FFh if error
;-----------------------------------------------------------------------------
@SvcSetTime MACRO HOUR, MINUTE, SECOND, HUNDREDTH
        IFNB <HOUR>
            MOV CH, HOUR
        ENDIF
        IFNB <MINUTE>
            MOV CL, MINUTE
        ENDIF
        IFNB <SECOND>
            MOV DH, SECOND
        ENDIF
        IFNB <HUNDREDTH>
            MOV DL, HUNDREDTH
        ENDIF
        SVCCALL SVC_SETTIME
ENDM

;-----------------------------------------------------------------------------
; @SvcGetDate - Get system date (SVC 12, INT 21h AH=2Ah)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:12
; Output: CX = year, DH = month, DL = day, AL = day of week
;-----------------------------------------------------------------------------
@SvcGetDate MACRO
        SVCCALL SVC_GETDATE
ENDM

;-----------------------------------------------------------------------------
; @SvcGetTime - Get system time (SVC 13, INT 21h AH=2Ch)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:13
; Output: CH = hour, CL = minute, DH = second, DL = hundredth
;-----------------------------------------------------------------------------
@SvcGetTime MACRO
        SVCCALL SVC_GETTIME
ENDM

;-----------------------------------------------------------------------------
; @SvcRead - Read from file handle (SVC 14, INT 21h AH=3Fh)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:14
; Input: HANDLE - file handle
;        BUFFER - data buffer
;        BYTES  - number of bytes to read
;        SEGMNT - optional segment of buffer (default DS)
; Output: AX = actual bytes read
;-----------------------------------------------------------------------------
@SvcRead MACRO HANDLE, BUFFER, BYTES, SEGMNT
        IFNB <HANDLE>
            MOV BX, HANDLE
        ENDIF
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <BUFFER>
            MOV DX, OFFSET BUFFER
        ENDIF
        IFNB <BYTES>
            MOV CX, BYTES
        ENDIF
        SVCCALL SVC_READ
ENDM

;-----------------------------------------------------------------------------
; @SvcChgFilePtr - Move file pointer (SVC 15, INT 21h AH=42h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:15
; Input: HANDLE - file handle
;        HIGH   - high word of offset
;        LOW    - low word of offset
;        METHOD - method: 0=begin, 1=current, 2=end
; Output: DX:AX = new file pointer
;-----------------------------------------------------------------------------
@SvcChgFilePtr MACRO HANDLE, HIGH, LOW, METHOD
        IFNB <HANDLE>
            MOV BX, HANDLE
        ENDIF
        IFNB <HIGH>
            MOV CX, HIGH
        ENDIF
        IFNB <LOW>
            MOV DX, LOW
        ENDIF
        IFNB <METHOD>
            MOV AL, METHOD
        ENDIF
        SVCCALL SVC_CHGFILEPTR
ENDM

;-----------------------------------------------------------------------------
; @SvcRmDir - Remove directory (SVC 16, INT 21h AH=3Ah)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:16
; Input: PATH - ASCIZ path
;        SEGMNT - optional segment (default DS)
;-----------------------------------------------------------------------------
@SvcRmDir MACRO PATH, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <PATH>
            MOV DX, OFFSET PATH
        ENDIF
        SVCCALL SVC_RMDIR
ENDM

;-----------------------------------------------------------------------------
; @SvcMkDir - Create directory (SVC 17, INT 21h AH=39h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:17
; Input: PATH - ASCIZ path
;        SEGMNT - optional segment (default DS)
;-----------------------------------------------------------------------------
@SvcMkDir MACRO PATH, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <PATH>
            MOV DX, OFFSET PATH
        ENDIF
        SVCCALL SVC_MKDIR
ENDM

;-----------------------------------------------------------------------------
; @SvcQCurDir - Get current directory (SVC 18, INT 21h AH=47h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:18
; Input: DRIVE - drive number (0=default, 1=A, ...)
;        BUFFER - buffer for path
;        SEGMNT - optional segment of buffer (default DS)
; Output: buffer filled with ASCIZ path
;-----------------------------------------------------------------------------
@SvcQCurDir MACRO DRIVE, BUFFER, SEGMNT
        IFNB <DRIVE>
            MOV DL, DRIVE
        ENDIF
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <BUFFER>
            MOV SI, OFFSET BUFFER
        ENDIF
        SVCCALL SVC_QCURDIR
ENDM

;-----------------------------------------------------------------------------
; @SvcChDir - Change current directory (SVC 19, INT 21h AH=3Bh)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:19
; Input: PATH - ASCIZ path
;        SEGMNT - optional segment (default DS)
;-----------------------------------------------------------------------------
@SvcChDir MACRO PATH, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <PATH>
            MOV DX, OFFSET PATH
        ENDIF
        SVCCALL SVC_CHDIR
ENDM

;-----------------------------------------------------------------------------
; @SvcGetSetFileTime - Get/Set file time (SVC 20, INT 21h AH=57h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:20
; Input: HANDLE - file handle
;        ACTION - 0=get, 1=set
;        TIME   - new time (for set)
;        DATE   - new date (for set)
; Output: on get: CX = time, DX = date
;-----------------------------------------------------------------------------
@SvcGetSetFileTime MACRO HANDLE, ACTION, TIME, DATE
        IFNB <HANDLE>
            MOV BX, HANDLE
        ENDIF
        IFNB <ACTION>
            MOV AL, ACTION
        ENDIF
        IFNB <TIME>
            MOV CX, TIME
        ENDIF
        IFNB <DATE>
            MOV DX, DATE
        ENDIF
        SVCCALL SVC_GETSETFILETIME
ENDM

;-----------------------------------------------------------------------------
; @SvcCommitFile - Commit file buffers (SVC 21, INT 21h AH=68h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:21
; Input: HANDLE - file handle
;-----------------------------------------------------------------------------
@SvcCommitFile MACRO HANDLE
        IFNB <HANDLE>
            MOV BX, HANDLE
        ENDIF
        SVCCALL SVC_COMMITFILE
ENDM

;-----------------------------------------------------------------------------
; @SvcFindFirstFCB - Find first file using FCB (SVC 22, INT 21h AH=11h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:22
; Input: FCB - address of FCB
;        SEGMNT - optional segment of FCB (default DS)
; Output: AL = 0 if found, FFh if error
;-----------------------------------------------------------------------------
@SvcFindFirstFCB MACRO FCB, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <FCB>
            MOV DX, OFFSET FCB
        ENDIF
        SVCCALL SVC_FINDFIRSTFCB
ENDM

;-----------------------------------------------------------------------------
; @SvcFindNextFCB - Find next file using FCB (SVC 23, INT 21h AH=12h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:23
; Input: FCB - address of FCB
;        SEGMNT - optional segment of FCB (default DS)
; Output: AL = 0 if found, FFh if error
;-----------------------------------------------------------------------------
@SvcFindNextFCB MACRO FCB, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <FCB>
            MOV DX, OFFSET FCB
        ENDIF
        SVCCALL SVC_FINDNEXTFCB
ENDM

;-----------------------------------------------------------------------------
; @SvcFindFirst - Find first file (SVC 25, INT 21h AH=4Eh)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:25
; Input: PATH - ASCIZ path (may include wildcards)
;        ATTRIB - file attributes
;        SEGMNT - optional segment of path (default DS)
;-----------------------------------------------------------------------------
@SvcFindFirst MACRO PATH, ATTRIB, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <PATH>
            MOV DX, OFFSET PATH
        ENDIF
        IFNB <ATTRIB>
            MOV CX, ATTRIB
        ELSE
            XOR CX, CX
        ENDIF
        SVCCALL SVC_FINDFIRST
ENDM

;-----------------------------------------------------------------------------
; @SvcOpen - Open file (SVC 28, INT 21h AH=3Dh)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:28
; Input: PATH - ASCIZ path
;        ACCESS - access mode (see DOS)
;        SEGMNT - optional segment of path (default DS)
; Output: AX = file handle (if CF clear) or error code
;-----------------------------------------------------------------------------
@SvcOpen MACRO PATH, ACCESS, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <PATH>
            MOV DX, OFFSET PATH
        ENDIF
        IFNB <ACCESS>
            MOV AL, ACCESS
        ENDIF
        SVCCALL SVC_OPEN
ENDM

;-----------------------------------------------------------------------------
; @SvcDeleteFCB - Delete file using FCB (SVC 29, INT 21h AH=13h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:29
; Input: FCB - address of FCB
;        SEGMNT - optional segment of FCB (default DS)
; Output: AL = 0 if success, FFh if error
;-----------------------------------------------------------------------------
@SvcDeleteFCB MACRO FCB, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <FCB>
            MOV DX, OFFSET FCB
        ENDIF
        SVCCALL SVC_DELETEFCB
ENDM

;-----------------------------------------------------------------------------
; @SvcRenameFCB - Rename file using FCB (SVC 30, INT 21h AH=17h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:30
; Input: FCB - address of FCB (modified with new name)
;        SEGMNT - optional segment of FCB (default DS)
; Output: AL = 0 if success, FFh if error
;-----------------------------------------------------------------------------
@SvcRenameFCB MACRO FCB, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <FCB>
            MOV DX, OFFSET FCB
        ENDIF
        SVCCALL SVC_RENAMEFCB
ENDM

;-----------------------------------------------------------------------------
; @SvcDelete - Delete file (SVC 31, INT 21h AH=41h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:31
; Input: PATH - ASCIZ path
;        SEGMNT - optional segment of path (default DS)
;-----------------------------------------------------------------------------
@SvcDelete MACRO PATH, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <PATH>
            MOV DX, OFFSET PATH
        ENDIF
        SVCCALL SVC_DELETE
ENDM

;-----------------------------------------------------------------------------
; @SvcMove - Rename file (SVC 32, INT 21h AH=56h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:32
; Input: OLD - old ASCIZ path
;        NEW - new ASCIZ path
;        SEGOLD - optional segment of old path (default DS)
;        SEGNEW - optional segment of new path (default DS)
;-----------------------------------------------------------------------------
@SvcMove MACRO OLD, NEW, SEGOLD, SEGNEW
        IFNB <SEGOLD>
            MOV AX, SEGOLD
            MOV DS, AX
        ENDIF
        MOV DX, OFFSET OLD
        IFNB <SEGNEW>
            MOV AX, SEGNEW
            MOV ES, AX
        ENDIF
        MOV DI, OFFSET NEW
        SVCCALL SVC_MOVE
ENDM

;-----------------------------------------------------------------------------
; @SvcOpenFCB - Open file using FCB (SVC 37, INT 21h AH=0Fh)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:37
; Input: FCB - address of unopened FCB
;        SEGMNT - optional segment of FCB (default DS)
; Output: AL = 0 if success, FFh if error
;-----------------------------------------------------------------------------
@SvcOpenFCB MACRO FCB, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <FCB>
            MOV DX, OFFSET FCB
        ENDIF
        SVCCALL SVC_OPENFCB
ENDM

;-----------------------------------------------------------------------------
; @SvcCloseFCB - Close file using FCB (SVC 39, INT 21h AH=10h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:39
; Input: FCB - address of opened FCB
;        SEGMNT - optional segment of FCB (default DS)
; Output: AL = 0 if success, FFh if error
;-----------------------------------------------------------------------------
@SvcCloseFCB MACRO FCB, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <FCB>
            MOV DX, OFFSET FCB
        ENDIF
        SVCCALL SVC_CLOSEFCB
ENDM

;-----------------------------------------------------------------------------
; @SvcSetVerify - Set verify flag (SVC 44, INT 21h AH=2Eh)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:44
; Input: STATE - 0=off, 1=on
;-----------------------------------------------------------------------------
@SvcSetVerify MACRO STATE
        IFNB <STATE>
            MOV AL, STATE
        ENDIF
        SVCCALL SVC_SETVERIFY
ENDM

;-----------------------------------------------------------------------------
; @SvcFindNext - Find next file (SVC 45, INT 21h AH=4Fh)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:45
;-----------------------------------------------------------------------------
@SvcFindNext MACRO
        SVCCALL SVC_FINDNEXT
ENDM

;-----------------------------------------------------------------------------
; @SvcCreate - Create new file (SVC 46, INT 21h AH=3Ch)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:46
; Input: PATH - ASCIZ path
;        ATTRIB - file attributes
;        SEGMNT - optional segment of path (default DS)
; Output: AX = file handle (if CF clear) or error code
;-----------------------------------------------------------------------------
@SvcCreate MACRO PATH, ATTRIB, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <PATH>
            MOV DX, OFFSET PATH
        ENDIF
        IFNB <ATTRIB>
            MOV CX, ATTRIB
        ELSE
            XOR CX, CX
        ENDIF
        SVCCALL SVC_CREATE
ENDM

;-----------------------------------------------------------------------------
; @SvcCreateTemp - Create temporary file (SVC 47, INT 21h AH=5Ah)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:47
; Input: PATH - ASCIZ path (directory), will be modified with unique name
;        ATTRIB - file attributes
;        SEGMNT - optional segment of path (default DS)
; Output: AX = file handle (if CF clear) or error code
;-----------------------------------------------------------------------------
@SvcCreateTemp MACRO PATH, ATTRIB, SEGMNT
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <PATH>
            MOV DX, OFFSET PATH
        ENDIF
        IFNB <ATTRIB>
            MOV CX, ATTRIB
        ELSE
            XOR CX, CX
        ENDIF
        SVCCALL SVC_CREATETEMP
ENDM

;-----------------------------------------------------------------------------
; @SvcParseFileName - Parse filename into FCB (SVC 49, INT 21h AH=29h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:49
; Input: STRING - ASCIZ filename
;        FCB - FCB buffer
;        SEGSTR - optional segment of string (default DS)
;        SEGFCB - optional segment of FCB (default ES)
;-----------------------------------------------------------------------------
@SvcParseFileName MACRO STRING, FCB, SEGSTR, SEGFCB
        IFNB <SEGSTR>
            MOV AX, SEGSTR
            MOV DS, AX
        ENDIF
        MOV SI, OFFSET STRING
        IFNB <SEGFCB>
            MOV AX, SEGFCB
            MOV ES, AX
        ELSE
            PUSH DS
            POP ES
        ENDIF
        MOV DI, OFFSET FCB
        MOV AL, 0Fh   ; control flags (scan leading separators, set all fields)
        SVCCALL SVC_PARSEFILENAME
ENDM

;-----------------------------------------------------------------------------
; @SvcAbsDiskRead - Absolute disk read (SVC 61, INT 25h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:61
; Input: DRIVE - drive number (0=A, 1=B, ...)
;        BUFFER - data buffer
;        NUM_SECTORS - number of sectors to read
;        FIRST_SECTOR - starting sector
;        SEGMNT - optional segment of buffer (default DS)
; Output: CF set on error, error code in AL
; Note: After call, FLAGS are popped automatically? In DOS we need POPF.
;       SVC may handle differently. This macro does not modify flags.
;-----------------------------------------------------------------------------
@SvcAbsDiskRead MACRO DRIVE, BUFFER, NUM_SECTORS, FIRST_SECTOR, SEGMNT
        IFNB <DRIVE>
            MOV AL, DRIVE
        ENDIF
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <BUFFER>
            MOV BX, OFFSET BUFFER
        ENDIF
        IFNB <NUM_SECTORS>
            MOV CX, NUM_SECTORS
        ENDIF
        IFNB <FIRST_SECTOR>
            MOV DX, FIRST_SECTOR
        ENDIF
        SVCCALL SVC_ABSDISKREAD
ENDM

;-----------------------------------------------------------------------------
; @SvcAbsDiskWrite - Absolute disk write (SVC 62, INT 26h)
;   Documentation: https://osfree.org/doku/en:docs:mvm:api:62
; Input: same as @SvcAbsDiskRead
; Output: CF set on error, error code in AL
;-----------------------------------------------------------------------------
@SvcAbsDiskWrite MACRO DRIVE, BUFFER, NUM_SECTORS, FIRST_SECTOR, SEGMNT
        IFNB <DRIVE>
            MOV AL, DRIVE
        ENDIF
        IFNB <SEGMNT>
            MOV AX, SEGMNT
            MOV DS, AX
        ENDIF
        IFNB <BUFFER>
            MOV BX, OFFSET BUFFER
        ENDIF
        IFNB <NUM_SECTORS>
            MOV CX, NUM_SECTORS
        ENDIF
        IFNB <FIRST_SECTOR>
            MOV DX, FIRST_SECTOR
        ENDIF
        SVCCALL SVC_ABSDISKWRITE
ENDM

ENDIF   ; SVC_DEFINED
