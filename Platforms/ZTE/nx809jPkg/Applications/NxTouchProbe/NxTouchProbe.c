/** @file
  Read-only NX809J touch handoff diagnostics. Run from the Mu shell.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/EFIClock.h>

#include "../../Drivers/TouchHandoffDxe/TouchHardware.h"

STATIC CONST CHAR8 *mClockNames[] = { NX809J_TOUCH_CLOCK_NAMES };

STATIC
BOOLEAN
PrintClocks (
  IN EFI_CLOCK_PROTOCOL *Clock
  )
{
  EFI_STATUS Status;
  UINTN      Index;
  UINTN      Id;
  BOOLEAN    AllOn;
  BOOLEAN    On;
  UINT32     Frequency;

  AllOn = TRUE;
  for (Index = 0; Index < ARRAY_SIZE (mClockNames); Index++) {
    Status = Clock->GetClockID (Clock, mClockNames[Index], &Id);
    if (EFI_ERROR (Status)) {
      Print (L"%a: lookup failed (%r)\n", mClockNames[Index], Status);
      AllOn = FALSE;
      continue;
    }

    On     = FALSE;
    Status = Clock->IsClockOn (Clock, Id, &On);
    Print (L"%a: %s (%r)\n", mClockNames[Index], On ? L"on" : L"off/unknown", Status);
    if (EFI_ERROR (Status) || !On) {
      AllOn = FALSE;
    }

    if (Index == ARRAY_SIZE (mClockNames) - 1U) {
      Frequency = 0;
      Status    = Clock->GetClockFreqHz (Clock, Id, &Frequency);
      Print (L"SE19 source: %u Hz (%r), expected at most %u Hz\n",
             Frequency, Status, NX809J_SE19_SOURCE_HZ);
    }
  }

  return AllOn;
}

EFI_STATUS
EFIAPI
NxTouchProbeMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_CLOCK_PROTOCOL *Clock;
  EFI_STATUS         Status;
  UINT32             Gpio;
  UINT32             Config;
  UINT32             Revision;
  UINT32             Protocol;
  UINT32             FifoDisable;

  (VOID)ImageHandle;
  (VOID)SystemTable;

  Print (L"NX809J touch handoff probe\n");
  Status = gBS->LocateProtocol (&gEfiClockProtocolGuid, NULL, (VOID **)&Clock);
  if (EFI_ERROR (Status)) {
    Print (L"Qualcomm Clock protocol unavailable: %r\n", Status);
    return Status;
  }

  if (!PrintClocks (Clock)) {
    Print (L"Clock preflight failed. Skipping SE19 register reads.\n");
    return EFI_NOT_READY;
  }

  for (Gpio = NX809J_SPI_FIRST_GPIO; Gpio <= NX809J_SPI_LAST_GPIO; Gpio++) {
    Config = MmioRead32 (NX809J_TLMM_GPIO_CFG (Gpio));
    Print (L"GPIO%u: cfg=0x%08x mux=%u\n", Gpio, Config,
           (Config & NX809J_TLMM_MUX_MASK) >> NX809J_TLMM_MUX_SHIFT);
  }

  Print (L"GPIO%u reset input=%u, GPIO%u IRQ input=%u\n",
         NX809J_TOUCH_RESET_GPIO,
         MmioRead32 (NX809J_TLMM_GPIO_IO (NX809J_TOUCH_RESET_GPIO)) & 1U,
         NX809J_TOUCH_IRQ_GPIO,
         MmioRead32 (NX809J_TLMM_GPIO_IO (NX809J_TOUCH_IRQ_GPIO)) & 1U);

  Print (L"Reading SE19 registers at 0x%08x\n", NX809J_SE19_BASE);
  Revision    = MmioRead32 (NX809J_SE19_BASE + NX809J_GENI_FW_REVISION);
  Protocol    = NX809J_GENI_PROTOCOL (Revision);
  FifoDisable = MmioRead32 (NX809J_SE19_BASE + NX809J_GENI_IF_DISABLE);
  Print (L"SE19 revision=0x%08x protocol=%u (%s) FIFO=%s (IF_DISABLE=0x%08x)\n",
         Revision, Protocol, Protocol == NX809J_GENI_PROTOCOL_SPI ? L"SPI" : L"not SPI",
         (FifoDisable & 1U) == 0 ? L"enabled" : L"disabled", FifoDisable);
  Print (L"SE19 status=0x%08x HW_PARAM_0=0x%08x HW_PARAM_1=0x%08x\n",
         MmioRead32 (NX809J_SE19_BASE + NX809J_GENI_STATUS),
         MmioRead32 (NX809J_SE19_BASE + NX809J_GENI_HW_PARAM_0),
         MmioRead32 (NX809J_SE19_BASE + NX809J_GENI_HW_PARAM_1));

  if ((Protocol != NX809J_GENI_PROTOCOL_SPI) || ((FifoDisable & 1U) != 0)) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}
