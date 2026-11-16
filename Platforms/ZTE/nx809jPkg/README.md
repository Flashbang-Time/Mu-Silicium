# RedMagic 11 Pro (NX809J)

This fork contains the NX809J / qwjujube bring-up. Windows 11 ARM64 PE has booted to a command prompt on the physical phone. This is not a finished Windows phone port.

The working early-boot fix disables one optional Windows AMU capability probe. It is a guarded, in-memory patch for the tested ADK kernel only (timestamp CC2E25DD, image size 01450000, entry B05000). Other kernels are refused rather than patched at guessed offsets. The resident module must remain loaded through ExitBootServices. There is no diagnostic power-off hook in this build.

Touch is experimental and its latest observed failure is Code 52 / C0000428. The clean PE BCD lacked TESTSIGNING; a corrected BCD has been prepared, but successful touch after that change is not yet confirmed. USB has device-start/reinstallation errors and is unverified. UEFI storage enumeration does not prove Windows UFS operation. GPU acceleration, networking, modem, audio, suspend and power management remain unverified.

## Build

Follow the upstream Building.md prerequisites, initialize pinned submodules including nested dependencies, and complete the upstream tool setup/update steps. Then prepare local assets and dependency patches:

```sh
python Tools/nx809j/prepare.py --from-port /path/to/existing-local-nx809j-port
python Platforms/ZTE/nx809jPkg/Acpi/generate.py --pe
python build_uefi.py -d nx809j
```

The repository includes an asset hash manifest, not stock firmware dumps. Most firmware files can be reused byte-for-byte from the upstream Binaries submodule; the remaining original assets and DTBs must be supplied locally. `--from-port` expects the recovered local port layout. Preparation changes local dependency source files, recorded as reviewed patches in Tools/nx809j. It does not flash anything.

The AMU compatibility module is enabled by default. `NX809J_AMU_FIX=0` disables it for development; the known Windows boot then may fail. Keep the tested AMU-fix image and PE media as recovery references. A rebuild on newer upstream dependencies still requires on-phone verification.

## Recovery and PE

Use the owner's established recovery and slot-B flashing procedure. On this phone, EDL entered directly using the power-button combination worked after automatic post-crash EDL repeatedly lost the programmer connection. That observation does not prove the underlying reset cause. No script in this port flashes the phone automatically.

Development drivers are test-signed. The active PE loader must enable TESTSIGNING and trust the matching certificate; adding an INF alone is not successful driver startup. Keep a clean PE backup before changing the boot configuration.

Touch driver source and current diagnostic checkpoints: https://github.com/Flashbang-Time/nx809j-touch-driver

The platform was reconstructed using the original image's memory/configuration maps, recovered DT information and the upstream OnePlus infiniti platform as a starting point. That is not evidence that the boards are interchangeable.

The documented `--pe` ACPI profile exposes `ACPI\ZTE0002` with its two dedicated GPIO pages and does not require the Qualcomm GPIO provider. The generator without that option emits the `ACPI\ZTE0001` provider-based profile. `--pe19042` remains an alias for compatibility with older bring-up scripts.
