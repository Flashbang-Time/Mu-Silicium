/** @file
  NX809J touch handoff hardware definitions.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef NX809J_TOUCH_HARDWARE_H_
#define NX809J_TOUCH_HARDWARE_H_

#define NX809J_TLMM_BASE              0x0F100000U
#define NX809J_TLMM_GPIO_STRIDE       0x00001000U
#define NX809J_TLMM_GPIO_CFG(Gpio)    (NX809J_TLMM_BASE + ((Gpio) * NX809J_TLMM_GPIO_STRIDE))
#define NX809J_TLMM_GPIO_IO(Gpio)     (NX809J_TLMM_GPIO_CFG (Gpio) + 4U)
#define NX809J_TLMM_PULL_MASK         0x00000003U
#define NX809J_TLMM_MUX_MASK          0x0000003CU
#define NX809J_TLMM_MUX_SHIFT         2U
#define NX809J_TLMM_DRIVE_MASK        0x000001C0U
#define NX809J_TLMM_DRIVE_SHIFT       6U
#define NX809J_QUP4_SE0_MUX           1U
#define NX809J_DRIVE_6MA              2U

#define NX809J_SPI_FIRST_GPIO         48U
#define NX809J_SPI_LAST_GPIO          51U
#define NX809J_TOUCH_RESET_GPIO       66U
#define NX809J_TOUCH_IRQ_GPIO         158U

#define NX809J_SE19_BASE              0x01A80000U
#define NX809J_SE19_SOURCE_HZ         19200000U
#define NX809J_SE19_CLOCK_NAME        "gcc_qupv3_wrap4_s0_clk"
#define NX809J_GENI_STATUS            0x040U
#define NX809J_GENI_IF_DISABLE        0x064U
#define NX809J_GENI_FW_REVISION       0x068U
#define NX809J_GENI_HW_PARAM_0        0xE24U
#define NX809J_GENI_HW_PARAM_1        0xE28U
#define NX809J_GENI_PROTOCOL(Rev)     (((Rev) >> 8U) & 0xFFU)
#define NX809J_GENI_PROTOCOL_SPI      1U

#define NX809J_TOUCH_CLOCK_NAMES \
  "gcc_qupv3_wrap_4_m_ahb_clk", \
  "gcc_qupv3_wrap_4_s_ahb_clk", \
  "gcc_qupv3_wrap4_core_clk", \
  "gcc_qupv3_wrap4_core_2x_clk", \
  NX809J_SE19_CLOCK_NAME

#endif
