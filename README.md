# MacroLib
MASM compatible macro library for BIOS and DOS

## @SetMode

### Brief

Set the current video mode

### Syntax

@SetMode mode

### Parameters

mode (BYTE) - mode

### Return

none

## @GetMode

### Brief

Get the current video mode and	page

### Syntax

@GetMode

### Parameters

none

### Return

AL	    = Mode
AH	    = Width in characters
BH	    = Page
