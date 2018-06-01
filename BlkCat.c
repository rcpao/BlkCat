
/** @file
BlkCat -- UEFI Block Device Concatenator Driver
Copyright (C) 2018 Roger C. Pao.  All rights reserved.

https://github.com/rcpao/BlkCat

Roger C. Pao <rcpao+BlkCatEfi(at)gmail.com>

Apache License 2.0

**/

#include "BlkCat.h"


/* DRIVER_NAME_STR must match TierDisk.inf/BASE_NAME */
#define DRIVER_NAME_STR "BlkCat"
#define DRIVER_NAME_LSTR L"BlkCat"


#define EFI_BLK_CAT_DRIVER_PROTOCOL_GUID \
  {0x26accc9d, 0xd1ed, 0x4cae, {0x86, 0x3b, 0x28, 0x01, 0xd8, 0x81, 0x75, 0xce}}

/* CONST */ EFI_GUID gEfiBlkCatDriverProtocolGuid = EFI_BLK_CAT_DRIVER_PROTOCOL_GUID;

typedef struct _BLK_CAT_DEVICE_PATH {
  EFI_DEVICE_PATH Header;
  EFI_GUID Guid; /* EFI_BLK_CAT_DRIVER_PROTOCOL_GUID */
  UINT8 DiskId[8];
  EFI_DEVICE_PATH EndDevicePath;
} BLK_CAT_DEVICE_PATH;

CONST BLK_CAT_DEVICE_PATH TierDiskDevicePathTemplate = {
  {
    MESSAGING_DEVICE_PATH, /* Type */
    MSG_VENDOR_DP, /* SubType */
    {
      sizeof(BLK_CAT_DEVICE_PATH) - END_DEVICE_PATH_LENGTH, /* Length LSB */
      0, /* Length MSB */
    }
  },
  /* GUID {2e1a32a1-d1db-11e5-8bff-005056c00008} */
  EFI_BLK_CAT_DRIVER_PROTOCOL_GUID,
  {0,0,0,0,0,0,0,0},	// DiskId = TdRef assigned later
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {END_DEVICE_PATH_LENGTH, 0}
  }
};


///
/// Driver Support EFI Version Protocol instance
///
GLOBAL_REMOVE_IF_UNREFERENCED
EFI_DRIVER_SUPPORTED_EFI_VERSION_PROTOCOL gBlkCatDriverSupportedEfiVersion = {
  sizeof (EFI_DRIVER_SUPPORTED_EFI_VERSION_PROTOCOL),
  0x0002001E
};


///
/// Driver Binding Protocol instance
///
EFI_DRIVER_BINDING_PROTOCOL gBlkCatDriverBinding = {
  BlkCatDriverBindingSupported,
  BlkCatDriverBindingStart,
  BlkCatDriverBindingStop,
  BLD_NUM /* BLK_CAT_VERSION */,
  NULL,
  NULL
};


/**
  Unloads an image.

  @param  ImageHandle           Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS           The image has been unloaded.
  @retval EFI_INVALID_PARAMETER ImageHandle is not a valid image handle.

**/
EFI_STATUS
EFIAPI
BlkCatUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
#undef FN
#define FN "BlkCatUnload"
#define DBG_BlkCatUnload DL_80 /* DL_DISABLED DL_80 */

  EFI_STATUS  Status;

  EFI_HANDLE  *HandleBuffer;
  UINTN       HandleCount;
  UINTN       Index;


  DBG_PR(DBG_BlkCatUnload, "ImageHandle=%"PRIx64"\n", ImageHandle);

  Status = EFI_SUCCESS;

  //
  // Retrieve array of all handles in the handle database
  //
  Status = gBS->LocateHandleBuffer (
                  AllHandles,
                  NULL,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Disconnect the current driver from handles in the handle database
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->DisconnectController (HandleBuffer[Index], gImageHandle, NULL);
  }

  //
  // Free the array of handles
  //
  FreePool (HandleBuffer);


  //
  // Uninstall protocols installed in the driver entry point
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  ImageHandle,
                  &gEfiDriverBindingProtocolGuid,  &gBlkCatDriverBinding,
                  &gEfiComponentNameProtocolGuid,  &gBlkCatComponentName,
                  &gEfiComponentName2ProtocolGuid, &gBlkCatComponentName2,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }


  //
  // Uninstall Driver Supported EFI Version Protocol onto ImageHandle
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  ImageHandle,
                  &gEfiDriverSupportedEfiVersionProtocolGuid, &gBlkCatDriverSupportedEfiVersion,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }


  //
  // Do any additional cleanup that is required for this driver
  //

  return EFI_SUCCESS;
} /* BlkCatUnload */


/**
  This is the declaration of an EFI image entry point. This entry point is
  the same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including
  both device drivers and bus drivers.

  @param  ImageHandle           The firmware allocated handle for the UEFI image.
  @param  SystemTable           A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval Others                An unexpected error occurred.
**/
EFI_STATUS
EFIAPI
BlkCatDriverEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
#undef FN
#define FN "BlkCatDriverEntryPoint"
#define DBG_BlkCatDriverEntryPoint DL_DISABLED /* DL_DISABLED DL_80 */

  EFI_STATUS  Status;


  /* PR() in DriverEntry is a no op if embedded in UEFI firmware (maybe 'bcfg driver add' is also silent). */
  PR(
    DRIVER_NAME_STR
    ".efi v"
    VERSION_STR
    "\n",
    BLD_NUM
  );
  DBG_PR(DBG_BlkCatDriverEntryPoint, "ImageHandle=%"PRIx64" SystemTable=%"PRIx64" \n", ImageHandle, SystemTable);

  Status = EFI_SUCCESS;


#if 1
#define INSTALL_MULTIPLE_PROTOCOL_INTERFACES(Handle, ProtocolGuid, ProtocolInterfaces) { \
  DBG_PR(DBG_BlkCatDriverEntryPoint, "INSTALL_MULTIPLE_PROTOCOL_INTERFACES(Handle="); \
  DBG_X(DBG_BlkCatDriverEntryPoint, (PrBufxxdr(Handle, 8))); \
  if ((Handle != NULL) && (*Handle != NULL)) { \
    _DBG_PR(DBG_BlkCatDriverEntryPoint, "; Handle->Signature="); \
    DBG_X(DBG_BlkCatDriverEntryPoint, (PrBufxxdr(*Handle, 8))); \
  } \
  _DBG_PR(DBG_BlkCatDriverEntryPoint, "; ProtocolGuid=%p,%g", ProtocolGuid, ProtocolGuid); \
  _DBG_PR(DBG_BlkCatDriverEntryPoint, "; ProtocolInterfaces="); \
  DBG_X(DBG_BlkCatDriverEntryPoint, (PrBufxxdr(ProtocolInterfaces, 8))); \
  _DBG_PR(DBG_BlkCatDriverEntryPoint, ";\n"); \
  Status = gBS->InstallMultipleProtocolInterfaces( \
    Handle, \
    ProtocolGuid, \
    ProtocolInterfaces, \
    NULL, \
    NULL \
  ); \
}

  INSTALL_MULTIPLE_PROTOCOL_INTERFACES(
    &ImageHandle,
    &gEfiDevicePathProtocolGuid,
    (VOID *)&TierDiskDevicePathTemplate
  );
  DBG_PR(DBG_BlkCatDriverEntryPoint, "InstallMultipleProtocolInterfaces gEfiDevicePathProtocolGuid TierDiskDevicePathTemplate '%r'\n", Status);
  ASSERT_EFI_ERROR (Status); /* ASSERT_EFI_ERROR() does not actually exit!  Without DEBUG firmware, it is a no op. */
  if (EFI_ERROR(Status)) goto ReturnStatus;
#endif


  //
  // Install UEFI Driver Model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gBlkCatDriverBinding,
             ImageHandle,
             &gBlkCatComponentName,
             &gBlkCatComponentName2
             );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) goto ReturnStatus;


  //
  // Install Driver Supported EFI Version Protocol onto ImageHandle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiDriverSupportedEfiVersionProtocolGuid, &gBlkCatDriverSupportedEfiVersion,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) goto ReturnStatus;


ReturnStatus:

  if (EFI_ERROR(Status)) {
    PR(
      DRIVER_NAME_STR
      ".efi v"
      VERSION_STR
      " '%r'\n",
      BLD_NUM, Status
    );
  }

  return Status;
} /* BlkCatDriverEntryPoint */


/**
  Tests to see if this driver supports a given controller. If a child device is provided,
  it further tests to see if this driver supports creating a handle for the specified child device.

  This function checks to see if the driver specified by This supports the device specified by
  ControllerHandle. Drivers will typically use the device path attached to
  ControllerHandle and/or the services from the bus I/O abstraction attached to
  ControllerHandle to determine if the driver supports ControllerHandle. This function
  may be called many times during platform initialization. In order to reduce boot times, the tests
  performed by this function must be very small, and take as little time as possible to execute. This
  function must not change the state of any hardware devices, and this function must be aware that the
  device specified by ControllerHandle may already be managed by the same driver or a
  different driver. This function must match its calls to AllocatePages() with FreePages(),
  AllocatePool() with FreePool(), and OpenProtocol() with CloseProtocol().
  Because ControllerHandle may have been previously started by the same driver, if a protocol is
  already in the opened state, then it must not be closed with CloseProtocol(). This is required
  to guarantee the state of ControllerHandle is not modified by this function.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to test. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For bus drivers, if this parameter is not NULL, then
                                   the bus driver must determine if the bus controller specified
                                   by ControllerHandle and the child controller specified
                                   by RemainingDevicePath are both supported by this
                                   bus driver.

  @retval EFI_SUCCESS              The device specified by ControllerHandle and
                                   RemainingDevicePath is supported by the driver specified by This.
  @retval EFI_ALREADY_STARTED      The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by the driver
                                   specified by This.
  @retval EFI_ACCESS_DENIED        The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by a different
                                   driver or an application that requires exclusive access.
                                   Currently not implemented.
  @retval EFI_UNSUPPORTED          The device specified by ControllerHandle and
                                   RemainingDevicePath is not supported by the driver specified by This.
**/
EFI_STATUS
EFIAPI
BlkCatDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
#undef FN
#define FN "BlkCatDriverBindingSupported"
#define DBG_BlkCatDriverBindingSupported DL_80 /* DL_DISABLED DL_80 */

  EFI_STATUS Status = EFI_SUCCESS;


  //DBG_PR(DBG_BlkCatDriverBindingSupported, "This=%"PRIx64" ControllerHandle=%"PRIx64" RemainingDevicePath=%p\n", This, ControllerHandle, RemainingDevicePath);


#if 1
  /* from C:\edk2\MdeModulePkg\Universal\Disk\PartitionDxe\Partition.c */
  //
  // Check RemainingDevicePath validation
  //
  if (RemainingDevicePath != NULL) {
    //
    // Check if RemainingDevicePath is the End of Device Path Node,
    // if yes, go on checking other conditions
    //
    if (!IsDevicePathEnd(RemainingDevicePath)) {
      EFI_DEV_PATH *Node;
      //
      // If RemainingDevicePath isn't the End of Device Path Node,
      // check its validation
      //
      Node = (EFI_DEV_PATH *)RemainingDevicePath;
      if (
        (Node->DevPath.Type != MEDIA_DEVICE_PATH) ||
        (Node->DevPath.SubType != MEDIA_HARDDRIVE_DP) ||
        (DevicePathNodeLength(&Node->DevPath) != sizeof(HARDDRIVE_DEVICE_PATH))
      ) {
        //return (EFI_UNSUPPORTED);
        Status = EFI_UNSUPPORTED; goto ReturnStatus;
      }
    }
  }
#endif


#if 1
  /* from C:\edk2\MdeModulePkg\Universal\Disk\PartitionDxe\Partition.c */
  {
    EFI_DEVICE_PATH_PROTOCOL *ParentDevicePath;

    //
    // Open the EFI Device Path protocol needed to perform the supported test
    //
    Status = gBS->OpenProtocol(
      ControllerHandle,
      &gEfiDevicePathProtocolGuid,
      (VOID **)&ParentDevicePath,
      This->DriverBindingHandle,
      ControllerHandle,
      EFI_OPEN_PROTOCOL_BY_DRIVER
    );
#if 1 //TBD
    if (Status == EFI_ALREADY_STARTED) {
      DBG_PR(DBG_BlkCatDriverBindingSupported, "gEfiDevicePathProtocolGuid %r but returning EFI_SUCCESS\n", Status);
      //return (EFI_SUCCESS);
      Status = EFI_SUCCESS; goto ReturnStatus;
    }
#endif
    if (EFI_ERROR(Status)) {
      //DBG_PR(DBG_BlkCatDriverBindingSupported, "gEfiDevicePathProtocolGuid %r\n", Status);
      //return (Status);
      goto ReturnStatus;
    }

    //
    // Close protocol, don't use device path protocol in the Support() function
    //
    gBS->CloseProtocol(
      ControllerHandle,
      &gEfiDevicePathProtocolGuid,
      This->DriverBindingHandle,
      ControllerHandle
    );
  }
#endif /* #if 1 */


#if 1
  /* from C:\edk2\MdeModulePkg\Universal\Disk\PartitionDxe\Partition.c */
  //
  // Open the IO Abstraction(s) needed to perform the supported test
  //
  Status = gBS->OpenProtocol(
    ControllerHandle,
    &gEfiBlockIoProtocolGuid,
    NULL,
    This->DriverBindingHandle,
    ControllerHandle,
    EFI_OPEN_PROTOCOL_TEST_PROTOCOL
  );
  /* EFI_OPEN_PROTOCOL_TEST_PROTOCOL does not require CloseProtocol() */
#endif /* #if 1 */

  //DBG_PR(DBG_BlkCatDriverBindingSupported, "leaving EFI_OPEN_PROTOCOL_TEST_PROTOCOL='%r'\n", Status);


ReturnStatus:

  return (Status);
} /* BlkCatDriverBindingSupported */

/**
  Starts a device controller or a bus controller.

  The Start() function is designed to be invoked from the EFI boot service ConnectController().
  As a result, much of the error checking on the parameters to Start() has been moved into this
  common boot service. It is legal to call Start() from other locations,
  but the following calling restrictions must be followed, or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE.
  2. If RemainingDevicePath is not NULL, then it must be a pointer to a naturally aligned
     EFI_DEVICE_PATH_PROTOCOL.
  3. Prior to calling Start(), the Supported() function for the driver specified by This must
     have been called with the same calling parameters, and Supported() must have returned EFI_SUCCESS.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to start. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For a bus driver, if this parameter is NULL, then handles
                                   for all the children of Controller are created by this driver.
                                   If this parameter is not NULL and the first Device Path Node is
                                   not the End of Device Path Node, then only the handle for the
                                   child device specified by the first Device Path Node of
                                   RemainingDevicePath is created by this driver.
                                   If the first Device Path Node of RemainingDevicePath is
                                   the End of Device Path Node, no child handle is created by this
                                   driver.

  @retval EFI_SUCCESS              The device was started.
  @retval EFI_DEVICE_ERROR         The device could not be started due to a device error.Currently not implemented.
  @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.
  @retval Others                   The driver failded to start the device.

**/
EFI_STATUS
EFIAPI
BlkCatDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath OPTIONAL
  )
{
#undef FN
#define FN "BlkCatDriverBindingStart"
#define DBG_BlkCatDriverBindingStart DL_80 /* DL_DISABLED DL_80 */
  EFI_TPL OldTpl;
  EFI_BLOCK_IO_PROTOCOL *BlockIo;
  //EFI_DISK_IO_PROTOCOL *DiskIo;
  EFI_DEVICE_PATH_PROTOCOL *ParentDevicePath;
  BOOLEAN MediaPresent;
  //EFI_STATUS OpenStatus;
  EFI_STATUS Status = EFI_UNSUPPORTED;


  DBG_PR(DBG_BlkCatDriverBindingStart, "This=%"PRIx64" ControllerHandle=%"PRIx64" RemainingDevicePath=%p\n", This, ControllerHandle, RemainingDevicePath);


  /* from C:\edk2\MdeModulePkg\Universal\Disk\PartitionDxe\Partition.c */
  OldTpl = gBS->RaiseTPL(TPL_CALLBACK);
  //
  // Check RemainingDevicePath validation
  //
  if (RemainingDevicePath != NULL) {
    //
    // Check if RemainingDevicePath is the End of Device Path Node,
    // if yes, return EFI_SUCCESS
    //
    if (IsDevicePathEnd(RemainingDevicePath)) {
      DBG_PR(DBG_BlkCatDriverBindingStart, "RemainingDevicePath %x = END\n", RemainingDevicePath);
      Status = EFI_SUCCESS;
      goto ReturnStatus;
    }
  }


  #if 1
    /* from C:\edk2\MdeModulePkg\Universal\Disk\PartitionDxe\Partition.c */
    //
    // Try to open BlockIO. If BlockIO would be opened, continue,
    // otherwise, return error.
    //
    Status = gBS->OpenProtocol(
      ControllerHandle,
      &gEfiBlockIoProtocolGuid,
      (VOID **)&BlockIo,
      This->DriverBindingHandle,
      ControllerHandle,
      EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    if (EFI_ERROR(Status)) {
      DBG_PR(DBG_BlkCatDriverBindingStart, "gEfiBlockIoProtocolGuid %r\n", Status);
      goto ReturnStatus;
    }
    /* EFI_OPEN_PROTOCOL_GET_PROTOCOL does not require CloseProtocol(). */
  #endif


#if 1
    {
      /* from C:\edk2\MdeModulePkg\Universal\Disk\PartitionDxe\Partition.c */
      //
      // Get the Device Path Protocol on ControllerHandle's handle.
      //
      Status = gBS->OpenProtocol(
        ControllerHandle,
        &gEfiDevicePathProtocolGuid,
        (VOID **)&ParentDevicePath,
        This->DriverBindingHandle,
        ControllerHandle,
        EFI_OPEN_PROTOCOL_BY_DRIVER
      );
      if (EFI_ERROR(Status) && Status != EFI_ALREADY_STARTED) {
        DBG_PR(DBG_BlkCatDriverBindingStart, "gEfiDevicePathProtocolGuid %r\n", Status);
        goto ReturnStatus;
      }

      //AsciiPrint(" '%s'\n", ConvertDevicePathToText(DevicePathFromHandle(BlockIo), TRUE, TRUE));
  }
#endif


#if 1
    Status = EFI_UNSUPPORTED;
    MediaPresent = BlockIo->Media->MediaPresent;
    if (BlockIo->Media->MediaPresent ||
      (BlockIo->Media->RemovableMedia && !BlockIo->Media->LogicalPartition)) {
        UINT64 Capacity;


        PR("$=0x%"PRIx64"=%lld bs=0x%"PRIx64"=%lld '",
          BlockIo->Media->LastBlock,
          BlockIo->Media->LastBlock,
          BlockIo->Media->BlockSize,
          BlockIo->Media->BlockSize
        );
        Capacity = InternalMathMultU64x32(BlockIo->Media->LastBlock + 1, BlockIo->Media->BlockSize);
        PrCapacityDec1000_000(Capacity);
        AsciiPrint("' %a", (BlockIo->Media->LogicalPartition)? "(partition)": "");
        AsciiPrint("%a", (BlockIo->Media->MediaPresent)? "": "(no media)");
        AsciiPrint("%a", (BlockIo->Media->ReadOnly)? "(ro)": "");
        AsciiPrint("\n");
        AsciiPrint("  This='%s'\n",
          ConvertDevicePathToText(DevicePathFromHandle(This), TRUE, TRUE)
        );
        AsciiPrint("  ControllerHandle='%s'\n",
          ConvertDevicePathToText(DevicePathFromHandle(ControllerHandle), TRUE, TRUE)
        );
        AsciiPrint("  BlockIo='%s'\n",
          ConvertDevicePathToText(DevicePathFromHandle(BlockIo), TRUE, TRUE)
        );
        AsciiPrint("  ParentDevicePath='%s'\n",
          ConvertDevicePathToText(DevicePathFromHandle(ParentDevicePath), TRUE, TRUE)
        );
        AsciiPrint("  This->DriverBindingHandle='%s'\n",
          ConvertDevicePathToText(DevicePathFromHandle(This->DriverBindingHandle), TRUE, TRUE)
        );

  	  /* Install the BlkCat Disks */
  	  //Status = InstallBlkCatDisks(This, ControllerHandle); /* Driver Options works in GA-990FX-Gaming, fails in MinnowBoard Turbot */
  	  //Status = InstallBlkCatDisks(This->DriverBindingHandle); /* Driver Options fails */
  	  /*
  	  http://wiki.phoenix.com/wiki/index.php/EFI_BOOT_SERVICES#OpenProtocol.28.29
  	    AgentHandle
  	    The handle of the agent that is opening the protocol interface specified by Protocol and Interface. For agents that follow the UEFI Driver Model, this parameter is the handle that contains the EFI_DRIVER_BINDING_PROTOCOL instance that is produced by the UEFI driver that is opening the protocol interface. For UEFI applications, this is the image handle of the UEFI application that is opening the protocol interface. For applications that use HandleProtocol() to open a protocol interface, this parameter is the image handle of the EFI firmware.
  	  */
  	  //DBG_PR(DBG_BlkCatDriverBindingStart, "after InstallBlkCatDisks %r\n", Status);
  	  if (EFI_ERROR(Status)) {
  	    //DBG_PR(DBG_BlkCatDriverBindingStart, "InstallBlkCatDisks failed %r\n", Status);
        goto ReturnStatus;
  	    //return (Status);
  	  }
    }
#endif


ReturnStatus:
  gBS->RestoreTPL(OldTpl);

  return (Status);

} /* BlkCatDriverBindingStart */


/**
  Stops a device controller or a bus controller.

  The Stop() function is designed to be invoked from the EFI boot service DisconnectController().
  As a result, much of the error checking on the parameters to Stop() has been moved
  into this common boot service. It is legal to call Stop() from other locations,
  but the following calling restrictions must be followed, or the system behavior will not be deterministic.
  1. ControllerHandle must be a valid EFI_HANDLE that was used on a previous call to this
     same driver's Start() function.
  2. The first NumberOfChildren handles of ChildHandleBuffer must all be a valid
     EFI_HANDLE. In addition, all of these handles must have been created in this driver's
     Start() function, and the Start() function must have called OpenProtocol() on
     ControllerHandle with an Attribute of EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER.

  @param[in]  This              A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle  A handle to the device being stopped. The handle must
                                support a bus specific I/O protocol for the driver
                                to use to stop the device.
  @param[in]  NumberOfChildren  The number of child device handles in ChildHandleBuffer.
  @param[in]  ChildHandleBuffer An array of child handles to be freed. May be NULL
                                if NumberOfChildren is 0.

  @retval EFI_SUCCESS           The device was stopped.
  @retval EFI_DEVICE_ERROR      The device could not be stopped due to a device error.

**/
EFI_STATUS
EFIAPI
BlkCatDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   ControllerHandle,
  IN UINTN                        NumberOfChildren,
  IN EFI_HANDLE                   *ChildHandleBuffer OPTIONAL
  )
{
#undef FN
#define FN "BlkCatDriverBindingStop"
#define DBG_BlkCatDriverBindingStop DL_80 /* DL_DISABLED DL_80 */

  DBG_PR(DBG_BlkCatDriverBindingStop, "This=%"PRIx64" ControllerHandle=%"PRIx64" NumberOfChildren=%d ChildHandleBuffer=%p\n", This, ControllerHandle, NumberOfChildren, ChildHandleBuffer);

  return EFI_UNSUPPORTED;
} /* BlkCatDriverBindingStop */
