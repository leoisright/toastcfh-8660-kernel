/*
 * sh7372 processor support - INTC hardware block
 *
 * Copyright (C) 2010  Magnus Damm
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/sh_intc.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

enum {
	UNUSED_INTCA = 0,
	ENABLED,
	DISABLED,

	/* interrupt sources INTCA */
	IRQ0A, IRQ1A, IRQ2A, IRQ3A, IRQ4A, IRQ5A, IRQ6A, IRQ7A,
	IRQ8A, IRQ9A, IRQ10A, IRQ11A, IRQ12A, IRQ13A, IRQ14A, IRQ15A,
	IRQ16A, IRQ17A, IRQ18A, IRQ19A, IRQ20A, IRQ21A, IRQ22A, IRQ23A,
	IRQ24A, IRQ25A, IRQ26A, IRQ27A, IRQ28A, IRQ29A, IRQ30A, IRQ31A,
	DIRC,
	CRYPT_STD,
	IIC1_ALI1, IIC1_TACKI1, IIC1_WAITI1, IIC1_DTEI1,
	AP_ARM_IRQPMU, AP_ARM_COMMTX, AP_ARM_COMMRX,
	MFI_MFIM, MFI_MFIS,
	BBIF1, BBIF2,
	USBHSDMAC0_USHDMI,
	_3DG_SGX540,
	CMT1_CMT10, CMT1_CMT11, CMT1_CMT12, CMT1_CMT13, CMT2, CMT3,
	KEYSC_KEY,
	SCIFA0, SCIFA1, SCIFA2, SCIFA3,
	MSIOF2, MSIOF1,
	SCIFA4, SCIFA5, SCIFB,
	FLCTL_FLSTEI, FLCTL_FLTENDI, FLCTL_FLTREQ0I, FLCTL_FLTREQ1I,
	SDHI0,
	SDHI1,
	IRREM,
	IRDA,
	TPU0,
	TTI20,
	DDM,
	SDHI2,
	RWDT0,
	DMAC1_1_DEI0, DMAC1_1_DEI1, DMAC1_1_DEI2, DMAC1_1_DEI3,
	DMAC1_2_DEI4, DMAC1_2_DEI5, DMAC1_2_DADERR,
	DMAC2_1_DEI0, DMAC2_1_DEI1, DMAC2_1_DEI2, DMAC2_1_DEI3,
	DMAC2_2_DEI4, DMAC2_2_DEI5, DMAC2_2_DADERR,
	DMAC3_1_DEI0, DMAC3_1_DEI1, DMAC3_1_DEI2, DMAC3_1_DEI3,
	DMAC3_2_DEI4, DMAC3_2_DEI5, DMAC3_2_DADERR,
	SHWYSTAT_RT, SHWYSTAT_HS, SHWYSTAT_COM,
	HDMI,
	SPU2_SPU0, SPU2_SPU1,
	FSI, FMSI,
	MIPI_HSI,
	IPMMU_IPMMUD,
	CEC_1, CEC_2,
	AP_ARM_CTIIRQ, AP_ARM_DMAEXTERRIRQ, AP_ARM_DMAIRQ, AP_ARM_DMASIRQ,
	MFIS2,
	CPORTR2S,
	CMT14, CMT15,
	MMC_MMC_ERR, MMC_MMC_NOR,
	IIC4_ALI4, IIC4_TACKI4, IIC4_WAITI4, IIC4_DTEI4,
	IIC3_ALI3, IIC3_TACKI3, IIC3_WAITI3, IIC3_DTEI3,
	USB0_USB0I1, USB0_USB0I0,
	USB1_USB1I1, USB1_USB1I0,
	USBHSDMAC1_USHDMI,

	/* interrupt groups INTCA */
	DMAC1_1, DMAC1_2, DMAC2_1, DMAC2_2, DMAC3_1, DMAC3_2, SHWYSTAT,
	AP_ARM1, AP_ARM2, SPU2, FLCTL, IIC1
};

static struct intc_vect intca_vectors[] __initdata = {
	INTC_VECT(IRQ0A, 0x0200), INTC_VECT(IRQ1A, 0x0220),
	INTC_VECT(IRQ2A, 0x0240), INTC_VECT(IRQ3A, 0x0260),
	INTC_VECT(IRQ4A, 0x0280), INTC_VECT(IRQ5A, 0x02a0),
	INTC_VECT(IRQ6A, 0x02c0), INTC_VECT(IRQ7A, 0x02e0),
	INTC_VECT(IRQ8A, 0x0300), INTC_VECT(IRQ9A, 0x0320),
	INTC_VECT(IRQ10A, 0x0340), INTC_VECT(IRQ11A, 0x0360),
	INTC_VECT(IRQ12A, 0x0380), INTC_VECT(IRQ13A, 0x03a0),
	INTC_VECT(IRQ14A, 0x03c0), INTC_VECT(IRQ15A, 0x03e0),
	INTC_VECT(IRQ16A, 0x3200), INTC_VECT(IRQ17A, 0x3220),
	INTC_VECT(IRQ18A, 0x3240), INTC_VECT(IRQ19A, 0x3260),
	INTC_VECT(IRQ20A, 0x3280), INTC_VECT(IRQ31A, 0x32a0),
	INTC_VECT(IRQ22A, 0x32c0), INTC_VECT(IRQ23A, 0x32e0),
	INTC_VECT(IRQ24A, 0x3300), INTC_VECT(IRQ25A, 0x3320),
	INTC_VECT(IRQ26A, 0x3340), INTC_VECT(IRQ27A, 0x3360),
	INTC_VECT(IRQ28A, 0x3380), INTC_VECT(IRQ29A, 0x33a0),
	INTC_VECT(IRQ30A, 0x33c0), INTC_VECT(IRQ31A, 0x33e0),
	INTC_VECT(DIRC, 0x0560),
	INTC_VECT(CRYPT_STD, 0x0700),
	INTC_VECT(IIC1_ALI1, 0x0780), INTC_VECT(IIC1_TACKI1, 0x07a0),
	INTC_VECT(IIC1_WAITI1, 0x07c0), INTC_VECT(IIC1_DTEI1, 0x07e0),
	INTC_VECT(AP_ARM_IRQPMU, 0x0800), INTC_VECT(AP_ARM_COMMTX, 0x0840),
	INTC_VECT(AP_ARM_COMMRX, 0x0860),
	INTC_VECT(MFI_MFIM, 0x0900), INTC_VECT(MFI_MFIS, 0x0920),
	INTC_VECT(BBIF1, 0x0940), INTC_VECT(BBIF2, 0x0960),
	INTC_VECT(USBHSDMAC0_USHDMI, 0x0a00),
	INTC_VECT(_3DG_SGX540, 0x0a60),
	INTC_VECT(CMT1_CMT10, 0x0b00), INTC_VECT(CMT1_CMT11, 0x0b20),
	INTC_VECT(CMT1_CMT12, 0x0b40), INTC_VECT(CMT1_CMT13, 0x0b60),
	INTC_VECT(CMT2, 0x0b80), INTC_VECT(CMT3, 0x0ba0),
	INTC_VECT(KEYSC_KEY, 0x0be0),
	INTC_VECT(SCIFA0, 0x0c00), INTC_VECT(SCIFA1, 0x0c20),
	INTC_VECT(SCIFA2, 0x0c40), INTC_VECT(SCIFA3, 0x0c60),
	INTC_VECT(MSIOF2, 0x0c80), INTC_VECT(MSIOF1, 0x0d00),
	INTC_VECT(SCIFA4, 0x0d20), INTC_VECT(SCIFA5, 0x0d40),
	INTC_VECT(SCIFB, 0x0d60),
	INTC_VECT(FLCTL_FLSTEI, 0x0d80), INTC_VECT(FLCTL_FLTENDI, 0x0da0),
	INTC_VECT(FLCTL_FLTREQ0I, 0x0dc0), INTC_VECT(FLCTL_FLTREQ1I, 0x0de0),
	INTC_VECT(SDHI0, 0x0e00), INTC_VECT(SDHI0, 0x0e20),
	INTC_VECT(SDHI0, 0x0e40), INTC_VECT(SDHI0, 0x0e60),
	INTC_VECT(SDHI1, 0x0e80), INTC_VECT(SDHI1, 0x0ea0),
	INTC_VECT(SDHI1, 0x0ec0),
	INTC_VECT(IRREM, 0x0f60),
	INTC_VECT(IRDA, 0x0480),
	INTC_VECT(TPU0, 0x04a0),
	INTC_VECT(TTI20, 0x1100),
	INTC_VECT(DDM, 0x1140),
	INTC_VECT(SDHI2, 0x1200), INTC_VECT(SDHI2, 0x1220),
	INTC_VECT(SDHI2, 0x1240), INTC_VECT(SDHI2, 0x1260),
	INTC_VECT(RWDT0, 0x1280),
	INTC_VECT(DMAC1_1_DEI0, 0x2000), INTC_VECT(DMAC1_1_DEI1, 0x2020),
	INTC_VECT(DMAC1_1_DEI2, 0x2040), INTC_VECT(DMAC1_1_DEI3, 0x2060),
	INTC_VECT(DMAC1_2_DEI4, 0x2080), INTC_VECT(DMAC1_2_DEI5, 0x20a0),
	INTC_VECT(DMAC1_2_DADERR, 0x20c0),
	INTC_VECT(DMAC2_1_DEI0, 0x2100), INTC_VECT(DMAC2_1_DEI1, 0x2120),
	INTC_VECT(DMAC2_1_DEI2, 0x2140), INTC_VECT(DMAC2_1_DEI3, 0x2160),
	INTC_VECT(DMAC2_2_DEI4, 0x2180), INTC_VECT(DMAC2_2_DEI5, 0x21a0),
	INTC_VECT(DMAC2_2_DADERR, 0x21c0),
	INTC_VECT(DMAC3_1_DEI0, 0x2200), INTC_VECT(DMAC3_1_DEI1, 0x2220),
	INTC_VECT(DMAC3_1_DEI2, 0x2240), INTC_VECT(DMAC3_1_DEI3, 0x2260),
	INTC_VECT(DMAC3_2_DEI4, 0x2280), INTC_VECT(DMAC3_2_DEI5, 0x22a0),
	INTC_VECT(DMAC3_2_DADERR, 0x22c0),
	INTC_VECT(SHWYSTAT_RT, 0x1300), INTC_VECT(SHWYSTAT_HS, 0x1320),
	INTC_VECT(SHWYSTAT_COM, 0x1340),
	INTC_VECT(HDMI, 0x17e0),
	INTC_VECT(SPU2_SPU0, 0x1800), INTC_VECT(SPU2_SPU1, 0x1820),
	INTC_VECT(FSI, 0x1840),
	INTC_VECT(FMSI, 0x1860),
	INTC_VECT(MIPI_HSI, 0x18e0),
	INTC_VECT(IPMMU_IPMMUD, 0x1920),
	INTC_VECT(CEC_1, 0x1940), INTC_VECT(CEC_2, 0x1960),
	INTC_VECT(AP_ARM_CTIIRQ, 0x1980),
	INTC_VECT(AP_ARM_DMAEXTERRIRQ, 0x19a0),
	INTC_VECT(AP_ARM_DMAIRQ, 0x19c0),
	INTC_VECT(AP_ARM_DMASIRQ, 0x19e0),
	INTC_VECT(MFIS2, 0x1a00),
	INTC_VECT(CPORTR2S, 0x1a20),
	INTC_VECT(CMT14, 0x1a40), INTC_VECT(CMT15, 0x1a60),
	INTC_VECT(MMC_MMC_ERR, 0x1ac0), INTC_VECT(MMC_MMC_NOR, 0x1ae0),
	INTC_VECT(IIC4_ALI4, 0x1b00), INTC_VECT(IIC4_TACKI4, 0x1b20),
	INTC_VECT(IIC4_WAITI4, 0x1b40), INTC_VECT(IIC4_DTEI4, 0x1b60),
	INTC_VECT(IIC3_ALI3, 0x1b80), INTC_VECT(IIC3_TACKI3, 0x1ba0),
	INTC_VECT(IIC3_WAITI3, 0x1bc0), INTC_VECT(IIC3_DTEI3, 0x1be0),
	INTC_VECT(USB0_USB0I1, 0x1c80), INTC_VECT(USB0_USB0I0, 0x1ca0),
	INTC_VECT(USB1_USB1I1, 0x1cc0), INTC_VECT(USB1_USB1I0, 0x1ce0),
	INTC_VECT(USBHSDMAC1_USHDMI, 0x1d00),
};

static struct intc_group intca_groups[] __initdata = {
	INTC_GROUP(DMAC1_1, DMAC1_1_DEI0,
		   DMAC1_1_DEI1, DMAC1_1_DEI2, DMAC1_1_DEI3),
	INTC_GROUP(DMAC1_2, DMAC1_2_DEI4,
		   DMAC1_2_DEI5, DMAC1_2_DADERR),
	INTC_GROUP(DMAC2_1, DMAC2_1_DEI0,
		   DMAC2_1_DEI1, DMAC2_1_DEI2, DMAC2_1_DEI3),
	INTC_GROUP(DMAC2_2, DMAC2_2_DEI4,
		   DMAC2_2_DEI5, DMAC2_2_DADERR),
	INTC_GROUP(DMAC3_1, DMAC3_1_DEI0,
		   DMAC3_1_DEI1, DMAC3_1_DEI2, DMAC3_1_DEI3),
	INTC_GROUP(DMAC3_2, DMAC3_2_DEI4,
		   DMAC3_2_DEI5, DMAC3_2_DADERR),
	INTC_GROUP(AP_ARM1, AP_ARM_IRQPMU, AP_ARM_COMMTX, AP_ARM_COMMRX),
	INTC_GROUP(AP_ARM2, AP_ARM_CTIIRQ, AP_ARM_DMAEXTERRIRQ,
		   AP_ARM_DMAIRQ, AP_ARM_DMASIRQ),
	INTC_GROUP(SPU2, SPU2_SPU0, SPU2_SPU1),
	INTC_GROUP(FLCTL, FLCTL_FLSTEI, FLCTL_FLTENDI,
		   FLCTL_FLTREQ0I, FLCTL_FLTREQ1I),
	INTC_GROUP(IIC1, IIC1_ALI1, IIC1_TACKI1, IIC1_WAITI1, IIC1_DTEI1),
	INTC_GROUP(SHWYSTAT, SHWYSTAT_RT, SHWYSTAT_HS, SHWYSTAT_COM),
};

static struct intc_mask_reg intca_mask_registers[] __initdata = {
	{ 0xe6900040, 0xe6900060, 8, /* INTMSK00A / INTMSKCLR00A */
	  { IRQ0A, IRQ1A, IRQ2A, IRQ3A, IRQ4A, IRQ5A, IRQ6A, IRQ7A } },
	{ 0xe6900044, 0xe6900064, 8, /* INTMSK10A / INTMSKCLR10A */
	  { IRQ8A, IRQ9A, IRQ10A, IRQ11A, IRQ12A, IRQ13A, IRQ14A, IRQ15A } },
	{ 0xe6900048, 0xe6900068, 8, /* INTMSK20A / INTMSKCLR20A */
	  { IRQ16A, IRQ17A, IRQ18A, IRQ19A, IRQ20A, IRQ21A, IRQ22A, IRQ23A } },
	{ 0xe690004c, 0xe690006c, 8, /* INTMSK30A / INTMSKCLR30A */
	  { IRQ24A, IRQ25A, IRQ26A, IRQ27A, IRQ28A, IRQ29A, IRQ30A, IRQ31A } },

	{ 0xe6940080, 0xe69400c0, 8, /* IMR0A / IMCR0A */
	  { DMAC2_1_DEI3, DMAC2_1_DEI2, DMAC2_1_DEI1, DMAC2_1_DEI0,
	    AP_ARM_IRQPMU, 0, AP_ARM_COMMTX, AP_ARM_COMMRX } },
	{ 0xe6940084, 0xe69400c4, 8, /* IMR1A / IMCR1A */
	  { 0, CRYPT_STD, DIRC, 0,
	    DMAC1_1_DEI3, DMAC1_1_DEI2, DMAC1_1_DEI1, DMAC1_1_DEI0 } },
	{ 0xe6940088, 0xe69400c8, 8, /* IMR2A / IMCR2A */
	  { 0, 0, 0, 0,
	    BBIF1, BBIF2, MFI_MFIS, MFI_MFIM } },
	{ 0xe694008c, 0xe69400cc, 8, /* IMR3A / IMCR3A */
	  { DMAC3_1_DEI3, DMAC3_1_DEI2, DMAC3_1_DEI1, DMAC3_1_DEI0,
	    DMAC3_2_DADERR, DMAC3_2_DEI5, DMAC3_2_DEI4, IRDA } },
	{ 0xe6940090, 0xe69400d0, 8, /* IMR4A / IMCR4A */
	  { DDM, 0, 0, 0,
	    0, 0, 0, 0 } },
	{ 0xe6940094, 0xe69400d4, 8, /* IMR5A / IMCR5A */
	  { KEYSC_KEY, DMAC1_2_DADERR, DMAC1_2_DEI5, DMAC1_2_DEI4,
	    SCIFA3, SCIFA2, SCIFA1, SCIFA0 } },
	{ 0xe6940098, 0xe69400d8, 8, /* IMR6A / IMCR6A */
	  { SCIFB, SCIFA5, SCIFA4, MSIOF1,
	    0, 0, MSIOF2, 0 } },
	{ 0xe694009c, 0xe69400dc, 8, /* IMR7A / IMCR7A */
	  { DISABLED, DISABLED, ENABLED, ENABLED,
	    FLCTL_FLTREQ1I, FLCTL_FLTREQ0I, FLCTL_FLTENDI, FLCTL_FLSTEI } },
	{ 0xe69400a0, 0xe69400e0, 8, /* IMR8A / IMCR8A */
	  { 0, DISABLED, ENABLED, ENABLED,
	    TTI20, USBHSDMAC0_USHDMI, 0, 0 } },
	{ 0xe69400a4, 0xe69400e4, 8, /* IMR9A / IMCR9A */
	  { CMT1_CMT13, CMT1_CMT12, CMT1_CMT11, CMT1_CMT10,
	    CMT2, 0, 0, _3DG_SGX540 } },
	{ 0xe69400a8, 0xe69400e8, 8, /* IMR10A / IMCR10A */
	  { 0, DMAC2_2_DADERR, DMAC2_2_DEI5, DMAC2_2_DEI4,
	    0, 0, 0, 0 } },
	{ 0xe69400ac, 0xe69400ec, 8, /* IMR11A / IMCR11A */
	  { IIC1_DTEI1, IIC1_WAITI1, IIC1_TACKI1, IIC1_ALI1,
	    0, 0, IRREM, 0 } },
	{ 0xe69400b0, 0xe69400f0, 8, /* IMR12A / IMCR12A */
	  { 0, 0, TPU0, 0,
	    0, 0, 0, 0 } },
	{ 0xe69400b4, 0xe69400f4, 8, /* IMR13A / IMCR13A */
	  { DISABLED, DISABLED, ENABLED, ENABLED,
	    0, CMT3, 0, RWDT0 } },
	{ 0xe6950080, 0xe69500c0, 8, /* IMR0A3 / IMCR0A3 */
	  { SHWYSTAT_RT, SHWYSTAT_HS, SHWYSTAT_COM, 0,
	    0, 0, 0, 0 } },
	{ 0xe6950090, 0xe69500d0, 8, /* IMR4A3 / IMCR4A3 */
	  { 0, 0, 0, 0,
	    0, 0, 0, HDMI } },
	{ 0xe6950094, 0xe69500d4, 8, /* IMR5A3 / IMCR5A3 */
	  { SPU2_SPU0, SPU2_SPU1, FSI, FMSI,
	    0, 0, 0, MIPI_HSI } },
	{ 0xe6950098, 0xe69500d8, 8, /* IMR6A3 / IMCR6A3 */
	  { 0, IPMMU_IPMMUD, CEC_1, CEC_2,
	    AP_ARM_CTIIRQ, AP_ARM_DMAEXTERRIRQ,
	    AP_ARM_DMAIRQ, AP_ARM_DMASIRQ } },
	{ 0xe695009c, 0xe69500dc, 8, /* IMR7A3 / IMCR7A3 */
	  { MFIS2, CPORTR2S, CMT14, CMT15,
	    0, 0, MMC_MMC_ERR, MMC_MMC_NOR } },
	{ 0xe69500a0, 0xe69500e0, 8, /* IMR8A3 / IMCR8A3 */
	  { IIC4_ALI4, IIC4_TACKI4, IIC4_WAITI4, IIC4_DTEI4,
	    IIC3_ALI3, IIC3_TACKI3, IIC3_WAITI3, IIC3_DTEI3 } },
	{ 0xe69500a4, 0xe69500e4, 8, /* IMR9A3 / IMCR9A3 */
	  { 0, 0, 0, 0,
	    USB0_USB0I1, USB0_USB0I0, USB1_USB1I1, USB1_USB1I0 } },
	{ 0xe69500a8, 0xe69500e8, 8, /* IMR10A3 / IMCR10A3 */
	  { USBHSDMAC1_USHDMI, 0, 0, 0,
	    0, 0, 0, 0 } },
};

static struct intc_prio_reg intca_prio_registers[] __initdata = {
	{ 0xe6900010, 0, 32, 4, /* INTPRI00A */
	  { IRQ0A, IRQ1A, IRQ2A, IRQ3A, IRQ4A, IRQ5A, IRQ6A, IRQ7A } },
	{ 0xe6900014, 0, 32, 4, /* INTPRI10A */
	  { IRQ8A, IRQ9A, IRQ10A, IRQ11A, IRQ12A, IRQ13A, IRQ14A, IRQ15A } },
	{ 0xe6900018, 0, 32, 4, /* INTPRI20A */
	  { IRQ16A, IRQ17A, IRQ18A, IRQ19A, IRQ20A, IRQ21A, IRQ22A, IRQ23A } },
	{ 0xe690001c, 0, 32, 4, /* INTPRI30A */
	  { IRQ24A, IRQ25A, IRQ26A, IRQ27A, IRQ28A, IRQ29A, IRQ30A, IRQ31A } },

	{ 0xe6940000, 0, 16, 4, /* IPRAA */ { DMAC3_1, DMAC3_2, CMT2, 0 } },
	{ 0xe6940004, 0, 16, 4, /* IPRBA */ { IRDA, 0, BBIF1, BBIF2 } },
	{ 0xe6940008, 0, 16, 4, /* IPRCA */ { 0, CRYPT_STD,
					      CMT1_CMT11, AP_ARM1 } },
	{ 0xe694000c, 0, 16, 4, /* IPRDA */ { 0, 0,
					      CMT1_CMT12, 0 } },
	{ 0xe6940010, 0, 16, 4, /* IPREA */ { DMAC1_1, MFI_MFIS,
					      MFI_MFIM, 0 } },
	{ 0xe6940014, 0, 16, 4, /* IPRFA */ { KEYSC_KEY, DMAC1_2,
					      _3DG_SGX540, CMT1_CMT10 } },
	{ 0xe6940018, 0, 16, 4, /* IPRGA */ { SCIFA0, SCIFA1,
					      SCIFA2, SCIFA3 } },
	{ 0xe694001c, 0, 16, 4, /* IPRGH */ { MSIOF2, USBHSDMAC0_USHDMI,
					      FLCTL, SDHI0 } },
	{ 0xe6940020, 0, 16, 4, /* IPRIA */ { MSIOF1, SCIFA4,
					      0/* MSU */, IIC1 } },
	{ 0xe6940024, 0, 16, 4, /* IPRJA */ { DMAC2_1, DMAC2_2,
					      0/* MSUG */, TTI20 } },
	{ 0xe6940028, 0, 16, 4, /* IPRKA */ { 0, CMT1_CMT13, IRREM, SDHI1 } },
	{ 0xe694002c, 0, 16, 4, /* IPRLA */ { TPU0, 0, 0, 0 } },
	{ 0xe6940030, 0, 16, 4, /* IPRMA */ { 0, CMT3, 0, RWDT0 } },
	{ 0xe6940034, 0, 16, 4, /* IPRNA */ { SCIFB, SCIFA5, 0, DDM } },
	{ 0xe6940038, 0, 16, 4, /* IPROA */ { 0, 0, DIRC, SDHI2 } },
	{ 0xe6950000, 0, 16, 4, /* IPRAA3 */ { SHWYSTAT, 0, 0, 0 } },
	{ 0xe6950024, 0, 16, 4, /* IPRJA3 */ { 0, 0, 0, HDMI } },
	{ 0xe6950028, 0, 16, 4, /* IPRKA3 */ { SPU2, 0, FSI, FMSI } },
	{ 0xe695002c, 0, 16, 4, /* IPRLA3 */ { 0, 0, 0, MIPI_HSI } },
	{ 0xe6950030, 0, 16, 4, /* IPRMA3 */ { IPMMU_IPMMUD, 0,
					       CEC_1, CEC_2 } },
	{ 0xe6950034, 0, 16, 4, /* IPRNA3 */ { AP_ARM2, 0, 0, 0 } },
	{ 0xe6950038, 0, 16, 4, /* IPROA3 */ { MFIS2, CPORTR2S,
					       CMT14, CMT15 } },
	{ 0xe694003c, 0, 16, 4, /* IPRPA3 */ { 0, 0,
					       MMC_MMC_ERR, MMC_MMC_NOR } },
	{ 0xe6940040, 0, 16, 4, /* IPRQA3 */ { IIC4_ALI4, IIC4_TACKI4,
					       IIC4_WAITI4, IIC4_DTEI4 } },
	{ 0xe6940044, 0, 16, 4, /* IPRRA3 */ { IIC3_ALI3, IIC3_TACKI3,
					       IIC3_WAITI3, IIC3_DTEI3 } },
	{ 0xe6940048, 0, 16, 4, /* IPRSA3 */ { 0/*ERI*/, 0/*RXI*/,
					       0/*TXI*/, 0/*TEI*/} },
	{ 0xe694004c, 0, 16, 4, /* IPRTA3 */ { USB0_USB0I1, USB0_USB0I0,
					       USB1_USB1I1, USB1_USB1I0 } },
	{ 0xe6940050, 0, 16, 4, /* IPRUA3 */ { USBHSDMAC1_USHDMI, 0, 0, 0 } },
};

static struct intc_sense_reg intca_sense_registers[] __initdata = {
	{ 0xe6900000, 32, 4, /* ICR1A */
	  { IRQ0A, IRQ1A, IRQ2A, IRQ3A, IRQ4A, IRQ5A, IRQ6A, IRQ7A } },
	{ 0xe6900004, 32, 4, /* ICR2A */
	  { IRQ8A, IRQ9A, IRQ10A, IRQ11A, IRQ12A, IRQ13A, IRQ14A, IRQ15A } },
	{ 0xe6900008, 32, 4, /* ICR3A */
	  { IRQ16A, IRQ17A, IRQ18A, IRQ19A, IRQ20A, IRQ21A, IRQ22A, IRQ23A } },
	{ 0xe690000c, 32, 4, /* ICR4A */
	  { IRQ24A, IRQ25A, IRQ26A, IRQ27A, IRQ28A, IRQ29A, IRQ30A, IRQ31A } },
};

static struct intc_mask_reg intca_ack_registers[] __initdata = {
	{ 0xe6900020, 0, 8, /* INTREQ00A */
	  { IRQ0A, IRQ1A, IRQ2A, IRQ3A, IRQ4A, IRQ5A, IRQ6A, IRQ7A } },
	{ 0xe6900024, 0, 8, /* INTREQ10A */
	  { IRQ8A, IRQ9A, IRQ10A, IRQ11A, IRQ12A, IRQ13A, IRQ14A, IRQ15A } },
	{ 0xe6900028, 0, 8, /* INTREQ20A */
	  { IRQ16A, IRQ17A, IRQ18A, IRQ19A, IRQ20A, IRQ21A, IRQ22A, IRQ23A } },
	{ 0xe690002c, 0, 8, /* INTREQ30A */
	  { IRQ24A, IRQ25A, IRQ26A, IRQ27A, IRQ28A, IRQ29A, IRQ30A, IRQ31A } },
};

static struct intc_desc intca_desc __initdata = {
	.name = "sh7372-intca",
	.force_enable = ENABLED,
	.force_disable = DISABLED,
	.hw = INTC_HW_DESC(intca_vectors, intca_groups,
			   intca_mask_registers, intca_prio_registers,
			   intca_sense_registers, intca_ack_registers),
};

void __init sh7372_init_irq(void)
{
	register_intc_controller(&intca_desc);
}
