##
#
#  Copyright (c) 2011 - 2022, ARM Limited. All rights reserved.
#  Copyright (c) 2014, Linaro Limited. All rights reserved.
#  Copyright (c) 2015 - 2020, Intel Corporation. All rights reserved.
#  Copyright (c) 2018, Bingxing Wang. All rights reserved.
#  Copyright (c) Microsoft Corporation.
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = nx809j
  PLATFORM_GUID                  = 49301A81-23FB-4EDB-851D-1B4FBE643D04
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/nx809jPkg
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = RELEASE|DEBUG
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = nx809jPkg/nx809j.fdf
  USE_CUSTOM_DISPLAY_DRIVER      = 0
  DEFINE SKIP_KERNEL_ERRATA_PATCHER = TRUE
  DEFINE NX809J_DIRECT_WINPE        = TRUE
  DEFINE PLATFORM_BOOT_LOGO_BMP   = nx809jPkg/Resources/BootLogo.bmp

  #
  # 0 = SM8850-AC
  # 1 = SM8850-5-AC
  # 2 = SM8850-1-AD
  #
  SOC_TYPE                       = 0

!include KaanapaliPkg/KaanapaliPkg.dsc.inc

[PcdsFixedAtBuild]
!if $(NX809J_BOOT_PROBE) == 1
  # Resident Windows AMU compatibility module GUID.
  gSiliciumPkgTokenSpaceGuid.PcdPreBootProbeFile|{ 0x52, 0xA1, 0xC9, 0x1B, 0x24, 0x67, 0x7B, 0x4D, 0x86, 0x48, 0xB7, 0xE7, 0x91, 0xF9, 0xD8, 0x09 }
!endif
  #
  # DDR Memory
  #
  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x80000000

  #
  # UEFI Stack
  #
  gArmPlatformTokenSpaceGuid.PcdCPUCoresStackBase|0xC6E0D000
  gArmPlatformTokenSpaceGuid.PcdCPUCorePrimaryStackSize|0x40000

  #
  # SMBIOS
  #
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemManufacturer|"ZTE"
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemModel|"RedMagic 11 Pro"
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemRetailModel|"nx809j"
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemRetailSku|"NX809J"
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemBoardModel|"NX809J"

  #
  # Simple Frame Buffer
  #
  gSiliciumPkgTokenSpaceGuid.PcdFrameBufferWidth|1216
  gSiliciumPkgTokenSpaceGuid.PcdFrameBufferHeight|2688
  gSiliciumPkgTokenSpaceGuid.PcdFrameBufferColorDepth|32

  #
  # Platform PEI
  #
  gQcomPkgTokenSpaceGuid.PcdPlatformType|"LA"

[LibraryClasses]
  #
  # Memory Libraries
  #
  MemoryMapLib|nx809jPkg/Library/MemoryMapLib/MemoryMapLib.inf

  #
  # QCOM Libraries
  #
  ConfigurationMapLib|nx809jPkg/Library/ConfigurationMapLib/ConfigurationMapLib.inf

[Components]
  #
  # ACPI Tables
  #
  nx809jPkg/Acpi/AcpiTables.inf

  #
  # Keep the recovered QUP4 SE19 clock and SPI pin configuration alive for
  # the direct-GENI Windows touchscreen driver.
  #
  nx809jPkg/Drivers/TouchHandoffDxe/TouchHandoffDxe.inf

  # Built separately for the UEFI shell. It is not part of the boot image.
  nx809jPkg/Applications/NxTouchProbe/NxTouchProbe.inf
  nx809jPkg/Drivers/WindowsAmuFixDxe/WindowsAmuFix.inf
