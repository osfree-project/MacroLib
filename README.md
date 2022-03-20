# MacroLib

ALP/MASM compatible macro library for BIOS and DOS

## @SetMode

### Brief

Set the current video mode

### Syntax

@SetMode mode

### Parameters

mode (BYTE) - mode

### Return

none

## @SetCurSz

### Brief

Set Cursor Shape Size

### Syntax

@SetCurSz first, last

### Parameters

first (BYTE) - first scan line
last (BYTE) - last scan line

### Return

none

## @SetCurPos

### Brief

Set Cursor Position

### Syntax

@SetCurPos column, row, page

### Parameters

column (BYTE) - new cursor column (zero based)
row (BYTE) - new cursor row (zero based)
page (BYTE) - optional video page number

### Return

none

## @GetCur

### Brief

Get Current Cursor Position and Size

### Syntax

@GetCur page

### Parameters

page (BYTE) - optional video page number

### Return

## @SetPage

### Brief

Set Active Video Page

### Syntax

@SetPage page

### Parameters

page (BYTE) - optional video page number

### Return

## @ScrollUp

### Brief

Scroll Screen Area Up

### Syntax

@ScrollUp	MACRO	DISTANCE,ATRIB,UPCOL,UPROW,DOWNCOL,DOWNROW

### Parameters
### Return

## @ScrollDn

### Brief

Scroll Screen Area Down

### Syntax

@ScrollDn	MACRO	DISTANCE,ATRIB,UPCOL,UPROW,DOWNCOL,DOWNROW

### Parameters
### Return

## @GetChAtr

### Brief

Get Character and Attribute

### Syntax

@GetChAtr page

### Parameters

page (BYTE) - optional video page number

### Return

## @PutChAtr

### Brief

Print Char With Attribute

### Syntax

@PutChAtr char, atrib, page, repeat

### Parameters

char (BYTE) - code of character
atrib (BYTE) - character attribute
col (BYTE) - column
row (BYTE) - row
page (BYTE) - optional video page number

### Return

## @PutCh

### Brief

Print Char

### Syntax

@PutCh char, atrib, page, repeat

### Parameters

char (BYTE) - code of character
atrib (BYTE) - optional character attribute
col (BYTE) - column
row (BYTE) - row
page (BYTE) - optional video page number

### Return

## @SetPalet

### Brief

Set CGA Palette

### Syntax

@SetPalet color

### Parameters
### Return

## @SetColor

### Brief

Set Border Color

### Syntax

@SetColor color

### Parameters
### Return

## @SetDot

### Brief

Draw Pixel

### Syntax

@SetDot	color, row, col, page

### Parameters

color (BYTE) - color
col (BYTE) - column
row (BYTE) - row
page (BYTE) - optional video page number

### Return

## @GetDot

### Brief

Get Pixel Color

### Syntax

@GetDot col, row, page

### Parameters

col (BYTE) - column
row (BYTE) - row
page (BYTE) - optional video page number

### Return

## @WrtTTY

### Brief

Write char in TTY mode

### Syntax

@WrtTTY char, page

### Parameters

char (BYTE) - character code
page (BYTE) - optional video page number

### Return

none

## @VideoState

### Brief

Return Video State

### Syntax

@VideoState

### Parameters

none

### Return

## @GetMode

### Brief

Get the current video mode and page

### Syntax

@GetMode

### Parameters

none

### Return

AL	    = Mode
AH	    = Width in characters
BH	    = Page

## @GetDisplay

### Brief

Get Display Configuration

### Syntax

@GetDisplay

### Parameters

none

### Return

## @GetVideoState

### Brief

Get Video State

### Syntax

@GetVideoState

### Parameters

none

### Return

## @GetEGAInfo

### Brief

Get EGA information

### Syntax

@GetEGAInfo

### Parameters

none

### Return

