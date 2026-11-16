/*
 * Intel ACPI Component Architecture
 * AML/ASL+ Disassembler version 20260408 (64-bit version)
 * Copyright (c) 2000 - 2026 Intel Corporation
 *
 * Disassembly of Platforms/ZTE/nx809jPkg/Acpi/IORT.aml
 *
 * ACPI Data Table [IORT]
 *
 * Format: [HexOffset DecimalOffset ByteLength]  FieldName : FieldValue (in hex)
 */

[000h 0000 004h]                   Signature : "IORT"    [IO Remapping Table]
[004h 0004 004h]                Table Length : 0000049E
[008h 0008 001h]                    Revision : 00
[009h 0009 001h]                    Checksum : 9E
[00Ah 0010 006h]                      Oem ID : "ZTE   "
[010h 0016 008h]                Oem Table ID : "NX809J  "
[018h 0024 004h]                Oem Revision : 00000002
[01Ch 0028 004h]             Asl Compiler ID : "PSIL"
[020h 0032 004h]       Asl Compiler Revision : 00000002

[024h 0036 004h]                  Node Count : 00000003
[028h 0040 004h]                 Node Offset : 00000030
[02Ch 0044 004h]                    Reserved : 00000000

[030h 0048 001h]                        Type : 03
[031h 0049 002h]                      Length : 03CC
[033h 0051 001h]                    Revision : 00
[034h 0052 004h]                    Reserved : 00000000
[038h 0056 004h]               Mapping Count : 00000000
[03Ch 0060 004h]              Mapping Offset : 00000000

[040h 0064 008h]                Base Address : 0000000015000000
[048h 0072 008h]                        Span : 0000000000100000
[050h 0080 004h]                       Model : 00000003
[054h 0084 004h]       Flags (decoded below) : 00000000
                               DVM Supported : 0
                               Coherent Walk : 0
[058h 0088 004h]     Global Interrupt Offset : 0000003C
[05Ch 0092 004h]     Context Interrupt Count : 00000070
[060h 0096 004h]    Context Interrupt Offset : 0000004C
[064h 0100 004h]         PMU Interrupt Count : 00000000
[068h 0104 004h]        PMU Interrupt Offset : 00000000

[06Ch 0108 004h]                     NSgIrpt : 00000061
[070h 0112 004h] NSgIrpt Flags (decoded below) : 00000000
                              Edge Triggered : 0
[074h 0116 004h]                  NSgCfgIrpt : 00000000
[078h 0120 004h] NSgCfgIrpt Flags (decoded below) : 00000000
                              Edge Triggered : 0
[07Ch 0124 008h]           Context Interrupt : 0000000100000081
[084h 0132 008h]           Context Interrupt : 0000000100000082
[08Ch 0140 008h]           Context Interrupt : 0000000100000083
[094h 0148 008h]           Context Interrupt : 0000000100000084
[09Ch 0156 008h]           Context Interrupt : 0000000100000085
[0A4h 0164 008h]           Context Interrupt : 0000000100000086
[0ACh 0172 008h]           Context Interrupt : 0000000100000087
[0B4h 0180 008h]           Context Interrupt : 0000000100000088
[0BCh 0188 008h]           Context Interrupt : 0000000100000089
[0C4h 0196 008h]           Context Interrupt : 000000010000008A
[0CCh 0204 008h]           Context Interrupt : 000000010000008B
[0D4h 0212 008h]           Context Interrupt : 000000010000008C
[0DCh 0220 008h]           Context Interrupt : 000000010000008D
[0E4h 0228 008h]           Context Interrupt : 000000010000008E
[0ECh 0236 008h]           Context Interrupt : 000000010000008F
[0F4h 0244 008h]           Context Interrupt : 0000000100000090
[0FCh 0252 008h]           Context Interrupt : 0000000100000091
[104h 0260 008h]           Context Interrupt : 0000000100000092
[10Ch 0268 008h]           Context Interrupt : 0000000100000093
[114h 0276 008h]           Context Interrupt : 0000000100000094
[11Ch 0284 008h]           Context Interrupt : 0000000100000095
[124h 0292 008h]           Context Interrupt : 0000000100000096
[12Ch 0300 008h]           Context Interrupt : 00000001000000D5
[134h 0308 008h]           Context Interrupt : 00000001000000D6
[13Ch 0316 008h]           Context Interrupt : 00000001000000D7
[144h 0324 008h]           Context Interrupt : 00000001000000D8
[14Ch 0332 008h]           Context Interrupt : 00000001000000D9
[154h 0340 008h]           Context Interrupt : 00000001000000DA
[15Ch 0348 008h]           Context Interrupt : 00000001000000DB
[164h 0356 008h]           Context Interrupt : 00000001000000DC
[16Ch 0364 008h]           Context Interrupt : 00000001000000DD
[174h 0372 008h]           Context Interrupt : 00000001000000DE
[17Ch 0380 008h]           Context Interrupt : 00000001000000DF
[184h 0388 008h]           Context Interrupt : 00000001000000E0
[18Ch 0396 008h]           Context Interrupt : 000000010000015B
[194h 0404 008h]           Context Interrupt : 000000010000015C
[19Ch 0412 008h]           Context Interrupt : 000000010000015D
[1A4h 0420 008h]           Context Interrupt : 000000010000015E
[1ACh 0428 008h]           Context Interrupt : 000000010000015F
[1B4h 0436 008h]           Context Interrupt : 0000000100000160
[1BCh 0444 008h]           Context Interrupt : 0000000100000161
[1C4h 0452 008h]           Context Interrupt : 0000000100000162
[1CCh 0460 008h]           Context Interrupt : 0000000100000163
[1D4h 0468 008h]           Context Interrupt : 0000000100000164
[1DCh 0476 008h]           Context Interrupt : 0000000100000165
[1E4h 0484 008h]           Context Interrupt : 0000000100000166
[1ECh 0492 008h]           Context Interrupt : 0000000100000167
[1F4h 0500 008h]           Context Interrupt : 0000000100000168
[1FCh 0508 008h]           Context Interrupt : 0000000100000169
[204h 0516 008h]           Context Interrupt : 000000010000016A
[20Ch 0524 008h]           Context Interrupt : 000000010000016B
[214h 0532 008h]           Context Interrupt : 000000010000016C
[21Ch 0540 008h]           Context Interrupt : 000000010000016D
[224h 0548 008h]           Context Interrupt : 000000010000016E
[22Ch 0556 008h]           Context Interrupt : 000000010000016F
[234h 0564 008h]           Context Interrupt : 0000000100000170
[23Ch 0572 008h]           Context Interrupt : 0000000100000171
[244h 0580 008h]           Context Interrupt : 0000000100000172
[24Ch 0588 008h]           Context Interrupt : 0000000100000173
[254h 0596 008h]           Context Interrupt : 0000000100000174
[25Ch 0604 008h]           Context Interrupt : 0000000100000175
[264h 0612 008h]           Context Interrupt : 0000000100000176
[26Ch 0620 008h]           Context Interrupt : 0000000100000177
[274h 0628 008h]           Context Interrupt : 0000000100000178
[27Ch 0636 008h]           Context Interrupt : 0000000100000179
[284h 0644 008h]           Context Interrupt : 00000001000001AB
[28Ch 0652 008h]           Context Interrupt : 00000001000001AC
[294h 0660 008h]           Context Interrupt : 00000001000001AD
[29Ch 0668 008h]           Context Interrupt : 00000001000001AE
[2A4h 0676 008h]           Context Interrupt : 00000001000001AF
[2ACh 0684 008h]           Context Interrupt : 00000001000001B0
[2B4h 0692 008h]           Context Interrupt : 00000001000001B1
[2BCh 0700 008h]           Context Interrupt : 00000001000001B2
[2C4h 0708 008h]           Context Interrupt : 00000001000001B3
[2CCh 0716 008h]           Context Interrupt : 00000001000001B4
[2D4h 0724 008h]           Context Interrupt : 00000001000001B5
[2DCh 0732 008h]           Context Interrupt : 00000001000001B6
[2E4h 0740 008h]           Context Interrupt : 00000001000001B7
[2ECh 0748 008h]           Context Interrupt : 00000001000001B8
[2F4h 0756 008h]           Context Interrupt : 00000001000001B9
[2FCh 0764 008h]           Context Interrupt : 00000001000001C2
[304h 0772 008h]           Context Interrupt : 00000001000001C3
[30Ch 0780 008h]           Context Interrupt : 00000001000001BC
[314h 0788 008h]           Context Interrupt : 00000001000001C5
[31Ch 0796 008h]           Context Interrupt : 00000001000002E3
[324h 0804 008h]           Context Interrupt : 00000001000001C7
[32Ch 0812 008h]           Context Interrupt : 00000001000001C8
[334h 0820 008h]           Context Interrupt : 00000001000001C9
[33Ch 0828 008h]           Context Interrupt : 00000001000002D2
[344h 0836 008h]           Context Interrupt : 00000001000002D3
[34Ch 0844 008h]           Context Interrupt : 00000001000002D4
[354h 0852 008h]           Context Interrupt : 00000001000002D5
[35Ch 0860 008h]           Context Interrupt : 00000001000002D6
[364h 0868 008h]           Context Interrupt : 00000001000002D7
[36Ch 0876 008h]           Context Interrupt : 00000001000002D8
[374h 0884 008h]           Context Interrupt : 00000001000002D9
[37Ch 0892 008h]           Context Interrupt : 00000001000001BA
[384h 0900 008h]           Context Interrupt : 0000000100000208
[38Ch 0908 008h]           Context Interrupt : 0000000100000209
[394h 0916 008h]           Context Interrupt : 000000010000020A
[39Ch 0924 008h]           Context Interrupt : 000000010000020B
[3A4h 0932 008h]           Context Interrupt : 000000010000020C
[3ACh 0940 008h]           Context Interrupt : 000000010000020D
[3B4h 0948 008h]           Context Interrupt : 000000010000020E
[3BCh 0956 008h]           Context Interrupt : 000000010000020F
[3C4h 0964 008h]           Context Interrupt : 0000000100000210
[3CCh 0972 008h]           Context Interrupt : 0000000100000211
[3D4h 0980 008h]           Context Interrupt : 0000000100000212
[3DCh 0988 008h]           Context Interrupt : 0000000100000213
[3E4h 0996 008h]           Context Interrupt : 0000000100000214
[3ECh 1004 008h]           Context Interrupt : 0000000100000215
[3F4h 1012 008h]           Context Interrupt : 0000000100000216

[3FCh 1020 001h]                        Type : 01
[3FDh 1021 002h]                      Length : 0051
[3FFh 1023 001h]                    Revision : 00
[400h 1024 004h]                    Reserved : 00000000
[404h 1028 004h]               Mapping Count : 00000001
[408h 1032 004h]              Mapping Offset : 0000003D

[40Ch 1036 004h]                  Node Flags : 00000000
[410h 1040 008h]           Memory Properties : [IORT Memory Access Properties]
[410h 1040 004h]             Cache Coherency : 00000001
[414h 1044 001h]       Hints (decoded below) : 00
                                   Transient : 0
                              Write Allocate : 0
                               Read Allocate : 0
                                    Override : 0
[415h 1045 002h]                    Reserved : 0000
[417h 1047 001h] Memory Flags (decoded below) : 01
                                   Coherency : 1
                            Device Attribute : 0
               Ensured Coherency of Accesses : 0
[418h 1048 001h]           Memory Size Limit : 28
[419h 1049 00Ah]                 Device Name : "\_SB.UFS0"
[423h 1059 016h]                     Padding : 49 4F 52 54 9E 04 00 00 00 9E 5A 54 45 20 20 20 /* IORT......ZTE    */\
/* 433h 1075   6 */                            4E 58 38 30 39 4A                               /* NX809J */\

[439h 1081 004h]                  Input base : 81030000
[43Dh 1085 004h]                    ID Count : 00000000
[441h 1089 004h]                 Output Base : 00000060
[445h 1093 004h]            Output Reference : 00000030
[449h 1097 004h]       Flags (decoded below) : 00000000
                              Single Mapping : 0

[44Dh 1101 001h]                        Type : 01
[44Eh 1102 002h]                      Length : 0051
[450h 1104 001h]                    Revision : 00
[451h 1105 004h]                    Reserved : 00000000
[455h 1109 004h]               Mapping Count : 00000001
[459h 1113 004h]              Mapping Offset : 0000003D

[45Dh 1117 004h]                  Node Flags : 00000000
[461h 1121 008h]           Memory Properties : [IORT Memory Access Properties]
[461h 1121 004h]             Cache Coherency : 00000001
[465h 1125 001h]       Hints (decoded below) : 00
                                   Transient : 0
                              Write Allocate : 0
                               Read Allocate : 0
                                    Override : 0
[466h 1126 002h]                    Reserved : 0000
[468h 1128 001h] Memory Flags (decoded below) : 01
                                   Coherency : 1
                            Device Attribute : 0
               Ensured Coherency of Accesses : 0
[469h 1129 001h]           Memory Size Limit : 28
[46Ah 1130 00Ah]                 Device Name : "\_SB.URS0"
[474h 1140 016h]                     Padding : 49 4F 52 54 9E 04 00 00 00 9E 5A 54 45 20 20 20 /* IORT......ZTE    */\
/* 484h 1156   6 */                            4E 58 38 30 39 4A                               /* NX809J */\

[48Ah 1162 004h]                  Input base : 80030000
[48Eh 1166 004h]                    ID Count : 00000000
[492h 1170 004h]                 Output Base : 00000040
[496h 1174 004h]            Output Reference : 00000030
[49Ah 1178 004h]       Flags (decoded below) : 00000000
                              Single Mapping : 0

Raw Table Data: Length 1182 (0x49E)

    0000: 49 4F 52 54 9E 04 00 00 00 9E 5A 54 45 20 20 20  // IORT......ZTE
    0010: 4E 58 38 30 39 4A 20 20 02 00 00 00 50 53 49 4C  // NX809J  ....PSIL
    0020: 02 00 00 00 03 00 00 00 30 00 00 00 00 00 00 00  // ........0.......
    0030: 03 CC 03 00 00 00 00 00 00 00 00 00 00 00 00 00  // ................
    0040: 00 00 00 15 00 00 00 00 00 00 10 00 00 00 00 00  // ................
    0050: 03 00 00 00 00 00 00 00 3C 00 00 00 70 00 00 00  // ........<...p...
    0060: 4C 00 00 00 00 00 00 00 00 00 00 00 61 00 00 00  // L...........a...
    0070: 00 00 00 00 00 00 00 00 00 00 00 00 81 00 00 00  // ................
    0080: 01 00 00 00 82 00 00 00 01 00 00 00 83 00 00 00  // ................
    0090: 01 00 00 00 84 00 00 00 01 00 00 00 85 00 00 00  // ................
    00A0: 01 00 00 00 86 00 00 00 01 00 00 00 87 00 00 00  // ................
    00B0: 01 00 00 00 88 00 00 00 01 00 00 00 89 00 00 00  // ................
    00C0: 01 00 00 00 8A 00 00 00 01 00 00 00 8B 00 00 00  // ................
    00D0: 01 00 00 00 8C 00 00 00 01 00 00 00 8D 00 00 00  // ................
    00E0: 01 00 00 00 8E 00 00 00 01 00 00 00 8F 00 00 00  // ................
    00F0: 01 00 00 00 90 00 00 00 01 00 00 00 91 00 00 00  // ................
    0100: 01 00 00 00 92 00 00 00 01 00 00 00 93 00 00 00  // ................
    0110: 01 00 00 00 94 00 00 00 01 00 00 00 95 00 00 00  // ................
    0120: 01 00 00 00 96 00 00 00 01 00 00 00 D5 00 00 00  // ................
    0130: 01 00 00 00 D6 00 00 00 01 00 00 00 D7 00 00 00  // ................
    0140: 01 00 00 00 D8 00 00 00 01 00 00 00 D9 00 00 00  // ................
    0150: 01 00 00 00 DA 00 00 00 01 00 00 00 DB 00 00 00  // ................
    0160: 01 00 00 00 DC 00 00 00 01 00 00 00 DD 00 00 00  // ................
    0170: 01 00 00 00 DE 00 00 00 01 00 00 00 DF 00 00 00  // ................
    0180: 01 00 00 00 E0 00 00 00 01 00 00 00 5B 01 00 00  // ............[...
    0190: 01 00 00 00 5C 01 00 00 01 00 00 00 5D 01 00 00  // ....\.......]...
    01A0: 01 00 00 00 5E 01 00 00 01 00 00 00 5F 01 00 00  // ....^......._...
    01B0: 01 00 00 00 60 01 00 00 01 00 00 00 61 01 00 00  // ....`.......a...
    01C0: 01 00 00 00 62 01 00 00 01 00 00 00 63 01 00 00  // ....b.......c...
    01D0: 01 00 00 00 64 01 00 00 01 00 00 00 65 01 00 00  // ....d.......e...
    01E0: 01 00 00 00 66 01 00 00 01 00 00 00 67 01 00 00  // ....f.......g...
    01F0: 01 00 00 00 68 01 00 00 01 00 00 00 69 01 00 00  // ....h.......i...
    0200: 01 00 00 00 6A 01 00 00 01 00 00 00 6B 01 00 00  // ....j.......k...
    0210: 01 00 00 00 6C 01 00 00 01 00 00 00 6D 01 00 00  // ....l.......m...
    0220: 01 00 00 00 6E 01 00 00 01 00 00 00 6F 01 00 00  // ....n.......o...
    0230: 01 00 00 00 70 01 00 00 01 00 00 00 71 01 00 00  // ....p.......q...
    0240: 01 00 00 00 72 01 00 00 01 00 00 00 73 01 00 00  // ....r.......s...
    0250: 01 00 00 00 74 01 00 00 01 00 00 00 75 01 00 00  // ....t.......u...
    0260: 01 00 00 00 76 01 00 00 01 00 00 00 77 01 00 00  // ....v.......w...
    0270: 01 00 00 00 78 01 00 00 01 00 00 00 79 01 00 00  // ....x.......y...
    0280: 01 00 00 00 AB 01 00 00 01 00 00 00 AC 01 00 00  // ................
    0290: 01 00 00 00 AD 01 00 00 01 00 00 00 AE 01 00 00  // ................
    02A0: 01 00 00 00 AF 01 00 00 01 00 00 00 B0 01 00 00  // ................
    02B0: 01 00 00 00 B1 01 00 00 01 00 00 00 B2 01 00 00  // ................
    02C0: 01 00 00 00 B3 01 00 00 01 00 00 00 B4 01 00 00  // ................
    02D0: 01 00 00 00 B5 01 00 00 01 00 00 00 B6 01 00 00  // ................
    02E0: 01 00 00 00 B7 01 00 00 01 00 00 00 B8 01 00 00  // ................
    02F0: 01 00 00 00 B9 01 00 00 01 00 00 00 C2 01 00 00  // ................
    0300: 01 00 00 00 C3 01 00 00 01 00 00 00 BC 01 00 00  // ................
    0310: 01 00 00 00 C5 01 00 00 01 00 00 00 E3 02 00 00  // ................
    0320: 01 00 00 00 C7 01 00 00 01 00 00 00 C8 01 00 00  // ................
    0330: 01 00 00 00 C9 01 00 00 01 00 00 00 D2 02 00 00  // ................
    0340: 01 00 00 00 D3 02 00 00 01 00 00 00 D4 02 00 00  // ................
    0350: 01 00 00 00 D5 02 00 00 01 00 00 00 D6 02 00 00  // ................
    0360: 01 00 00 00 D7 02 00 00 01 00 00 00 D8 02 00 00  // ................
    0370: 01 00 00 00 D9 02 00 00 01 00 00 00 BA 01 00 00  // ................
    0380: 01 00 00 00 08 02 00 00 01 00 00 00 09 02 00 00  // ................
    0390: 01 00 00 00 0A 02 00 00 01 00 00 00 0B 02 00 00  // ................
    03A0: 01 00 00 00 0C 02 00 00 01 00 00 00 0D 02 00 00  // ................
    03B0: 01 00 00 00 0E 02 00 00 01 00 00 00 0F 02 00 00  // ................
    03C0: 01 00 00 00 10 02 00 00 01 00 00 00 11 02 00 00  // ................
    03D0: 01 00 00 00 12 02 00 00 01 00 00 00 13 02 00 00  // ................
    03E0: 01 00 00 00 14 02 00 00 01 00 00 00 15 02 00 00  // ................
    03F0: 01 00 00 00 16 02 00 00 01 00 00 00 01 51 00 00  // .............Q..
    0400: 00 00 00 00 01 00 00 00 3D 00 00 00 00 00 00 00  // ........=.......
    0410: 01 00 00 00 00 00 00 01 28 5C 5F 53 42 2E 55 46  // ........(\_SB.UF
    0420: 53 30 00 00 00 00 00 00 00 00 00 00 00 00 00 00  // S0..............
    0430: 00 00 00 00 00 00 00 00 00 00 00 03 81 00 00 00  // ................
    0440: 00 60 00 00 00 30 00 00 00 00 00 00 00 01 51 00  // .`...0........Q.
    0450: 00 00 00 00 00 01 00 00 00 3D 00 00 00 00 00 00  // .........=......
    0460: 00 01 00 00 00 00 00 00 01 28 5C 5F 53 42 2E 55  // .........(\_SB.U
    0470: 52 53 30 00 00 00 00 00 00 00 00 00 00 00 00 00  // RS0.............
    0480: 00 00 00 00 00 00 00 00 00 00 00 00 03 80 00 00  // ................
    0490: 00 00 40 00 00 00 30 00 00 00 00 00 00 00        // ..@...0.......
