# NX809J ACPI platform

`generate.py` derives and asserts the hardware resources in these tables from
the preserved NX809J `kaanapali-nx809j.dtb`.

Implemented tables:

- DSDT: eight `ACPI0007` processors, UFS0, the primary USB controller, the
  TLMM GPIO controller and the NX809J Synaptics touch device.
- MADT: actual CPU MPIDRs, GICv3 distributor, explicit per-CPU
  redistributor bases with the observed 256 KiB stride, and ITS.
- GTDT: the four architectural timer interrupts and the enabled
  DT MMIO timer frame at `0x17811000` (physical GSI 40, virtual GSI 38).
- FADT: mobile, hardware-reduced ACPI and PSCI over SMC.
- PPTT: the recovered six-plus-two CPU topology, without guessed cache data.
- IORT: the MMU-500 instance, all 112 context interrupts, and UFS/USB stream
  mappings (`0x60` and `0x40`) from the NX809J DT.

The UFS resource window is `0x01D84000..0x01DA3FFF`: the controller's DT
resource plus its contiguous ICE resource. This matches the `0x1C000` window
used by the closest production Windows donor. `QCOM24A5` is the established
Qualcomm Windows UFS ID; `DEV0` exposes the non-removable logical unit at
address 8. `_CLS` also exposes the standard UFS class tuple `01/09/01`, which
the verified Windows 11 ARM64 inbox `storufs.inf` accepts as a generic fallback.

The USB controller is `QCOM0C8B` at `0x0A600000`, the same ID and base used by
the Kaanapali-class Purwa donor. It also carries `_CID "PNP0D10"` so an inbox
xHCI driver has a standards-based match, and exposes the recovered core GSIV.
The existing patched `UsbConfigDxe` remains responsible for leaving the IOMMU
attachment intact across ExitBootServices.

`GIO0` exposes the exact NX809J TLMM window (`0x0F100000+0x300000`), summary
GSIV `0xF0` and DT-derived count of 218 GPIOs. Its `QCOM0C0C` identity and
standard Qualcomm register-layout package match the production Kailua ACPI
table with the same resources; `_CID "QCOMFFEB"` selects the verified Windows
11 ARM64 inbox `qcgpio_i.sys`. Unrecoverable per-pin PDC wake mappings are not
invented, so ordinary GPIO I/O/interrupts are the target, not connected standby.

`TPAD` uses the private ID `ZTE0001` consumed by the NX809J touch driver. It
exposes GENI SE19 (`0x01A80000+0x4000`) and its QUPv3 wrapper 4 registers
(`0x01AC0000+0x2000`), plus GPIO158 as a level-low interrupt and GPIO66 as the
reset output. `_DEP` forces the Qualcomm GPIO controller to start first. The
SPI pin mux and QUP4 clock vote are owned by `TouchHandoffDxe`; there is no
overlapping TLMM memory resource on `TPAD`.

Known limits:

- PEP0/UCS0 power dependencies are intentionally omitted: their SM8850 power
  contracts have not been recovered. Firmware must leave the UFS and USB
  clocks, PHYs and power domains usable for Windows hand-off.
- `QCOM24A5` and `QCOM0C8B` are strong donor matches, not proof that a specific
  Windows build contains compatible drivers.
- USB Type-C role, VBUS/orientation policy and the PHY wake interrupts still
  need live validation. The first target is fixed host mode and a USB mouse.
- Touch is implemented but remains hardware-untested. GPU, modem, battery and
  sleep support are outside this table set.

Run `python3 generate.py` to regenerate everything. The generator compiles the
DSDT, disassembles every data table with `iasl`, checks table checksums, and
fails if the recovered DT resources differ from the asserted port.

## PE polling profile

Run `python Platforms/ZTE/nx809jPkg/Acpi/generate.py --pe` for the current PE touch driver. It removes the GPIO-provider dependency and exposes `ACPI\ZTE0002` and the two assigned pin pages. The no-option profile exposes `ACPI\ZTE0001` and requires the Qualcomm GPIO provider. The older `--pe19042` spelling remains supported. Generated AML is built locally, not shipped as a prebuilt firmware asset.
