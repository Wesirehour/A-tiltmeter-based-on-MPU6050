// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2017 NXP
 *
 * Peng Fan <peng.fan@nxp.com>
 */

#include <common.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/io.h>
#include <errno.h>

static struct ccm_reg *ccm_reg = (struct ccm_reg *)CCM_BASE_ADDR;

#ifdef CONFIG_IMX8MQ
static struct clk_root_map root_array[] = {
	{ARM_A53_CLK_ROOT, CORE_CLOCK_SLICE, 0,
	 {OSC_25M_CLK, ARM_PLL_CLK, SYSTEM_PLL2_500M_CLK,
	  SYSTEM_PLL2_1000M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL1_400M_CLK, AUDIO_PLL1_CLK, SYSTEM_PLL3_CLK}
	},
	{ARM_M4_CLK_ROOT, CORE_CLOCK_SLICE, 1,
	 {OSC_25M_CLK, SYSTEM_PLL2_200M_CLK, SYSTEM_PLL2_250M_CLK,
	  SYSTEM_PLL1_266M_CLK, SYSTEM_PLL1_800M_CLK,
	  AUDIO_PLL1_CLK, VIDEO_PLL_CLK, SYSTEM_PLL3_CLK}
	},
	{VPU_A53_CLK_ROOT, CORE_CLOCK_SLICE, 2,
	 {OSC_25M_CLK, ARM_PLL_CLK, SYSTEM_PLL2_500M_CLK,
	  SYSTEM_PLL2_1000M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL1_400M_CLK, AUDIO_PLL1_CLK, VPU_PLL_CLK}
	},
	{GPU_CORE_CLK_ROOT, CORE_CLOCK_SLICE, 3,
	 {OSC_25M_CLK, GPU_PLL_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL2_1000M_CLK,
	  AUDIO_PLL1_CLK, VIDEO_PLL_CLK, AUDIO_PLL2_CLK}
	},
	{GPU_SHADER_CLK_ROOT, CORE_CLOCK_SLICE, 4,
	 {OSC_25M_CLK, GPU_PLL_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL2_1000M_CLK,
	  AUDIO_PLL1_CLK, VIDEO_PLL_CLK, AUDIO_PLL2_CLK}
	},
	{MAIN_AXI_CLK_ROOT, BUS_CLOCK_SLICE, 0,
	 {OSC_25M_CLK, SYSTEM_PLL2_333M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_250M_CLK, SYSTEM_PLL2_1000M_CLK,
	  AUDIO_PLL1_CLK, VIDEO_PLL_CLK, SYSTEM_PLL1_100M_CLK}
	},
	{ENET_AXI_CLK_ROOT, BUS_CLOCK_SLICE, 1,
	 {OSC_25M_CLK, SYSTEM_PLL1_266M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_250M_CLK, SYSTEM_PLL2_200M_CLK,
	  AUDIO_PLL1_CLK, VIDEO_PLL_CLK, SYSTEM_PLL3_CLK}
	},
	{NAND_USDHC_BUS_CLK_ROOT, BUS_CLOCK_SLICE, 2,
	 {OSC_25M_CLK, SYSTEM_PLL1_266M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_200M_CLK, SYSTEM_PLL1_133M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL2_250M_CLK, AUDIO_PLL1_CLK}
	},
	{VPU_BUS_CLK_ROOT, BUS_CLOCK_SLICE, 3,
	 {OSC_25M_CLK, SYSTEM_PLL1_800M_CLK, VPU_PLL_CLK,
	  AUDIO_PLL2_CLK, SYSTEM_PLL3_CLK, SYSTEM_PLL2_1000M_CLK,
	  SYSTEM_PLL2_200M_CLK, SYSTEM_PLL1_100M_CLK}
	},
	{DISPLAY_AXI_CLK_ROOT, BUS_CLOCK_SLICE, 4,
	 {OSC_25M_CLK, SYSTEM_PLL2_125M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL1_400M_CLK, AUDIO_PLL2_CLK,
	  EXT_CLK_1, EXT_CLK_4}
	},
	{DISPLAY_APB_CLK_ROOT, BUS_CLOCK_SLICE, 5,
	 {OSC_25M_CLK, SYSTEM_PLL2_125M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL1_400M_CLK, AUDIO_PLL2_CLK,
	  EXT_CLK_1, EXT_CLK_3}
	},
	{DISPLAY_RTRM_CLK_ROOT, BUS_CLOCK_SLICE, 6,
	 {OSC_25M_CLK, SYSTEM_PLL1_800M_CLK, SYSTEM_PLL2_200M_CLK,
	  SYSTEM_PLL1_400M_CLK, AUDIO_PLL1_CLK, VIDEO_PLL_CLK,
	  EXT_CLK_2, EXT_CLK_3}
	},
	{USB_BUS_CLK_ROOT, BUS_CLOCK_SLICE, 7,
	 {OSC_25M_CLK, SYSTEM_PLL2_500M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL2_200M_CLK,
	  EXT_CLK_2, EXT_CLK_4, AUDIO_PLL2_CLK}
	},
	{GPU_AXI_CLK_ROOT, BUS_CLOCK_SLICE, 8,
	 {OSC_25M_CLK, SYSTEM_PLL1_800M_CLK, GPU_PLL_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL2_1000M_CLK,
	  AUDIO_PLL1_CLK, VIDEO_PLL_CLK, AUDIO_PLL2_CLK}
	},
	{GPU_AHB_CLK_ROOT, BUS_CLOCK_SLICE, 9,
	 {OSC_25M_CLK, SYSTEM_PLL1_800M_CLK, GPU_PLL_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL2_1000M_CLK,
	  AUDIO_PLL1_CLK, VIDEO_PLL_CLK, AUDIO_PLL2_CLK}
	},
	{NOC_CLK_ROOT, BUS_CLOCK_SLICE, 10,
	 {OSC_25M_CLK, SYSTEM_PLL1_800M_CLK, SYSTEM_PLL3_CLK,
	  SYSTEM_PLL2_1000M_CLK, SYSTEM_PLL2_500M_CLK,
	  AUDIO_PLL1_CLK, VIDEO_PLL_CLK, AUDIO_PLL2_CLK}
	},
	{NOC_APB_CLK_ROOT, BUS_CLOCK_SLICE, 11,
	 {OSC_25M_CLK, SYSTEM_PLL1_400M_CLK, SYSTEM_PLL3_CLK,
	  SYSTEM_PLL2_333M_CLK, SYSTEM_PLL2_200M_CLK,
	  SYSTEM_PLL1_800M_CLK, AUDIO_PLL1_CLK, VIDEO_PLL_CLK}
	},
	{AHB_CLK_ROOT, AHB_CLOCK_SLICE, 0,
	 {OSC_25M_CLK, SYSTEM_PLL1_133M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL1_400M_CLK, SYSTEM_PLL2_125M_CLK,
	  SYSTEM_PLL3_CLK, AUDIO_PLL1_CLK, VIDEO_PLL_CLK}
	},
	{IPG_CLK_ROOT, IPG_CLOCK_SLICE, 0,
	 {}
	},
	{AUDIO_AHB_CLK_ROOT, AHB_CLOCK_SLICE, 1,
	 {OSC_25M_CLK, SYSTEM_PLL2_500M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_1000M_CLK, SYSTEM_PLL2_166M_CLK,
	  SYSTEM_PLL3_CLK, AUDIO_PLL1_CLK, VIDEO_PLL_CLK}
	},
	{MIPI_DSI_ESC_RX_CLK_ROOT, AHB_CLOCK_SLICE, 2,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_40M_CLK,
	  SYSTEM_PLL1_800M_CLK, SYSTEM_PLL2_1000M_CLK,
	  SYSTEM_PLL3_CLK, EXT_CLK_3, AUDIO_PLL1_CLK },
	},
	{DRAM_ALT_CLK_ROOT, IP_CLOCK_SLICE, 0,
	 {OSC_25M_CLK, SYSTEM_PLL1_800M_CLK, SYSTEM_PLL1_100M_CLK,
	  SYSTEM_PLL2_500M_CLK, SYSTEM_PLL2_250M_CLK,
	  SYSTEM_PLL1_400M_CLK, AUDIO_PLL1_CLK, SYSTEM_PLL1_266M_CLK}
	},
	{DRAM_APB_CLK_ROOT, IP_CLOCK_SLICE, 1,
	 {OSC_25M_CLK, SYSTEM_PLL2_200M_CLK, SYSTEM_PLL1_40M_CLK,
	  SYSTEM_PLL1_160M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL2_250M_CLK, AUDIO_PLL2_CLK}
	},
	{VPU_G1_CLK_ROOT, IP_CLOCK_SLICE, 2,
	 {OSC_25M_CLK, VPU_PLL_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_1000M_CLK, SYSTEM_PLL1_100M_CLK,
	  SYSTEM_PLL2_125M_CLK, SYSTEM_PLL3_CLK, AUDIO_PLL1_CLK}
	},
	{VPU_G2_CLK_ROOT, IP_CLOCK_SLICE, 3,
	 {OSC_25M_CLK, VPU_PLL_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_1000M_CLK, SYSTEM_PLL1_100M_CLK,
	  SYSTEM_PLL2_125M_CLK, SYSTEM_PLL3_CLK, AUDIO_PLL1_CLK}
	},
	{DISPLAY_DTRC_CLK_ROOT, IP_CLOCK_SLICE, 4,
	 {OSC_25M_CLK, VPU_PLL_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_1000M_CLK, SYSTEM_PLL1_160M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL3_CLK, AUDIO_PLL2_CLK}
	},
	{DISPLAY_DC8000_CLK_ROOT, IP_CLOCK_SLICE, 5,
	 {OSC_25M_CLK, VPU_PLL_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_1000M_CLK, SYSTEM_PLL1_160M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL3_CLK, AUDIO_PLL2_CLK}
	},
	{PCIE1_CTRL_CLK_ROOT, IP_CLOCK_SLICE, 6,
	 {OSC_25M_CLK, SYSTEM_PLL2_250M_CLK, SYSTEM_PLL2_200M_CLK,
	  SYSTEM_PLL1_266M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_500M_CLK, SYSTEM_PLL2_333M_CLK, SYSTEM_PLL3_CLK}
	},
	{PCIE1_PHY_CLK_ROOT, IP_CLOCK_SLICE, 7,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL2_500M_CLK,
	  EXT_CLK_1, EXT_CLK_2, EXT_CLK_3, EXT_CLK_4,
	  SYSTEM_PLL1_400M_CLK}
	},
	{PCIE1_AUX_CLK_ROOT, IP_CLOCK_SLICE, 8,
	 {OSC_25M_CLK, SYSTEM_PLL2_200M_CLK, SYSTEM_PLL2_50M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_80M_CLK,
	  SYSTEM_PLL1_160M_CLK, SYSTEM_PLL1_200M_CLK}
	},
	{DC_PIXEL_CLK_ROOT, IP_CLOCK_SLICE, 9,
	 {OSC_25M_CLK, VIDEO_PLL_CLK, AUDIO_PLL2_CLK,
	  AUDIO_PLL1_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_1000M_CLK, SYSTEM_PLL3_CLK, EXT_CLK_4}
	},
	{LCDIF_PIXEL_CLK_ROOT, IP_CLOCK_SLICE, 10,
	 {OSC_25M_CLK, VIDEO_PLL_CLK, AUDIO_PLL2_CLK,
	  AUDIO_PLL1_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_1000M_CLK, SYSTEM_PLL3_CLK, EXT_CLK_4}
	},
	{SAI1_CLK_ROOT, IP_CLOCK_SLICE, 11,
	 {OSC_25M_CLK, AUDIO_PLL1_CLK, AUDIO_PLL2_CLK,
	  VIDEO_PLL_CLK, SYSTEM_PLL1_133M_CLK,
	  OSC_27M_CLK, EXT_CLK_1, EXT_CLK_2}
	},
	{SAI2_CLK_ROOT, IP_CLOCK_SLICE, 12,
	 {OSC_25M_CLK, AUDIO_PLL1_CLK, AUDIO_PLL2_CLK,
	  VIDEO_PLL_CLK, SYSTEM_PLL1_133M_CLK,
	  OSC_27M_CLK, EXT_CLK_2, EXT_CLK_3}
	},
	{SAI3_CLK_ROOT, IP_CLOCK_SLICE, 13,
	 {OSC_25M_CLK, AUDIO_PLL1_CLK, AUDIO_PLL2_CLK,
	  VIDEO_PLL_CLK, SYSTEM_PLL1_133M_CLK,
	  OSC_27M_CLK, EXT_CLK_3, EXT_CLK_4}
	},
	{SAI4_CLK_ROOT, IP_CLOCK_SLICE, 14,
	 {OSC_25M_CLK, AUDIO_PLL1_CLK, AUDIO_PLL2_CLK,
	  VIDEO_PLL_CLK, SYSTEM_PLL1_133M_CLK,
	  OSC_27M_CLK, EXT_CLK_1, EXT_CLK_2}
	},
	{SAI5_CLK_ROOT, IP_CLOCK_SLICE, 15,
	 {OSC_25M_CLK, AUDIO_PLL1_CLK, AUDIO_PLL2_CLK,
	  VIDEO_PLL_CLK, SYSTEM_PLL1_133M_CLK,
	  OSC_27M_CLK, EXT_CLK_2, EXT_CLK_3}
	},
	{SAI6_CLK_ROOT, IP_CLOCK_SLICE, 16,
	 {OSC_25M_CLK, AUDIO_PLL1_CLK, AUDIO_PLL2_CLK,
	  VIDEO_PLL_CLK, SYSTEM_PLL1_133M_CLK,
	  OSC_27M_CLK, EXT_CLK_3, EXT_CLK_4}
	},
	{SPDIF1_CLK_ROOT, IP_CLOCK_SLICE, 17,
	 {OSC_25M_CLK, AUDIO_PLL1_CLK, AUDIO_PLL2_CLK,
	  VIDEO_PLL_CLK, SYSTEM_PLL1_133M_CLK,
	  OSC_27M_CLK, EXT_CLK_2, EXT_CLK_3}
	},
	{SPDIF2_CLK_ROOT, IP_CLOCK_SLICE, 18,
	 {OSC_25M_CLK, AUDIO_PLL1_CLK, AUDIO_PLL2_CLK,
	  VIDEO_PLL_CLK, SYSTEM_PLL1_133M_CLK,
	  OSC_27M_CLK, EXT_CLK_3, EXT_CLK_4}
	},
	{ENET_REF_CLK_ROOT, IP_CLOCK_SLICE, 19,
	 {OSC_25M_CLK, SYSTEM_PLL2_125M_CLK, SYSTEM_PLL2_50M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_160M_CLK,
	  AUDIO_PLL1_CLK, VIDEO_PLL_CLK, EXT_CLK_4}
	},
	{ENET_TIMER_CLK_ROOT, IP_CLOCK_SLICE, 20,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, AUDIO_PLL1_CLK,
	  EXT_CLK_1, EXT_CLK_2, EXT_CLK_3, EXT_CLK_4,
	  VIDEO_PLL_CLK}
	},
	{ENET_PHY_REF_CLK_ROOT, IP_CLOCK_SLICE, 21,
	 {OSC_25M_CLK, SYSTEM_PLL2_50M_CLK, SYSTEM_PLL2_125M_CLK,
	  SYSTEM_PLL2_200M_CLK, SYSTEM_PLL2_500M_CLK,
	  AUDIO_PLL1_CLK, VIDEO_PLL_CLK, AUDIO_PLL2_CLK}
	},
	{NAND_CLK_ROOT, IP_CLOCK_SLICE, 22,
	 {OSC_25M_CLK, SYSTEM_PLL2_500M_CLK, AUDIO_PLL1_CLK,
	  SYSTEM_PLL1_400M_CLK, AUDIO_PLL2_CLK, SYSTEM_PLL3_CLK,
	  SYSTEM_PLL2_250M_CLK, VIDEO_PLL_CLK}
	},
	{QSPI_CLK_ROOT, IP_CLOCK_SLICE, 23,
	 {OSC_25M_CLK, SYSTEM_PLL1_400M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_500M_CLK, AUDIO_PLL2_CLK,
	  SYSTEM_PLL1_266M_CLK, SYSTEM_PLL3_CLK, SYSTEM_PLL1_100M_CLK}
	},
	{USDHC1_CLK_ROOT, IP_CLOCK_SLICE, 24,
	 {OSC_25M_CLK, SYSTEM_PLL1_400M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_500M_CLK, AUDIO_PLL2_CLK,
	  SYSTEM_PLL1_266M_CLK, SYSTEM_PLL3_CLK, SYSTEM_PLL1_100M_CLK}
	},
	{USDHC2_CLK_ROOT, IP_CLOCK_SLICE, 25,
	 {OSC_25M_CLK, SYSTEM_PLL1_400M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_500M_CLK, AUDIO_PLL2_CLK,
	  SYSTEM_PLL1_266M_CLK, SYSTEM_PLL3_CLK, SYSTEM_PLL1_100M_CLK}
	},
	{I2C1_CLK_ROOT, IP_CLOCK_SLICE, 26,
	 {OSC_25M_CLK, SYSTEM_PLL1_160M_CLK, SYSTEM_PLL2_50M_CLK,
	  SYSTEM_PLL3_CLK, AUDIO_PLL1_CLK, VIDEO_PLL_CLK,
	  AUDIO_PLL2_CLK, SYSTEM_PLL1_133M_CLK}
	},
	{I2C2_CLK_ROOT, IP_CLOCK_SLICE, 27,
	 {OSC_25M_CLK, SYSTEM_PLL1_160M_CLK, SYSTEM_PLL2_50M_CLK,
	  SYSTEM_PLL3_CLK, AUDIO_PLL1_CLK, VIDEO_PLL_CLK,
	  AUDIO_PLL2_CLK, SYSTEM_PLL1_133M_CLK}
	},
	{I2C3_CLK_ROOT, IP_CLOCK_SLICE, 28,
	 {OSC_25M_CLK, SYSTEM_PLL1_160M_CLK, SYSTEM_PLL2_50M_CLK,
	  SYSTEM_PLL3_CLK, AUDIO_PLL1_CLK, VIDEO_PLL_CLK,
	  AUDIO_PLL2_CLK, SYSTEM_PLL1_133M_CLK}
	},
	{I2C4_CLK_ROOT, IP_CLOCK_SLICE, 29,
	 {OSC_25M_CLK, SYSTEM_PLL1_160M_CLK, SYSTEM_PLL2_50M_CLK,
	  SYSTEM_PLL3_CLK, AUDIO_PLL1_CLK, VIDEO_PLL_CLK,
	  AUDIO_PLL2_CLK, SYSTEM_PLL1_133M_CLK}
	},
	{UART1_CLK_ROOT, IP_CLOCK_SLICE, 30,
	 {OSC_25M_CLK, SYSTEM_PLL1_80M_CLK, SYSTEM_PLL2_200M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL3_CLK,
	  EXT_CLK_2, EXT_CLK_4, AUDIO_PLL2_CLK}
	},
	{UART2_CLK_ROOT, IP_CLOCK_SLICE, 31,
	 {OSC_25M_CLK, SYSTEM_PLL1_80M_CLK, SYSTEM_PLL2_200M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL3_CLK,
	  EXT_CLK_2, EXT_CLK_3, AUDIO_PLL2_CLK}
	},
	{UART3_CLK_ROOT, IP_CLOCK_SLICE, 32,
	 {OSC_25M_CLK, SYSTEM_PLL1_80M_CLK, SYSTEM_PLL2_200M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL3_CLK,
	  EXT_CLK_2, EXT_CLK_4, AUDIO_PLL2_CLK}
	},
	{UART4_CLK_ROOT, IP_CLOCK_SLICE, 33,
	 {OSC_25M_CLK, SYSTEM_PLL1_80M_CLK, SYSTEM_PLL2_200M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL3_CLK,
	  EXT_CLK_2, EXT_CLK_3, AUDIO_PLL2_CLK}
	},
	{USB_CORE_REF_CLK_ROOT, IP_CLOCK_SLICE, 34,
	 {OSC_25M_CLK, SYSTEM_PLL1_100M_CLK, SYSTEM_PLL1_40M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL2_200M_CLK,
	  EXT_CLK_2, EXT_CLK_3, AUDIO_PLL2_CLK}
	},
	{USB_PHY_REF_CLK_ROOT, IP_CLOCK_SLICE, 35,
	 {OSC_25M_CLK, SYSTEM_PLL1_100M_CLK, SYSTEM_PLL1_40M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL2_200M_CLK,
	  EXT_CLK_2, EXT_CLK_3, AUDIO_PLL2_CLK}
	},
	{GIC_CLK_ROOT, IP_CLOCK_SLICE, 36,
	 {OSC_25M_CLK, SYSTEM_PLL2_200M_CLK, SYSTEM_PLL1_40M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_800M_CLK,
	  EXT_CLK_2, EXT_CLK_4, AUDIO_PLL2_CLK}
	},
	{ECSPI1_CLK_ROOT, IP_CLOCK_SLICE, 37,
	 {OSC_25M_CLK, SYSTEM_PLL2_200M_CLK, SYSTEM_PLL1_40M_CLK,
	  SYSTEM_PLL1_160M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL2_250M_CLK, AUDIO_PLL2_CLK}
	},
	{ECSPI2_CLK_ROOT, IP_CLOCK_SLICE, 38,
	 {OSC_25M_CLK, SYSTEM_PLL2_200M_CLK, SYSTEM_PLL1_40M_CLK,
	  SYSTEM_PLL1_160M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL2_250M_CLK, AUDIO_PLL2_CLK}
	},
	{PWM1_CLK_ROOT, IP_CLOCK_SLICE, 39,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_160M_CLK,
	  SYSTEM_PLL1_40M_CLK, SYSTEM_PLL3_CLK, EXT_CLK_1,
	  SYSTEM_PLL1_80M_CLK, VIDEO_PLL_CLK}
	},
	{PWM2_CLK_ROOT, IP_CLOCK_SLICE, 40,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_160M_CLK,
	  SYSTEM_PLL1_40M_CLK, SYSTEM_PLL3_CLK, EXT_CLK_1,
	  SYSTEM_PLL1_80M_CLK, VIDEO_PLL_CLK}
	},
	{PWM3_CLK_ROOT, IP_CLOCK_SLICE, 41,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_160M_CLK,
	  SYSTEM_PLL1_40M_CLK, SYSTEM_PLL3_CLK, EXT_CLK_1,
	  SYSTEM_PLL1_80M_CLK, VIDEO_PLL_CLK}
	},
	{PWM4_CLK_ROOT, IP_CLOCK_SLICE, 42,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_160M_CLK,
	  SYSTEM_PLL1_40M_CLK, SYSTEM_PLL3_CLK, EXT_CLK_1,
	  SYSTEM_PLL1_80M_CLK, VIDEO_PLL_CLK}
	},
	{GPT1_CLK_ROOT, IP_CLOCK_SLICE, 43,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_400M_CLK,
	  SYSTEM_PLL1_40M_CLK, VIDEO_PLL_CLK,
	  SYSTEM_PLL1_80M_CLK, AUDIO_PLL1_CLK, EXT_CLK_1}
	},
	{GPT2_CLK_ROOT, IP_CLOCK_SLICE, 44,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_400M_CLK,
	  SYSTEM_PLL1_40M_CLK, VIDEO_PLL_CLK,
	  SYSTEM_PLL1_80M_CLK, AUDIO_PLL1_CLK, EXT_CLK_2}
	},
	{GPT3_CLK_ROOT, IP_CLOCK_SLICE, 45,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_400M_CLK,
	  SYSTEM_PLL1_40M_CLK, VIDEO_PLL_CLK,
	  SYSTEM_PLL1_80M_CLK, AUDIO_PLL1_CLK, EXT_CLK_3}
	},
	{GPT4_CLK_ROOT, IP_CLOCK_SLICE, 46,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_400M_CLK,
	  SYSTEM_PLL1_40M_CLK, VIDEO_PLL_CLK,
	  SYSTEM_PLL1_80M_CLK, AUDIO_PLL1_CLK, EXT_CLK_1}
	},
	{GPT5_CLK_ROOT, IP_CLOCK_SLICE, 47,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_400M_CLK,
	  SYSTEM_PLL1_40M_CLK, VIDEO_PLL_CLK,
	  SYSTEM_PLL1_80M_CLK, AUDIO_PLL1_CLK, EXT_CLK_2}
	},
	{GPT6_CLK_ROOT, IP_CLOCK_SLICE, 48,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_400M_CLK,
	  SYSTEM_PLL1_40M_CLK, VIDEO_PLL_CLK,
	  SYSTEM_PLL1_80M_CLK, AUDIO_PLL1_CLK, EXT_CLK_3}
	},
	{TRACE_CLK_ROOT, IP_CLOCK_SLICE, 49,
	 {OSC_25M_CLK, SYSTEM_PLL1_133M_CLK, SYSTEM_PLL1_160M_CLK,
	  VPU_PLL_CLK, SYSTEM_PLL2_125M_CLK,
	  SYSTEM_PLL3_CLK, EXT_CLK_1, EXT_CLK_3}
	},
	{WDOG_CLK_ROOT, IP_CLOCK_SLICE, 50,
	 {OSC_25M_CLK, SYSTEM_PLL1_133M_CLK, SYSTEM_PLL1_160M_CLK,
	  VPU_PLL_CLK, SYSTEM_PLL2_125M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL1_80M_CLK, SYSTEM_PLL2_166M_CLK}
	},
	{WRCLK_CLK_ROOT, IP_CLOCK_SLICE, 51,
	 {OSC_25M_CLK, SYSTEM_PLL1_40M_CLK, VPU_PLL_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL2_200M_CLK,
	  SYSTEM_PLL1_266M_CLK, SYSTEM_PLL2_500M_CLK, SYSTEM_PLL1_100M_CLK}
	},
	{IPP_DO_CLKO1, IP_CLOCK_SLICE, 52,
	 {OSC_25M_CLK, SYSTEM_PLL1_800M_CLK, OSC_27M_CLK,
	  SYSTEM_PLL1_200M_CLK, AUDIO_PLL2_CLK,
	  SYSTEM_PLL2_500M_CLK, VPU_PLL_CLK, SYSTEM_PLL1_80M_CLK}
	},
	{IPP_DO_CLKO2, IP_CLOCK_SLICE, 53,
	 {OSC_25M_CLK, SYSTEM_PLL2_200M_CLK, SYSTEM_PLL1_400M_CLK,
	  SYSTEM_PLL2_166M_CLK, SYSTEM_PLL3_CLK,
	  AUDIO_PLL1_CLK, VIDEO_PLL_CLK, OSC_32K_CLK}
	},
	{MIPI_DSI_CORE_CLK_ROOT, IP_CLOCK_SLICE, 54,
	 {OSC_25M_CLK, SYSTEM_PLL1_266M_CLK, SYSTEM_PLL2_250M_CLK,
	  SYSTEM_PLL1_800M_CLK, SYSTEM_PLL2_1000M_CLK,
	  SYSTEM_PLL3_CLK, AUDIO_PLL2_CLK, VIDEO_PLL_CLK}
	},
	{MIPI_DSI_PHY_REF_CLK_ROOT, IP_CLOCK_SLICE, 55,
	 {OSC_25M_CLK, SYSTEM_PLL2_125M_CLK, SYSTEM_PLL2_100M_CLK,
	  SYSTEM_PLL1_800M_CLK, SYSTEM_PLL2_1000M_CLK,
	  EXT_CLK_2, AUDIO_PLL2_CLK, VIDEO_PLL_CLK}
	},
	{MIPI_DSI_DBI_CLK_ROOT, IP_CLOCK_SLICE, 56,
	 {OSC_25M_CLK, SYSTEM_PLL1_266M_CLK, SYSTEM_PLL2_100M_CLK,
	  SYSTEM_PLL1_800M_CLK, SYSTEM_PLL2_1000M_CLK,
	  SYSTEM_PLL3_CLK, AUDIO_PLL2_CLK, VIDEO_PLL_CLK}
	},
	{OLD_MIPI_DSI_ESC_CLK_ROOT, IP_CLOCK_SLICE, 57,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_80M_CLK,
	  SYSTEM_PLL1_800M_CLK, SYSTEM_PLL2_1000M_CLK,
	  SYSTEM_PLL3_CLK, EXT_CLK_3, AUDIO_PLL2_CLK}
	},
	{MIPI_CSI1_CORE_CLK_ROOT, IP_CLOCK_SLICE, 58,
	 {OSC_25M_CLK, SYSTEM_PLL1_266M_CLK, SYSTEM_PLL2_250M_CLK,
	  SYSTEM_PLL1_800M_CLK, SYSTEM_PLL2_1000M_CLK,
	  SYSTEM_PLL3_CLK, AUDIO_PLL2_CLK, VIDEO_PLL_CLK}
	},
	{MIPI_CSI1_PHY_REF_CLK_ROOT, IP_CLOCK_SLICE, 59,
	 {OSC_25M_CLK, SYSTEM_PLL2_125M_CLK, SYSTEM_PLL2_100M_CLK,
	  SYSTEM_PLL1_800M_CLK, SYSTEM_PLL2_1000M_CLK,
	  EXT_CLK_2, AUDIO_PLL2_CLK, VIDEO_PLL_CLK}
	},
	{MIPI_CSI1_ESC_CLK_ROOT, IP_CLOCK_SLICE, 60,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_80M_CLK,
	  SYSTEM_PLL1_800M_CLK, SYSTEM_PLL2_1000M_CLK,
	  SYSTEM_PLL3_CLK, EXT_CLK_3, AUDIO_PLL2_CLK}
	},
	{MIPI_CSI2_CORE_CLK_ROOT, IP_CLOCK_SLICE, 61,
	 {OSC_25M_CLK, SYSTEM_PLL1_266M_CLK, SYSTEM_PLL2_250M_CLK,
	  SYSTEM_PLL1_800M_CLK, SYSTEM_PLL2_1000M_CLK,
	  SYSTEM_PLL3_CLK, AUDIO_PLL2_CLK, VIDEO_PLL_CLK}
	},
	{MIPI_CSI2_PHY_REF_CLK_ROOT, IP_CLOCK_SLICE, 62,
	 {OSC_25M_CLK, SYSTEM_PLL2_125M_CLK, SYSTEM_PLL2_100M_CLK,
	  SYSTEM_PLL1_800M_CLK, SYSTEM_PLL2_1000M_CLK,
	  EXT_CLK_2, AUDIO_PLL2_CLK, VIDEO_PLL_CLK}
	},
	{MIPI_CSI2_ESC_CLK_ROOT, IP_CLOCK_SLICE, 63,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_80M_CLK,
	  SYSTEM_PLL1_800M_CLK, SYSTEM_PLL2_1000M_CLK,
	  SYSTEM_PLL3_CLK, EXT_CLK_3, AUDIO_PLL2_CLK}
	},
	{PCIE2_CTRL_CLK_ROOT, IP_CLOCK_SLICE, 64,
	 {OSC_25M_CLK, SYSTEM_PLL2_250M_CLK, SYSTEM_PLL2_200M_CLK,
	  SYSTEM_PLL1_266M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_500M_CLK, SYSTEM_PLL2_333M_CLK, SYSTEM_PLL3_CLK}
	},
	{PCIE2_PHY_CLK_ROOT, IP_CLOCK_SLICE, 65,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL2_500M_CLK,
	  EXT_CLK_1, EXT_CLK_2, EXT_CLK_3,
	  EXT_CLK_4, SYSTEM_PLL1_400M_CLK}
	},
	{PCIE2_AUX_CLK_ROOT, IP_CLOCK_SLICE, 66,
	 {OSC_25M_CLK, SYSTEM_PLL2_200M_CLK, SYSTEM_PLL2_50M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL2_100M_CLK,
	  SYSTEM_PLL1_80M_CLK, SYSTEM_PLL1_160M_CLK, SYSTEM_PLL1_200M_CLK}
	},
	{ECSPI3_CLK_ROOT, IP_CLOCK_SLICE, 67,
	 {OSC_25M_CLK, SYSTEM_PLL2_200M_CLK, SYSTEM_PLL1_40M_CLK,
	  SYSTEM_PLL1_160M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL2_250M_CLK, AUDIO_PLL2_CLK}
	},
	{OLD_MIPI_DSI_ESC_RX_ROOT, IP_CLOCK_SLICE, 68,
	 {OSC_25M_CLK, SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_80M_CLK,
	  SYSTEM_PLL1_800M_CLK, SYSTEM_PLL2_1000M_CLK,
	  SYSTEM_PLL3_CLK, EXT_CLK_3, AUDIO_PLL2_CLK},
	},
	{DISPLAY_HDMI_CLK_ROOT, IP_CLOCK_SLICE, 69,
	 {OSC_25M_CLK, SYSTEM_PLL1_200M_CLK, SYSTEM_PLL2_200M_CLK,
	  VPU_PLL_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_1000M_CLK, SYSTEM_PLL3_CLK, EXT_CLK_4}
	},
	{DRAM_SEL_CFG, DRAM_SEL_CLOCK_SLICE, 0,
	 {DRAM_PLL1_CLK}
	},
	{CORE_SEL_CFG, CORE_SEL_CLOCK_SLICE, 0,
	 {DRAM_PLL1_CLK}
	},
};
#elif defined(CONFIG_IMX8MM) || defined(CONFIG_IMX8MN)
static struct clk_root_map root_array[] = {
	{NAND_USDHC_BUS_CLK_ROOT, BUS_CLOCK_SLICE, 2,
	 {OSC_24M_CLK, SYSTEM_PLL1_266M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL2_200M_CLK, SYSTEM_PLL1_133M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL2_250M_CLK, AUDIO_PLL1_CLK}
	},
	{NOC_CLK_ROOT, BUS_CLOCK_SLICE, 10,
	 {OSC_24M_CLK, SYSTEM_PLL1_800M_CLK, SYSTEM_PLL3_CLK,
	  SYSTEM_PLL2_1000M_CLK, SYSTEM_PLL2_500M_CLK,
	  AUDIO_PLL1_CLK, VIDEO_PLL_CLK, AUDIO_PLL2_CLK}
	},
#ifdef CONFIG_IMX8MM
	{NOC_APB_CLK_ROOT, BUS_CLOCK_SLICE, 11,
	 {OSC_24M_CLK, SYSTEM_PLL1_400M_CLK, SYSTEM_PLL3_CLK,
	  SYSTEM_PLL2_333M_CLK, SYSTEM_PLL2_200M_CLK,
	  SYSTEM_PLL1_800M_CLK, AUDIO_PLL1_CLK, VIDEO_PLL_CLK}
	},
#endif
	{DRAM_ALT_CLK_ROOT, IP_CLOCK_SLICE, 0,
	 {OSC_24M_CLK, SYSTEM_PLL1_800M_CLK, SYSTEM_PLL1_100M_CLK,
	  SYSTEM_PLL2_500M_CLK, SYSTEM_PLL2_1000M_CLK,
	  SYSTEM_PLL3_CLK, AUDIO_PLL1_CLK, SYSTEM_PLL1_266M_CLK}
	},
	{DRAM_APB_CLK_ROOT, IP_CLOCK_SLICE, 1,
	 {OSC_24M_CLK, SYSTEM_PLL2_200M_CLK, SYSTEM_PLL1_40M_CLK,
	  SYSTEM_PLL1_160M_CLK, SYSTEM_PLL1_800M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL2_250M_CLK, AUDIO_PLL2_CLK}
	},
	{UART1_CLK_ROOT, IP_CLOCK_SLICE, 30,
	 {OSC_24M_CLK, SYSTEM_PLL1_80M_CLK, SYSTEM_PLL2_200M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL3_CLK,
	  EXT_CLK_2, EXT_CLK_4, AUDIO_PLL2_CLK}
	},
	{UART2_CLK_ROOT, IP_CLOCK_SLICE, 31,
	 {OSC_24M_CLK, SYSTEM_PLL1_80M_CLK, SYSTEM_PLL2_200M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL3_CLK,
	  EXT_CLK_2, EXT_CLK_3, AUDIO_PLL2_CLK}
	},
	{UART3_CLK_ROOT, IP_CLOCK_SLICE, 32,
	 {OSC_24M_CLK, SYSTEM_PLL1_80M_CLK, SYSTEM_PLL2_200M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL3_CLK,
	  EXT_CLK_2, EXT_CLK_4, AUDIO_PLL2_CLK}
	},
	{UART4_CLK_ROOT, IP_CLOCK_SLICE, 33,
	 {OSC_24M_CLK, SYSTEM_PLL1_80M_CLK, SYSTEM_PLL2_200M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL3_CLK,
	  EXT_CLK_2, EXT_CLK_3, AUDIO_PLL2_CLK}
	},
	{GIC_CLK_ROOT, IP_CLOCK_SLICE, 36,
	 {OSC_24M_CLK, SYSTEM_PLL2_200M_CLK, SYSTEM_PLL1_40M_CLK,
	  SYSTEM_PLL2_100M_CLK, SYSTEM_PLL1_800M_CLK,
	  EXT_CLK_2, EXT_CLK_4, AUDIO_PLL2_CLK}
	},
	{WDOG_CLK_ROOT, IP_CLOCK_SLICE, 50,
	 {OSC_24M_CLK, SYSTEM_PLL1_133M_CLK, SYSTEM_PLL1_160M_CLK,
	  VPU_PLL_CLK, SYSTEM_PLL2_125M_CLK,
	  SYSTEM_PLL3_CLK, SYSTEM_PLL1_80M_CLK, SYSTEM_PLL2_166M_CLK}
	},
	{DRAM_SEL_CFG, DRAM_SEL_CLOCK_SLICE, 0,
	 {DRAM_PLL1_CLK}
	},
};
#endif

static int select(enum clk_root_index clock_id)
{
	int i, size;
	struct clk_root_map *p = root_array;

	size = ARRAY_SIZE(root_array);

	for (i = 0; i < size; i++, p++) {
		if (clock_id == p->entry)
			return i;
	}

	return -EINVAL;
}

static void __iomem *get_clk_root_target(enum clk_slice_type slice_type,
					 u32 slice_index)
{
	void __iomem *clk_root_target;

	switch (slice_type) {
	case CORE_CLOCK_SLICE:
		clk_root_target =
		(void __iomem *)&ccm_reg->core_root[slice_index];
		break;
	case BUS_CLOCK_SLICE:
		clk_root_target =
			(void __iomem *)&ccm_reg->bus_root[slice_index];
		break;
	case IP_CLOCK_SLICE:
		clk_root_target =
			(void __iomem *)&ccm_reg->ip_root[slice_index];
		break;
	case AHB_CLOCK_SLICE:
		clk_root_target =
			(void __iomem *)&ccm_reg->ahb_ipg_root[slice_index * 2];
		break;
	case IPG_CLOCK_SLICE:
		clk_root_target =
			(void __iomem *)&ccm_reg->ahb_ipg_root[slice_index * 2 + 1];
		break;
	case CORE_SEL_CLOCK_SLICE:
		clk_root_target = (void __iomem *)&ccm_reg->core_sel;
		break;
	case DRAM_SEL_CLOCK_SLICE:
		clk_root_target = (void __iomem *)&ccm_reg->dram_sel;
		break;
	default:
		return NULL;
	}

	return clk_root_target;
}

int clock_get_target_val(enum clk_root_index clock_id, u32 *val)
{
	int root_entry;
	struct clk_root_map *p;
	void __iomem *clk_root_target;

	if (clock_id >= CLK_ROOT_MAX)
		return -EINVAL;

	root_entry = select(clock_id);
	if (root_entry < 0)
		return -EINVAL;

	p = &root_array[root_entry];
	clk_root_target = get_clk_root_target(p->slice_type, p->slice_index);
	if (!clk_root_target)
		return -EINVAL;

	*val = readl(clk_root_target);

	return 0;
}

int clock_set_target_val(enum clk_root_index clock_id, u32 val)
{
	int root_entry;
	struct clk_root_map *p;
	void __iomem *clk_root_target;

	if (clock_id >= CLK_ROOT_MAX)
		return -EINVAL;

	root_entry = select(clock_id);
	if (root_entry < 0)
		return -EINVAL;

	p = &root_array[root_entry];
	clk_root_target = get_clk_root_target(p->slice_type, p->slice_index);
	if (!clk_root_target)
		return -EINVAL;

	writel(val, clk_root_target);

	return 0;
}

int clock_root_enabled(enum clk_root_index clock_id)
{
	void __iomem *clk_root_target;
	u32 slice_index, slice_type;
	u32 val;
	int root_entry;

	if (clock_id >= CLK_ROOT_MAX)
		return -EINVAL;

	root_entry = select(clock_id);
	if (root_entry < 0)
		return -EINVAL;

	slice_type = root_array[root_entry].slice_type;
	slice_index = root_array[root_entry].slice_index;

	if ((slice_type == IPG_CLOCK_SLICE) ||
	    (slice_type == DRAM_SEL_CLOCK_SLICE) ||
	    (slice_type == CORE_SEL_CLOCK_SLICE)) {
		/*
		 * Not supported, from CCM doc
		 * TODO
		 */
		return 0;
	}

	clk_root_target = get_clk_root_target(slice_type, slice_index);
	if (!clk_root_target)
		return -EINVAL;

	val = readl(clk_root_target);

	return (val & CLK_ROOT_ON) ? 1 : 0;
}

/* CCGR CLK gate operation */
int clock_enable(enum clk_ccgr_index index, bool enable)
{
	void __iomem *ccgr;

	if (index >= CCGR_MAX)
		return -EINVAL;

	if (enable)
		ccgr = (void __iomem *)&ccm_reg->ccgr_array[index].ccgr_set;
	else
		ccgr = (void __iomem *)&ccm_reg->ccgr_array[index].ccgr_clr;

	writel(CCGR_CLK_ON_MASK, ccgr);

	return 0;
}

int clock_get_prediv(enum clk_root_index clock_id, enum root_pre_div *pre_div)
{
	u32 val;
	int root_entry;
	struct clk_root_map *p;
	void __iomem *clk_root_target;

	if (clock_id >= CLK_ROOT_MAX)
		return -EINVAL;

	root_entry = select(clock_id);
	if (root_entry < 0)
		return -EINVAL;

	p = &root_array[root_entry];

	if ((p->slice_type == CORE_CLOCK_SLICE) ||
	    (p->slice_type == IPG_CLOCK_SLICE) ||
	    (p->slice_type == CORE_SEL_CLOCK_SLICE) ||
	    (p->slice_type == DRAM_SEL_CLOCK_SLICE)) {
		*pre_div = 0;
		return 0;
	}

	clk_root_target = get_clk_root_target(p->slice_type, p->slice_index);
	if (!clk_root_target)
		return -EINVAL;

	val = readl(clk_root_target);
	val &= CLK_ROOT_PRE_DIV_MASK;
	val >>= CLK_ROOT_PRE_DIV_SHIFT;

	*pre_div = val;

	return 0;
}

int clock_get_postdiv(enum clk_root_index clock_id,
		      enum root_post_div *post_div)
{
	u32 val, mask;
	int root_entry;
	struct clk_root_map *p;
	void __iomem *clk_root_target;

	if (clock_id >= CLK_ROOT_MAX)
		return -EINVAL;

	root_entry = select(clock_id);
	if (root_entry < 0)
		return -EINVAL;

	p = &root_array[root_entry];

	if ((p->slice_type == CORE_SEL_CLOCK_SLICE) ||
	    (p->slice_type == DRAM_SEL_CLOCK_SLICE)) {
		*post_div = 0;
		return 0;
	}

	clk_root_target = get_clk_root_target(p->slice_type, p->slice_index);
	if (!clk_root_target)
		return -EINVAL;

	if (p->slice_type == IPG_CLOCK_SLICE)
		mask = CLK_ROOT_IPG_POST_DIV_MASK;
	else if (p->slice_type == CORE_CLOCK_SLICE)
		mask = CLK_ROOT_CORE_POST_DIV_MASK;
	else
		mask = CLK_ROOT_POST_DIV_MASK;

	val = readl(clk_root_target);
	val &= mask;
	val >>= CLK_ROOT_POST_DIV_SHIFT;

	*post_div = val;

	return 0;
}

int clock_get_src(enum clk_root_index clock_id, enum clk_root_src *p_clock_src)
{
	u32 val;
	int root_entry;
	struct clk_root_map *p;
	void __iomem *clk_root_target;

	if (clock_id >= CLK_ROOT_MAX)
		return -EINVAL;

	root_entry = select(clock_id);
	if (root_entry < 0)
		return -EINVAL;

	p = &root_array[root_entry];

	clk_root_target = get_clk_root_target(p->slice_type, p->slice_index);
	if (!clk_root_target)
		return -EINVAL;

	val = readl(clk_root_target);
	val &= CLK_ROOT_SRC_MUX_MASK;
	val >>= CLK_ROOT_SRC_MUX_SHIFT;

	*p_clock_src = p->src_mux[val];

	return 0;
}
