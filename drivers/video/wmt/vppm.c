/**************************************************************		
Some descriptions of such software. Copyright (c) 2008  WonderMedia Technologies, Inc.	
This program is free software: you can redistribute it and/or modify it under the terms 	
of the GNU General Public License as published by the Free Software Foundation, either
 	version 2 of the License, or (at your option) any later version.
	
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the GNU General Public License for more details. You should have received
 a copy of the GNU General Public License along with this program.  If not, see
 <http://www.gnu.org/licenses/>.

WonderMedia Technologies, Inc.

--*/

#define VPPM_C
#include "vppm.h"

#ifdef VPP_DEBUG
#define VPPMSG(fmt, args...)  DPRINT("[VPP] %s: " fmt, __FUNCTION__ , ## args)
#else
#define VPPMSG(fmt, args...) do {} while(0)
#endif

void vppm_set_int_enable(vpp_flag_t enable, vpp_int_t int_bit)
{
#ifdef WMT_FTBLK_SCL
	if (int_bit & VPP_INT_SCL_VBIE) {
		vppif_reg32_write(VPP_SCL_INTEN_VBIE, enable);
	}
	if (int_bit & VPP_INT_SCL_VBIS) {
		vppif_reg32_write(VPP_SCL_INTEN_VBIS, enable);
	}
	if (int_bit & VPP_INT_SCL_PVBI) {
		vppif_reg32_write(VPP_SCL_INTEN_PVBI, enable);
	}
#endif
#ifdef WMT_FTBLK_GOVRH
	if (int_bit & VPP_INT_GOVRH_VBIE) {
		vppif_reg32_write(VPP_GOVRH_INTEN_VBIE, enable);
	}
	if (int_bit & VPP_INT_GOVRH_VBIS) {
		vppif_reg32_write(VPP_GOVRH_INTEN_VBIS, enable);
	}
	if (int_bit & VPP_INT_GOVRH_PVBI) {
		vppif_reg32_write(VPP_GOVRH_INTEN_PVBI, enable);
	}
#endif
#ifdef WMT_FTBLK_GOVW
	if (int_bit & VPP_INT_GOVW_VBIE) {
		vppif_reg32_write(VPP_GOVW_INTEN_VBIE, enable);
	}
	if (int_bit & VPP_INT_GOVW_VBIS) {
		vppif_reg32_write(VPP_GOVW_INTEN_VBIS, enable);
	}
	if (int_bit & VPP_INT_GOVW_PVBI) {
		vppif_reg32_write(VPP_GOVW_INTEN_PVBI, enable);
	}
#endif
#ifdef WMT_FTBLK_DISP
	if (int_bit & VPP_INT_DISP_VBIE) {
		vppif_reg32_write(VPP_DISP_INTEN_VBIE, enable);
	}
	if (int_bit & VPP_INT_DISP_VBIS) {
		vppif_reg32_write(VPP_DISP_INTEN_VBIS, enable);
	}
	if (int_bit & VPP_INT_DISP_PVBI) {
		vppif_reg32_write(VPP_DISP_INTEN_PVBI, enable);
	}
#endif
#ifdef WMT_FTBLK_VPU
	if (int_bit & VPP_INT_VPU_VBIE) {
		vppif_reg32_write(VPP_VPU_INTEN_VBIE, enable);
	}
	if (int_bit & VPP_INT_VPU_VBIS) {
		vppif_reg32_write(VPP_VPU_INTEN_VBIS, enable);
	}
	if (int_bit & VPP_INT_VPU_PVBI) {
		vppif_reg32_write(VPP_VPU_INTEN_PVBI, enable);
	}
#endif
}

vpp_int_t vppm_get_int_status(void)
{
	unsigned int int_enable_reg;
	unsigned int int_sts_reg;
	vpp_int_t int_sts = 0;

	int_enable_reg = vppif_reg32_in(REG_VPP_INTEN);
	int_sts_reg = vppif_reg32_in(REG_VPP_INTSTS);

#ifdef WMT_FTBLK_SCL
	if ((int_enable_reg & BIT18) && (int_sts_reg & BIT18)) {
		int_sts |= VPP_INT_SCL_VBIE;
	}
	if ((int_enable_reg & BIT17) && (int_sts_reg & BIT17)) {
		int_sts |= VPP_INT_SCL_VBIS;
	}
	if ((int_enable_reg & BIT16) && (int_sts_reg & BIT16)) {
		int_sts |= VPP_INT_SCL_PVBI;
	}
#endif
#ifdef WMT_FTBLK_GOVRH
	if ((int_enable_reg & BIT10) && (int_sts_reg & BIT10)) {
		int_sts |= VPP_INT_GOVRH_VBIE;
	}
	if ((int_enable_reg & BIT9) && (int_sts_reg & BIT9)) {
		int_sts |= VPP_INT_GOVRH_VBIS;
	}
	if ((int_enable_reg & BIT8) && (int_sts_reg & BIT8)) {
		int_sts |= VPP_INT_GOVRH_PVBI;
	}
#endif
#ifdef WMT_FTBLK_GOVW
	if ((int_enable_reg & BIT2) && (int_sts_reg & BIT2)) {
		int_sts |= VPP_INT_GOVW_VBIE;
	}
	if ((int_enable_reg & BIT1) && (int_sts_reg & BIT1)) {
		int_sts |= VPP_INT_GOVW_VBIS;
	}
	if ((int_enable_reg & BIT0) && (int_sts_reg & BIT0)) {
		int_sts |= VPP_INT_GOVW_PVBI;
	}
#endif
#ifdef WMT_FTBLK_DISP
	if ((int_enable_reg & BIT26) && (int_sts_reg & BIT26)) {
		int_sts |= VPP_INT_DISP_VBIE;
	}
	if ((int_enable_reg & BIT25) && (int_sts_reg & BIT25)) {
		int_sts |= VPP_INT_DISP_VBIS;
	}
	if ((int_enable_reg & BIT24) && (int_sts_reg & BIT24)) {
		int_sts |= VPP_INT_DISP_PVBI;
	}
#endif
#ifdef WMT_FTBLK_VPU
	if ((int_enable_reg & BIT6) && (int_sts_reg & BIT6)) {
		int_sts |= VPP_INT_VPU_VBIE;
	}
	if ((int_enable_reg & BIT5) && (int_sts_reg & BIT5)) {
		int_sts |= VPP_INT_VPU_VBIS;
	}
	if ((int_enable_reg & BIT4) && (int_sts_reg & BIT4)) {
		int_sts |= VPP_INT_VPU_PVBI;
	}
#endif
	return int_sts;
}

void vppm_clean_int_status(vpp_int_t int_sts)
{
#ifdef WMT_FTBLK_SCL
	if (int_sts & VPP_INT_SCL_VBIE) {
		vppif_reg8_out(REG_VPP_INTSTS+0x2,0x4);
	}
	if (int_sts & VPP_INT_SCL_VBIS) {
		vppif_reg8_out(REG_VPP_INTSTS+0x2,0x2);
	}
	if (int_sts & VPP_INT_SCL_PVBI) {
		vppif_reg8_out(REG_VPP_INTSTS+0x2,0x1);
	}
#endif
#ifdef WMT_FTBLK_GOVRH
	if (int_sts & VPP_INT_GOVRH_VBIE) {
		vppif_reg8_out(REG_VPP_INTSTS+0x1,0x4);
	}
	if (int_sts & VPP_INT_GOVRH_VBIS) {
		vppif_reg8_out(REG_VPP_INTSTS+0x1,0x2);
	}
	if (int_sts & VPP_INT_GOVRH_PVBI) {
		vppif_reg8_out(REG_VPP_INTSTS+0x1,0x1);
	}
#endif
#ifdef WMT_FTBLK_GOVW
	if (int_sts & VPP_INT_GOVW_VBIE) {
		vppif_reg8_out(REG_VPP_INTSTS+0x0,0x4);
	}
	if (int_sts & VPP_INT_GOVW_VBIS) {
		vppif_reg8_out(REG_VPP_INTSTS+0x0,0x2);
	}
	if (int_sts & VPP_INT_GOVW_PVBI) {
		vppif_reg8_out(REG_VPP_INTSTS+0x0,0x1);
	}
#endif
#ifdef WMT_FTBLK_DISP
	if (int_sts & VPP_INT_DISP_VBIE) {
		vppif_reg8_out(REG_VPP_INTSTS+0x3,0x4);
	}
	if (int_sts & VPP_INT_DISP_VBIS) {
		vppif_reg8_out(REG_VPP_INTSTS+0x3,0x2);
	}
	if (int_sts & VPP_INT_DISP_PVBI) {
		vppif_reg8_out(REG_VPP_INTSTS+0x3,0x1);
	}
#endif
#ifdef WMT_FTBLK_VPU
	if (int_sts & VPP_INT_VPU_VBIE) {
		vppif_reg8_out(REG_VPP_INTSTS+0x0,0x40);
	}
	if (int_sts & VPP_INT_VPU_VBIS) {
		vppif_reg8_out(REG_VPP_INTSTS+0x0,0x20);
	}
	if (int_sts & VPP_INT_VPU_PVBI) {
		vppif_reg8_out(REG_VPP_INTSTS+0x0,0x10);
	}
#endif
}

void vppm_set_module_reset(vpp_mod_t mod)
{
	unsigned int value1 = 0x00, value2 = 0x00;

#if 0
#ifdef WMT_FTBLK_GE
	if(mod == VPP_MOD_GE)
		value1 |= BIT16;
#endif
#ifdef WMT_FTBLK_VID
	if(mod == VPP_MOD_VID)
		value1 |= BIT8;
#endif
#endif
#ifdef WMT_FTBLK_SCL
	if(mod == VPP_MOD_SCL)
		value1 |= BIT0;
#endif
#ifdef WMT_FTBLK_GOVW
	if(mod == VPP_MOD_GOVW)
		value2 |= BIT16;
#endif
#ifdef WMT_FTBLK_GOVRH
	if(mod & VPP_MOD_GOVRH)
		value2 |= (BIT0 | BIT8);
#endif	
#ifdef WMT_FTBLK_VPU
	if(mod == VPP_MOD_VPU)
		value1 |= BIT24;
#endif
#ifdef WMT_FTBLK_DISP
	if(mod == VPP_MOD_DISP)
		value2 |= BIT24;
#endif	
	vppif_reg32_out(REG_VPP_SWRST1_SEL, ~value1);
	vppif_reg32_out(REG_VPP_SWRST1_SEL, 0x1010101);
	vppif_reg32_out(REG_VPP_SWRST2_SEL, ~value2);
	vppif_reg32_out(REG_VPP_SWRST2_SEL, 0x1010101);
}

#ifdef WMT_FTBLK_DISP
void vppm_set_DAC_select(int tvmode)
{
	vppif_reg32_write(0xd8110200,0x80000000,31,tvmode);	
	vppif_reg32_write(VPP_DAC_SEL,tvmode);
}
#endif

#ifdef CONFIG_PM
void vppm_suspend(int sts)
{
	switch( sts ){
		case 0:	// disable module
			break;
		case 1: // disable tg
			break;
		case 2:	// backup register
			p_vppm->reg_bk = vpp_backup_reg(VPP_BASE_ADDR+0x00,0x1c);
			break;
		default:
			break;
	}
}

void vppm_resume(int sts)
{
	switch( sts ){
		case 0:	// restore register
			vpp_restore_reg(VPP_BASE_ADDR+0x00,0x1c,p_vppm->reg_bk);
			p_vppm->reg_bk = 0;
			break;
		case 1:	// enable module
			break;
		case 2: // enable tg
			break;
		default:
			break;
	}
}
#else
#define vppm_suspend NULL
#define vppm_resume NULL
#endif

void vppm_init(void *base)
{
	vppm_mod_t *mod_p;

	mod_p = (vppm_mod_t *) base;

	vppm_set_module_reset(0);
	vppm_set_int_enable(VPP_FLAG_ENABLE, mod_p->int_catch);
}

int vppm_mod_init(void)
{
	vppm_mod_t *mod_p;

	mod_p = (vppm_mod_t *) vpp_mod_register(VPP_MOD_VPPM,sizeof(vppm_mod_t),0);
	if( !mod_p ){
		DPRINT("*E* VPP module register fail\n");
		return -1;
	}

	/* module member variable */
	mod_p->int_catch = VPP_INT_NULL;

	/* module member function */
	mod_p->init = vppm_init;
//	mod_p->dump_reg = vppm_reg_dump;
	mod_p->get_sts = vppm_get_int_status;
	mod_p->clr_sts = vppm_clean_int_status;
	mod_p->suspend = vppm_suspend;
	mod_p->resume = vppm_resume;

	p_vppm = mod_p;
	return 0;
}
#ifdef __KERNEL__
module_init(vppm_mod_init);
#endif
//#endif                                //WMT_FTBLK_VPP
