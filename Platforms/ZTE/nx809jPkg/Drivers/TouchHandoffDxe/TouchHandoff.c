/** @file
  Prepare the NX809J touch SPI bus for the Windows driver.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Guid/EventGroup.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIClock.h>

#include "TouchHardware.h"

STATIC CONST CHAR8        *mClockNames[] = { NX809J_TOUCH_CLOCK_NAMES };
STATIC EFI_CLOCK_PROTOCOL *mClockProtocol;
STATIC BOOLEAN             mClockVoted[ARRAY_SIZE (mClockNames)];
STATIC BOOLEAN             mHandoffPrepared;
STATIC EFI_EVENT           mReadyToBootEvent;
STATIC EFI_EVENT           mExitBootServicesEvent;

STATIC
VOID
ConfigureTouchPins (
  VOID
  )
{
  UINT32 Gpio;
  UINT32 Value;
  UINTN  Address;

  // MISO, MOSI, CLK, CS0. Preserve output-enable and unrelated TLMM bits.
  for (Gpio = NX809J_SPI_FIRST_GPIO; Gpio <= NX809J_SPI_LAST_GPIO; Gpio++) {
    Address = NX809J_TLMM_GPIO_CFG (Gpio);
    Value   = MmioRead32 (Address);
    Value  &= ~(NX809J_TLMM_PULL_MASK | NX809J_TLMM_MUX_MASK | NX809J_TLMM_DRIVE_MASK);
    Value  |= (NX809J_QUP4_SE0_MUX << NX809J_TLMM_MUX_SHIFT);
    Value  |= (NX809J_DRIVE_6MA << NX809J_TLMM_DRIVE_SHIFT);
    MmioWrite32 (Address, Value);
  }

  MemoryFence ();
}

STATIC
VOID
PrepareTouchHandoff (
  VOID
  )
{
  EFI_STATUS Status;
  UINTN      Id;
  UINTN      Index;
  UINT32     Frequency;

  mHandoffPrepared = FALSE;

  for (Index = 0; Index < ARRAY_SIZE (mClockNames); Index++) {
    if (mClockVoted[Index]) {
      continue;
    }

    Status = mClockProtocol->GetClockID (mClockProtocol, mClockNames[Index], &Id);
    if (!EFI_ERROR (Status)) {
      Status = mClockProtocol->EnableClock (mClockProtocol, Id);
    }

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "NX809J touch: clock vote %a failed: %r\n", mClockNames[Index], Status));
      return;
    }

    // Hold one vote across ExitBootServices. A ReadyToBoot retry only
    // acquires missing votes, since EnableClock is reference counted.
    mClockVoted[Index] = TRUE;
  }

  Status = mClockProtocol->GetClockID (mClockProtocol, NX809J_SE19_CLOCK_NAME, &Id);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "NX809J touch: SE19 source lookup failed: %r\n", Status));
    return;
  }

  Frequency = 0;
  Status = mClockProtocol->SetClockFreqHz (
                             mClockProtocol,
                             Id,
                             NX809J_SE19_SOURCE_HZ,
                             EFI_CLOCK_FREQUENCY_HZ_AT_MOST,
                             &Frequency
                             );
  if (EFI_ERROR (Status) || (Frequency == 0) || (Frequency > NX809J_SE19_SOURCE_HZ)) {
    DEBUG ((DEBUG_ERROR, "NX809J touch: SE19 source rejected: %r, %u Hz\n", Status, Frequency));
    return;
  }

  ConfigureTouchPins ();
  mHandoffPrepared = TRUE;
  DEBUG ((DEBUG_INFO, "NX809J touch: SE19 source %u Hz\n", Frequency));
}

STATIC
VOID
EFIAPI
TouchReadyToBoot (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  (VOID)Event;
  (VOID)Context;
  PrepareTouchHandoff ();
}

STATIC
VOID
EFIAPI
TouchExitBootServices (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  (VOID)Event;
  (VOID)Context;

  // Only refresh the mux here. Vendor clock calls may need Boot Services.
  if (mHandoffPrepared) {
    ConfigureTouchPins ();
  }
}

EFI_STATUS
EFIAPI
TouchHandoffEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;

  (VOID)ImageHandle;
  (VOID)SystemTable;

  Status = gBS->LocateProtocol (&gEfiClockProtocolGuid, NULL, (VOID **)&mClockProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "NX809J touch: Clock protocol unavailable: %r\n", Status));
    return Status;
  }

  PrepareTouchHandoff ();

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  TouchReadyToBoot,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &mReadyToBootEvent
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "NX809J touch: ReadyToBoot event failed: %r\n", Status));
  }

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  TouchExitBootServices,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &mExitBootServicesEvent
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "NX809J touch: ExitBootServices event failed: %r\n", Status));
  }

  return EFI_SUCCESS;
}
