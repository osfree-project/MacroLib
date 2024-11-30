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
;   This is Supervisor calls for OS/2 MVDM.
;
;   @author Yuri Prokushev (yuri.prokushev@gmail.com)
;
;*/

SVCCALL		MACRO	xxx
			HLT
			DB	xxx, not xxx
		ENDM
