
/** @file
BlkCat -- UEFI Block Device Concatenator Driver
Copyright (C) 2018 Roger C. Pao.  All rights reserved.

https://github.com/rcpao/BlkCat

Roger C. Pao <rcpao+BlkCatEfi(at)gmail.com>

Apache License 2.0

**/

#ifndef __EFI_BLK_CAT_H__
#define __EFI_BLK_CAT_H__

#include <Uefi.h>

//
// Libraries
//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>


//
// UEFI Driver Model Protocols
//
#include <Protocol/DriverBinding.h>
#include <Protocol/DriverSupportedEfiVersion.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/ComponentName.h>


//
// Consumed Protocols
//
#include <Protocol/PciIo.h>


//
// Produced Protocols
//
#include <Protocol/BlockIo.h>
#include <Protocol/BlockIo2.h>


//
// Guids
//

//
// Driver Version
//
#define BLK_CAT_VERSION  0x10


//
// Protocol instances
//
extern EFI_DRIVER_BINDING_PROTOCOL  gBlkCatDriverBinding;
extern EFI_COMPONENT_NAME2_PROTOCOL  gBlkCatComponentName2;
extern EFI_COMPONENT_NAME_PROTOCOL  gBlkCatComponentName;


//
// Include files with function prototypes
//
#include <Library/BaseLib/BaseLibInternals.h> /* InternalMathMultU64x64 */
#include "DriverBinding.h"
#include "ComponentName.h"
#include "BlockIo.h"
#include "bldnum.h"
#include "pr.h"


/*
+---------+        +---------+
| CatDev0 |---+----| BlkDev0 |
+---------+   |    +---------+
              |
              |    +---------+
              +----| BlkDev1 |
                   +---------+

BLK0 LBA0:
0000 "BlkCat\0"
0010 1=Version
0014 0=Span (type)
0018 2=BlkDevNum (number of BLK devices)
0020 BLK0_GUID (identifies this particular BLK#, must be unique in this UEFI system)
0030 CAT0_GUID (identifies this RAID disk, must be unique in this UEFI system)
0040 BLK0_GUID (identifies BLK[0])
0050 BLK1_GUID (identifies BLK[1])
0060 BLK2_GUID (identifies BLK[2])
0070 BLK..._GUID (identifies BLK[...])
...0 BLK{BlkDevNum - 1}_GUID (identifies BLK[BlkDevNum - 1])

BLK1 LBA0:
0000 "BlkCat\0"
0010 1=Version
0014 0=span (type)
0018 2=BlkDevNum (number of BLK devices)
0020 BLK1_GUID (identifies this particular BLK#, must be unique in this UEFI system)
0030 CAT0_GUID (identifies this RAID disk, must be unique in this UEFI system)
0040 BLK0_GUID (identifies BLK[0])
0050 BLK1_GUID (identifies BLK[1])
0060 BLK2_GUID (identifies BLK[2])
0070 BLK..._GUID (identifies BLK[...])
...0 BLK{BlkDevNum - 1}_GUID (identifies BLK[BlkDevNum - 1])

*/

#define BLK_SIG_32 SIGNATURE_32('b', 'k', 'b', 'k')
typedef struct _BLK_DEV {
  UINT64                    Signature;

  EFI_GUID BlkGuid;

  EFI_HANDLE                Handle;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_BLOCK_IO_PROTOCOL     BlockIo;
  //EFI_BLOCK_IO2_PROTOCOL    BlockIo2;
  EFI_BLOCK_IO_MEDIA        Media;
  //EFI_BLOCK_IO_MEDIA        Media2; //For BlockIO2

  //EFI_DISK_IO_PROTOCOL      *DiskIo;
  //EFI_DISK_IO2_PROTOCOL     *DiskIo2;
  EFI_BLOCK_IO_PROTOCOL     *ParentBlockIo;
  //EFI_BLOCK_IO2_PROTOCOL    *ParentBlockIo2;
  //UINT64                    Start;
  //UINT64                    End;
  //UINT32                    BlockSize;
  //BOOLEAN                   InStop;

  //EFI_GUID                  *EspGuid;
} BLK_DEV;


#define CAT_SIG_32 SIGNATURE_32('b', 'k', 'c', 't')
typedef struct _CAT_DEV {
  UINT64 Signature;
  UINT32 BlkDevNum; /* Number of BlkDev allocated */
  UINT32 Type; /* 0=span */
  EFI_GUID CatGuid;
  EFI_GUID *BlkGuids;
  BLK_DEV *BlkDevs;
} CAT_DEV;


#if (defined(__BLK_CAT_C))


//LIST_ENTRY gBlkList;
//LIST_ENTRY gCatList;


#else /* #if (defined(__BLK_CAT_C)) */


//extern LIST_ENTRY gBlkList;
//extern LIST_ENTRY gCatList;


#endif /* #if (defined(__BLK_CAT_C)) */


#endif
