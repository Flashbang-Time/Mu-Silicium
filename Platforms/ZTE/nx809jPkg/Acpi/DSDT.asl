DefinitionBlock ("", "DSDT", 2, "ZTE   ", "NX809J  ", 2)
{
    Scope (\_SB)
    {
        Device (CPU0) { Name (_HID, "ACPI0007") Name (_UID, 0) Name (_STA, 0x0F) }
        Device (CPU1) { Name (_HID, "ACPI0007") Name (_UID, 1) Name (_STA, 0x0F) }
        Device (CPU2) { Name (_HID, "ACPI0007") Name (_UID, 2) Name (_STA, 0x0F) }
        Device (CPU3) { Name (_HID, "ACPI0007") Name (_UID, 3) Name (_STA, 0x0F) }
        Device (CPU4) { Name (_HID, "ACPI0007") Name (_UID, 4) Name (_STA, 0x0F) }
        Device (CPU5) { Name (_HID, "ACPI0007") Name (_UID, 5) Name (_STA, 0x0F) }
        Device (CPU6) { Name (_HID, "ACPI0007") Name (_UID, 6) Name (_STA, 0x0F) }
        Device (CPU7) { Name (_HID, "ACPI0007") Name (_UID, 7) Name (_STA, 0x0F) }

        Device (UFS0)
        {
            Name (_HID, "QCOM24A5")
            Name (_CLS, Package (0x03) { 0x01, 0x09, 0x01 })
            Name (_UID, Zero)
            Name (_CCA, One)
            Name (_STA, 0x0F)
            Name (_CRS, ResourceTemplate ()
            {
                Memory32Fixed (ReadWrite, 0x01D84000, 0x0001C000)
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 0x129 }
            })

            Device (DEV0)
            {
                Name (_ADR, 0x08)
                Method (_RMV, 0, NotSerialized) { Return (Zero) }
            }
        }

        Device (URS0)
        {
            Name (_HID, "QCOM0C8B")
            Name (_CID, "PNP0D10")
            Name (_UID, Zero)
            Name (_CCA, One)
            Name (_STA, 0x0F)
            Name (_CRS, ResourceTemplate ()
            {
                Memory32Fixed (ReadWrite, 0x0A600000, 0x000FC100)
                Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 0xA5 }
            })

            Device (RHUB)
            {
                Name (_ADR, Zero)
                // The USB 2 and SuperSpeed ports share one Type-C receptacle.
                Name (CPLD, Package (One)
                {
                    Buffer (0x10)
                    {
                        0x81, 0, 0, 0, 0, 0, 0, 0, 0x31, 0x1C, 0, 0, 0, 0, 0, 0
                    }
                })
                Device (HS01)
                {
                    Name (_ADR, One)
                    Name (_UPC, Package (0x04) { 0xFF, 0x09, Zero, Zero })
                    Method (_PLD, 0, NotSerialized) { Return (CPLD) }
                }
                Device (SS01)
                {
                    Name (_ADR, 0x02)
                    Name (_UPC, Package (0x04) { 0xFF, 0x09, Zero, Zero })
                    Method (_PLD, 0, NotSerialized) { Return (CPLD) }
                }
            }
        }

        Device (TPAD)
        {
            Name (_HID, "ZTE0002")
            Name (_UID, Zero)
            Name (_STA, 0x0F)
            Name (_CRS, ResourceTemplate ()
            {
                Memory32Fixed (ReadWrite, 0x01A80000, 0x00004000)
                Memory32Fixed (ReadWrite, 0x01AC0000, 0x00002000)
                Memory32Fixed (ReadWrite, 0x0F142000, 0x1000)
                Memory32Fixed (ReadWrite, 0x0F19E000, 0x1000)
            })
        }
    }
}
