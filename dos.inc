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

.8086

terminate  macro
           int 20H
           endm

abs_disk_read  macro  disk,buffer,num_sectors,first_sector
                    mov    al,disk
                    mov    bx,offset buffer
                    mov    cx,num_sectors
                    mov    dx,first_sector
                    int    25H
                    popf
                    endm

abs_disk_write  macro  disk,buffer,num_sectors,first_sector
                    mov    al,disk
                    mov    bx,offset buffer
                    mov    cx,num_sectors
                    mov    dx,first_sector
                    int    26H
                    popf
                    endm


flush macro handle
		mov bx, handle
		mov ah,68h
		int 21h
		endm

read_kbd_and_echo  macro
                        mov  ah, 01H
                        int  21H
                        endm

display_char macro   character
                mov     dl,character
                mov     ah,02H
                int     21H
                endm

aux_input  macro
                mov  ah,03H
                int  21H
                endm

aux_output  macro  character
                mov  dl,character
                mov  ah,04H
                int  21H
                endm


dir_console_io  macro switch
                    mov  dl,switch
                    mov  ah,06H
                    int  21H
                    endm

dir_console_input  macro
                        mov  ah,07H
                        int  21H
                        endm

read_kbd  macro
            mov   ah,08H
            int   21H
            endm

get_string  macro  limit,string
                mov    dx,offset string
                mov    string,limit
                mov    ah,0AH
                int    21H
                endm

check_kbd_status  macro
                    mov   ah,0BH
                    int   21H
                    endm

open  macro fcb
        mov   dx,offset fcb
        mov   ah,0FH
        int   21H
        endm

close  macro fcb
            mov   dx,offset fcb
            mov   ah,10H
            int   21H
            endm

search_first  macro fcb
                mov   dx,offset fcb
                mov   ah,11H
                int   21H
                endm

search_next  macro fcb
                mov   dx,offset fcb
                mov   ah,12H
                int   21H
                endm

delete  macro  fcb
            mov    dx,offset fcb
            mov    ah,13H
            int    21H
            endm

read_seq  macro fcb
            mov   dx,offset fcb
            mov   ah,14H
            int   21H
            endm

write_seq  macro fcb
                mov   dx,offset fcb
                mov   ah,15H
                int   21H
                endm

create  macro fcb
            mov   dx,offset fcb
            mov   ah,16H
            int   21H
            endm

set_dta  macro  buffer
            mov    dx,offset buffer
            mov    ah,1AH
            int    21H
            endm

def_drive_data  macro
                    push   ds
                    mov    ah,1BH
                    int    21H
                    mov    al,byte ptr[bx]
                    pop    ds
                    endm

drive_data  macro  drive
                push   ds
                mov    dl,drive
                mov    ah,1BH
                int    21H
                mov    al, byte ptr[bx]
                pop    ds
                endm

read_ran  macro  fcb
            mov    dx,offset fcb
            mov    ah,21H
            int    21H
            endm

write_ran  macro  fcb
                mov    dx,offset fcb
                mov    ah,22H
                int    21H
                endm

file_size  macro  fcb
                mov    dx,offset fcb
                mov    ah,23H
                int    21H
                endm

set_relative_record  macro  fcb
                        mov    dx,offset fcb
                        mov    ah,24H
                        int    21H
                        endm

set_vector  macro  interrupt,handler_start
                mov    al,interrupt
                mov    dx,offset handler_start
                mov    ah,25H
                endm

create_psp  macro  seg_addr
                mov    dx,seg_addr
                mov    ah,26H
                endm

ran_block_read  macro  fcb,count,rec_size
                    mov    dx,offset fcb
                    mov    cx,count
                    mov    word ptr fcb[14],rec_size
                    mov    ah,27H
                    int    21H
                    endm

ran_block_write  macro  fcb,count,rec_size
                    mov    dx,offset fcb
                    mov    cx,count
                    mov    word ptr fcb[14],rec_size
                    mov    ah,28H
                    int    21H
                    endm

parse macro string,fcb
        mov   si,offset string
        mov   di,offset fcb
        push  es
        push  ds
        pop   es
        mov   al,0FH        ; bits 0-3 on
        mov   ah,29H
        int   21H
        pop   es
        endm

get_dta  macro
            mov    ah,2fH
            int    21H
            endm


keep_process  macro return_code,last_byte
                mov   al,return_code
                mov   dx,offset last_byte
                mov   cl,4
                shr   dx,cl
                inc   dx
                mov   ah,31H
                int   21H
                endm

ctrl_c_ck  macro  action,state
                mov    al,action
                mov    dl,state
                mov    ah,33H
                int    21H
                endm

get_vector  macro  interrupt
                mov    al,interrupt
                mov    ah,35H
                int    21H
                endm

get_disk_space  macro  drive
                    mov    dl,drive
                    mov    ah,36H
                    int    21H
                    endm

get_country macro  country,buffer
                local     gc_01
                mov       dx,offset buffer
                mov       ax,country
                cmp       ax,OFFH
                jl        gc_01
                mov       al,OFFh
                mov       bx,country
    gc_01:      mov       ah,38h
                int       21H
                endm

set_country macro  country
                local  sc_01
                mov    dx,0FFFFH
                mov    ax,country
                cmp    ax,0FFH
                jl     sc_01
                mov    bx,country
                mov    al,0ffh
    sc_01:      mov    ah,38H
                int    21H
                endm


read_handle  macro  handle,buffer,bytes
                mov    bx,handle
                mov    dx,offset buffer
                mov    cx,bytes
                mov    ah,3FH
                int    21H
                endm

ioctl_data macro  code,handle
                mov    bx,handle
                mov    al,code
                mov    ah,44H
                int    21H
                endm

ioctl_char  macro  code,handle,buffer
                mov    bx,handle
                mov    dx,offset buffer
                mov    al,code
                mov    ah,44H
                int    21H
                endm

ioctl_block macro  code,drive,buffer
                mov    bl,drive
                mov    dx,offset buffer
                mov    al,code
                mov    ah,44H
                int    21H
                endm

ioctl_status  macro  code,handle
                mov    bx,handle
                mov    al,code
                mov    ah,44H
                int    21H
                endm

ioctl_change  macro  drive
                mov    bl, drive
                mov    al, 08H
                mov    ah, 44H
                int    21H
                endm

ioctl_rblock  macro  drive
                mov    bl, drive
                mov    al, 09H
                mov    ah, 44H
                int    21H
                endm

ioctl_rhandle macro  handle
                mov    bx, handle
                mov    al, 0AH
                mov    ah, 44H
                int    21H
                endm

ioctl_retry  macro  retries, wait
                mov    dx, retries
                mov    cx, wait
                mov    al, 0BH
                mov    ah, 44H
                int    21H
                endm

ioctl_handles        macro handle,function,category,buffer
                    mov ch,05H
                    mov cl,function
                    mov dx,offset buffer
                    mov bx,handle
                    mov ah,44H
                    mov al,0CH
                    int 21H
                    endm


set_block  macro   last_byte
                mov     bx,offset last_byte
                mov     cl,4
                shr     bx,cl
                add     bx,17
                mov     ah,4AH
                int     21H
                mov     ax,bx
                shl     ax,cl
                dec     ax
                mov     sp,ax
                mov     bp,sp
                endm

exec  macro  path,command,parms
        mov    dx,offset path
        mov    bx,offset parms
        mov    word ptr parms[02H],offset command
        mov    word ptr parms[04H],cs
        mov    word ptr parms[06H],5CH
        mov    word ptr parms[08H],es
        mov    word ptr parms[0AH],6CH
        mov    word ptr parms[0CH],es
        mov    al,0
        mov    ah,4BH
        int    21H
        endm

exec_ovl  macro  path,parms,seg_addr
            mov    dx,offset path
            mov    bx,offset parms
            mov    parms,seg_addr
            mov    parms[02H],seg_addr
            mov    al,3
            mov    ah,4BH
            int    21H
            endm

find_first_file  macro  path,attrib
                    mov    dx,offset path
                    mov    cx,attrib
                    mov    ah,4EH
                    int    21H
                    endm

find_next_file  macro
                    mov    ah,4FH
                    int    21H
                    endm

alloc_strat macro  code,strategy
                mov    bx,strategy
                mov    al,code
                mov    ah,58H
                int    21H
                endm

create_temp macro  pathname,attrib
                mov    cx,attrib
                mov    dx,offset pathname
                mov    ah,5AH
                int    21H
                endm

create_new macro  pathname,attrib
                mov    cx, attrib
                mov    dx, offset pathname
                mov    ah, 5BH
                int    21H
                endm

file_unlock  macro  handle,start,bytes
            mov    bx, handle
            mov    cx, word ptr start
            mov    dx, word ptr start+2
            mov    si, word ptr bytes
            mov    di, word ptr bytes+2
            mov    al, 1
            mov    ah, 5CH
            int    21H
            endm

get_machine_name  macro  buffer
                    mov    dx,offset buffer
                    mov    al,0
                    mov    ah,5EH
                    int    21H
                    endm

printer_setup  macro  index,lgth,string
                    mov    bx, index
                    mov    cx, lgth
                    mov    dx, offset string
                    mov    al, 2
                    mov    ah, 5EH
                    int    21H
                    endm

get_list  macro  index,local,remote
            mov    bx, index
            mov    si, offset local
            mov    di, offset remote
            mov    al,2
            mov    ah, 5FH
            int    21H
            endm

redir macro  device,value,source,destination
        mov    bl, device
        mov    cx, value
        mov    si, offset source
        mov    es, seg destination
        mov    di, offset destination
        mov    al, 03H
        mov    ah, 5FH
        int    21H
        endm

cancel_redir  macro  local
                mov    si, offset local
                mov    al, 4
                mov    ah, 5FH
                int    21H
                endm


;  Macro Definitions for MS-DOS System Call Examples
;
; *******************
;  Interrupts
; *******************
;                                 Interrupt 25H
    ABS_DISK_READ  macro  disk,buffer,num_sectors,first_sector
        mov    al,disk
        mov    bx,offset buffer
        mov    cx,num_sectors
        mov    dx,first_sector
        int    25H
        popf
        endm
    ;                                 Interrupt 26H
    ABS_DISK_WRITE  macro  disk,buffer,num_sectors,first_sector
        mov    al,disk
        mov    bx,offset buffer
        mov    cx,num_sectors
        mov    dx,first_sector
        int    26H
        popf
        endm
    ;                                 Interrupt 27H

    STAY_RESIDENT  macro  last_instruc
        mov    dx,offset last_instruc
        inc    dx
        int    27H
        endm
    ;
    ;
    ; *******************
    ;  Function Requests
    ; *******************
    ;                                 Function Request 00H
    TERMINATE_PROGRAM  macro
        xor    ah,ah
        int    21H
        endm
    ;                                 Function Request 01H
    READ_KBD_AND_ECHO  macro
        mov    ah,01H
        int    21H
        endm
    ;                                 Function Request 02H
    DISPLAY_CHAR  macro  character
        mov    dl,character
        mov    ah,02H
        int    21H
        endm
    ;                                 Function Request 03H
    AUX_INPUT    macro
        mov     ah,03H
        int     21H
        endm
    ;                                 Function Request 04H
    AUX_OUTPUT  macro
        mov    ah,04H
        int    21H
        endm

;                                 Function Request 05H
PRINT_CHAR  macro  character
        mov    dl,character
        mov    ah,05H
        int    21H
        endm
    ;                                 Function Request 06H
    DIR_CONSOLE_IO  macro  switch
        mov    dl,switch
        mov    ah,06H
        int    21H
        endm
    ;                                 Function Request 07H
    DIR_CONSOLE_INPUT  macro
        mov    ah,07H
        int    21H
        endm
    ;                                 Function Request 08H
    READ_KBD  macro
        mov    ah,08H
        int    21H
        endm
    ;                                 Function Request 09H
    DISPLAY  macro  string
        mov    dx,offset string
        mov    ah,09H
        int    21H
        endm
    ;                                 Function Request 0AH
    GET_STRING  macro  limit,string
        mov    dx,offset string
        mov    string,limit
        mov    ah,0AH
        int    21H
        endm
    ;                                 Function Request 0BH
    CHECK_KBD_STATUS  macro
        mov    ah,0BH
        int    21H
        endm
;                                 Function Request 0CH
FLUSH_AND_READ_KBD  macro  switch
        mov    al,switch
        mov    ah,0CH
        int    21H
        endm

;                                 Function Request 0DH
RESET_DISK  macro
        mov    ah,0DH
        int    21H
        endm

LFN_RESET_DISK macro disk
	MOV	AX, 710dH
	MOV	CX, 2
	MOV	DX, disk
	int	21h
	endm
;                                 Function Request 0EH
SELECT_DISK  macro  disk
        mov    dl,disk
	SUB	DL, 65		; 'A'
        mov    ah,0EH
        int    21H
        endm

    ;                                 Function Request 0FH
    OPEN  macro  fcb
        mov    dx,offset fcb
        mov    ah,0FH
        int    21H
        endm
    ;                                 Function Request 10H
    CLOSE  macro  fcb
        mov    dx,offset fcb
        mov    ah,10H
        int    21H
        endm
    ;                                 Function Request 11H
    SEARCH_FIRST  macro  fcb
        mov    dx,offset fcb
        mov    ah,11H
        int    21H
        endm
    ;                                 Function Request 12H
    SEARCH_NEXT  macro  fcb
        mov    dx,offset fcb
        mov    ah,12H
        int    21H
        endm
    ;                                 Function Request 13H
    DELETE  macro  fcb
        mov    dx,offset fcb
        mov    ah,13H
        int    21H
        endm
    ;                                 Function Request 14H
    READ_SEQ  macro  fcb
        mov    dx,offset fcb
        mov    ah,14H
        int    21H
        endm
    ;                                 Function Request 15H
    WRITE_SEQ  macro  fcb
        mov    dx,offset fcb
        mov    ah,15H
        int    21H
        endm
    ;                                 Function Request 16H
    CREATE  macro  fcb
        mov    dx,offset fcb
        mov    ah,16H
        int    21H
        endm
    ;                                 Function Request 17H
    RENAME  macro  fcb,newname
        mov    dx,offset fcb
        mov    ah,17H
        int    21H
        endm

;                                 Function Request 19H
CURRENT_DISK  macro
        mov    ah,19H
        int    21H
        endm

    ;                                 Function Request 1AH
    SET_DTA  macro  buffer
        mov    dx,offset buffer
        mov    ah,1AH
        endm
    ;                                 Function Request 1BH
    DEF_DRIVE_DATA  macro
        mov    ah,1BH
        int    21H
        endm
    ;                                 Function Request 1CH
    DRIVE_DATA  macro  drive
        mov    dl,drive
        mov    ah,1CH
        int    21H
        endm
    ;                                 Function Request 21H
    READ_RAN  macro  fcb
        mov    dx,offset fcb
        mov    ah,21H
        int    21H
        endm
    ;                                 Function Request 22H
    WRITE_RAN  macro  fcb
        mov    dx,offset fcb
        mov    ah,22H
        int    21H
        endm
    ;                                 Function Request 23H
    FILE_SIZE  macro  fcb
        mov    dx,offset fcb
        mov    ah,23H
        int    21H
        endm
    ;                                 Function Request 24H
    SET_RELATIVE_RECORD  macro  fcb
        mov    dx,offset fcb
        mov    ah,24H
        int    21H
        endm
    ;                                 Function Request 25H
    SET_VECTOR  macro  interrupt,handler_start
        mov    al,interrupt
        mov    dx,offset handler_start
        mov    ah,25H
        int    21H
        endm
    ;                                 Function Request 26H
    CREATE_PSP  macro  seg_addr
        mov    dx,offset seg_addr
        mov    ah,26H
        int    21H
        endm
    ;                                  Function Request 27H
    RAN_BLOCK_READ  macro  fcb,count,rec_size
        mov    dx,offset fcb
        mov    cx,count
        mov    word ptr fcb[14],rec_size
        mov    ah,27H
        int    21H
        endm
    ;                                  Function Request 28H
    RAN_BLOCK_WRITE  macro  fcb,count,rec_size
        mov    dx,offset fcb
        mov    cx,count
        mov    word ptr fcb[14],rec_size
        mov    ah,28H
        int    21H
        endm
    ;                                 Function Request 29H
    PARSE  macro  string,fcb
        mov    si,offset string
        mov    di,offset fcb
        push   es
        push   ds
        pop    es
        mov    al,0FH
        mov    ah,29H
        int    21H
        pop    es
        endm

;                                  Function Request 2AH
GET_DATE  macro
        mov    ah,2AH
        int    21H
        endm

;                                  Function Request 2BH
SET_DATE  macro  year,month,day
        mov    cx,year
        mov    dh,month
        mov    dl,day
        mov    ah,2BH
        int    21H
        endm

;                                  Function Request 2CH
GET_TIME  macro
        mov    ah,2CH
        int    21H
        endm

;                                  Function Request 2DH
SET_TIME  macro  hour,minutes,seconds,hundredths
        mov    ch,hour
        mov    cl,minutes
        mov    dh,seconds
        mov    dl,hundredths
        mov    ah,2DH
        int    21H
        endm

;                                  Function Request 2EH
VERIFY  macro  switch
        mov    al,switch
        mov    ah,2EH
        int    21H
        endm

    ;                                  Function Request 2FH
    GET_DTA  macro
        mov    ah,2FH
        int    21H
        endm

;                                  Function Request 30H
GET_VERSION  macro
        mov    ax,3000H			; AL=0 for DOS 5+ compatibility
        int    21H
        endm

    ;                                  Function Request 31H
    KEEP_PROCESS  macro  return_code,last_byte
        mov    al,return_code
        mov    dx,offset last_byte
        mov    cl,4
        shr    dx,cl
        inc    dx
        mov    ah,31H
        int    21H
        endm
    ;                                  Function Request 33H
    CTRL_C_CK  macro  action,state
        mov    al,action
        mov    dl,state
        mov    ah,33H
        int    21H
        endm
    ;                                  Function Request 35H
    GET_VECTOR  macro  interrupt
        mov    al,interrupt
        mov    ah,35H
        int    21H
        endm
    ;                                  Function Request 36H
    GET_DISK_SPACE  macro  drive
        mov    dl,drive
        mov    ah,36H
        int    21H
        endm

;                                  Function Request 38H
GET_COUNTRY  macro  country,buffer
                local  gc_01
                mov    dx,offset buffer
                mov    ax,country
                cmp    ax,0FFH
                jl     gc_01
                mov    al,0ffh
                mov    bx,country
    gc_01:       mov    ah,38H
                int    21H
                endm
;                                  Function Request 66H
GET_UPPERCASE_TABLE	macro country, codepage, buffer, buflen
		MOV	bx, codepage
		MOV	DX, COUNTRY
		MOV	CX, buflen
		LES	DI, buffer
		MOV	AX, 6602H
		INT	21H
		endm
;                                  Function Request 38H
    SET_COUNTRY  macro  country
                local  sc_01
                mov    dx,0FFFFH
                mov    ax,country
                cmp    ax,0FFH
                jl     sc_01
                mov    al,0ffh
                mov    bx,country
    sc_01:       mov    ah,38H
                int    21H
                endm

;                                  Function Request 39H
MAKE_DIR  macro  path
        lds    dx, path
        mov    ah,39H
        int    21H
        endm

;                                  Function Request 7139H
LFN_MAKE_DIR  macro  path
        lds    dx, path
        mov    ax,7139H
        int    21H
        endm

;                                  Function Request 3AH
REM_DIR  macro  path
        lds    dx, path
        mov    ah,3AH
        int    21H
        endm

;                                  Function Request 713AH
LFN_REM_DIR  macro  path
        lds    dx, path
        mov    ax, 713AH
        int    21H
        endm

;                                  Function Request 3BH
CHANGE_DIR  macro  path
        lds    dx, path
        mov    ah,3BH
        int    21H
        endm

;                                  Function Request 713BH
LFN_CHANGE_DIR  macro  path
        lds    dx, path
        mov    ax,713BH
        int    21H
        endm

;                                  Function Request 3CH
CREATE_HANDLE  macro  path,attrib
        lds    dx,path
        mov    cx,attrib
        mov    ah,3CH
        int    21H
        endm

;                                  Function Request 3DH
OPEN_HANDLE  macro  path,access
        lds    dx,path
        mov    al,access
        mov     ah,3DH
        int    21H
        endm
;                                  Function Request 3EH
CLOSE_HANDLE  macro  handle
        mov    bx,handle
        mov    ah,3EH
        int    21H
        endm

    ;                                  Function Request 3FH
    READ_HANDLE  macro  handle,buffer,bytes
        mov    bx,handle
        mov    dx,offset buffer
        mov    cx,bytes
        mov    ah,3FH
        int    21H
        endm

;                                  Function Request 40H
WRITE_HANDLE  macro  handle,buffer,bytes
        mov    bx,handle
        lds    dx,buffer
        mov    cx,bytes
        mov    ah,40H
        int    21H
        endm

;                                  Function Request 41H
DELETE_ENTRY  macro  path
        lds    dx, path
        mov    ah,41H
        int    21H
        endm
;                                  Function Request 7141H
LFN_DELETE_ENTRY  macro  path
        lds    dx, path
        mov    ax,7141H
        int    21H
        endm

;                                  Function Request 42H
MOVE_PTR  macro  handle,high,low,method
        mov    bx,handle
        mov    cx,high
        mov    dx,low
        mov    al,method
        mov    ah,42H
        int    21H
        endm

;                                  Function Request 43H
CHANGE_MODE  macro  path,action,attrib
        lds    dx,path
        mov    al,action
        mov    cx,attrib
        mov    ah,43H
        int    21H
        endm
;                                  Function Request 7143H
LFN_CHANGE_MODE  macro  path,action,attrib
        lds    dx,path
        mov    bl,action
        mov    cx,attrib
        mov    ax,7143H
        int    21H
        endm

;                                  Function Request 4400H,01H
IOCTL_DATA  macro  code,handle
        mov    bx,handle
        mov    al,code
        mov    ah,44H
        int    21H
        endm

    ;                                  Function Request 4402H,03H

    IOCTL_CHAR  macro  code,handle,buffer
        mov    bx,handle
        mov    dx,offset buffer
        mov    al,code
        mov    ah,44H
        int    21H
        endm
    ;                                  Function Request 4404H,05H
    IOCTL_STATUS  macro  code,drive,buffer
        mov    bl,drive
        mov    dx,offset buffer
        mov    al,code
        mov    ah,44H
        int    21H
        endm
    ;                                  Function Request 4406H,07H
    IOCTL_STATUS macro  code,handle
        mov    bx,handle
        mov    al,code
        mov    ah,44H
        int    21H
        endm
    ;                                  Function Request 4408H
    IOCTL_CHANGE  macro  drive
        mov    bl,drive
        mov    al,08H
        mov    ah,44H
        int    21H
        endm
    ;                                  Function Request 4409H
    IOCTL_RBLOCK  macro  drive
        mov    bl,drive
        mov    al,09H
        mov    ah,44H
        int    21H
        endm
    ;                                  Function Request 440AH
    IOCTL_RHANDLE  macro  handle
        mov    bx,handle
        mov    al,0AH
        mov    ah,44H
        int    21H
        endm
    ;                                  Function Request 440BH
    IOCTL_RETRY  macro  retries,wait
        mov    dx,retries
        mov    cx,wait
        mov    al,0BH
        mov    ah,44H
        int    21H
        endm
    ;                                  Function Request 440CH
    GENERIC_IOCTL_HANDLES macro handle,function,category,buffer
        mov    ch,05H
        mov    cl,function
        mov    dx,offset buffer
        mov    bx,handle
        mov    ah,44H
        mov    al,0CH
        int    21H
        endm
    ;                                  Function Request 440DH
    GENERIC_IOCTL_BLOCK macro drive_num,function,category,parm_blk
        mov    ch,08H
        mov    cl,function
        mov    dx,offset parm_blk - 1
        mov    bx,drive_num
        mov    ah,44H
        mov    al,0DH
        int    21H
        endm
    ;                                  Function Request 440EH
    IOCTL_GET_DRIVE_MAP macro   logical_drv
        mov    bx,logical_drv
        mov    ah,44H
        mov    al,0EH
        int    21H
        endm
    ;                                  Function Request 440FH
    IOCTL_SET_DRIVE_MAP macro   logical_drv
        mov    bx,logical_drv
        mov    ah,44H
        mov    al,0FH
        int    21H
        endm

;                                  Function Request 45H
XDUP  macro  handle
        mov    bx,handle
        mov    ah,45H
        int    21H
        endm

;                                  Function Request 46H
XDUP2  macro  handle1,handle2
        mov    bx,handle1
        mov    cx,handle2
        mov    ah,46H
        int    21H
        endm

;                                  Function Request 47H
GET_DIR  macro  drive,buffer
        mov    dl,drive
        mov    si,buffer
        mov    ah,47H
        int    21H
        endm

;                                  Function Request 48H
ALLOCATE_MEMORY  macro  bytes
        mov    bx,bytes
        mov    cl,4
        shr          bx,cl
        inc    bx
        mov    ah,48H
        int    21H
        endm

;                                  Function Request 49H
FREE_MEMORY  macro  seg_addr
        mov    ax,seg_addr
        mov    es,ax
        mov    ah,49H
        int    21H
        endm
    ;                                  Function Request 4AH
    SET_BLOCK  macro  last_byte
        mov    bx,offset last_byte
        mov    cl,4
        shr    bx,cl
        add    bx,17
        mov    ah,4AH
        int    21H
        mov     ax,bx
        shl     ax,cl
        dec     ax
        mov     sp,ax
        mov     bp,sp
        endm
    ;                                  Function Request 4B00H
    EXEC macro  path,command,parms
        mov    dx,offset path
        mov    bx,offset parms
        mov    word ptr parms[02h],offset command
        mov    word ptr parms[04h],cs
        mov    word ptr parms[06h],5ch
        mov    word ptr parms[08h],es
        mov    word ptr parms[0ah],6ch
        mov    word ptr parms[0ch],es
        mov    al,0
        mov    ah,4BH
        int    21H
        endm
    ;                                  Function Request 4B03H
    EXEC_OVL  macro  path,parms,seg_addr
        mov    dx,offset path
        mov    bx,offset parms
        mov    parms,seg_addr
        mov    parms[02H],seg_addr
        mov    al,3
        mov    ah,4BH
        int    21H
        endm

;                                  Function Request 4CH
END_PROCESS  macro  return_code
                mov    al,return_code
                mov    ah,4CH
                int    21H
                endm
;                                  Function Request 4DH
RET_CODE macro
        mov    ah,4DH
        int    21H
        endm
;                                  Function Request 4EH
    FIND_FIRST_FILE  macro  path,attrib
        mov    dx,offset path
        mov    cx,attrib
        mov    ah,4EH
        int    21H
        endm
    ;                                  Function Request 4FH
    FIND_NEXT_FILE  macro
        mov    ah,4FH
        int    21H
        endm

;                                  Function Request 54H
GET_VERIFY  macro
        mov    ah,54H
        int    21H
        endm
;                                  Function Request 56H
RENAME_FILE  macro  old_path,new_path
        LDS    dx, old_path
        LES    di, new_path
        mov    ah,56H
        int    21H
        endm

;                                  Function Request 7156H
LFN_RENAME_FILE  macro  old_path,new_path
        LDS    dx, old_path
        LES    di, new_path
        mov    ax,7156H
        int    21H
        endm
;                                  Function Request 57H
GET_SET_DATE_TIME  macro  handle,action,time,date
        mov     bx,handle
        mov    al,action
        mov    cx,word ptr time
        mov    dx,word ptr date
        mov    ah,57H
        int    21H
        endm
    ;                                  Function Request 58H
    ALLOC_STRAT  macro  code,strategy
        mov    bx,strategy
        mov    al,code
        mov    ah,58H
        int    21H
        endm

;                                  Function Request 59H
GET_ERROR  macro
        mov    ah,59h
	XOR	BX, BX
        int    21H
        endm

    ;                                  Function Request 5AH
    CREATE_TEMP  macro  pathname,attrib
                mov    cx,attrib
                mov    dx,offset pathname
                mov    ah,5AH
                int    21H
                endm
    ;                                  Function Request 5BH
    CREATE_NEW  macro  pathname,attrib
                mov    cx,attrib
                mov    dx,offset pathname
                mov    ah,5BH
                int    21H
                endm
;                                  Function Request 5C00H
LOCK_FILE        macro  handle,start,bytes
                mov    bx,handle
                mov    cx,word ptr start
                mov    dx,word ptr start+2
                mov    si,word ptr bytes
                mov    di,word ptr bytes+2
                mov    al,0
                mov    ah,5CH
                int    21H
                endm
    ;                                  Function Request 5C01H
    UNLOCK      macro  handle,start,bytes
                mov    bx,handle
                mov    cx,word ptr start
                mov    dx,word ptr start+2
                mov    si,word ptr bytes
                mov    di,word ptr bytes+2
                mov    al,1
                mov    ah,5CH
                int    21H
                endm
    ;                                  Function Request 5E00H
    GET_MACHINE_NAME  macro  buffer
                mov    dx,offset buffer
                mov    al,0
                mov    ah,5EH
                int    21H
                endm
    ;                                  Function Request 5E02H
    PRINTER_SETUP  macro  index,lgth,string
                mov    bx,index
                mov    cx,lgth
                mov    dx,offset string
                mov    al,2
                mov    ah,5EH
                int    21H
                endm
    ;                                  Function Request 5F02H
    GET_LIST    macro  index,local,remote
                mov    bx,index
                mov    si,offset local
                mov    di,offset remote
                mov    al,2
                mov    ah,5FH
                int    21H
                endm
    ;                                  Function Request 5F03H
    REDIR       macro  device,value,source,destination
                mov    bl,device
                mov    cx,value
                mov    si,offset source
                mov    es,seg destination
                mov    di,offset destination
                mov    al,03H
                mov    ah,5FH
                int    21H
                endm
    ;                                  Function Request 5F04H
    CANCEL_REDIR  macro  local
                mov    si,offset local
                mov    al,4
                mov    ah,5FH
                int    21H
                endm
;                                  Function Request 62H
GET_PSP     macro
                mov    ah,62H
                int    21H
                endm
    ;
    ;
    ; *******************
    ;  General
    ; *******************
    ;
    DISPLAY_ASCIZ  macro  asciz_string
        local  search,found_it
        mov    bx,offset asciz_string

    search:
        cmp    byte ptr [bx],0
        je     found_it
        inc    bx
        jmp short search

    found_it:
        mov    byte ptr [bx],"$"
        display asciz_string
        mov    byte ptr [bx],0
        display_char 0DH
        display_char 0AH
        endm
    ;
    MOVE_STRING  macro  source,destination,count
        push   es
        push   ds
        pop    es
        assume es:code
        mov    si,offset source
        mov    di,offset destination
        mov    cx,count
    rep movs   es:destination,source
        assume es:nothing
        pop    es
        endm
    ;
    CONVERT  macro  value,base,destination
        local  table,start
        jmp    start
    table  db   "0123456789ABCDEF"

    start:
        push   ax
        push   bx
        push   dx
        mov    al,value
        xor    ah,ah
        xor    bx,bx
        div    base
        mov    bl,al
        mov    al,cs:table[bx]
        mov    destination,al
        mov    bl,ah
        mov    al,cs:table[bx]
        mov    destination[1],al
        pop    dx
        pop    bx
        pop    ax
        endm
    ;
    CONVERT_TO_BINARY  macro  string,number,value
        local  ten,start,calc,mult,no_mult
        jmp    start
    ten  db     10

    start:
        mov    value,0
        xor    cx,cx
        mov    cl,number
        xor    si,si

    calc:
        xor    ax,ax
        mov    al,string[si]
        sub    al,48
        cmp    cx,2
        jl     no_mult
        push   cx
        dec    cx

    mult:
        mul    cs:ten
        loop   mult
        pop    cx

    no_mult:
        add    value,ax
        inc    si
        loop   calc
        endm
    ;
    CONVERT_DATE  macro  dir_entry
        mov    dx,word ptr dir_entry[24]
        mov    cl,5
        shr    dl,cl
        mov    dh,dir_entry[24]
        and    dh,1FH
        xor    cx,cx
        mov    cl,dir_entry[25]
        shr    cl,1
        add    cx,1980
        endm
    ;
    PACK_DATE  macro  date
        local set_bit
    ;
    ;  On entry: DH=day, DL=month, CX=(year-1980)
    ;
        sub   cx,1980
        push  cx
        mov   date,dh
        mov   cl,5
        shl   dl,cl
        pop   cx
        jnc   set_bit
        or    cl,80h

    set_bit:
        or    date,dl
        rol   cl,1
        mov   date[1],cl
        endm
;
