#include <Library/MemoryMapLib.h>

STATIC
EFI_MEMORY_REGION_DESCRIPTOR
gMemoryDescriptor[] = {
  // Original NX809J SEC map with the documented platform corrections below.
  {"HYP", 0x80000000, 0xE00000, 1, 0, 0x703C07, 0, 1},
  {"RSRV0", 0x80E00000, 0x400000, 1, 0, 0x703C07, 0, 1},
  {"PDP", 0x81200000, 0x200000, 1, 5, 0x400, 0, 0},
  {"RSRV1", 0x81960000, 0xA0000, 1, 0, 0x703C07, 0, 1},
  {"XBL_DT", 0x81A00000, 0x60000, 1, 5, 0x703C07, 0, 1},
  {"AOP", 0x81C00000, 0xA0000, 1, 5, 0x400, 0, 0},
  {"TME", 0x81CA0000, 0x54000, 1, 5, 0x400, 0, 0},
  {"SMEM", 0x81D00000, 0x200000, 1, 5, 0x400, 0, 0},
  {"PDP_NS", 0x81F00000, 0x100000, 1, 5, 0x400, 0, 6},
  {"PvmFw", 0x824A0000, 0x100000, 1, 0, 0x703C07, 0, 1},
  {"PIL_Reserved", 0x8AA00000, 0x1D300000, 1, 5, 0x400, 0, 0},
  {"Display_Demura", 0xA7D00000, 0x2C80000, 1, 5, 0x703C07, 0, 5},
  {"DBI_Dump", 0xAA980000, 0x5770000, 0, 1, 0x2, 7, 0},
  {"DXE_Heap", 0xB00F0000, 0x131A0000, 1, 0, 0x703C07, 7, 1},
  {"FD_Reserved", 0xC6800000, 0x100000, 1, 0, 0x703C07, 4, 1},
  {"UEFI_FD", 0xC6900000, 0x300000, 1, 0, 0x703C07, 4, 1},
  {"UEFI_FD_Reserved", 0xC6C00000, 0x200000, 1, 0, 0x703C07, 4, 1},
  {"Info_Blk", 0xC6E01000, 0x1000, 1, 0, 0x703C07, 6, 1},
  {"MMU_PageTables", 0xC6E02000, 0x3000, 1, 0, 0x703C07, 4, 1},
  {"Log_Buffer", 0xC6E05000, 0x8000, 1, 0, 0x703C07, 6, 1},
  {"UEFI_Stack", 0xC6E0D000, 0x40000, 1, 0, 0x703C07, 4, 1},
  {"SEC_Heap", 0xC6E4D000, 0x8C000, 1, 0, 0x703C07, 4, 1},
  {"Sched_Heap", 0xC6ED9000, 0x400000, 1, 0, 0x703C07, 4, 1},
  {"FV_Region", 0xC72D9000, 0x400000, 1, 0, 0x703C07, 4, 1},
  {"UEFI_RESV", 0xC76D9000, 0x127000, 1, 0, 0x703C07, 4, 1},
  {"Kernel", 0xC7800000, 0x10000000, 1, 0, 0x703C07, 0, 1},
  {"XBL_Ramdump", 0xD7800000, 0x1C0000, 1, 5, 0x400, 0, 0},
  {"Global_SYNC", 0xD7A00000, 0x200000, 1, 5, 0x400, 0, 0},
  // The display carveout is DRAM consumed by GOP. The allocation HOB keeps
  // it out of the RAM allocator and exposes EfiMemoryMappedIO to the loader.
  // ARM Normal Non-Cacheable maps to EFI_MEMORY_WC in CpuDxe.
  {"Display_Reserved", 0xFC800000, 0x2B00000, AddMem, SYS_MEM,
   PRESENT | INITIALIZED | TESTED | WRITE_COMBINEABLE,
   EfiMemoryMappedIO, ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED},
  {"IMEM_Cookie_Base", 0x14680000, 0x1000, 2, 1, 0x2, 7, 6},
  {"IPC_ROUTER_TOP", 0x1100000, 0x400000, 2, 1, 0x400, 11, 6},
  {"SECURITY_CONTROL", 0x780000, 0x7000, 2, 1, 0x400, 11, 6},
  {"QUP", 0x800000, 0x300000, 2, 1, 0x400, 11, 6},
  {"QUP1", 0x1900000, 0x200000, 2, 1, 0x400, 11, 6},
  {"PRNG_CFG_PRNG", 0x10C0000, 0xC000, 2, 1, 0x400, 11, 6},
  {"CRYPTO0_CRYPTO", 0x1DC0000, 0x40000, 2, 1, 0x400, 11, 6},
  {"TCSR_TCSR_REGS", 0x1F00000, 0x100000, 2, 1, 0x400, 11, 6},
  {"PERIPH_SS", 0x8800000, 0x100000, 2, 1, 0x400, 11, 6},
  {"USB", 0xA600000, 0x200000, 2, 1, 0x400, 11, 6},
  {"AOSS", 0xB000000, 0x4000000, 2, 1, 0x400, 11, 6},
  {"TLMM", 0xF000000, 0x1000000, 2, 1, 0x400, 11, 6},
  {"SMMU", 0x15000000, 0x200000, 2, 1, 0x400, 11, 6},
  {"APSS_HM", 0x17000000, 0x8000000, 2, 1, 0x400, 11, 6},

  // Preserve the NX809J DT no-map ranges when RamManager adds conventional RAM.
  {"DSM1", 0x84A00000, 0x05500000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"DSM2", 0x89F00000, 0x00A80000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"RMTFS", 0xD7C00000, 0x00400000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},

  // Fixed reservations from the NX809J XBL configuration and captured stock DT.
  // Publish reserved HOBs without mapping or touching firmware-owned memory.
  {"HYP_TAIL", 0x81400000, 0x00560000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"CPUCP_DUMP", 0x82000000, 0x00380000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"ADSP_MHI", 0x82380000, 0x00020000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"SOCCP_SDI", 0x823A0000, 0x00040000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"PMIC_DUMP", 0x823E0000, 0x00080000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"QUP_FW", 0x82460000, 0x00018000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"TZ_STAT", 0x82700000, 0x00100000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"CPU_VECTORS", 0xC6E00000, 0x00001000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"TZ_MERGED", 0xD8000000, 0x00800000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"RAM_CONSOLE", 0xE2000000, 0x00200000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"LLCC_LPI", 0xFF800000, 0x00800000, HobOnlyNoCacheSetting, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
};

VOID
GetMemoryMap (
  OUT EFI_MEMORY_REGION_DESCRIPTOR **MemoryDescriptor,
  OUT UINT8                         *MemoryDescriptorCount)
{
  // Pass Data
  *MemoryDescriptor      = gMemoryDescriptor;
  *MemoryDescriptorCount = ARRAY_SIZE (gMemoryDescriptor);
}
