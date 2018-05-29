/** @file
BlkCat -- UEFI Block Device Concatenator Driver
Copyright (C) 2018 Roger C. Pao.  All rights reserved.

https://github.com/rcpao/BlkCat

Roger C. Pao <rcpao+BlkCatEfi@gmail.com>
  
Apache License 2.0

**/

/* ---------------------------------------------------------------------------
pr-cfg-normal.h

This file is copied to pr-cfg.h by mk-build-dbg.bat and 
#included by "pr.h" to display messages during load via Driver Options, 
v1shell, or v2shell.
--------------------------------------------------------------------------- */


/* set by mk-build-dbg.bat */
#define DEFAULT_VERBOSE_LEVEL VL_NORMAL /* VL_SILENT_PERM=0 VL_SILENT=1 VL_NORMAL=2 */
#define DEFAULT_DEBUG_LEVEL DL_MAX /* [DL_MIN=0x01..DL_MAX=0xFE] */
