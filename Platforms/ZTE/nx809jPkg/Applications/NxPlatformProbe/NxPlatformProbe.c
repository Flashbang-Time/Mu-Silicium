/** @file
  Read-only UEFI checks for the NX809J Windows bring-up.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/PartitionInfo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/UsbIo.h>
#include <Protocol/Usb2HostController.h>

STATIC
CONST UFS_DEVICE_PATH *
FindUfsNode (
  IN CONST EFI_DEVICE_PATH_PROTOCOL *Path
  )
{
  while ((Path != NULL) && !IsDevicePathEnd (Path)) {
    if ((DevicePathType (Path) == MESSAGING_DEVICE_PATH) &&
        (DevicePathSubType (Path) == MSG_UFS_DP) &&
        (DevicePathNodeLength (Path) >= sizeof (UFS_DEVICE_PATH))) {
      return (CONST UFS_DEVICE_PATH *)Path;
    }

    Path = NextDevicePathNode (Path);
  }

  return NULL;
}

STATIC
CONST HARDDRIVE_DEVICE_PATH *
FindPartitionNode (
  IN CONST EFI_DEVICE_PATH_PROTOCOL *Path
  )
{
  while ((Path != NULL) && !IsDevicePathEnd (Path)) {
    if ((DevicePathType (Path) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (Path) == MEDIA_HARDDRIVE_DP) &&
        (DevicePathNodeLength (Path) >= sizeof (HARDDRIVE_DEVICE_PATH))) {
      return (CONST HARDDRIVE_DEVICE_PATH *)Path;
    }

    Path = NextDevicePathNode (Path);
  }

  return NULL;
}

STATIC
UINTN
SelectProbeTextMode (
  VOID
  )
{
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *Console;
  UINTN                           Mode;
  UINTN                           BestMode;
  UINTN                           BestArea;
  UINTN                           Columns;
  UINTN                           Rows;

  Console = gST->ConOut;
  if ((Console == NULL) || (Console->Mode == NULL) || (Console->Mode->MaxMode <= 0)) {
    return 25;
  }

  BestMode = 0;
  BestArea = 0;
  for (Mode = 0; Mode < (UINTN)Console->Mode->MaxMode; Mode++) {
    if (!EFI_ERROR (Console->QueryMode (Console, Mode, &Columns, &Rows)) &&
        (Columns >= 60) && (Columns * Rows > BestArea)) {
      BestMode = Mode;
      BestArea = Columns * Rows;
    }
  }

  if ((BestArea != 0) && (Console->Mode->Mode != (INT32)BestMode)) {
    Console->SetMode (Console, BestMode);
  }

  Console->EnableCursor (Console, FALSE);
  Console->ClearScreen (Console);
  if ((Console->Mode->Mode >= 0) &&
      !EFI_ERROR (Console->QueryMode (Console, (UINTN)Console->Mode->Mode, &Columns, &Rows))) {
    return Rows;
  }

  return 25;
}

STATIC
BOOLEAN
IsPartitionHandle (
  IN EFI_HANDLE Handle
  )
{
  EFI_BLOCK_IO_PROTOCOL *Io;

  return !EFI_ERROR (gBS->HandleProtocol (Handle, &gEfiBlockIoProtocolGuid, (VOID **)&Io)) &&
         (Io->Media != NULL) && Io->Media->LogicalPartition;
}

STATIC
VOID
PrintPartition (
  IN EFI_HANDLE Handle,
  IN UINTN      Number
  )
{
  EFI_DEVICE_PATH_PROTOCOL       *Path;
  CONST UFS_DEVICE_PATH          *Ufs;
  CONST HARDDRIVE_DEVICE_PATH    *Partition;
  EFI_PARTITION_INFO_PROTOCOL    *Info;
  CHAR16                         Name[37];
  UINTN                          Index;

  Path      = DevicePathFromHandle (Handle);
  Ufs       = FindUfsNode (Path);
  Partition = FindPartitionNode (Path);
  if (Ufs != NULL) {
    Print (L"%u. UFS LUN %u", Number, Ufs->Lun);
  } else {
    Print (L"%u. Block device", Number);
  }

  if (Partition != NULL) {
    Print (L"  Part %u  Start %Lx  Sectors %Lx\n",
           Partition->PartitionNumber, Partition->PartitionStart, Partition->PartitionSize);
  } else {
    Print (L"  (no HD device path)\n");
  }

  if (!EFI_ERROR (gBS->HandleProtocol (Handle, &gEfiPartitionInfoProtocolGuid, (VOID **)&Info)) &&
      (Info->Type == PARTITION_TYPE_GPT)) {
    CopyMem (Name, Info->Info.Gpt.PartitionName, sizeof (Info->Info.Gpt.PartitionName));
    Name[ARRAY_SIZE (Name) - 1] = L'\0';
    for (Index = 0; (Index < ARRAY_SIZE (Name) - 1) && (Name[Index] != L'\0'); Index++) {
      if (Name[Index] < 0x20) {
        Name[Index] = L'?';
      }
    }

    Print (L"   %s%s\n", Name[0] != L'\0' ? Name : L"(unnamed GPT partition)",
           Info->System ? L"  [EFI system]" : L"");
  } else {
    Print (L"   (no GPT label exposed)\n");
  }
}

STATIC
VOID
PrintPartitionPages (
  IN UINTN Rows
  )
{
  EFI_STATUS Status;
  EFI_HANDLE *Handles;
  UINTN      Count;
  UINTN      Index;
  UINTN      Partitions;
  UINTN      Shown;
  UINTN      PerPage;
  UINTN      Pages;

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &Count, &Handles);
  if (EFI_ERROR (Status)) {
    Print (L"Partition list: no Block I/O handles (%r).\n", Status);
    return;
  }

  Partitions = 0;
  for (Index = 0; Index < Count; Index++) {
    if (IsPartitionHandle (Handles[Index])) {
      Partitions++;
    }
  }

  if (Partitions == 0) {
    Print (L"Mu published no partition handles.\n");
    FreePool (Handles);
    return;
  }

  // Reserve room for the page header and the next-page notice.
  PerPage = (Rows >= 8) ? (Rows - 6) / 2 : 1;
  if (PerPage > 16) {
    PerPage = 16;
  }

  Pages = (Partitions + PerPage - 1) / PerPage;
  Shown = 0;
  for (Index = 0; Index < Count; Index++) {
    if (!IsPartitionHandle (Handles[Index])) {
      continue;
    }

    if ((Shown % PerPage) == 0) {
      gST->ConOut->ClearScreen (gST->ConOut);
      Print (L"NX809J partitions: page %u of %u (%u total)\n\n",
             Shown / PerPage + 1, Pages, Partitions);
    }

    PrintPartition (Handles[Index], ++Shown);
    if (((Shown % PerPage) == 0) || (Shown == Partitions)) {
      Print (L"\n%s in 12 seconds.\n", Shown == Partitions ? L"Storage summary" : L"Next page");
      gBS->Stall (12000000);
    }
  }

  gST->ConOut->ClearScreen (gST->ConOut);
  Print (L"Mu exposed %u partitions.\n", Partitions);
  FreePool (Handles);
}

STATIC
VOID
CheckUfsReads (
  IN EFI_BLOCK_IO_PROTOCOL *Io
  )
{
  EFI_BLOCK_IO_MEDIA *Media;
  EFI_STATUS         Status;
  EFI_LBA            Samples[3];
  UINTN              Index;
  UINTN              Pages;
  UINTN              Align;
  UINTN              BlockSize;
  UINT32             MediaId;
  VOID               *First;
  VOID               *Second;

  Media = Io->Media;
  BlockSize = Media->BlockSize;
  Align = Media->IoAlign;
  if (Align < EFI_PAGE_SIZE) {
    Align = EFI_PAGE_SIZE;
  }

  if ((BlockSize == 0) || (BlockSize > 65536U) ||
      (Align > 0x100000U) || ((Align & (Align - 1)) != 0)) {
    Print (L"  Unsupported block size/alignment\n");
    return;
  }

  Pages = EFI_SIZE_TO_PAGES (BlockSize);
  First = AllocateAlignedPages (Pages, Align);
  Second = AllocateAlignedPages (Pages, Align);
  if ((First != NULL) && (Second != NULL)) {
    Samples[0] = 0;
    Samples[1] = Media->LastBlock / 2;
    Samples[2] = Media->LastBlock;
    MediaId = Media->MediaId;
    for (Index = 0; Index < ARRAY_SIZE (Samples); Index++) {
      Status = Io->ReadBlocks (Io, MediaId, Samples[Index], BlockSize, First);
      if (!EFI_ERROR (Status)) {
        Status = Io->ReadBlocks (Io, MediaId, Samples[Index], BlockSize, Second);
      }

      if (EFI_ERROR (Status)) {
        Print (L"  LBA 0x%Lx read failed: %r\n", Samples[Index], Status);
      } else {
        Print (L"  LBA 0x%Lx repeated read: %s\n", Samples[Index],
               CompareMem (First, Second, BlockSize) == 0 ? L"match" : L"MISMATCH");
      }
    }
  } else {
    Print (L"  Could not allocate aligned read buffers\n");
  }

  if (First != NULL) {
    FreeAlignedPages (First, Pages);
  }

  if (Second != NULL) {
    FreeAlignedPages (Second, Pages);
  }
}

STATIC
VOID
PrintStorage (
  VOID
  )
{
  EFI_STATUS               Status;
  EFI_HANDLE               *Handles;
  UINTN                    Count;
  UINTN                    Index;
  EFI_BLOCK_IO_PROTOCOL    *Io;
  EFI_DEVICE_PATH_PROTOCOL *Path;
  CHAR16                   *Text;
  BOOLEAN                  Ufs;

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &Count, &Handles);
  if (EFI_ERROR (Status)) {
    Print (L"No Block I/O handles: %r\n", Status);
    return;
  }

  for (Index = 0; Index < Count; Index++) {
    Status = gBS->HandleProtocol (Handles[Index], &gEfiBlockIoProtocolGuid, (VOID **)&Io);
    if (EFI_ERROR (Status) || (Io->Media == NULL)) {
      continue;
    }
    if (Io->Media->LogicalPartition) {
      continue;
    }

    Path = DevicePathFromHandle (Handles[Index]);
    Text = Path != NULL ? ConvertDevicePathToText (Path, FALSE, FALSE) : NULL;
    Ufs  = (FindUfsNode (Path) != NULL);
    Print (L"Block %u: %s disk media=%u size=%u last=0x%Lx %s\n",
           Index, Ufs ? L"UFS" : L"other",
           Io->Media->MediaPresent, Io->Media->BlockSize, Io->Media->LastBlock,
           Text != NULL ? Text : L"(no device path text)");
    if (Text != NULL) {
      FreePool (Text);
    }

    if (Ufs && Io->Media->MediaPresent) {
      CheckUfsReads (Io);
    }
  }

  FreePool (Handles);
}

STATIC
VOID
PrintBootFile (
  IN EFI_FILE_PROTOCOL *Directory,
  IN CHAR16            *Name
  )
{
  EFI_FILE_PROTOCOL *File;
  EFI_STATUS        Status;
  UINT64            Length;
  UINTN             Count;
  UINT8             Sample[4096];

  Status = Directory->Open (Directory, &File, Name, EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR (Status)) {
    Print (L"  %s: %r\n", Name, Status);
    return;
  }

  Length = 0;
  Status = File->SetPosition (File, MAX_UINT64);
  if (!EFI_ERROR (Status)) {
    Status = File->GetPosition (File, &Length);
  }

  if (!EFI_ERROR (Status)) {
    Status = File->SetPosition (File, 0);
  }

  if (!EFI_ERROR (Status)) {
    Count  = sizeof (Sample);
    Status = File->Read (File, &Count, Sample);
    if (!EFI_ERROR (Status) &&
        (Count == 0 || Count != (Length < sizeof (Sample) ? Length : sizeof (Sample)))) {
      Status = EFI_DEVICE_ERROR;
    }
  }

  if (!EFI_ERROR (Status) && (Length > sizeof (Sample))) {
    Status = File->SetPosition (File, Length - sizeof (Sample));
    if (!EFI_ERROR (Status)) {
      Count  = sizeof (Sample);
      Status = File->Read (File, &Count, Sample);
      if (!EFI_ERROR (Status) && (Count != sizeof (Sample))) {
        Status = EFI_DEVICE_ERROR;
      }
    }
  }

  Print (L"  %s: %Lu bytes, sampled read %r\n", Name, Length, Status);
  File->Close (File);
}

STATIC
VOID
PrintWinPeFiles (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_HANDLE                      *Handles;
  UINTN                           Count;
  UINTN                           Index;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
  EFI_FILE_PROTOCOL               *Root;
  EFI_DEVICE_PATH_PROTOCOL        *Path;
  CHAR16                          *Text;

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &Count, &Handles);
  if (EFI_ERROR (Status)) {
    Print (L"No filesystem volumes available to Mu: %r\n", Status);
    return;
  }

  for (Index = 0; Index < Count; Index++) {
    Status = gBS->HandleProtocol (Handles[Index], &gEfiSimpleFileSystemProtocolGuid, (VOID **)&FileSystem);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = FileSystem->OpenVolume (FileSystem, &Root);
    if (EFI_ERROR (Status)) {
      Print (L"Volume %u could not be opened: %r\n", Index, Status);
      continue;
    }

    Path = DevicePathFromHandle (Handles[Index]);
    Text = Path != NULL ? ConvertDevicePathToText (Path, FALSE, FALSE) : NULL;
    Print (L"Volume %u: %s\n", Index, Text != NULL ? Text : L"(no device path text)");
    if (Text != NULL) {
      FreePool (Text);
    }

    PrintBootFile (Root, L"\\sources\\boot.wim");
    PrintBootFile (Root, L"\\boot\\boot.sdi");
    Root->Close (Root);
  }

  FreePool (Handles);
}

STATIC
VOID
PrintUsb (
  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   *Handles;
  EFI_USB2_HC_PROTOCOL         *Controller;
  EFI_USB_PORT_STATUS          PortStatus;
  EFI_USB_IO_PROTOCOL          *Io;
  EFI_USB_DEVICE_DESCRIPTOR    Device;
  EFI_USB_INTERFACE_DESCRIPTOR Interface;
  UINTN                        Count;
  UINTN                        Index;
  UINT8                        Port;
  UINT8                        Ports;
  UINT8                        Speed;
  UINT8                        Dma64;

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiUsb2HcProtocolGuid, NULL, &Count, &Handles);
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < Count; Index++) {
      Status = gBS->HandleProtocol (Handles[Index], &gEfiUsb2HcProtocolGuid, (VOID **)&Controller);
      if (!EFI_ERROR (Status)) {
        Status = Controller->GetCapability (Controller, &Speed, &Ports, &Dma64);
      }

      if (EFI_ERROR (Status)) {
        Print (L"USB host %u capability query failed: %r\n", Index, Status);
        continue;
      }

      Print (L"USB host %u: %u root ports, max speed code %u, DMA64=%u\n", Index, Ports, Speed, Dma64);
      for (Port = 0; Port < Ports; Port++) {
        Status = Controller->GetRootHubPortStatus (Controller, Port, &PortStatus);
        if (!EFI_ERROR (Status)) {
          Print (L"  port %u: status=0x%04x change=0x%04x\n", Port + 1,
                 PortStatus.PortStatus, PortStatus.PortChangeStatus);
        }
      }
    }

    FreePool (Handles);
  } else {
    Print (L"No USB host controller handles: %r\n", Status);
  }

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiUsbIoProtocolGuid, NULL, &Count, &Handles);
  if (EFI_ERROR (Status)) {
    Print (L"No enumerated USB interfaces: %r\n", Status);
    return;
  }

  for (Index = 0; Index < Count; Index++) {
    Status = gBS->HandleProtocol (Handles[Index], &gEfiUsbIoProtocolGuid, (VOID **)&Io);
    if (EFI_ERROR (Status) || EFI_ERROR (Io->UsbGetDeviceDescriptor (Io, &Device)) ||
        EFI_ERROR (Io->UsbGetInterfaceDescriptor (Io, &Interface))) {
      continue;
    }

    Print (L"USB %04x:%04x interface %u class=%02x subclass=%02x protocol=%02x%s\n",
           Device.IdVendor, Device.IdProduct, Interface.InterfaceNumber,
           Interface.InterfaceClass, Interface.InterfaceSubClass, Interface.InterfaceProtocol,
           ((Interface.InterfaceClass == 3) && (Interface.InterfaceSubClass == 1) &&
            (Interface.InterfaceProtocol == 2)) ? L" (boot mouse)" : L"");
  }

  FreePool (Handles);
}

EFI_STATUS
EFIAPI
NxPlatformProbeMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
  EFI_STATUS                   Status;
  UINTN                        Rows;

  (VOID)ImageHandle;
  (VOID)SystemTable;

  if (gST->ConOut == NULL) {
    return EFI_NOT_READY;
  }

  Rows = SelectProbeTextMode ();
  PrintPartitionPages (Rows);
  Print (L"NX809J UEFI bring-up probe\n");
  Status = gBS->LocateProtocol (&gEfiGraphicsOutputProtocolGuid, NULL, (VOID **)&Gop);
  if (!EFI_ERROR (Status) && (Gop->Mode != NULL) && (Gop->Mode->Info != NULL)) {
    Print (L"GOP: %u x %u, stride %u pixels, pixel format %u\n",
           Gop->Mode->Info->HorizontalResolution, Gop->Mode->Info->VerticalResolution,
           Gop->Mode->Info->PixelsPerScanLine, Gop->Mode->Info->PixelFormat);
  } else {
    Print (L"No GOP mode: %r\n", Status);
  }

  PrintUsb ();
  PrintStorage ();
  PrintWinPeFiles ();
  return EFI_SUCCESS;
}
