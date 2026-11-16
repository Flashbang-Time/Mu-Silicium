// SPDX-License-Identifier: BSD-2-Clause-Patent
#include <Uefi.h>
#include <Guid/Acpi.h>
#include <Protocol/MemoryAttribute.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

// Pinned to Microsoft's ARM64 ADK kernel currently present in both boot.wim
// and test.wim. Refuse other builds rather than patch an assumed instruction.
#define KERNEL_STAMP 0xCC2E25DD
#define KERNEL_SIZE  0x01450000
#define KERNEL_ENTRY 0x00B05000
#define CHECKPOINT_RVA 0x00BA55E8
#define MAP_CAPACITY (1024 * 1024)

STATIC CONST UINT8 mExpectedEntry[] = {
  0x7F,0x23,0x03,0xD5,0xF3,0x0F,0x1F,0xF8,0xFD,0x7B,0xBF,0xA9,0xFD,0x03,0x00,0x91,0x4E,0xEE,0xE2,0x97
};

STATIC EFI_EXIT_BOOT_SERVICES mOriginalExit;
STATIC EFI_MEMORY_ATTRIBUTE_PROTOCOL *mAttributes;
STATIC EFI_MEMORY_DESCRIPTOR *mMap;
STATIC UINTN mMapSize;
STATIC UINTN mDescriptorSize;
STATIC BOOLEAN mArmed;

STATIC BOOLEAN LoaderType(UINT32 Type) {
  return Type == EfiLoaderCode || Type == EfiLoaderData;
}

STATIC BOOLEAN LoaderRange(UINTN Start, UINTN Size) {
  UINTN Cursor, End, Offset, Next;
  EFI_MEMORY_DESCRIPTOR *D;
  if (Size == 0 || Start > MAX_UINTN - Size) { return FALSE; }
  Cursor = Start;
  End = Start + Size;
  while (Cursor < End) {
    Next = Cursor;
    for (Offset = 0; Offset < mMapSize; Offset += mDescriptorSize) {
      D = (VOID *)((UINT8 *)mMap + Offset);
      if (LoaderType(D->Type) && D->PhysicalStart <= Cursor &&
          D->NumberOfPages <= (MAX_UINT64 - D->PhysicalStart) / EFI_PAGE_SIZE &&
          D->PhysicalStart + EFI_PAGES_TO_SIZE(D->NumberOfPages) > Cursor) {
        Next = D->PhysicalStart + EFI_PAGES_TO_SIZE(D->NumberOfPages);
        break;
      }
    }
    if (Next == Cursor) { return FALSE; }
    Cursor = Next;
  }
  return TRUE;
}

STATIC BOOLEAN IsPinnedKernel(UINTN Base) {
  UINT8 *B = (VOID *)Base;
  UINT32 Pe, Export, Name;
  if (ReadUnaligned16((VOID *)B) != 0x5A4D) { return FALSE; }
  Pe = ReadUnaligned32((VOID *)(B + 0x3C));
  if (Pe < 0x40 || Pe > 0x800 ||
      ReadUnaligned32((VOID *)(B + Pe)) != 0x4550 ||
      ReadUnaligned16((VOID *)(B + Pe + 4)) != 0xAA64 ||
      ReadUnaligned32((VOID *)(B + Pe + 8)) != KERNEL_STAMP ||
      ReadUnaligned16((VOID *)(B + Pe + 24)) != 0x20B ||
      ReadUnaligned32((VOID *)(B + Pe + 24 + 56)) != KERNEL_SIZE ||
      ReadUnaligned32((VOID *)(B + Pe + 24 + 16)) != KERNEL_ENTRY ||
      !LoaderRange(Base, KERNEL_SIZE)) { return FALSE; }
  Export = ReadUnaligned32((VOID *)(B + Pe + 24 + 112));
  if (Export > KERNEL_SIZE - 40) { return FALSE; }
  Name = ReadUnaligned32((VOID *)(B + Export + 12));
  if (Name > KERNEL_SIZE - sizeof("ntoskrnl.exe") ||
      CompareMem(B + Name, "ntoskrnl.exe", sizeof("ntoskrnl.exe")) != 0) { return FALSE; }
  return CompareMem(B + CHECKPOINT_RVA, mExpectedEntry, sizeof(mExpectedEntry)) == 0;
}

STATIC EFI_STATUS DisableOptionalAmu(UINTN Base) {
  EFI_STATUS Status;
  UINT64 Attributes = 0;
  UINTN Address = Base + 0x46BDF8;
  UINTN Page = Address & ~((UINTN)EFI_PAGE_SIZE - 1);
  if (ReadUnaligned32((VOID *)Address) != 0xD53BD2A8) { return EFI_COMPROMISED_DATA; }
  if (mAttributes != NULL) {
    Status = mAttributes->GetMemoryAttributes(mAttributes, Page, EFI_PAGE_SIZE, &Attributes);
    if (EFI_ERROR(Status)) { return Status; }
    if ((Attributes & EFI_MEMORY_RO) != 0) {
      Status = mAttributes->ClearMemoryAttributes(mAttributes, Page, EFI_PAGE_SIZE, EFI_MEMORY_RO);
      if (EFI_ERROR(Status)) { return Status; }
    }
  }
  // mov x8, #0: leave the optional AMU capability disabled.
  WriteUnaligned32((VOID *)Address, 0xD2800008);
  WriteBackInvalidateDataCacheRange((VOID *)Address, 4);
  InvalidateInstructionCacheRange((VOID *)Address, 4);
  if (mAttributes != NULL && (Attributes & EFI_MEMORY_RO) != 0) {
    Status = mAttributes->SetMemoryAttributes(mAttributes, Page, EFI_PAGE_SIZE, EFI_MEMORY_RO);
    if (EFI_ERROR(Status)) { return Status; }
  }
  Print(L"AMU WORKAROUND APPLIED: optional AMU capability disabled.\n");
  return EFI_SUCCESS;
}

STATIC EFI_STATUS ArmKernelEntry(VOID) {
  EFI_STATUS Status;
  EFI_MEMORY_DESCRIPTOR *D;
  UINTN Offset, Address, Limit, Found, Key;
  UINT32 Version;
  mMapSize = MAP_CAPACITY;
  Status = gBS->GetMemoryMap(&mMapSize, mMap, &Key, &mDescriptorSize, &Version);
  if (EFI_ERROR(Status)) { return Status; }
  Found = 0;
  for (Offset = 0; Offset < mMapSize; Offset += mDescriptorSize) {
    D = (VOID *)((UINT8 *)mMap + Offset);
    if (!LoaderType(D->Type) || D->NumberOfPages > MAX_UINTN / EFI_PAGE_SIZE ||
        D->PhysicalStart > MAX_UINTN - EFI_PAGES_TO_SIZE(D->NumberOfPages)) { continue; }
    Limit = D->PhysicalStart + EFI_PAGES_TO_SIZE(D->NumberOfPages);
    for (Address = D->PhysicalStart; Address < Limit; Address += EFI_PAGE_SIZE) {
      if (IsPinnedKernel(Address)) {
        if (Found != 0) { return EFI_COMPROMISED_DATA; }
        Found = Address;
      }
    }
  }
  if (Found == 0) { return EFI_NOT_FOUND; }
  Status = DisableOptionalAmu(Found);
  if (EFI_ERROR(Status)) { return Status; }
  Print(L"NX809J AMU WORKAROUND ACTIVE: continuing Windows boot.\n");
  gBS->Stall(5000000);
  return EFI_SUCCESS;
}

STATIC EFI_STATUS EFIAPI CheckKernelEntry(EFI_HANDLE ImageHandle, UINTN MapKey) {
  EFI_STATUS Status;
  if (!mArmed) {
    Status = ArmKernelEntry();
    if (EFI_ERROR(Status)) {
      Print(L"AMU WORKAROUND FAILED: %r. Take a photo.\n", Status);
      CpuDeadLoop();
    }
    mArmed = TRUE;
    // Protection changes and rendering may change MapKey. Ask the OS loader
    // to fetch its final map again; all subsequent attempts call the real EBS.
    return EFI_INVALID_PARAMETER;
  }
  return mOriginalExit(ImageHandle, MapKey);
}

EFI_STATUS EFIAPI NxWindowsAmuFixMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS Status;
  UINT32 OldCrc, NewCrc;
  EFI_TPL OldTpl;
  mMap = AllocatePool(MAP_CAPACITY);
  if (mMap == NULL) { return EFI_OUT_OF_RESOURCES; }
  gBS->LocateProtocol(&gEfiMemoryAttributeProtocolGuid, NULL, (VOID **)&mAttributes);
  mOriginalExit = gBS->ExitBootServices;
  OldTpl = gBS->RaiseTPL(TPL_HIGH_LEVEL);
  OldCrc = gBS->Hdr.CRC32;
  gBS->ExitBootServices = CheckKernelEntry;
  gBS->Hdr.CRC32 = 0;
  Status = gBS->CalculateCrc32(gBS, gBS->Hdr.HeaderSize, &NewCrc);
  if (EFI_ERROR(Status)) {
    gBS->ExitBootServices = mOriginalExit;
    gBS->Hdr.CRC32 = OldCrc;
  } else {
    gBS->Hdr.CRC32 = NewCrc;
  }
  gBS->RestoreTPL(OldTpl);
  return Status;
}
