"""Generate the NX809J ACPI platform from the recovered OS DTB."""

from pathlib import Path
import struct
import subprocess
import sys

HERE = Path(__file__).resolve().parent
DTB = HERE.parent / "FdtBlob/kaanapali-nx809j.dtb"
PE_POLLING = sys.argv[1:] in (["--pe"], ["--pe19042"])
if sys.argv[1:] and not PE_POLLING:
    raise SystemExit("usage: generate.py [--pe | --pe19042]")


def prop(node, key):
    return [
        int(value, 16)
        for value in subprocess.check_output(
            ["fdtget", "-t", "x", str(DTB), node, key], text=True
        ).split()
    ]


def string(node, key):
    return subprocess.check_output(
        ["fdtget", str(DTB), node, key], text=True
    ).strip()


def cells64(cells):
    assert len(cells) == 2
    return (cells[0] << 32) | cells[1]


def first_reg(node):
    values = prop(node, "reg")
    assert len(values) >= 4
    return cells64(values[:2]), cells64(values[2:4])


def gsiv(interrupt_type, number):
    assert interrupt_type in (0, 1)
    return number + (32 if interrupt_type == 0 else 16)


def put(buffer, offset, fmt, *values):
    struct.pack_into("<" + fmt, buffer, offset, *values)


def header(signature, revision, body):
    table = bytearray(
        struct.pack(
            "<4sIBB6s8sI4sI",
            signature.encode(),
            36 + len(body),
            revision,
            0,
            b"ZTE   ",
            b"NX809J  ",
            2,
            b"PSIL",
            2,
        )
        + body
    )
    table[9] = (-sum(table)) & 0xFF
    (HERE / f"{signature}.aml").write_bytes(table)
    assert sum(table) % 256 == 0
    return table


def iort_named_component(object_name, input_base, stream_id, output_reference):
    """Build a revision-0 Qualcomm-style fixed-name named component node."""
    encoded_name = object_name.encode("ascii") + b"\0"
    assert len(encoded_name) <= 32
    name_field = encoded_name.ljust(32, b"\0")
    id_reference = 29 + len(name_field)
    node_length = id_reference + 20
    node = bytearray(
        struct.pack("<BHBIII", 1, node_length, 0, 0, 1, id_reference)
    )
    # Flags, CacheCoherent, AllocationHints, Reserved, MemoryAccessFlags,
    # AddressSizeLimit. Both recovered devices are marked dma-coherent.
    node += struct.pack("<IIBHBB", 0, 1, 0, 0, 1, 40)
    node += name_field
    node += struct.pack(
        "<IIIII", input_base, 0, stream_id, output_reference, 0
    )
    assert len(node) == node_length
    return node


# CPU and interrupt-controller topology.
cpus = subprocess.check_output(
    ["fdtget", "-l", str(DTB), "/cpus"], text=True
).split()
mpidrs = [
    cells64(prop(f"/cpus/{name}", "reg"))
    for name in cpus
    if name.startswith("cpu@")
]
assert len(mpidrs) == 8 and len(set(mpidrs)) == 8
gic_node = "/soc/interrupt-controller@17000000"
gic_regs = prop(gic_node, "reg")
gicd = cells64(gic_regs[0:2])
gicr = cells64(gic_regs[4:6])
gicr_size = cells64(gic_regs[6:8])
stride = cells64(prop(gic_node, "redistributor-stride"))
assert stride * len(mpidrs) <= gicr_size
pmu = prop("/pmu", "interrupts")
vgic = prop(gic_node, "interrupts")
assert pmu[0] == vgic[0] == 1

# MADT ACPI 6.0: GICC entries with explicit per-CPU GICR bases, GICD and ITS.
# The on-device probe confirmed 256 KiB spacing with GICR_TYPER.VLPIS clear.
# A range-only descriptor would require the OS to infer the stride.
assert (gicr, gicr_size, stride) == (0x17080000, 0x200000, 0x40000)
assert mpidrs == [0, 0x100, 0x200, 0x300, 0x400, 0x500, 0x10000, 0x10100]
body = bytearray(8)
for uid, mpidr in enumerate(mpidrs):
    entry = bytearray(80)
    put(entry, 0, "BBHIII", 11, 80, 0, uid, uid, 1)
    put(entry, 20, "I", gsiv(pmu[0], pmu[1]))
    put(entry, 56, "I", gsiv(vgic[0], vgic[1]))
    put(entry, 60, "Q", gicr + uid * stride)
    put(entry, 68, "Q", mpidr)
    body += entry
entry = bytearray(24)
put(entry, 0, "BBHIQI", 12, 24, 0, 0, gicd, 0)
entry[20] = 3
body += entry
its_base, _ = first_reg(f"{gic_node}/msi-controller@17040000")
entry = bytearray(20)
put(entry, 0, "BBHIQI", 15, 20, 0, 0, its_base, 0)
body += entry
header("APIC", 5, body)

# FADT ACPI 6.0, mobile profile, hardware-reduced, PSCI using SMC.
assert string("/psci", "method") == "smc"
body = bytearray(276 - 36)
body[45 - 36] = 2
put(body, 112 - 36, "I", 1 << 20)
put(body, 129 - 36, "H", 1)
body[131 - 36] = 3
header("FACP", 6, body)

# Architectural timers and the enabled NX809J non-secure MMIO frame.
timers = prop("/timer", "interrupts")
assert len(timers) == 12
body = bytearray(96 - 36)
put(body, 0, "Q", 0xFFFFFFFFFFFFFFFF)
put(body, 80 - 36, "Q", 0xFFFFFFFFFFFFFFFF)
for index, offset in enumerate((48, 56, 64, 72)):
    irq_type, number, flags = timers[index * 3 : index * 3 + 3]
    assert irq_type == 1 and flags in (4, 8)
    put(body, offset - 36, "II", gsiv(irq_type, number), 2 if flags == 8 else 0)
# The recovered timer block provides one enabled frame with physical and
# virtual interrupts. Do not advertise the six disabled DT frames.
timer_block = "/soc/timer@17810000"
timer_frame = f"{timer_block}/frame@17811000"
assert "status" not in subprocess.check_output(
    ["fdtget", "-p", str(DTB), timer_frame], text=True
).split()
timer_base, timer_size = first_reg(timer_block)
frame_number, = prop(timer_frame, "frame-number")
frame_regs = prop(timer_frame, "reg")
assert len(frame_regs) == 6
frame_base, frame_size = cells64(frame_regs[:2]), frame_regs[2]
frame_el0, frame_el0_size = cells64(frame_regs[3:5]), frame_regs[5]
phys_type, phys_irq, phys_flags, virt_type, virt_irq, virt_flags = prop(
    timer_frame, "interrupts"
)
assert timer_size == frame_size == frame_el0_size == 0x1000
assert frame_number == 0 and phys_type == virt_type == 0
assert phys_flags == virt_flags == 4  # level-triggered, active high
put(body, 88 - 36, "II", 1, 96)
body += struct.pack("<BHBQII", 0, 60, 0, timer_base, 1, 20)
body += struct.pack(
    "<B3xQQIIIII",
    frame_number, frame_base, frame_el0,
    gsiv(phys_type, phys_irq), 0, gsiv(virt_type, virt_irq), 0, 0,
)
header("GTDT", 2, body)

# Processor hierarchy without guessed cache geometry.
clusters = sorted(set(mpidr >> 16 for mpidr in mpidrs))
body = bytearray()


def pptt_node(flags, parent, uid=0):
    return struct.pack("<BBHIIII", 0, 20, 0, flags, parent, uid, 0)


body += pptt_node(1, 0)
for _ in clusters:
    body += pptt_node(0, 36)
for uid, mpidr in enumerate(mpidrs):
    body += pptt_node(10, 36 + 20 * (1 + clusters.index(mpidr >> 16)), uid)
header("PPTT", 2, body)

# Storage, USB and IOMMU resources.
ufs_node = "/soc/ufs@1d84000"
ice_node = "/soc/crypto@1d88000"
usb_node = "/soc/usb@a600000"
smmu_node = "/soc/iommu@15000000"
tlmm_node = "/soc/pinctrl@f100000"
qup4_node = "/soc/geniqup@1ac0000"
se19_node = f"{qup4_node}/spi@1a80000"

ufs_base, ufs_length = first_reg(ufs_node)
ice_base, ice_length = first_reg(ice_node)
usb_base, usb_length = first_reg(usb_node)
smmu_base, smmu_span = first_reg(smmu_node)
tlmm_base, tlmm_length = first_reg(tlmm_node)
qup4_base, qup4_length = first_reg(qup4_node)
se19_base, se19_length = first_reg(se19_node)
assert (ufs_base, ufs_length) == (0x01D84000, 0x3000)
assert (ice_base, ice_length) == (0x01D88000, 0x18000)
assert (usb_base, usb_length) == (0x0A600000, 0xFC100)
assert (smmu_base, smmu_span) == (0x15000000, 0x100000)
assert (tlmm_base, tlmm_length) == (0x0F100000, 0x300000)
assert (qup4_base, qup4_length) == (0x01AC0000, 0x2000)
assert (se19_base, se19_length) == (0x01A80000, 0x4000)
ufs_windows_length = ice_base + ice_length - ufs_base
assert ufs_windows_length == 0x1C000

ufs_irq = prop(ufs_node, "interrupts")
assert len(ufs_irq) == 3 and ufs_irq[2] == 4
ufs_gsiv = gsiv(ufs_irq[0], ufs_irq[1])
ufs_iommu = prop(ufs_node, "iommus")
usb_iommu = prop(usb_node, "iommus")
assert ufs_iommu[1:] == [0x60, 0]
assert usb_iommu[1:] == [0x40, 0]
assert "dma-coherent" in subprocess.check_output(
    ["fdtget", "-p", str(DTB), ufs_node], text=True
).split()
assert "dma-coherent" in subprocess.check_output(
    ["fdtget", "-p", str(DTB), usb_node], text=True
).split()

usb_interrupts = prop(usb_node, "interrupts-extended")
gic_phandle = prop(gic_node, "phandle")[0]
assert usb_interrupts[0] == gic_phandle
usb_irq_type, usb_irq_number, usb_irq_flags = usb_interrupts[1:4]
assert usb_irq_flags == 4
usb_gsiv = gsiv(usb_irq_type, usb_irq_number)

tlmm_interrupt = prop(tlmm_node, "interrupts")
assert len(tlmm_interrupt) == 3 and tlmm_interrupt[2] == 4
tlmm_gsiv = gsiv(tlmm_interrupt[0], tlmm_interrupt[1])
gpio_ranges = prop(tlmm_node, "gpio-ranges")
assert gpio_ranges[1:3] == [0, 0]
gpio_count = gpio_ranges[3]
assert gpio_count == 0xDA

# The NX809J board overlay's synaptics,tcm-spi node supplies these GPIOs.
# They do not exist in the generic Kaanapali base DTB used above, so retain
# the recovered board values explicitly beside the bounds assertions.
touch_reset_gpio = 66
touch_irq_gpio = 158
volume_up_gpio = 101
assert 0 <= touch_reset_gpio < gpio_count
assert 0 <= touch_irq_gpio < gpio_count
assert 0 <= volume_up_gpio < gpio_count

# IORT revision 0 matches the Qualcomm Windows donor tables. The first DT
# interrupt is the global SMMU interrupt; the rest are context interrupts.
smmu_interrupt_cells = prop(smmu_node, "interrupts")
assert len(smmu_interrupt_cells) % 3 == 0
smmu_interrupts = [
    smmu_interrupt_cells[index : index + 3]
    for index in range(0, len(smmu_interrupt_cells), 3)
]
assert all(irq_type == 0 and flags == 4 for irq_type, _, flags in smmu_interrupts)
smmu_global_gsiv = gsiv(*smmu_interrupts[0][:2])
smmu_context_gsivs = [gsiv(*interrupt[:2]) for interrupt in smmu_interrupts[1:]]
assert len(smmu_context_gsivs) == 112

smmu_length = 76 + 8 * len(smmu_context_gsivs)
smmu = bytearray(struct.pack("<BHBIII", 3, smmu_length, 0, 0, 0, 0))
smmu += struct.pack(
    "<QQIIIIIII",
    smmu_base,
    smmu_span,
    3,
    0,
    60,
    len(smmu_context_gsivs),
    76,
    0,
    0,
)
smmu += struct.pack("<IIII", smmu_global_gsiv, 0, 0, 0)
# Production Qualcomm IORTs mark context IRQs edge-triggered even though Linux
# DTs describe the MMU-500 lines as level-high; retain the Windows donor form.
for context_gsiv in smmu_context_gsivs:
    smmu += struct.pack("<II", context_gsiv, 1)
assert len(smmu) == smmu_length

iort_node_offset = 48
iort_body = bytearray(struct.pack("<III", 3, iort_node_offset, 0))
iort_body += smmu
iort_body += iort_named_component(
    r"\_SB.UFS0", 0x81030000, ufs_iommu[1], iort_node_offset
)
iort_body += iort_named_component(
    r"\_SB.URS0", 0x80030000, usb_iommu[1], iort_node_offset
)
header("IORT", 0, iort_body)

# Windows-visible namespace. PEP/UCS dependencies are deliberately omitted
# until their SM8850 contracts are implemented.
asl = f'''DefinitionBlock ("", "DSDT", 2, "ZTE   ", "NX809J  ", 2)
{{
    Scope (\\_SB)
    {{
'''
for uid in range(len(mpidrs)):
    asl += (
        f"        Device (CPU{uid}) {{ Name (_HID, \"ACPI0007\") "
        f"Name (_UID, {uid}) Name (_STA, 0x0F) }}\n"
    )
asl += f'''
        Device (UFS0)
        {{
            Name (_HID, "QCOM24A5")
            Name (_CLS, Package (0x03) {{ 0x01, 0x09, 0x01 }})
            Name (_UID, Zero)
            Name (_CCA, One)
            Name (_STA, 0x0F)
            Name (_CRS, ResourceTemplate ()
            {{
                Memory32Fixed (ReadWrite, 0x{ufs_base:08X}, 0x{ufs_windows_length:08X})
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) {{ 0x{ufs_gsiv:X} }}
            }})

            Device (DEV0)
            {{
                Name (_ADR, 0x08)
                Method (_RMV, 0, NotSerialized) {{ Return (Zero) }}
            }}
        }}

        Device (URS0)
        {{
            Name (_HID, "QCOM0C8B")
            Name (_CID, "PNP0D10")
            Name (_UID, Zero)
            Name (_CCA, One)
            Name (_STA, 0x0F)
            Name (_CRS, ResourceTemplate ()
            {{
                Memory32Fixed (ReadWrite, 0x{usb_base:08X}, 0x{usb_length:08X})
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) {{ 0x{usb_gsiv:X} }}
            }})

            Device (RHUB)
            {{
                Name (_ADR, Zero)
                // The USB 2 and SuperSpeed ports share one Type-C receptacle.
                Name (CPLD, Package (One)
                {{
                    Buffer (0x10)
                    {{
                        0x81, 0, 0, 0, 0, 0, 0, 0, 0x31, 0x1C, 0, 0, 0, 0, 0, 0
                    }}
                }})
                Device (HS01)
                {{
                    Name (_ADR, One)
                    Name (_UPC, Package (0x04) {{ 0xFF, 0x09, Zero, Zero }})
                    Method (_PLD, 0, NotSerialized) {{ Return (CPLD) }}
                }}
                Device (SS01)
                {{
                    Name (_ADR, 0x02)
                    Name (_UPC, Package (0x04) {{ 0xFF, 0x09, Zero, Zero }})
                    Method (_PLD, 0, NotSerialized) {{ Return (CPLD) }}
                }}
            }}
        }}

        Device (GIO0)
        {{
            Name (_HID, "QCOM0C0C")
            Name (_CID, "QCOMFFEB")
            Name (_UID, Zero)
            Name (_STA, 0x0F)
            Name (_CRS, ResourceTemplate ()
            {{
                Memory32Fixed (ReadWrite, 0x{tlmm_base:08X}, 0x{tlmm_length:08X})
                Interrupt (ResourceConsumer, Level, ActiveHigh, Shared) {{ 0x{tlmm_gsiv:X} }}
                Interrupt (ResourceConsumer, Level, ActiveHigh, Shared) {{ 0x{tlmm_gsiv:X} }}
                Interrupt (ResourceConsumer, Level, ActiveHigh, Shared) {{ 0x{tlmm_gsiv:X} }}
                Interrupt (ResourceConsumer, Level, ActiveHigh, Shared) {{ 0x{tlmm_gsiv:X} }}
            }})

            Name (GPIV, Zero)
            Name (GPIC, 0x{gpio_count:X})
            Name (GPIW, 0x1000)
            Name (GPIB, Package (0x26)
            {{
                Zero, 0x04, 0x08, 0x0C, 0x03, Zero, 0x3C, 0x02,
                0x01C0, 0x06, 0x0200, 0x09, 0x0400, 0x0A, 0x0800, 0x0B,
                0x1000, 0x0C, 0x2000, 0x0D, One, Zero, 0x02, One,
                One, Zero, 0x02, One, 0x0C, 0x02, 0x10, 0x04,
                0xE0, 0x05, 0x0100, 0x08, One, Zero
            }})

            // Qualcomm's runtime TLMM layout interface. The PDC wake
            // mapping is board-specific and is deliberately not exposed.
            Method (_DSM, 4, Serialized)
            {{
                If (LEqual (Arg0, ToUUID ("98b9b2a4-1663-4a5f-82f2-c6c99a394726")))
                {{
                    If (LEqual (Arg1, Zero))
                    {{
                        Switch (ToInteger (Arg2))
                        {{
                            Case (Zero) {{ Return (Buffer (One) {{ 0x1F }}) }}
                            Case (One) {{ Return (GPIV) }}
                            Case (0x02) {{ Return (GPIC) }}
                            Case (0x03) {{ Return (GPIW) }}
                            Case (0x04) {{ Return (GPIB) }}
                        }}
                    }}
                }}
                Return (Buffer (One) {{ Zero }})
            }}
        }}

        Device (BTNS)
        {{
            Name (_HID, "ACPI0011")
            Name (_UID, Zero)
            Name (_DEP, Package (One) {{ \\_SB.GIO0 }})
            Name (_CRS, ResourceTemplate ()
            {{
                GpioInt (Edge, ActiveBoth, Exclusive, PullUp, 0,
                    "\\\\_SB.GIO0", 0, ResourceConsumer, ,) {{ {volume_up_gpio} }}
            }})
            // Volume Up is the only standard key wired directly to TLMM.
            Name (_DSD, Package ()
            {{
                ToUUID ("fa6bd625-9ce8-470d-a2c7-b3ca36c4282e"),
                Package ()
                {{
                    Package () {{ Zero, One, Zero, 0x01, 0x0D }},
                    Package () {{ One, Zero, One, 0x0C, 0xE9 }}
                }}
            }})
        }}

        Device (TPAD)
        {{
            Name (_HID, "ZTE0001")
            Name (_UID, Zero)
            Name (_STA, 0x0F)
            Name (_DEP, Package (0x01) {{ \\_SB.GIO0 }})
            Name (_CRS, ResourceTemplate ()
            {{
                Memory32Fixed (ReadWrite, 0x{se19_base:08X}, 0x{se19_length:08X})
                Memory32Fixed (ReadWrite, 0x{qup4_base:08X}, 0x{qup4_length:08X})
                GpioInt (Level, ActiveLow, Exclusive, PullUp, 0,
                    "\\\\_SB.GIO0", 0, ResourceConsumer, ,) {{ {touch_irq_gpio} }}
                GpioIo (Exclusive, PullUp, 0, 8000, IoRestrictionOutputOnly,
                    "\\\\_SB.GIO0", 0, ResourceConsumer, ,) {{ {touch_reset_gpio} }}
            }})
        }}
    }}
}}
'''
if PE_POLLING:
    # PE 19042 cannot load the available Qualcomm GPIO provider (KMDF 1.33).
    # Give the polling driver only its two pin pages, with no TLMM owner.
    gpio_start = asl.index("        Device (GIO0)")
    touch_start = asl.index("        Device (TPAD)")
    asl = asl[:gpio_start] + asl[touch_start:]
    asl = asl.replace('"ZTE0001"', '"ZTE0002"')
    asl = asl.replace("            Name (_DEP, Package (0x01) { \\_SB.GIO0 })\n", "")
    reset_base = tlmm_base + 0x1000 * touch_reset_gpio
    irq_base = tlmm_base + 0x1000 * touch_irq_gpio
    pin_start = asl.index("                GpioInt (Level, ActiveLow")
    pin_end = asl.index("            })", pin_start)
    asl = (
        asl[:pin_start]
        + f"                Memory32Fixed (ReadWrite, 0x{reset_base:08X}, 0x1000)\n"
        + f"                Memory32Fixed (ReadWrite, 0x{irq_base:08X}, 0x1000)\n"
        + asl[pin_end:]
    )
(HERE / "DSDT.asl").write_text(asl)
subprocess.run(
    ["iasl", "-we", "-p", str(HERE / "DSDT"), str(HERE / "DSDT.asl")],
    check=True,
)

# iasl parses non-ASL data tables and catches malformed node lengths.
for table in ("APIC", "FACP", "GTDT", "IORT", "PPTT"):
    subprocess.run(
        ["iasl", "-d", str(HERE / f"{table}.aml")],
        cwd=HERE,
        check=True,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )

print("Generated NX809J ACPI tables")
print("  MPIDRs:", [hex(mpidr) for mpidr in mpidrs])
print(
    f"  UFS0: 0x{ufs_base:x}+0x{ufs_windows_length:x}, "
    f"GSIV {ufs_gsiv}, SID 0x{ufs_iommu[1]:x}"
)
print(
    f"  URS0: 0x{usb_base:x}+0x{usb_length:x}, "
    f"GSIV {usb_gsiv}, SID 0x{usb_iommu[1]:x}"
)
print(
    f"  SMMU: 0x{smmu_base:x}+0x{smmu_span:x}, "
    f"{len(smmu_context_gsivs)} context IRQs"
)
print(
    f"  GIO0: 0x{tlmm_base:x}+0x{tlmm_length:x}, "
    f"GSIV {tlmm_gsiv}, {gpio_count} GPIOs"
)
print(
    f"  TPAD: SE19 0x{se19_base:x}+0x{se19_length:x}, "
    f"reset GPIO{touch_reset_gpio}, IRQ GPIO{touch_irq_gpio}"
)
