;/*!
;   @file
;
;   @ingroup fapi
;
;   @brief Family API implementation
;
;   (c) osFree Project 2018-2024, <http://www.osFree.org>
;   for licence see licence.txt in root directory, or project website
;
;   This is Family API implementation for DOS, used with BIND tools
;   to link required API
;
;   @author Yuri Prokushev (yuri.prokushev@gmail.com)
;
;*/

; Taken from MASM 6.0 Programming Guide
@ArgCount MACRO arglist:VARARG
        LOCAL count
        count = 0
        FOR arg, <arglist>
            count = count + 1       ;; Count the arguments
        ENDM
        EXITM %count
	ENDM

@ArgI MACRO index:REQ, arglist:VARARG
        LOCAL count, retstr
        retstr TEXTEQU <>            ;; Initialize count
        count  = 0                   ;; Initialize return string
        FOR arg, <arglist>
            count = count + 1
            IF count EQ index        ;; Item is found
                retstr TEXTEQU <arg> ;; Set return string
                EXITM                ;;  and exit IF
            ENDIF
        ENDM
        EXITM retstr                 ;; Exit function
	ENDM

@ArgRev MACRO arglist:REQ
        LOCAL txt, arg
        txt TEXTEQU <>
%	FOR arg, <arglist>
            txt CATSTR <arg>, <,>, txt      ;; Paste each onto list
        ENDM
                                            ;; Remove terminating comma
        txt SUBSTR  txt, 1, @SizeStr( %txt ) - 1
        txt CATSTR  <!<>, txt, <!>>         ;; Add angle brackets
        EXITM txt
	ENDM

; SaveRegs - Macro to generate a push instruction for each
; register in argument list. Saves each register name in the
; regpushed text macro.
regpushed TEXTEQU <>                    ;; Initialize empty string

@SaveRegs MACRO regs:VARARG
%        FOR reg, <regs>                     ;; Push each register
            push reg                        ;;  and add it to the list
            regpushed CATSTR   <reg>, <,>, regpushed
        ENDM                                ;; Strip off last comma
        regpushed CATSTR <!<>, regpushed    ;; Mark start of list with <
        regpushed SUBSTR regpushed, 1, @SizeStr( regpushed )
        regpushed CATSTR regpushed, <!>>    ;; Mark end with >
    ENDM

  ; RestoreRegs - Macro to generate a pop instruction for registers
    ; saved by the SaveRegs macro. Restores one group of registers.

@RestoreRegs MACRO
        LOCAL regs
%	FOR reg, regpushed                  ;; Pop each register        pop
	    reg
        ENDM
    ENDM

pushc		MACRO op				;; 80186 or higher
			IF  @Cpu AND 00000010B
		            push op
			ELSE			    	;; 8088/8086
		            mov  ax, op
		            push ax
			ENDIF
		ENDM

@PushAll MACRO
    IF @Cpu AND 00000010B	;; 80186 or higher
        pusha
    ELSE
        push ax
        push cx
        push dx
        push bx
        push sp
        push bp
        push si
        push di
    ENDIF
ENDM

@PopAll MACRO
    IF @Cpu AND 00000010B	;; 80186 or higher
        popa
    ELSE
        pop di
        pop si
        pop bp
        pop sp
        pop bx
        pop dx
        pop cx
        pop ax
    ENDIF
ENDM
