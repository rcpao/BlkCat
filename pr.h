/** @file
BlkCat -- UEFI Block Device Concatenator Driver
Copyright (C) 2018 Roger C. Pao.  All rights reserved.

https://github.com/rcpao/BlkCat

Roger C. Pao <rcpao+BlkCatEfi(at)gmail.com>

Apache License 2.0

**/

/* ---------------------------------------------------------------------------
pr.h
--------------------------------------------------------------------------- */


/* Verbose Levels (independent from DBG_* output) */
#define VL_SILENT_PERM	0 /* permanently silent as function calls are #define'd away */
#define VL_SILENT	1 /* silent, but can be overridden by LBA1.VerboseLevel */
#define VL_NORMAL	2 /* normal, but can be overridden by LBA1.VerboseLevel */

/* http://stackoverflow.com/questions/1644868/c-define-macro-for-debug-printing */
#define DL_00 0x00
#define DL_01 0x01
#define DL_0A 0x0A
#define DL_80 0x80
#define DL_FE 0xFE
#define DL_FF 0xFF
#define DL_MIN DL_01
#define DL_MAX DL_FE
#define DL_DISABLED DL_FF

#include "pr-cfg.h" /* modified by mk-build{,-silent}.bat or manually */

#if !defined(DEFAULT_VERBOSE_LEVEL)
#define DEFAULT_VERBOSE_LEVEL VL_SILENT /* VL_SILENT_PERM=0 VL_SILENT=1 VL_NORMAL=2 */
#endif /* #if !defined(DEFAULT_VERBOSE_LEVEL) */

#if !defined(DEFAULT_DEBUG_LEVEL)
#define DEFAULT_DEBUG_LEVEL DL_MIN /* [DL_MIN..DL_MAX] */
#endif /* #if !defined(DEFAULT_DEBUG_LEVEL) */


#ifndef __PR_H__
#define __PR_H__


/* ---------------------------------------------------------------------------
Defines
--------------------------------------------------------------------------- */

//#include <ctype.h> /* isprint */
#define isprint(c) ((c >= 0x20) && (c <= 0x7E))
  /* beware of side effects */
#define isdigit(x) ((x) >= '0' && (x) <= '9')
  /* beware of side effects */


/* %u does not work in AsciiPrint() */
#define PRId8 "d"
//#define PRIu8 "u"
#define PRIx8 "x"
#define PRIX8 "X"
#define PRId16 "d"
//#define PRIu16 "u"
#define PRIx16 "x"
#define PRIX16 "X"
#define PRId32 "d"
//#define PRIu32 "u"
#define PRIx32 "x"
#define PRIX32 "X"
/* %I64d */
#define PRId64 "ld"
//#define PRIu64 "lu"
#define PRIx64 "lx"
#define PRIX64 "lX"


#if (defined(__GNUC__))
#define PR(fmt, ...) \
	do { \
		AsciiPrint(fmt, ##__VA_ARGS__); \
	} while (0)
#else /* #if (defined(__GNUC__)) */
#define PR(fmt, ...) \
	do { \
		AsciiPrint(fmt, __VA_ARGS__); \
	} while (0)
#endif /* #if (defined(__GNUC__)) */


#if (DEFAULT_VERBOSE_LEVEL == VL_SILENT_PERM)
/* TBD DBG Define these macros to make this driver permanently silent. */
#define AsciiPrint(fmt, ...) 1
#define Print(fmt, ...) 1
#endif


#define DBG_ENABLED 1

/* Example use: DBG_X(dbglvl, (x)) */
#define DBG_X(dbglvl, x) \
	do { \
		if ((DBG_ENABLED) && (PrDbgLevel >= dbglvl)) { \
			x; \
		} \
	} while (0)

#if (defined(__GNUC__))
#define DBG_PR(dbglvl, fmt, ...) \
	do { \
		if ((DBG_ENABLED) && (PrDbgLevel >= dbglvl)) { \
			AsciiPrint("%a(%d): ", FN, __LINE__); \
			AsciiPrint(fmt, ##__VA_ARGS__); \
		} \
	} while (0)
#else /* #if (defined(__GNUC__)) */
#define DBG_PR(dbglvl, fmt, ...) \
	do { \
		if ((DBG_ENABLED) && (PrDbgLevel >= dbglvl)) { \
			AsciiPrint("%a(%d): ", FN, __LINE__); \
			AsciiPrint(fmt, __VA_ARGS__); \
		} \
	} while (0)
#endif /* #if (defined(__GNUC__)) */

/* Example use: PRX((fmt, ...)) */
#define PRX(args) \
	do { \
		AsciiPrint(args); \
	} while (0)


#if (defined(__GNUC__))
#define _DBG_PR(dbglvl, fmt, ...) \
	do { \
		if ((DBG_ENABLED) && (PrDbgLevel >= dbglvl)) { \
			AsciiPrint(fmt, ##__VA_ARGS__); \
		} \
	} while (0)
#else /* #if (defined(__GNUC__)) */
#define _DBG_PR(dbglvl, fmt, ...) \
	do { \
		if ((DBG_ENABLED) && (PrDbgLevel >= dbglvl)) { \
			AsciiPrint(fmt, __VA_ARGS__); \
		} \
	} while (0)
#endif /* #if (defined(__GNUC__)) */

#define DBG_PR_BUF(dbglvl, pBuf, Length) \
	do { \
		if ((DBG_ENABLED) && (PrDbgLevel >= dbglvl)) { \
			PrBufxxdr(pBuf, Length); \
		} \
	} while (0)


/* ---------------------------------------------------------------------------
Externals
--------------------------------------------------------------------------- */

#if (defined(__PR_C))

UINTN PrVerboseLevel = DEFAULT_VERBOSE_LEVEL;
UINTN PrDbgLevel = DEFAULT_DEBUG_LEVEL;
UINTN PrIndentLevel = 0;
UINT32 PrOptionsF8 = 0;

#else /* #if (defined(__PR_C)) */

extern UINTN PrVerboseLevel;
extern UINTN PrDbgLevel;
extern UINTN PrIndentLevel;
extern UINT32 PrOptionsF8;

#endif /* #if (defined(__PR_C)) */


/* ---------------------------------------------------------------------------
Function Prototypes
--------------------------------------------------------------------------- */

UINT64 PrChar(INT8 /* CHAR8 */ c);
UINT64 PrHexBits4(UINT8 n);
UINT64 PrHexBits8(UINT8 n);
UINT64 PrHexBits16(UINT16 n);
UINT64 PrHexBits32(UINT32 n);
UINT64 PrHexBits64(UINT64 n);
UINT64 PrAscii(void *pBuf);

UINT64 PrIndent(void);
UINT64 PrBuf(void *pBuf, UINTN Length);
UINT64 PrBufxxdr(void *pBuf, UINTN Length);
void PrCapacityDec1000_000(UINT64 capacity);
void PrCapacityDec1000(UINT64 capacity);
void PrCapacityDec1024(UINT64 capacity);

#if defined(USE_InKey)
EFI_STATUS InKey(OUT EFI_INPUT_KEY *Key);
#endif /* #if defined(USE_InKey) */


#endif /* #ifndef __PR_H__ */
