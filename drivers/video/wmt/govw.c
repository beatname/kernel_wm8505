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


/* PVCS version log
** $Log: POST/vpp/govw.c $
 *
 * 01	08/05/15, Andy Chiu, Initial version for vt8440 (pending)
 * 02	08/10/01, Andy Chiu, Refine for vt8510 
 *
 */
#define GOVW_C

#include "govw.h"
 
#ifdef WMT_FTBLK_GOVW

#ifdef GOVW_DEBUG
#define GOVWMSG(fmt, args...) DPRINT("[GOVW] %s: " fmt, __FUNCTION__ , ## args)
#else
#define GOVWMSG(fmt, args...) do {} while(0)
#endif

void govw_reg_dump(void)
{
	extern char *vpp_colfmt_str[];
	int i;

	DPRINT("========== GOVW register dump ==========\n");
	for(i=0;i<0xcc;i+=16){
		DPRINT("0x%8x : 0x%08x 0x%08x 0x%08x 0x%08x\n",GOVW_BASE_ADDR+i,vppif_reg32_in(GOVW_BASE_ADDR+i),
			vppif_reg32_in(GOVW_BASE_ADDR+i+4),vppif_reg32_in(GOVW_BASE_ADDR+i+8),vppif_reg32_in(GOVW_BASE_ADDR+i+12));
	}

	DPRINT("GOVW enable %d\n",vppif_reg32_read(GOVW_HD_MIF_ENABLE));
	DPRINT("color mode %s\n",vpp_colfmt_str[govw_get_hd_color_format()]);
	DPRINT("Y addr 0x%x,C addr 0x%x\n",vppif_reg32_in(REG_GOVW_HD_YSA),vppif_reg32_in(REG_GOVW_HD_CSA));
	DPRINT("Y width %d,fb width %d\n",vppif_reg32_read(GOVW_HD_YPXLWID),vppif_reg32_read(GOVW_HD_YBUFWID));
	DPRINT("C width %d,fb width %d\n",vppif_reg32_read(GOVW_HD_CPXLWID),vppif_reg32_read(GOVW_HD_CBUFWID));	

	DPRINT("---------- GOVW TG ----------\n");	
	DPRINT("TG enable %d, wait ready enable %d\n",vppif_reg32_read(GOVW_TG_ENABLE),vppif_reg32_read(GOVW_TG_WATCHDOG_ENABLE));	
	DPRINT("clk %d,Read cyc %d\n",vpp_get_base_clock(VPP_PLL_B,VPP_DIV_VPP),vppif_reg32_read(GOVW_TG_RDCYC));
	DPRINT("H total %d, beg %d, end %d\n",vppif_reg32_read(GOVW_TG_H_ALLPIXEL),
		vppif_reg32_read(GOVW_TG_H_ACTBG),vppif_reg32_read(GOVW_TG_H_ACTEND));
	DPRINT("V total %d, beg %d, end %d\n",vppif_reg32_read(GOVW_TG_V_ALLLINE),
		vppif_reg32_read(GOVW_TG_V_ACTBG),vppif_reg32_read(GOVW_TG_V_ACTEND));
	DPRINT("VBIE %d,PVBI %d\n",vppif_reg32_read(GOVW_TG_VBIE),vppif_reg32_read(GOVW_TG_PVBI));
	DPRINT("Watch dog %d\n",vppif_reg32_read(GOVW_TG_WATCHDOG_VALUE));

	DPRINT("INT MIF C err %d,Y err %d,TG err %d\n",vppif_reg32_read(GOVW_INT_MIFCERR_ENABLE),
		vppif_reg32_read(GOVW_INT_MIFYERR_ENABLE),vppif_reg32_read(GOVW_INT_TGERR_ENABLE));
}

void govw_set_tg_enable(vpp_flag_t enable)
{
	if( g_vpp.direct_path ){
		enable = VPP_FLAG_DISABLE;
	}

	vppif_reg32_write(GOVW_TG_ENABLE, enable);
}

void govw_set_timing(vpp_clock_t *timing)
{
	GOVWMSG("govw set timing\n");

	timing->read_cycle = ( timing->read_cycle < 3 )? 3:timing->read_cycle;
	timing->read_cycle = ( timing->read_cycle > 255 )? 0xFF:timing->read_cycle;

	govw_set_tg_enable(VPP_FLAG_DISABLE);	
	vppif_reg32_write(GOVW_TG_RDCYC,timing->read_cycle);
	vppif_reg32_write(GOVW_TG_H_ALLPIXEL,timing->total_pixel_of_line);
	vppif_reg32_write(GOVW_TG_H_ACTBG,timing->begin_pixel_of_active);
	vppif_reg32_write(GOVW_TG_H_ACTEND,timing->end_pixel_of_active);
	vppif_reg32_write(GOVW_TG_V_ALLLINE,timing->total_line_of_frame);
	vppif_reg32_write(GOVW_TG_V_ACTBG,timing->begin_line_of_active);
	vppif_reg32_write(GOVW_TG_V_ACTEND,timing->end_line_of_active);
	vppif_reg32_write(GOVW_TG_VBIE,timing->line_number_between_VBIS_VBIE);
	//vppif_reg32_write(GOVW_TG_PVBI,timing->line_number_between_PVBI_VBIS);
	vppif_reg32_write(GOVW_TG_PVBI,10);
	govw_set_tg_enable(VPP_FLAG_ENABLE);	
}

void govw_get_timing(vpp_clock_t * p_timing)
{
	p_timing->read_cycle = vppif_reg32_read(GOVW_TG_RDCYC);
	p_timing->total_pixel_of_line = vppif_reg32_read(GOVW_TG_H_ALLPIXEL);
	p_timing->begin_pixel_of_active = vppif_reg32_read(GOVW_TG_H_ACTBG);
	p_timing->end_pixel_of_active = vppif_reg32_read(GOVW_TG_H_ACTEND);
	p_timing->total_line_of_frame = vppif_reg32_read(GOVW_TG_V_ALLLINE);
	p_timing->begin_line_of_active = vppif_reg32_read(GOVW_TG_V_ACTBG);
	p_timing->end_line_of_active = vppif_reg32_read(GOVW_TG_V_ACTEND);
	p_timing->line_number_between_VBIS_VBIE = vppif_reg32_read(GOVW_TG_VBIE);
	p_timing->line_number_between_PVBI_VBIS = vppif_reg32_read(GOVW_TG_PVBI);
}

void govw_set_watchdog(U32 count)
{
	//if interval equal '0' then disable the watchdog function
	if (0 != count) {
		vppif_reg32_write(GOVW_TG_WATCHDOG_VALUE, count);
		vppif_reg32_write(GOVW_TG_WATCHDOG_ENABLE, VPP_FLAG_TRUE);
	} else {
		vppif_reg32_write(GOVW_TG_WATCHDOG_ENABLE, VPP_FLAG_FALSE);
	}
}

void govw_set_reg_level(vpp_reglevel_t level)
{
	switch (level) {
	case VPP_REG_LEVEL_1:
		vppif_reg32_write(GOVW_REG_LEVEL, 0x1);
		break;
	case VPP_REG_LEVEL_2:
		vppif_reg32_write(GOVW_REG_LEVEL, 0x0);
		break;
	default:
		GOVWMSG("*E* check the parameter.\n");
		return;
	}
}

void govw_set_reg_update(vpp_flag_t enable)
{
	vppif_reg32_write(GOVW_REG_UPDATE, enable);
}

void govw_set_hd_mif_enable(vpp_flag_t enable)
{
	switch (enable) {
	case VPP_FLAG_ENABLE:
	case VPP_FLAG_DISABLE:
		vppif_reg32_write(GOVW_HD_MIF_ENABLE, enable);
		break;
	default:
		GOVWMSG("*E* check the parameter.\n");
		return;
	}
}

vpp_flag_t govw_set_hd_color_format(vdo_color_fmt format)
{
	switch (format) {
	case VDO_COL_FMT_YUV422H:
#ifdef WMT_FTBLK_GOVW_CSC
		vppif_reg32_write(GOVW_COLFMT_RGB, 0);
		vppif_reg32_write(GOVW_HD_COLFMT, 0);
#else
		vppif_reg32_write(GOVW_HD_COLFMT, 1);
#endif
		break;
	case VDO_COL_FMT_YUV444:
#ifdef WMT_FTBLK_GOVW_CSC
		vppif_reg32_write(GOVW_COLFMT_RGB, 0);
		vppif_reg32_write(GOVW_HD_COLFMT, 1);
#else
		vppif_reg32_write(GOVW_HD_COLFMT, 0);
#endif
		break;
#ifdef WMT_FTBLK_GOVW_CSC
	case VDO_COL_FMT_ARGB:
		vppif_reg32_write(GOVW_COLFMT_RGB, 1);
		break;
#endif
	default:
		GOVWMSG("*E* check the parameter.\n");
		return VPP_FLAG_ERROR;
	}
	return VPP_FLAG_SUCCESS;
}

vdo_color_fmt govw_get_hd_color_format(void)
{
#ifdef WMT_FTBLK_GOVW_CSC
	if(	vppif_reg32_read(GOVW_COLFMT_RGB) ){
		return VDO_COL_FMT_ARGB;
	}
#endif

#ifdef WMT_FTBLK_GOVW_CSC
	if( !vppif_reg32_read(GOVW_HD_COLFMT) ){
#else
	if( vppif_reg32_read(GOVW_HD_COLFMT) ){
#endif
		return VDO_COL_FMT_YUV422H;
	}
	return VDO_COL_FMT_YUV444;
}


void govw_set_hd_fb_addr(U32 y_addr,U32 c_addr)
{
//	GOVWMSG("y_addr:0x%08x, c_addr:0x%08x\n",y_addr,c_addr);
	vppif_reg32_out(REG_GOVW_HD_YSA,y_addr);
	vppif_reg32_out(REG_GOVW_HD_CSA,c_addr);
}

void govw_get_hd_fb_addr(U32 * y_addr, U32 * c_addr)
{
	*y_addr = vppif_reg32_in(REG_GOVW_HD_YSA);
	*c_addr = vppif_reg32_in(REG_GOVW_HD_CSA);
//	GOVWMSG("y_addr:0x%08x, c_addr:0x%08x\n", (U32)*y_addr, (U32)*c_addr);
}

void govw_set_hd_width(U32 width, U32 fb_width)
{
	vdo_color_fmt colfmt;

	colfmt = govw_get_hd_color_format();
#ifdef WMT_FTBLK_GOVW_CSC	
	if( colfmt == VDO_COL_FMT_ARGB ){
		switch( vppif_reg32_read(GOVW_RGB_MODE) ){
			case 0x0:	// 888
			case 0x2:	// 666
			default:
				break;
			case 0x1:	// 555
			case 0x3:	// 565
				width /= 2;
				fb_width /= 2;
				break;
		}
	}
#endif

	vppif_reg32_write(GOVW_HD_YPXLWID, width);
	vppif_reg32_write(GOVW_HD_YBUFWID, fb_width);
	if( colfmt == VDO_COL_FMT_YUV444 ){
		vppif_reg32_write(GOVW_HD_CBUFWID, fb_width * 2);
		vppif_reg32_write(GOVW_HD_CPXLWID, width);
	}
	else {
		vppif_reg32_write(GOVW_HD_CBUFWID, fb_width);
		vppif_reg32_write(GOVW_HD_CPXLWID, width/2);
	}
}

void govw_get_hd_width(U32 *width,U32 *fb_width)
{
	*width = vppif_reg32_read(GOVW_HD_YPXLWID);
	*fb_width = vppif_reg32_read(GOVW_HD_YBUFWID);
}

vpp_int_err_t govw_get_int_status(void)
{
	vpp_int_err_t int_sts;

	int_sts = 0;
	if (vppif_reg32_read(GOVW_INTSTS_TGERR)) {
		int_sts |= VPP_INT_ERR_GOVW_TG;
	}
	if (vppif_reg32_read(GOVW_INTSTS_MIFYERR)) {
		int_sts |= VPP_INT_ERR_GOVW_MIFY;
	}
	if (vppif_reg32_read(GOVW_INTSTS_MIFCERR)) {
		int_sts |= VPP_INT_ERR_GOVW_MIFC;
	}
	return int_sts;
}

void govw_clean_int_status(vpp_int_err_t int_sts)
{
	unsigned int reg;

	reg = vppif_reg32_in(REG_GOVW_INT) & 0xFF;

	if (int_sts & VPP_INT_ERR_GOVW_TG) {
		reg |= BIT20;
	}
	if (int_sts & VPP_INT_ERR_GOVW_MIFY) {
		reg |= BIT19;
	}
	if (int_sts & VPP_INT_ERR_GOVW_MIFC) {
		reg |= BIT18;
	}
	vppif_reg32_out(REG_GOVW_INT,reg);
}

void govw_set_int_enable(vpp_flag_t enable, vpp_int_err_t int_bit)
{
	//clean status first before enable/disable interrupt
	govw_clean_int_status(int_bit);

	if (int_bit & VPP_INT_ERR_GOVW_TG) {
		vppif_reg32_write(GOVW_INT_TGERR_ENABLE, enable);
	}
	if (int_bit & VPP_INT_ERR_GOVW_MIFY) {
		vppif_reg32_write(GOVW_INT_MIFYERR_ENABLE, enable);
	}
	if (int_bit & VPP_INT_ERR_GOVW_MIFC) {
		vppif_reg32_write(GOVW_INT_MIFCERR_ENABLE, enable);
	}
}

/*----------------------- GOVW CSC --------------------------------------*/
#ifdef WMT_FTBLK_GOVW_CSC
void govw_set_rgb_mode(unsigned int mode)
{
	/* 0:888, 1:555, 2:666, 3:565 */
	vppif_reg32_write(GOVW_RGB_MODE,mode);
}

void govw_set_csc_mode(vpp_csc_t mode)
{
	vdo_color_fmt src_fmt,dst_fmt;
#ifdef WMT_FTBLK_PIP
	src_fmt = govm_get_pip_color_format();
//	src_fmt = vpu_r_get_color_format();
	dst_fmt = govw_get_hd_color_format();
#else
	src_fmt = VDO_COL_FMT_YUV444;
	dst_fmt = govw_get_hd_color_format();
//	mode = VPP_CSC_MAX;
#endif	
	mode = vpp_check_csc_mode(mode,src_fmt,dst_fmt,0);
	if (mode >= VPP_CSC_MAX) {
		vppif_reg32_out(REG_GOVW_CSC_COEF1, 0x400);	//CSC1
		vppif_reg32_out(REG_GOVW_CSC_COEF2, 0x0);	//CSC2
		vppif_reg32_out(REG_GOVW_CSC_COEF3, 0x400);	//CSC3
		vppif_reg32_out(REG_GOVW_CSC_COEF4, 0x0);	//CSC4
		vppif_reg32_out(REG_GOVW_CSC_COEF5, 0x400);	//CSC5
		vppif_reg32_out(REG_GOVW_CSC_COEF6, 0x0);	//CSC6
		vppif_reg32_out(REG_GOVW_CSC_MODE, 0x0);	//CSC_CTL
	} else {
		vppif_reg32_out(REG_GOVW_CSC_COEF1, vpp_csc_parm[mode][0]);	//CSC1
		vppif_reg32_out(REG_GOVW_CSC_COEF2, vpp_csc_parm[mode][1]);	//CSC2
		vppif_reg32_out(REG_GOVW_CSC_COEF3, vpp_csc_parm[mode][2]);	//CSC3
		vppif_reg32_out(REG_GOVW_CSC_COEF4, vpp_csc_parm[mode][3]);	//CSC4
		vppif_reg32_out(REG_GOVW_CSC_COEF5, vpp_csc_parm[mode][4]);	//CSC5
		vppif_reg32_out(REG_GOVW_CSC_COEF6, vpp_csc_parm[mode][5]);	//CSC6
		vppif_reg32_out(REG_GOVW_CSC_MODE, vpp_csc_parm[mode][6]);	//CSC_CTL
	}
}
#endif /* WMT_FTBLK_GOVW_CSC */

void govw_set_hd_framebuffer(vdo_framebuf_t *fb)
{
	vpp_clock_t tmr;

    govw_set_hd_fb_addr(fb->y_addr, fb->c_addr);
	govw_set_hd_color_format(fb->col_fmt);
	govw_set_hd_width(fb->img_w, fb->fb_w);
#ifdef WMT_FTBLK_GOVW_CSC
	govw_set_csc_mode(p_govw->fb_p->csc_mode);
#endif

	// vpu scale
#ifdef WMT_FTBLK_VPU
	vpu_set_scale(p_vpu->fb_p->fb.img_w, p_vpu->fb_p->fb.img_h, p_vpu->resx_visual, p_vpu->resy_visual);
#else
	scl_set_scale(p_vpu->fb_p->fb.img_w, p_vpu->fb_p->fb.img_h, p_vpu->resx_visual, p_vpu->resy_visual);
#endif

	// govw TG
	g_vpp.govr->get_tg(&tmr);
	if( tmr.total_line_of_frame ){
		vpp_calculate_timing(VPP_MOD_GOVW,p_govw->fb_p->framerate,&tmr);
		p_govw->set_tg(&tmr);
	}
	
	// govm
	govm_set_disp_coordinate(fb->img_w,fb->img_h);
}

#ifdef CONFIG_PM
static unsigned int govw_pm_enable,govw_pm_tg;
void govw_suspend(int sts)
{
	switch( sts ){
		case 0:	// disable module
			govw_pm_enable = vppif_reg32_read(GOVW_HD_MIF_ENABLE);
			vppif_reg32_write(GOVW_HD_MIF_ENABLE,0);
			break;
		case 1: // disable tg
			govw_pm_tg = vppif_reg32_read(GOVW_TG_ENABLE);
			vppif_reg32_write(GOVW_TG_ENABLE,0);
			break;
		case 2:	// backup register
			p_govw->reg_bk = vpp_backup_reg(GOVW_BASE_ADDR+0x00,0xCC); /* 0x00 - 0xCC */
			break;
		default:
			break;
	}
}

void govw_resume(int sts)
{
	switch( sts ){
		case 0:	// restore register
			vpp_restore_reg(GOVW_BASE_ADDR+0x00,0xCC,p_govw->reg_bk); /* 0x00 - 0xCC */	
			p_govw->reg_bk = 0;			
			break;
		case 1:	// enable module
			vppif_reg32_write(GOVW_HD_MIF_ENABLE,govw_pm_enable);
			break;
		case 2: // enable tg
			vppif_reg32_write(GOVW_TG_ENABLE,govw_pm_tg);
			break;
		default:
			break;
	}
}
#else
#define govw_suspend NULL
#define govw_resume NULL
#endif

void govw_init(void *base)
{
	govw_mod_t *mod_p;

	mod_p = (govw_mod_t *) base;

	//disable
	govw_set_reg_level(VPP_REG_LEVEL_1);
	govw_set_int_enable(VPP_FLAG_DISABLE, VPP_INT_ALL);
	govw_set_hd_mif_enable(VPP_FLAG_DISABLE);
	govw_set_tg_enable(VPP_FLAG_DISABLE);

	//register
	govw_set_hd_framebuffer(&mod_p->fb_p->fb);
	govw_set_watchdog(mod_p->fb_p->wait_ready);

	//enable
	govw_set_int_enable(VPP_FLAG_ENABLE, mod_p->int_catch);
	govw_set_hd_mif_enable(VPP_FLAG_ENABLE);
	govw_set_reg_update(VPP_FLAG_ENABLE);
	govw_set_tg_enable(VPP_FLAG_ENABLE);
}

int govw_mod_init(void)
{
	govw_mod_t *mod_p;
	vpp_fb_base_t *mod_fb_p;
	vdo_framebuf_t *fb_p;

	mod_p = (govw_mod_t *) vpp_mod_register(VPP_MOD_GOVW,sizeof(govw_mod_t),VPP_MOD_FLAG_FRAMEBUF);
	if( !mod_p ){
		DPRINT("*E* GOVW module register fail\n");
		return -1;
	}

	/* module member variable */
	mod_p->int_catch = 0; // VPP_INT_ERR_GOVW_TG | VPP_INT_ERR_GOVW_MIFY | VPP_INT_ERR_GOVW_MIFC;

	/* module member function */
	mod_p->init = govw_init;
	mod_p->dump_reg = govw_reg_dump;
	mod_p->set_enable = govw_set_hd_mif_enable;
	mod_p->set_tg = govw_set_timing;
	mod_p->get_tg = govw_get_timing;
	mod_p->get_sts = govw_get_int_status;
	mod_p->clr_sts = govw_clean_int_status;
	mod_p->suspend = govw_suspend;
	mod_p->resume = govw_resume;

	/* module frame buffer */
	mod_fb_p = mod_p->fb_p;
	mod_fb_p->csc_mode = VPP_CSC_RGB2YUV_JFIF_0_255;	
	mod_fb_p->framerate = 30;
	mod_fb_p->media_fmt = VPP_MEDIA_FMT_MPEG;
	mod_fb_p->wait_ready = 0x1000;
	mod_fb_p->capability = BIT(VDO_COL_FMT_YUV422H) | BIT(VDO_COL_FMT_YUV444);
#ifdef WMT_FTBLK_GOVW_CSC
	mod_fb_p->capability |= BIT(VDO_COL_FMT_ARGB) | VPP_FB_FLAG_CSC;
#endif

	/* module frame buffer member function */
	mod_fb_p->set_framebuf = govw_set_hd_framebuffer;
	mod_fb_p->set_addr = govw_set_hd_fb_addr;
	mod_fb_p->get_addr = govw_get_hd_fb_addr;
#ifdef WMT_FTBLK_GOVW_CSC
	mod_fb_p->set_csc = govw_set_csc_mode;
#endif

	fb_p = &mod_p->fb_p->fb;
#if (WMT_CUR_PID == WMT_PID_8425)
	fb_p->col_fmt = VDO_COL_FMT_ARGB;
#else
	fb_p->col_fmt = VDO_COL_FMT_YUV444;
#endif
	fb_p->y_addr = 0;
	fb_p->c_addr = 0;
	fb_p->img_w = VPP_HD_DISP_RESX;
	fb_p->img_h = VPP_HD_DISP_RESY;
	fb_p->fb_w = VPP_HD_MAX_RESX;
	fb_p->fb_h = VPP_HD_MAX_RESY;
	fb_p->h_crop = 0;
	fb_p->v_crop = 0;
	fb_p->flag = 0;

	p_govw = mod_p;
	return 0;
}
#ifdef __KERNEL__
module_init(govw_mod_init);
#endif
#endif				//WMT_FTBLK_GOVW
