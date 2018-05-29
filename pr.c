/** @file
BlkCat -- UEFI Block Device Concatenator Driver
Copyright (C) 2018 Roger C. Pao.  All rights reserved.

https://github.com/rcpao/BlkCat

Roger C. Pao <rcpao+BlkCatEfi@gmail.com>
  
Apache License 2.0

**/

/* ---------------------------------------------------------------------------
pr.c
--------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------
Include Files
--------------------------------------------------------------------------- */
#include <Library/UefiLib.h> /* Print() */
#include <Library/BaseLib/BaseLibInternals.h> /* InternalMathDivU64x32 */

//#include <stdarg.h> TBD does not exist!

#define __PR_C
#include "pr.h"


/* ---------------------------------------------------------------------------
Functions
--------------------------------------------------------------------------- */

#if 0
/* ---------------------------------------------------------------------------
_Pr

UEFI Notes:
  Print() format string is Unicode requiring Print(L"Hello world");
  AsciiPrint() format string is ASCII requiring AsciiPrint("Hello world");
  Within the format string: %a = ptr ASCII string; %s = ptr Unicode string.
--------------------------------------------------------------------------- */
void _Pr(char *format, ...) {
#if 0
  AsciiPrint(format);
  #define AsciiPrint _Pr
  /* FAIL: silent! */
#elif 0
  /* FAIL: stdarg.h does not exist */
  va_list args;


  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  #define AsciiPrint _Pr
#else
#endif
} /* _Pr */
#endif


#define USE_ASCII_PRINT 1 /* 1 = AsciiPrint, 0 = Print */

/* ---------------------------------------------------------------------------
PrChar
	Print one char.
Warning
  AsciiPrint("\n") will display "\r\n" to the console while
  AsciiPrint("%c", '\n') will only display "\n" to the console.
--------------------------------------------------------------------------- */
UINT64 PrChar(INT8 /* CHAR8 */ c) {
#if (USE_ASCII_PRINT)
  AsciiPrint("%c", c);
#else /* #if (USE_ASCII_PRINT) */
  Print(L"%c", c);
#endif /* #if (USE_ASCII_PRINT) */
  return (1);
} /* PrChar */


/* ---------------------------------------------------------------------------
PrHexBits4
  Print one hexadecimal digit.
--------------------------------------------------------------------------- */
UINT64 PrHexBits4(UINT8 n) {
  n &= 0xF;
  return(PrChar((n < 10)? (n + '0'): (n - 10 + 'A')));
} /* PrHexBits4 */


/* ---------------------------------------------------------------------------
PrHexBits8
  Print (8 bits | 1 byte | BYTE) in hexadecimal.
--------------------------------------------------------------------------- */
UINT64 PrHexBits8(UINT8 n) {
  UINT8 h = n >> 4;


  PrHexBits4(h);
  PrHexBits4(n); /* LoNibble */
  return (2);
} /* PrHexBits8 */


/* ---------------------------------------------------------------------------
PrHexBits16
  Print (16 bits | 2 bytes | WORD) in hexadecimal.
--------------------------------------------------------------------------- */
UINT64 PrHexBits16(UINT16 n) {
  UINT8 h = n >> 8;


  PrHexBits8(h);
  PrHexBits8((UINT8)n); /* LoByte */
  return (4);
} /* PrHexBits16 */


/* ---------------------------------------------------------------------------
PrHexBits32
  Print (32 bits | 4 bytes | DWORD) in hexadecimal.
--------------------------------------------------------------------------- */
UINT64 PrHexBits32(UINT32 n) {
  UINT16 h = (UINT16)(n >> (8 * 2));


  PrHexBits16(h);
  PrHexBits16((UINT16)n); /* LoBytes2 */
  return (8);
} /* PrHexBits32 */


/* ---------------------------------------------------------------------------
PrHexBits64
  Print (64 bits | 8 bytes | QWORD | EFI_LBA) in hexadecimal.
--------------------------------------------------------------------------- */
UINT64 PrHexBits64(UINT64 n) {
  UINT32 h = (UINT32)(n >> (8 * 4));


  PrHexBits32(h);
  PrHexBits32((UINT32)n); /* LoBytes4 */
  return (16);
} /* PrHexBits64 */


/* ---------------------------------------------------------------------------
PrAscii
	Print a NUL terminated ASCII string.
--------------------------------------------------------------------------- */
UINT64 PrAscii(void *pBuf) {
  unsigned char *p = pBuf;


  if (PrVerboseLevel > VL_SILENT) {
    while (*p != '\0') {
      PrChar((isprint(*p))? *p: '.');
      p++;
    }
  }
  return (p - (unsigned char *)pBuf);
} /* PrAscii */


/* ---------------------------------------------------------------------------
PrIndent
	Print PrIndentLevel spaces.
Input
	PrIndentLevel
--------------------------------------------------------------------------- */
UINT64 PrIndent(void) {
  UINTN i;

  for (i = 0; i < PrIndentLevel; i++) {
    PrChar(' ');
    PrChar(' ');
  }
  return (i * 2);
} /* PrIndent */


/* ---------------------------------------------------------------------------
;PrBuf
--------------------------------------------------------------------------- */
UINT64 PrBuf(void *pBuf, UINTN Length) {
  unsigned char *p = pBuf;
  UINTN PrintablesPrinted = 0;


  if (PrVerboseLevel > VL_SILENT) {
    for (PrintablesPrinted = 0; PrintablesPrinted < Length; PrintablesPrinted++) {
      PrChar((isprint(*p))? *p: '.');
      p++;
    }
  } /* if (PrVerboseLevel > VL_SILENT) */
  return (PrintablesPrinted);
} /* PrBuf */


/* ---------------------------------------------------------------------------
;PrBufxxdr
;  Print buffer using 'xxd -r' output format.
;$ xxd -g 1 input.bin
;0000000: fa 31 c0 8e d8 8e c0 8e-d0 bc 00 7c 89 e6 bf 00  .1.........|....
;0000010: 06 b9 00 01 f3 a5 ea 1b 06 00 00 6a 02 9d be c6  ...........j....
;0000020: 06 e8 75 00 be f6 06 e8 6f 00 e8 93 00 be 19 07  ..u.....o.......
;0000030: e8 66 00 be 1c 07 e8 60 00 52 b4 41 bb aa 55 cd  .f.....`.R.A..U.
;0000040: 13 5a 72 35 be 2c 07 e8 4f 00 be 8a 07 c7 44 04  .Zr5.,..O.....D.
;0000050: 00 08 b4 42 cd 13 72 29 be 2c 07 e8 3b 00 be 8a  ...B..r).,..;...
;0000060: 07 80 7c 02 27 75 22 be 2c 07 e8 2c 00 be f6 06  ..|.'u".,..,....
;0000070: e8 26 00 e8 4a 00 e9 87 01 be 2f 07 e8 1a 00 eb  .&..J...../.....
;0000080: 10 be 40 07 e8 12 00 eb 08 be 52 07 e8 0a 00 eb  ..@.......R.....
;0000090: 00 be 63 07 e8 02 00 eb fe 50 53 56 2e 8a 04 08  ..c......PSV....
;00000a0: c0 74 0a bb 01 00 b4 0e cd 10 46 eb ef 5e 5b 58  .t........F..^[X
;00000b0: c3 50 b4 01 cd 16 58 74 06 9c 30 e4 cd 16 9d c3  .P....Xt..0.....
;00000c0: e8 ee ff 74 fb c3 65 6e 5f 62 6f 6f 74 69 65 72  ...t..en_bootier
;00000d0: 20 6d 62 72 30 36 30 30 2d 61 73 61 70 2e 6e 61   mbr0600-asap.na
;00000e0: 73 6d 20 56 65 72 73 69 6f 6e 20 30 2e 30 2e 33  sm Version 0.0.3
;00000f0: 38 35 33 0d 0a 00 0d 0a 50 72 65 73 73 20 61 6e  853.....Press an
;0000100: 79 20 6b 65 79 20 74 6f 20 63 6f 6e 74 69 6e 75  y key to continu
;0000110: 65 20 2e 20 2e 20 2e 20 00 0d 0a 00 4c 6f 61 64  e . . . ....Load
;0000120: 69 6e 67 20 73 74 61 67 65 20 31 00 20 2e 00 0d  ing stage 1. ...
;0000130: 0a 4d 73 67 4e 6f 45 78 74 52 65 61 64 0d 0a 00  .MsgNoExtRead...
;Offset-  Hex--------------------------------------------  Printable-------
;
;To convert back to binary:
;$ xxd -r input.txt output.bin
;Warning: xdd -r may only allow 7 hex digits for the offset.
;
;Ported from en_bootier/common-asap.nasm.
--------------------------------------------------------------------------- */
UINT64 PrBufxxdr(void *pBuf, UINTN Length) {
  unsigned char *p = pBuf;
  UINT32 Offs = 0;
  int BytesInLinePrinted = 0;
  int PrintablesPrinted = 0;


  if (PrVerboseLevel > VL_SILENT) {
    /* Print pBuf pointer value */
    //Print(L"%p for %08x:\n", pBuf, Length);
    //AsciiPrint("%p for %08x:\n", pBuf, Length);
    PrHexBits32(*((UINT32 *)pBuf));
    PrAscii(" for ");
    PrHexBits32(*((UINT32 *)Length));
    AsciiPrint("\n"); //PrAscii("\n");

    do {
      PrHexBits32(Offs); PrChar(':');

      /* Print " 00 01 02 03 04 05 06 07-08 09 0A 0B 0C 0D 0E 0F" */
      for (BytesInLinePrinted = 0; (BytesInLinePrinted < 16) && (Length); BytesInLinePrinted++) {
        PrChar((BytesInLinePrinted == 8)? '-': ' ');
        PrHexBits8(*p);
	      p++;
        Offs++;
        Length--;
      }

      /* Pad hex line to Printables column */
      for (PrintablesPrinted = BytesInLinePrinted; PrintablesPrinted < 16; PrintablesPrinted++) {
        PrAscii("   ");
      }

      /* Print Printables line */
      p -= BytesInLinePrinted; /* Rewind for Printables */
      PrAscii(" ");
      p += PrBuf(p, BytesInLinePrinted);

      AsciiPrint("\n"); //PrAscii("\n");
    } while (Length);
  } /* if (PrVerboseLevel > VL_SILENT) */

  return (PrintablesPrinted);
} /* PrBufxxdr */


/* ---------------------------------------------------------------------------
PrCapacityDec

From en_bootier/init-asap.nasm
 
;-----------------------------------------------------------------------------
;PrCapacityDec
;  Print capacity (decimal) using the largest non-zero specific unit
;  (modulo 1024) with the appropriate symbol appended using specific units of
;  IEC 60027-2 A.2 and ISO/IEC 80000-13:2008.
;
;Input
;  edx:eax = Total number of user addressable 512 Byte sectors (64-bit LBA)
;
;       63  61|                  51|                  41|                32
;  edx   _ _ _|_:_ _ _ _  _ _ _ _:_|_ _ _  _ _ _ _:_ _ _|_  _ _ _ _:_ _ _ _
;           29|                  19|                   9|
;         ZiB |        EiB         |        PiB         |        TiB
;
;       31|                  21|                  11|                   1|0
;  eax   _|_ _ _:_ _ _ _  _ _ _|_:_ _ _ _  _ _ _ _:_|_ _ _  _ _ _ _:_ _ _|_
;       31|                  21|                  11|                   1|
;      TiB|       GiB          |       MiB          |        KiB         |
;
;Output
;  none
;Modifies
;  none
;Reference
;  svn://10.3.171.13/enmotus/bobcat/x86/op_rom/JWasm_Option_ROM/trunk/common-__LINE__.asm/PrCapacityDec
;  http://en.wikipedia.org/wiki/Binary_prefix#Specific_units_of_IEC_60027-2_A.2_and_ISO.2FIEC_80000
;-----------------------------------------------------------------------------
PrCapacityDec:	;proc	near
;DBG_PR_CAPACITY_DEC=1 ;comment out to disable
		push	eax
		push	ecx
		push	edx

.TryZiB:
		bsr	ecx, edx		;Scans edx for first bit set from bit n to 0
%ifdef DBG_PR_CAPACITY_DEC
DBG_PR_STR_CRLF DL_MAX, "__LINE__ PrCapacityDec: bsr ecx, edx ;Scans edx for first bit set from bit n to 0"
DBG_PR_REGS DL_MAX, "__LINE__"
%endif ;DBG_PR_CAPACITY_DEC
		jz	.Hi32isZero		; if all bits are 0, zf = 1 (contrary to Internet documentation)
		cmp	ecx, 29			;Is ZiB zero?
		jb	.TryEiB			; yes: try EiB
.PrZiB:						; no: PrZiB
    		shr	edx, 29			;edx = ZiB
    		mov	eax, edx		;eax = ZiB
    		mov	cl, 'Z'			;Print eax+" ZiB"
    		jmp	.PrEaxUnit		;
.TryEiB:
		cmp	ecx, 19			;Is EiB zero?
		jb	.TryPiB			; yes: try PiB
.PrEiB:						; no: Print EiB
    		shr	edx, 19			;edx = EiB
    		mov	eax, edx		;eax = EiB
    		mov	cl, 'E'			;Print eax+" EiB"
    		jmp	.PrEaxUnit		;
.TryPiB:
		cmp	ecx, 9			;Is PiB zero?
		jb	.TryTiB			; yes: try TiB
.PrPiB:						; no: Print PiB
    		shr	edx, 9			;edx = PiB
    		mov	eax, edx		;eax = PiB
    		mov	cl, 'P'			;Print eax+" PiB"
    		jmp	.PrEaxUnit		;

.Hi32isZero:					;edx = 0
.TryTiB:					;edx = TiB
		shl	edx, 1			;make room for TiB bit 0
		test	eax, 80000000h		;is TiB bit 0 = 0?
		jz	.SkipTiBbit0		; yes: skip
		or	edx, 1			; no: set TiB bit 0 = 1
.SkipTiBbit0:
   		or	edx, edx		;Is TiB zero?
   		jz	.TryGiB			; yes: try GiB
.PrTiB:						; no: Print TiB
    		mov	eax, edx		;eax = TiB
    		mov	cl, 'T'			;Print eax+" TiB"
    		jmp	.PrEaxUnit		;

.TryGiB:
       		and	eax, 7FFFFFFFh		;TiB bit 0 = 0
		bsr	ecx, eax		;Scans eax for first bit set from bit n to 0
%ifdef DBG_PR_CAPACITY_DEC
DBG_PR_STR_CRLF DL_MAX, "__LINE__ PrCapacityDec: bsr ecx, eax ;Scans eax for first bit set from bit n to 0"
DBG_PR_REGS DL_MAX, "__LINE__"
%endif ;DBG_PR_CAPACITY_DEC
		jz	.Lo32isZero		; if all bits are 0, zf = 1

		cmp	ecx, 21                 ;Is GiB zero?
		jb	.TryMiB			; yes: try MiB
.PrGiB:						; no: Print GiB
    		shr	eax, 21			;eax = GiB
    		mov	cl, 'G'			;Print eax+" GiB"
    		jmp	.PrEaxUnit		;
.TryMiB:
		cmp	ecx, 11			;Is MiB zero?
		jb	.TryKiB			; yes: try KiB
.PrMiB:						; no: Print MiB
    		shr	eax, 11			;eax = MiB
    		mov	cl, 'M'			;Print eax+" MiB"
    		jmp	.PrEaxUnit		;
.TryKiB:
		cmp	ecx, 1                  ;Is KiB zero?
		jb	.TryB			; yes: try B
.PrKiB:						; no: Print KiB
    		shr	eax, 1			;eax = KiB
    		mov	cl, 'K'			;Print eax+" KiB"
    		jmp	.PrEaxUnit		;

.Lo32isZero:					;eax = 0
.TryB:						;KiB = 0
.PrB:
		shl	eax, 9			;0 or 512 KiB
    		mov	cl, ' '			;Print eax+" B"
    		;jmp	PrEaxUnit		;

.PrEaxUnit:	;Print dword+' '+((cl != ' ')? (cl+'iB'): 'B')
		;eax = dword
		;cl = '[ZEPTGMK]'
    		;call	PrHexDwordEAX		;Print eax in hexadecimal
    		call	PrDecDwordEAX		;Print eax in decimal
    		mov	al, ' '
    		call	PrCharAL
    		cmp	cl, ' '			;Is cl = ' '?
    		je	.SkipCLi
		mov	al, cl
    		call	PrCharAL
    		mov	al, 'i'
    		call	PrCharAL
.SkipCLi:
    		mov	al, 'B'
    		call	PrCharAL

.Done:
		pop	edx
		pop	ecx
		pop	eax
		ret

;PrCapacityDec	endp


--------------------------------------------------------------------------- */

/*
FS0:\> EFI\enmotus\dbdblkf.efi
dblkf 0.0.1.13509
Copyright (C) 2016-2017 Enmotus, Inc.  All rights reserved.

dblkf DBG PrCapacityDec(1.88 TiB) = '2075877953241 2.075 TB' '2075877953241 1r909 2 TiB'
dblkf DBG PrCapacityDec(2075877953241L) = '2075877953241 2.075 TB' '2075877953241 1r909 2 TiB'
dblkf DBG PrCapacityDec(580.97 GiB = 623811787489L) = '623811787489 623.811 GB' '623811787489 580r993 581 GiB'
dblkf -b=blknum=0x0 -l=lbastart=0x0 -n=lbanum=0x0 errflg=0
dblkf ofn='(null)'
dblkf OptionBitsSet=0x0
dblkf -b0 1474560 1.474 MB
  'PciRoot(0x0)/Pci(0x1,0x0)/Floppy(0x0)'
dblkf malloc((lbanum=0x0)*(BlockBytes=0x200)=(BufferBytes=0x0)) = (pBuffer=0x0)
dblkf malloc((lbanum=0x0)*(BlockBytes=0x200)=(BufferBytes=0x0)) = (pBuffer=0x0)
dblkf ReadBlocks() error No Media
FS0:\> 
*/

void PrCapacityDec1000_000(UINT64 capacity) {
//#if (DEFAULT_VERBOSE_LEVEL > VL_SILENT)

  /* International System of Units (SI) */

  UINT64 prevcap = capacity; /* previous capacity before division */
  const UINT32 Divisor = 1000;
  UINT32 capUnit;
#if (USE_ASCII_PRINT)
  CHAR8 *capUnits[] = {
    "B",
    "KB",
    "MB",
    "GB",
    "TB",
    "PB",
    "EB",
    "ZB"
  };
#else /* #if (USE_ASCII_PRINT) */
  UINT16 *capUnits[] = {
    L"B",
    L"KB",
    L"MB",
    L"GB",
    L"TB",
    L"PB",
    L"EB",
    L"ZB"
  };
#endif /* #if (USE_ASCII_PRINT) */


  //Print(L"%lld ", capacity); //DBG show original capacity

  capUnit = 0; /* B */
  if(capacity > Divisor - 1) {
    capUnit++; /* KiB */
    prevcap = capacity;
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* MiB */
    prevcap = capacity;
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* GiB */
    prevcap = capacity;
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* TiB */
    prevcap = capacity;
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* PiB */
    prevcap = capacity;
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* EiB */
    prevcap = capacity;
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* ZiB */
    prevcap = capacity;
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }

  prevcap %= Divisor;
  
  if (PrVerboseLevel > VL_SILENT) {
#if (USE_ASCII_PRINT)
    AsciiPrint("%lld.%03lld %a", capacity, prevcap, capUnits[capUnit]);
#else /* #if (USE_ASCII_PRINT) */
    Print(L"%lld.%03lld %s", capacity, prevcap, capUnits[capUnit]);
#endif /* #if (USE_ASCII_PRINT) */
  }

//#endif /* #if (DEFAULT_VERBOSE_LEVEL > VL_SILENT) */
} /* PrCapacityDec1000_000 */


void PrCapacityDec1000(UINT64 capacity) {
//#if (DEFAULT_VERBOSE_LEVEL > VL_SILENT)

  /* International System of Units (SI) */

  const UINT32 Divisor = 1000;
  UINT32 capUnit;
#if (USE_ASCII_PRINT)
  CHAR8 *capUnits[] = {
    "B",
    "KB",
    "MB",
    "GB",
    "TB",
    "PB",
    "EB",
    "ZB"
  };
#else /* #if (USE_ASCII_PRINT) */
  UINT16 *capUnits[] = {
    L"B",
    L"KB",
    L"MB",
    L"GB",
    L"TB",
    L"PB",
    L"EB",
    L"ZB"
  };
#endif /* #if (USE_ASCII_PRINT) */


  //Print(L"%lld ", capacity); //DBG show original capacity

  capUnit = 0; /* B */
  if(capacity > Divisor - 1) {
    capUnit++; /* KiB */
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* MiB */
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* GiB */
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* TiB */
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* PiB */
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* EiB */
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* ZiB */
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }

  if (PrVerboseLevel > VL_SILENT) {
#if (USE_ASCII_PRINT)
    AsciiPrint("%lld %a", capacity, capUnits[capUnit]);
#else /* #if (USE_ASCII_PRINT) */
    Print(L"%lld %s", capacity, capUnits[capUnit]);
#endif /* #if (USE_ASCII_PRINT) */
  }

//#endif /* #if (DEFAULT_VERBOSE_LEVEL > VL_SILENT) */
} /* PrCapacityDec1000 */


void PrCapacityDec1024(UINT64 capacity) {
//#if (DEFAULT_VERBOSE_LEVEL > VL_SILENT)

  /* International Electrotechnical Commission (IEC) */

  UINT64 prevcap = capacity; /* previous capacity before division */
  const UINT32 Divisor = 1024;
  UINT32 capUnit;
#if (USE_ASCII_PRINT)
  CHAR8 *capUnits[] = {
    "B",
    "KiB",
    "MiB",
    "GiB",
    "TiB",
    "PiB",
    "EiB",
    "ZiB"
  };
#else /* #if (USE_ASCII_PRINT) */
  UINT16 *capUnits[] = {
    L"B",
    L"KiB",
    L"MiB",
    L"GiB",
    L"TiB",
    L"PiB",
    L"EiB",
    L"ZiB"
  };
#endif /* #if (USE_ASCII_PRINT) */


  //Print(L"%lld ", capacity); //DBG show original capacity dividend

  capUnit = 0; /* B */
  if(capacity > Divisor - 1) {
    capUnit++; /* KiB */
    prevcap = capacity;
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* MiB */
    prevcap = capacity;
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* GiB */
    prevcap = capacity;
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* TiB */
    prevcap = capacity;
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* PiB */
    prevcap = capacity;
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* EiB */
    prevcap = capacity;
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }
  if (capacity > Divisor - 1) {
    capUnit++; /* ZiB */
    prevcap = capacity;
    capacity = InternalMathDivU64x32(capacity, Divisor);
  }

  prevcap %= Divisor;
  
  if (PrVerboseLevel > VL_SILENT) {
#if (USE_ASCII_PRINT)
    AsciiPrint("%lld %a", capacity, capUnits[capUnit]);
#else /* #if (USE_ASCII_PRINT) */
    Print(L"%lld %s", capacity, capUnits[capUnit]);
#endif /* #if (USE_ASCII_PRINT) */
  }

//#endif /* #if (DEFAULT_VERBOSE_LEVEL > VL_SILENT) */
} /* PrCapacityDec1024 */


#if defined(USE_InKey)
/* ---------------------------------------------------------------------------
InKey
--------------------------------------------------------------------------- */
EFI_STATUS InKey(OUT EFI_INPUT_KEY *Key) {
  EFI_STATUS EfiStatus;
  UINTN index;


  do {
    EfiStatus = gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &index);
    if (EFI_ERROR(EfiStatus)) {
      break;
    }
    EfiStatus = gST->ConIn->ReadKeyStroke(gST->ConIn, Key);
  } while (EFI_ERROR(EfiStatus));
  return (EfiStatus);
} /* InKey */
#endif /* #if defined(USE_InKey) */
