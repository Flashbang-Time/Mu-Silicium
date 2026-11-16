# NX809J touch firmware handoff

`TouchHandoffDxe` holds one firmware vote on each of the five QUP4 clocks.
It requests an SE19 source at or below 19.2 MHz and sets GPIO48–51 to
`qup4_se0` at 6 mA. ReadyToBoot retries clock setup, while
ExitBootServices only refreshes the pin mux. This keeps vendor clock
protocol calls out of the ExitBootServices callback.

The Windows driver owns GPIO66, GPIO158, and the Synaptics reset/identify
sequence. The ACPI contract and test-signed Windows driver from the first
touch bundle still apply.

## UEFI shell check

Use only on the RedMagic 11 Pro NX809J with the matching Mu image. Copy
`NxTouchProbe.efi` to a filesystem visible to the Mu shell and run it there
before starting WinPE. The probe queries the firmware clock protocol and
reads pin and SE19 registers. It skips the raw register reads if the clock
preflight fails. It issues no SPI transactions and makes no hardware writes.

- Expect the five clock branches on, an SE19 source at or below 19.2 MHz,
  GPIO48–51 mux value 1, and `protocol=1 (SPI) FIFO=enabled`.
- If the probe reports a different protocol or a disabled FIFO, capture the
  full output. The existing Windows transport requires the SPI microcode
  and FIFO interface.
- GPIO66 and GPIO158 show sampled logic levels. They do not establish that
  PMH0101 L4B (1.8 V) and L14B (3.2 V) are powered. The image makes no new
  PMIC writes. A successful probe still needs a WinPE controller identify
  and input test.

All five clock names were found in the recovered Kaanapali ClockDxe. The
image retains the recovered QupDxe and SPIDxe. Their presence does not prove
that they initialize SE19 for SPI on this boot path.
