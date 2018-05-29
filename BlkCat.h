
/** @file
BlkCat -- UEFI Block Device Concatenator Driver
Copyright (C) 2018 Roger C. Pao.  All rights reserved.

https://github.com/rcpao/BlkCat

Roger C. Pao <rcpao+BlkCatEfi@gmail.com>
  
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
#include "DriverBinding.h"
#include "ComponentName.h"
#include "BlockIo.h"


#endif
