;/*!
;   @file
;
;   @brief DPMI Macro Library
;
;   @author 
;
;   This is macros adopted from HX-DOS package
;
;   Documentation: http://osfree.org/doku/en:docs:macrolib
;
;*/

ifndef DPMI_DEFINED

DPMI_DEFINED equ 1

;*** definitions DOS Protected Mode Interface ***

RMCS    struct          ;real mode call structure
union
rEDI    dd ?            ;+0
rDI		dw ?
ends
union
rESI    dd ?            ;+4
rSI		dw ?
ends
union
rEBP    dd ?            ;+8
rBP		dw ?
ends
RESERVE dd ?            ;+12
union
rEBX    dd ?            ;+16
rBX     dw ?
ends
union
rEDX    dd ?            ;+20
rDX     dw ?
ends
union
rECX    dd ?            ;+24
rCX		dw ?
ends
union
rEAX    dd ?            ;+28
rAX		dw ?
ends
rFlags  dw ?            ;+32
rES     dw ?            ;+34
rDS     dw ?            ;+36
rFS     dw ?            ;+38    
rGS     dw ?            ;+40  
union
rCSIP	dd ?            ;+42
struct
rIP     dw ?
rCS     dw ?
ends
ends
union
rSSSP	dd ?            ;+46
struct
rSP     dw ?
rSS     dw ?
ends
ends
RMCS    ends

MEMINFO struct
maxBlock      dd ?            ;0	;in bytes
freeUnlocked  dd ?            ;4	;pages (max unlocked allocation)
maxLockable   dd ?            ;8	;pages (max locked allocation)
totalAdrSpace dd ?            ;12
unlocked      dd ?            ;16	;total unlocked (=virtual)
freePhys      dd ?            ;20
totalPhys     dd ?            ;24
freeAdrSpace  dd ?            ;28
swapFile      dd ?            ;32
res           db 12 dup (?)
MEMINFO ends

EXCFRAME struct
_dpmieip	dd ?
_dpmics		dd ?
errorcode	dd ?
_eip		dd ?
_cs			dd ?
_eflags		dd ?
_esp		dd ?
_ss			dd ?
EXCFRAME ends

EXCFRAME16 struct
_dpmieip	dw ?
_dpmics		dw ?
errorcode	dw ?
_eip		dw ?
_cs			dw ?
_eflags		dw ?
_esp		dw ?
_ss			dw ?
EXCFRAME16 ends


DPMI_AllocDesc    equ 0
DPMI_FreeDesc     equ 1
DPMI_GetBase      equ 6
DPMI_SetBase      equ 7
DPMI_SetLimit     equ 8
DPMI_SetAccRights equ 9

@DpmiCall macro xx
        ifnb <xx>
            mov ax,xx
        endif
        int 31h
        endm

@DPMI_AllocDesc macro xx      ;allocate descriptor
        ifnb <xx>
          ifdifi <xx>, <cx>
            mov cx,xx
          endif
        else
          mov cx,1
        endif
        @DpmiCall DPMI_AllocDesc
        endm

@DPMI_FreeDesc macro xx      ;free descriptor
        ifnb <xx>
          ifdifi <xx>, <bx>
            mov bx,xx
          endif
        endif
        @DpmiCall DPMI_FreeDesc
        endm

@DPMI_Seg2Desc macro segment_  ;segment to descriptor
	ifnb <segment_>
		move bx,segment_
	endif
        @DpmiCall 0002h
        endm

@DPMI_GetIncValue macro
        @DpmiCall 0003h
        endm

@DPMI_LockSel macro xx        ;undocumented: lock selector region
        mov bx,xx
        @DpmiCall 0004h
        endm

@DPMI_UnlockSel macro xx      ;undocumented: unlock selector region
        mov bx,xx
        @DpmiCall 0005h
        endm

@DPMI_GetBase  macro selector ;get base address in CX:DX
        ifnb <selector>
           mov bx,word ptr selector
        endif
        @DpmiCall DPMI_GetBase
        endm

@DPMI_SetBase  macro selector ;set base address in CX:DX
        ifnb <selector>
           mov bx,word ptr selector
        endif
        @DpmiCall DPMI_SetBase
        endm

@DPMI_SetLimit  macro selector ;set limit in CX:DX
        ifnb <selector>
           mov bx,word ptr selector
        endif
        @DpmiCall DPMI_SetLimit
        endm

@DPMI_SetAccRights macro selector,rights  ;set access rights in CX
        ifnb <rights>
           mov cx,rights
        endif
        ifnb <selector>
           mov bx,selector
        endif
        @DpmiCall DPMI_SetAccRights
        endm

@DPMI_CreateCSAlias macro selector  ;create code segment alias descriptor
        ifnb <selector>
          mov bx,selector
        endif
        @DpmiCall 000Ah
        endm

@DPMI_GetDescriptor macro selector,puffer  ;get 8 byte descriptor in ES:DI
        ifnb <selector>
          mov bx,selector
        endif
        ifnb <puffer>
             les di,puffer
        endif
        @DpmiCall 000Bh
        endm

@DPMI_SetDescriptor macro selector,puffer  ;set descriptor ES:DI
        ifnb <selector>
          mov bx,selector
        endif
        ifnb <puffer>
             les di,puffer
        endif
        @DpmiCall 000Ch
        endm

@DPMI_DOSALLOC macro length   ;allocate DOS memory block
        mov bx,length
        @DpmiCall 0100h
        endm

@DPMI_DOSFREE macro selektor  ;free DOS memory block
        ifnb <selector>
          mov dx,selektor
	endif
        @DpmiCall 0101h
        endm

@DPMI_DOSSIZE macro selektor,parag  ;resize DOS memory block
        ifnb <selektor>
          mov dx, selektor
	endif
        ifnb <parag>
          mov bx, parag
	endif
        @DpmiCall 0102h
        endm

@DPMI_GetRMIntVec macro xx    ;get realmode interrupt vector in CX:DX
ifnb <xx>
        mov bl,xx
endif        
        @DpmiCall 0200h
        endm

@DPMI_SetRMIntVec macro xx    ;set realmode interrupt vector CX:DX
ifnb <xx>
        mov bl,xx             
endif        
        @DpmiCall 0201h
        endm

@DPMI_GetExcVec macro xx      ;get exception vector in CX:DX
ifnb <xx>
        mov bl,xx
endif        
        @DpmiCall 0202h
        endm

@DPMI_SetExcVec macro xx      ;set exception vector in CX:DX
ifnb <xx>
        mov bl,xx
endif        
        @DpmiCall 0203h
        endm

@DPMI_GetPMIntVec macro xx    ;get PM interrupt vector into CX:DX
ifnb <xx>
        mov bl,xx
endif        
        @DpmiCall 0204h
        endm

@DPMI_SetPMIntVec macro xx    ;set PM interrupt vector in CX:DX
ifnb <xx>
        mov bl,xx
endif        
        @DpmiCall 0205h
        endm

@DPMI_SimRMInt macro xx,yy    ;simulate real mode interrupt
        mov   bx,xx           ;ES:DI->RMCS
        ifnb <yy>
          mov cx,yy           ;CX = words to copy to RM stack
        endif
        @DpmiCall 0300h
        endm

@DPMI_CallRMFarProc macro     ;call real mode far proc
        mov   bh,00           ;ES:DI->RMCS
        @DpmiCall 0301h       ;CX = words to copy to RM stack
        endm

@DPMI_CallRMIRProc macro      ;call real mode far proc with iret frame
        mov   bh,00           ;
        @DpmiCall 0302h       ;CX = words to copy to RM stack
        endm

@DPMI_AllocRMCallback macro   ;allocate real mode callback
        @DpmiCall 0303h       ;DS:SI protected mode proc to call
        endm                  ;ES:DI RMCS to use

@DPMI_FreeRMCallback macro    ;free real mode callback in CX:DX
        @DpmiCall 0304h
        endm

@DPMI_GETRMSA macro           ;get raw mode switch address
        @DpmiCall 0306h
        endm

@DPMI_GETVERSION macro        ;get version
        @DpmiCall 0400h
        endm

@DPMI_GETFREEMEMINFO macro    ;get free memory information
        @DpmiCall 0500h
        endm

@DPMI_ALLOCMEM macro          ;alloc memory (bytes in BX:CX)
        @DpmiCall 0501h       ;returns handle in SI:DI, address in BX:CX 
        endm

@DPMI_FREEMEM macro           ;free memory handle in SI:DI
        @DpmiCall 0502h
        endm

@DPMI_SIZE macro           ;refree memory handle in SI:DI
        @DpmiCall 0502h
        endm

@DPMI_GETPAGESIZE macro       ;get page size
        @DpmiCall 0604h
        endm

@DPMI_MAPPHYS macro addr_,size_
        mov di,size_+0
        mov si,size_+2
        mov cx,addr_+0
        mov bx,addr_+2
        @DpmiCall 0800h
        endm

@DPMI_DisableInt macro
	@DpmiCall 0900h
	endm

;INT 2F - DOS Protected-Mode Interface - INSTALLATION CHECK
;
;	AX = 1687h
;Return: AX = 0000h if installed
;	    BX = flags
;		bit 0: 32-bit programs supported
;	    CL = processor type (02h=80286, 03h=80386, 04h=80486)
;	    DH = DPMI major version
;	    DL = two-digit DPMI minor version (binary)
;	    SI = number of paragraphs of DOS extender private data
;	    ES:DI -> DPMI mode-switch entry point (see #02718)
;	AX nonzero if not installed
;SeeAlso: AX=1686h,AX=43E0h,AX=DE01h/BX=4450h,AX=FB42h/BX=0001h
;SeeAlso: INT 31/AX=0400h,INT 31/AX=5702h,INT D4/AH=10h
;

@DPMI_SwitchEntry macro
	mov	ax, 1687h
	int	2fh
	endm

;INT 2F - DPMI 0.9+ - GET VENDOR-SPECIFIC API ENTRY POINT
;
;	AX = 168Ah
;	DS:(E)SI = selector:offset of ASCIZ vendor name (see #02719)
;Return: AL = status
;	    00h successful
;	       ES:(E)DI -> extended API entry point
;	    8Ah unsuccessful
;Notes:	the vendor name is used to determine which entry point to return; it is
;	  case-sensitive
;	available in protected mode only
;	32-bit applications use ESI and EDI, 16-bit applications use SI and DI
;	this call is present but not documented for DPMI 0.9
;	the Borland C++ 3.1 DPMILOAD does not handle requests for entry points
;	  other than the MS-DOS one gracefully, producing an unhandled
;	  exception report; this has been fixed in the Borland Pascal 7 version
;SeeAlso: AX=1687h,INT 31/AX=0A00h,INT 31/AH=57h

@DPMI_VendorEntry macro vendor
	mov	si,offset vendor
	mov	ax, 168ah
	int	2fh
	endm

;        popcontext listing
endif
