#include "vpp-project.h"
#include "com-vpp.h"

#ifndef VPPIF_H
#define VPPIF_H

/* VPP feature config */
/* 0 - right edge black edge, 1 - scale more to cut right edge for fit visual window */
// #define CONFIG_VPP_SCALEDWN_FIT_RIGHT_EDGE		// WM3426 don't support bcz SCLR and GOVW width should equal
#define CONFIG_VPP_INTERRUPT
#define CONFIG_VPP_VT1632_SW
#define CONFIG_VPP_DUAL_BUFFER
// #define CONFIG_VPP_ALLOC_VPU_FB
#define CONFIG_LCD_BACKLIGHT
#define CONFIG_VPP_GOVW_TG_ERR_DROP_FRAME		// drop display frame when govw tg error (bandwidth not enough)
#define CONFIG_GOVW_FPS_AUTO_ADJUST
#define PATCH_VPP_SCALE_DOWN_QUALITY			// scale down quality in 4/5, 5/6, ...
// #define CONFIG_GOVW_FBSWAP_VBIE
#ifdef VPU_DEI_ENABLE
#define CONFIG_VPP_DYNAMIC_DEI
#endif
#define CONFIG_VPP_EDID

// SW patch to fix bug
#define PATCH_SCL_SCALEUP			// scl hw bug : scale up right edge	
//#if(WMT_CUR_PID == WMT_PID_8510)	//ProjectID: wm3426
// #define PATCH_GOV_COORDINATE		// GOV hw bug : GOV vpu coordinate 2 or 3, video should NG, adjust to 1 to avoid hw bug
#define PATCH_SCL_R_WIDTH			// SCL hw bug : active width should align 8

#define PATCH_GOVRH_ASYNC_FIFO	// govrh hw bug : garbage in first pixel
#define PATCH_SCL_SCALEDN			// scl hw bug : scale up blink in ARGB
#define PATCH_SCLW_RGB_SRAM_SWITCH	// sclw hw bug : sclw dst img_w 64 - 80 data not correct in RGB mode
//#endif
#define PATCH_GE_NOT_READY			// govw hw bug : tg error in ge not ready
// #define PATCH_GE_NOT_READY_2		// govw hw bug : tg error in ge not ready

// VPP constant define
#define VPP_DAC_SENSE_SECOND			5
#define VPP_VOUT_FRAMERATE_DEFAULT		60
#define GOVRH_DAC_SENSE_VALUE			0x42	// 0x55

#define VPP_SCALE_UP_RATIO_H			31
#define VPP_SCALE_DN_RATIO_H			32
#define VPP_SCALE_UP_RATIO_V			31
#ifdef WMT_FTBLK_SCL_VSCL_32
#define VPP_SCALE_DN_RATIO_V			32
#else
#define VPP_SCALE_DN_RATIO_V			16
#endif

// VPP FB capability flag
#define VPP_FB_FLAG_COLFMT		0xFFFF
#define VPP_FB_FLAG_SCALE		BIT(16)
#define VPP_FB_FLAG_CSC			BIT(17)
#define VPP_FB_FLAG_MEDIA		BIT(18)
#define VPP_FB_FLAG_FIELD		BIT(19)

typedef struct {
	vdo_framebuf_t fb;
	vpp_csc_t csc_mode;
	int	framerate;
	vpp_media_format_t media_fmt;
	int wait_ready;
	unsigned int capability;

	void (*set_framebuf)(vdo_framebuf_t *fb);
	void (*set_addr)(unsigned int yaddr,unsigned int caddr);
	void (*get_addr)(unsigned int *yaddr,unsigned int *caddr);
	void (*set_csc)(vpp_csc_t mode);
	vdo_color_fmt (*get_color_fmt)(void);
	void (*set_color_fmt)(vdo_color_fmt colfmt);
	void (*fn_view)(int read,vdo_view_t *view);
} vpp_fb_base_t;

#define VPP_MOD_BASE \
	vpp_mod_t mod; /* module id*/\
	unsigned int int_catch; /* interrupt catch */\
	vpp_fb_base_t *fb_p; /* framebuf base pointer */\
	unsigned int *reg_bk; /* register backup pointer */\
	void  (*init)(void *base); /* module initial */\
	void (*dump_reg)(void); /* dump hardware register */\
	void (*set_enable)(vpp_flag_t enable); /* module enable/disable */\
	void (*set_colorbar)(vpp_flag_t enable,int mode,int inv); /* hw colorbar enable/disable & mode */\
	void (*set_tg)(vpp_clock_t *tmr); /* set timing */\
	void (*get_tg)(vpp_clock_t *tmr); /* get timing */\
	unsigned int (*get_sts)(void); /* get interrupt or error status */\
	void (*clr_sts)(unsigned int sts); /* clear interrupt or error status */\
	void (*suspend)(int sts); /* module suspend */\
	void (*resume)(int sts) /* module resume */
/* End of vpp_mod_base_t */

typedef struct {
	VPP_MOD_BASE;
} vpp_mod_base_t;

#define VPP_MOD_FLAG_FRAMEBUF	BIT(0)

#include "vppm.h"

#include "vpu.h"
#include "lcd.h"

// #ifdef WMT_FTBLK_SCL
#include "scl.h"
// #endif
/*
#ifdef WMT_FTBLK_GE
#include "ge.h"
#endif
*/
#ifdef WMT_FTBLK_GOVM
#include "govm.h"
#endif
#ifdef WMT_FTBLK_GOVW
#include "govw.h"
#endif
#ifdef WMT_FTBLK_GOVRS
#include "govrs.h"
#endif
#ifdef WMT_FTBLK_GOVRH
#include "govrh.h"
#endif
#ifdef WMT_FTBLK_DISP
#include "disp.h"
#endif
#ifdef WMT_FTBLK_LCDC
#include "lcdc.h"
#endif

typedef enum {
	VPP_DBGLVL_DISABLE = 0x0,
	VPP_DBGLVL_SCALE = 1,
	VPP_DBGLVL_DISPFB = 2,
	VPP_DBGLVL_INT = 3,
	VPP_DBGLVL_TG = 4,
	VPP_DBGLVL_IOCTL = 5,
	VPP_DBGLVL_DIAG = 6,
	VPP_DBGLVL_DEI = 7,
	VPP_DBGLVL_PLAYBACK = 8,
	VPP_DBGLVL_ALL = 0xFF,
} vpp_dbg_level_t;

typedef struct {
	// internal parameter
	int vo_enable;
	int govrh_preinit;
	vpp_mod_base_t *govr;	// module pointer
	int govw_skip_frame;
	int govw_skip_all;

	// video parameter
	int video_quality_mode;	// 1: quality mode, 0: performance mode (drop line)

	// scale parameter
	int scale_keep_ratio;

	// alloc frame buffer
	unsigned int mb[2];	
	unsigned int resx;
	unsigned int resy;

	// display framebuf queue
	int disp_fb_max;
	int disp_fb_cnt;

	// direct path
	int direct_path;
	vdo_color_fmt direct_path_colfmt;

	// VO PTS
	vpp_pts_t frame_pts;
	vpp_pts_t govw_pts;
	vpp_pts_t disp_pts;

	// GOVRH async fifo patch
	int govrh_async_fifo_patch;
	unsigned int govrh_async_fifo_reg;
	int govrh_async_fifo_cnt;
	int govrh_field;
	int govrh_interlace_mode;

	// auto adjust fps for bandwidth
	int govw_tg_dynamic;
	unsigned int govw_tg_rcyc;
	unsigned int govw_tg_rtn_cnt;
	unsigned int govw_tg_rtn_max;

	// debug
	int dbg_msg_level;
	int dbg_govw_fb_cnt;
	int dbg_govw_vbis_cnt;
	int dbg_govw_pvbi_cnt;
	int dbg_vpu_dispfb_skip_cnt;
	int dbg_govw_tg_err_cnt;
	int dbg_vpu_disp_cnt;
	int dbg_pip_disp_cnt;
	
} vpp_info_t;

typedef enum {
	VPP_PATTERN_HW_COLORBAR,
	VPP_PATTERN_WHITE,
	VPP_PATTERN_BLACK,
	VPP_PATTERN_WHITE_BLACK,
	VPP_PATTERN_FILL,
	VPP_PATTERN_GRADIENT,
	VPP_PATTERN_BLOCK,
	VPP_PATTERN_MAX
} vpp_pattern_t;

typedef enum {
	VPP_PLL_A = 0xd8130200,
	VPP_PLL_B = 0xd8130204,
	VPP_PLL_C = 0xd8130208,
	VPP_PLL_D = 0xd813020C,
	VPP_PLL_MAX
} vpp_clk_pll_t;

typedef enum {
	VPP_DIV_ARM = 0xd8130300,
	VPP_DIV_AHB = 0xd8130304,
	VPP_DIV_APB = 0xd8130350,
	VPP_DIV_GENET = 0xd813032C,
	VPP_DIV_NOR = 0xd8130334,
	VPP_DIV_PWM = 0xd8130348,
	VPP_DIV_SD_MMC = 0xd8130328,
	VPP_DIV_Serial_Flash = 0xd8130314,
	VPP_DIV_SPI0 = 0xd813033C,
	VPP_DIV_SPI1 = 0xd8130340,
	VPP_DIV_SPI2 = 0xd8130344,
	VPP_DIV_NAND = 0xd8130330,
	VPP_DIV_I2C0 = 0xd813036C,
	VPP_DIV_I2C1 = 0xd8130370,
	VPP_DIV_Keyboard_Pre = 0xd8130318,
	VPP_DIV_Keyboard = 0xd813031C,
	VPP_DIV_NA0 = 0xd8130358,
	VPP_DIV_NA12 = 0xd813035C,
	VPP_DIV_DVO = 0xd8130374,
	VPP_DIV_DDR = 0xd8130310,
	VPP_DIV_HDMI = 0xd8130354,
	VPP_DIV_RingOsc1 = 0xd8130378,
	VPP_DIV_RingOsc2 = 0xd813037C,
	VPP_DIV_LCD = 0xd8130338,
	VPP_DIV_MAX
} vpp_clk_div_t;

#ifdef __KERNEL__
//#include <asm/uaccess.h>
//#include <asm/io.h>
#define DPRINT printk
#else
//#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#define DPRINT printf
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef VPPIF_C
#define EXTERN

const unsigned int vpp_csc_parm[VPP_CSC_MAX][7] = {
	{0x000004a8, 0x04a80662, 0x1cbf1e70, 0x081204a8, 0x00010000, 0x00010001, 0x00000101},	//YUV2RGB_SDTV_0_255
	{0x00000400, 0x0400057c, 0x1d351ea8, 0x06ee0400, 0x00010000, 0x00010001, 0x00000001},	//YUV2RGB_SDTV_16_235
	{0x000004a8, 0x04a8072c, 0x1ddd1f26, 0x087604a8, 0x00010000, 0x00010001, 0x00000101},	//YUV2RGB_HDTV_0_255
	{0x00000400, 0x04000629, 0x1e2a1f45, 0x07440400, 0x00010000, 0x00010001, 0x00000001},	//YUV2RGB_HDTV_16_235
	{0x00000400, 0x0400059c, 0x1d251ea0, 0x07170400, 0x00010000, 0x00010001, 0x00000001},	//YUV2RGB_JFIF_0_255	
	{0x00000400, 0x0400057c, 0x1d351ea8, 0x06ee0400, 0x00010000, 0x00010001, 0x00000001},	//YUV2RGB_SMPTE170M
	{0x00000400, 0x0400064d, 0x1e001f19, 0x074f0400, 0x00010000, 0x00010001, 0x00000001},	//YUV2RGB_SMPTE240M
	{0x02040107, 0x1f680064, 0x01c21ed6, 0x1e8701c2, 0x00211fb7, 0x01010101, 0x00000000},	//RGB2YUV_SDTV_0_255
	{0x02590132, 0x1f500075, 0x020b1ea5, 0x1e4a020b, 0x00011fab, 0x01010101, 0x00000000},	//RGB2YUV_SDTV_16_235
	{0x027500bb, 0x1f99003f, 0x01c21ea6, 0x1e6701c2, 0x00211fd7, 0x01010101, 0x00000000},	//RGB2YUV_HDTV_0_255
	{0x02dc00da, 0x1f88004a, 0x020b1e6d, 0x1e25020b, 0x00011fd0, 0x01010101, 0x00000000},	//RGB2YUV_HDTV_16_235
	{0x02590132, 0x1f530075, 0x02001ead, 0x1e530200, 0x00011fad, 0x00ff00ff, 0x00000000},	//RGB2YUV_JFIF_0_255
	{0x02590132, 0x1f500075, 0x020b1ea5, 0x1e4a020b, 0x00011fab, 0x01010101, 0x00000000},	//RGB2YUV_SMPTE170M
	{0x02ce00d9, 0x1f890059, 0x02001e77, 0x1e380200, 0x00011fc8, 0x01010101, 0x00000000},	//RGB2YUV_SMPTE240M
};

const vpp_timing_t vpp_video_mode_table[] = {
	{	/* 640x480@60 DMT/CEA861 */
	25175000,				/* pixel clock */	
	0, 						/* option */
	96, 48, 640, 16,		/* H sync, bp, pixel, fp */
	2, 33, 480, 10			/* V sync, bp, line, fp */
	},
	{	/* 640x480@60 CVT */
	23750000,				/* pixel clock */				
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	64, 80, 640, 16,		/* H sync, bp, pixel, fp */
	4, 13, 480, 3			/* V sync, bp, line, fp */
	},
	{	/* 640x480@75 DMT */
	31500000,				/* pixel clock */				
	0, 						/* option */
	64, 120, 640, 16,		/* H sync, bp, pixel, fp */
	3, 16, 480, 1			/* V sync, bp, line, fp */
	},
	{	/* 640x480@75 CVT */
	30750000,				/* pixel clock */				
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	64, 88, 640, 24,		/* H sync, bp, pixel, fp */
	4, 17, 480, 3			/* V sync, bp, line, fp */
	},
	{	/* 720x480i@60 CEA861 */
	13514000,				/* pixel clock */				
	VPP_OPT_INTERLACE,		/* option */
	62, 57, 720, 19,		/* H sync, bp, pixel, fp */
	3, 16, 240, 4			/* V sync, bp, line, fp */
	},
	{	/* 720x480i@60 CEA861 */
	13514000,				/* pixel clock */				
	VPP_OPT_INTERLACE,		/* option */
	62, 57, 720, 19,		/* H sync, bp, pixel, fp */
	3, 15, 240, 4			/* V sync, bp, line, fp */
	},
	{	/* 720x480p@60 CEA861 */
	27027060,				/* pixel clock */				
	0,						/* option */
	62, 60, 720, 16,		/* H sync, bp, pixel, fp */
	6, 30, 480, 9			/* V sync, bp, line, fp */
	},
	{	/* 720x576i@50 CEA861 */
	13514000,				/* pixel clock */
	VPP_OPT_INTERLACE,		/* option */
	63, 69, 720, 12,		/* H sync, bp, pixel, fp */
	3, 20, 288, 2			/* V sync, bp, line, fp */
	},
	{	/* 720x576i@50  */ /* Twin mode */
	13514000,				/* pixel clock */
	VPP_OPT_INTERLACE,		/* option */
	63, 69, 720, 12,		/* H sync, bp, pixel, fp */
	3, 19, 288, 2			/* V sync, bp, line, fp */
	},
	{	/* 720x576p@50 CEA861 */
	27000000,				/* pixel clock */				
	0,						/* option */
	64, 68, 720, 12,		/* H sync, bp, pixel, fp */
	5, 39, 576, 5			/* V sync, bp, line, fp */
	},
	{	/* 800x480@60 CVT */
	29500000,				/* pixel clock */				
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	72, 96, 800, 24,		/* H sync, bp, pixel, fp */
	7, 10, 480, 3			/* V sync, bp, line, fp */
	},
	{	/* 800x480@75 CVT */
	38500000,				/* pixel clock */				
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	80, 112, 800, 32,		/* H sync, bp, pixel, fp */
	7, 14, 480, 3			/* V sync, bp, line, fp */
	},
	{	/* 800x600@60 DMT */
	40000000,				/* pixel clock */				
	VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI, 		/* option */
	128, 88, 800, 40,		/* H sync, bp, pixel, fp */
	4, 23, 600, 1			/* V sync, bp, line, fp */
	},
	{	/* 800x600@60 CVT */
	38250000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	80, 112, 800, 32,		/* H sync, bp, pixel, fp */
	4, 17, 600, 3			/* V sync, bp, line, fp */
	},
	{	/* 800x600@75 DMT */
	49500000,				/* pixel clock */
	VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI, 		/* option */
	80, 160, 800, 16,		/* H sync, bp, pixel, fp */
	3, 21, 600, 1			/* V sync, bp, line, fp */
	},
	{	/* 800x600@75 CVT */
	49000000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	80, 120, 800, 40,		/* H sync, bp, pixel, fp */
	4, 22, 600, 3			/* V sync, bp, line, fp */
	},
	{	/* 1024x600@60 DMT */
	49000000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	104, 144, 1024, 40,		/* H sync, bp, pixel, fp */
	10, 11, 600, 3			/* V sync, bp, line, fp */
	},
	{	/* 1024x768@60 DMT */
	65000000,				/* pixel clock */
	0, 						/* option */
	136, 160, 1024, 24,		/* H sync, bp, pixel, fp */
	6, 29, 768, 3			/* V sync, bp, line, fp */
	},
	{	/* 1024x768@60 CVT */
	63500000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	104, 152, 1024, 48,		/* H sync, bp, pixel, fp */
	4, 23, 768, 3			/* V sync, bp, line, fp */
	},
	{	/* 1024x768@75 DMT */
	78750000,				/* pixel clock */
	VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI, 		/* option */
	96, 176, 1024, 16,		/* H sync, bp, pixel, fp */
	3, 28, 768, 1			/* V sync, bp, line, fp */
	},
	{	/* 1024x768@75 CVT */
	82000000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	104, 168, 1024, 64,		/* H sync, bp, pixel, fp */
	4, 30, 768, 3			/* V sync, bp, line, fp */
	},
	{	/* 1152x864@60 CVT */
	81750000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	120, 184, 1152, 64,		/* H sync, bp, pixel, fp */
	4, 26, 864, 3			/* V sync, bp, line, fp */
	},
	{	/* 1152x864@75 DMT */
	108000000,				/* pixel clock */
	VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI, 		/* option */
	128, 256, 1152, 64,		/* H sync, bp, pixel, fp */
	3, 32, 864, 1			/* V sync, bp, line, fp */
	},
	{	/* 1152x864@75 CVT */
	104000000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI, 		/* option */
	120, 192, 1152, 72,		/* H sync, bp, pixel, fp */
	4, 34, 864, 3			/* V sync, bp, line, fp */
	},
	{	/* 1280x720@50 CEA861 */
	74250050,				/* pixel clock */
	VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI,		/* option */
	40, 220, 1280, 440,		/* H sync, bp, pixel, fp */
	5, 20, 720, 5			/* V sync, bp, line, fp */
	},
	{	/* 1280x720@60 CEA861 */
	74250060,				/* pixel clock */
	VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI,		/* option */
	40, 220, 1280, 110,		/* H sync, bp, pixel, fp */
	5, 20, 720, 5			/* V sync, bp, line, fp */
	},
	{	/* 1280x720@60 CVT */
	74500000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	128, 192, 1280, 64,		/* H sync, bp, pixel, fp */
	5, 20, 720, 3			/* V sync, bp, line, fp */
	},
	{	/* 1280x720@75 CVT */
	95750000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	128, 208, 1280, 80,		/* H sync, bp, pixel, fp */
	5, 27, 720, 3			/* V sync, bp, line, fp */
	},
	{	/* 1280x768@60 DMT/CVT */
	79500000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI,	/* option */
	128, 192, 1280, 64,		/* H sync, bp, pixel, fp */
	7, 20, 768, 3			/* V sync, bp, line, fp */
	},
	{	/* 1280x768@75 DMT/CVT */
	102250000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI,	/* option */
	128, 208, 1280, 80,		/* H sync, bp, pixel, fp */
	7, 27, 768, 3			/* V sync, bp, line, fp */
	},
	{	/* 1280x800@60 DMT/CVT */
	83500000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI,	/* option */
	128, 200, 1280, 72,		/* H sync, bp, pixel, fp */
	6, 22, 800, 3			/* V sync, bp, line, fp */
	},
	{	/* 1280x800@75 DMT/CVT */
	106500000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI,	/* option */
	128, 208, 1280, 80,		/* H sync, bp, pixel, fp */
	6, 29, 800, 3			/* V sync, bp, line, fp */
	},
	{	/* 1280x960@60 DMT */
	108000000,				/* pixel clock */
	VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI, 		/* option */
	112, 312, 1280, 96,		/* H sync, bp, pixel, fp */
	3, 36, 960, 1			/* V sync, bp, line, fp */
	},
	{	/* 1280x960@60 CVT */
	101250000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	128, 208, 1280, 80,		/* H sync, bp, pixel, fp */
	4, 29, 960, 3			/* V sync, bp, line, fp */
	},
	{	/* 1280x960@75 CVT */
	130000000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	136, 224, 1280, 88,		/* H sync, bp, pixel, fp */
	4, 38, 960, 3			/* V sync, bp, line, fp */
	},
	{	/* 1280x1024@60 DMT */
	108000000,				/* pixel clock */
	VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI, 		/* option */
	112, 248, 1280, 48,		/* H sync, bp, pixel, fp */
	3, 38, 1024, 1			/* V sync, bp, line, fp */
	},
	{	/* 1280x1024@60 CVT */
	109000000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	136, 216, 1280, 80,		/* H sync, bp, pixel, fp */
	7, 29, 1024, 3			/* V sync, bp, line, fp */
	},
	{	/* 1280x1024@75 DMT */
	135000000,				/* pixel clock */
	VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI, 		/* option */
	144, 248, 1280, 16,		/* H sync, bp, pixel, fp */
	3, 38, 1024, 1			/* V sync, bp, line, fp */
	},
	{	/* 1280x1024@75 CVT */
	138750000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI, /* option */
	136, 224, 1280, 88,		/* H sync, bp, pixel, fp */
	7, 38, 1024, 3			/* V sync, bp, line, fp */
	},
	{	/* 1400x1050@60 DMT/CVT */
	121750000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI,	/* option */
	144, 232, 1400, 88,		/* H sync, bp, pixel, fp */
	4, 32, 1050, 3			/* V sync, bp, line, fp */
	},
	{	/* 1400x1050@60+R DMT/CVT */
	101000000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI,	/* option */
	32, 80, 1400, 48,		/* H sync, bp, pixel, fp */
	4, 23, 1050, 3			/* V sync, bp, line, fp */
	},
	{	/* 1440x480i@60 CEA861 */
	27000000,				/* pixel clock */
	VPP_OPT_INTERLACE,		/* option */
	124, 114, 1440, 38,		/* H sync, bp, pixel, fp */
	3, 16, 240, 4			/* V sync, bp, line, fp */
	},
	{	/* 1440x480i@60  */ /* Twin mode */
	27000000,				/* pixel clock */
	VPP_OPT_INTERLACE,		/* option */
	124, 114, 1440, 38,		/* H sync, bp, pixel, fp */
	3, 15, 240, 4			/* V sync, bp, line, fp */
	},
	{	/* 1440x480p@60 CEA861 */
	54054000,				/* pixel clock */
	0,						/* option */
	124, 120, 1440, 32,		/* H sync, bp, pixel, fp */
	6, 30, 480, 9			/* V sync, bp, line, fp */
	},
	{	/* 1440x576i@50 CEA861 */
	27000000,				/* pixel clock */
	VPP_OPT_INTERLACE,		/* option */
	126, 138, 1440, 24,		/* H sync, bp, pixel, fp */
	3, 20, 288, 2			/* V sync, bp, line, fp */
	},
	{	/* 1440x576i@50  */ /* Twin mode */
	27000000,				/* pixel clock */
	VPP_OPT_INTERLACE,		/* option */
	126, 138, 1440, 24,		/* H sync, bp, pixel, fp */
	3, 19, 288, 2			/* V sync, bp, line, fp */
	},
	{	/* 1440x900@60 DMT/CVT */
	106500000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI,	/* option */
	152, 232, 1440, 80,		/* H sync, bp, pixel, fp */
	6, 25, 900, 3			/* V sync, bp, line, fp */
	},
	{	/* 1440x900@75 DMT/CVT */
	136750000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI,	/* option */
	152, 248, 1440, 96,		/* H sync, bp, pixel, fp */
	6, 33, 900, 3			/* V sync, bp, line, fp */
	},
	{	/* 1600x1200@60+R DMT/CVT */
	162000000,				/* pixel clock */
	VPP_VGA_VSYNC_POLAR_HI,	/* option */
	192, 304, 1600, 64,		/* H sync, bp, pixel, fp */
	3, 46, 1200, 1			/* V sync, bp, line, fp */
	},
	{	/* 1920x1080p@25 CEA861 */
	74250025,				/* pixel clock */
	VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI,	/* option */
	44, 148, 1920, 528,		/* H sync, bp, pixel, fp */
	5, 36, 1080, 4			/* V sync, bp, line, fp */
	},
	{	/* 1920x1080p@30 CEA861 */
	74250030,				/* pixel clock */
	VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI,						/* option */
	44, 148, 1920, 88,		/* H sync, bp, pixel, fp */
	5, 36, 1080, 4			/* V sync, bp, line, fp */
	},
	{	/* 1920x1080i@50 CEA861 */
	74250050,				/* pixel clock */
	VPP_OPT_INTERLACE+VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI,	/* option */
	44, 148, 1920, 528,		/* H sync, bp, pixel, fp */
	5, 16, 540, 2			/* V sync, bp, line, fp */
	},
	{	/* 1920x1080i@50  */ /* Twin mode */
	74250050,				/* pixel clock */
	VPP_OPT_INTERLACE+VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI,	/* option */
	44, 148, 1920, 528,		/* H sync, bp, pixel, fp */
	5, 15, 540, 2			/* V sync, bp, line, fp */
	},
	{	/* 1920x1080i@60 CEA861 */
	74250060,				/* pixel clock */
	VPP_OPT_INTERLACE+VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI,	/* option */
	44, 148, 1920, 88,		/* H sync, bp, pixel, fp */
	5, 16, 540, 2			/* V sync, bp, line, fp */
	},
	{	/* 1920x1080i@60  */ /* Twin mode */
	74250060,				/* pixel clock */
	VPP_OPT_INTERLACE+VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI,	/* option */
	44, 148, 1920, 88,		/* H sync, bp, pixel, fp */
	5, 15, 540, 2			/* V sync, bp, line, fp */
	},
	{	/* 1920x1080p@60  */
	148500000,				/* pixel clock */
	VPP_VGA_HSYNC_POLAR_HI+VPP_VGA_VSYNC_POLAR_HI,	/* option */
	44, 148, 1920, 88,		/* H sync, bp, pixel, fp */
	5, 36, 1080, 4			/* V sync, bp, line, fp */
	},
	{ 0 }
};

const char *vpp_vout_str[] = {"SDA","SDD","LCD","DVI","HDMI","DVO2HDMI","DVO","VGA"};
unsigned int vpp_vo_boot_arg[6] = {0xFF};
unsigned int vpp_vo_boot_arg2[3] = {0xFF};

#else
#define EXTERN extern

extern const unsigned int vpp_csc_parm[VPP_CSC_MAX][7];
extern char *vpp_colfmt_str[];
extern const vpp_timing_t vpp_video_mode_table[];
extern const char *vpp_vout_str[];
extern unsigned int vpp_vo_boot_arg[6];
extern unsigned int vpp_vo_boot_arg2[3];

#endif

EXTERN vpp_info_t g_vpp;
EXTERN int vpp_dbg_msg_enable;

//Internal functions
EXTERN int get_key(void);
EXTERN U8 vppif_reg8_in(U32 offset);
EXTERN U8 vppif_reg8_out(U32 offset, U8 val);
EXTERN U16 vppif_reg16_in(U32 offset);
EXTERN U16 vppif_reg16_out(U32 offset, U16 val);
EXTERN U32 vppif_reg32_in(U32 offset);
EXTERN U32 vppif_reg32_out(U32 offset, U32 val);
EXTERN U32 vppif_reg32_write(U32 offset, U32 mask, U32 shift, U32 val);
EXTERN U32 vppif_reg32_read(U32 offset, U32 mask, U32 shift);
EXTERN U32 vppif_reg32_mask(U32 offset, U32 mask, U32 shift);
EXTERN int vpp_check_dbg_level(vpp_dbg_level_t level);
EXTERN void vpp_set_dbg_gpio(int no,int value);

//Export functions
EXTERN void vpp_mod_unregister(vpp_mod_t mod);
EXTERN vpp_mod_base_t *vpp_mod_register(vpp_mod_t mod,int size,unsigned int flags);
EXTERN vpp_mod_base_t *vpp_mod_get_base(vpp_mod_t mod);
EXTERN vpp_fb_base_t *vpp_mod_get_fb_base(vpp_mod_t mod);
EXTERN vdo_framebuf_t *vpp_mod_get_framebuf(vpp_mod_t mod);
EXTERN void vpp_mod_set_timing(vpp_mod_t mod,vpp_timing_t *tmr);
EXTERN void vpp_mod_init(void);

EXTERN unsigned int vpp_get_base_clock(vpp_clk_pll_t pll,vpp_clk_div_t div);
EXTERN unsigned int vpp_set_base_clock(vpp_mod_t mod,unsigned int pixel_clock);
EXTERN void vpp_set_video_scale(vdo_view_t *vw);
EXTERN int vpp_set_recursive_scale(vdo_framebuf_t *src_fb,vdo_framebuf_t *dst_fb);
EXTERN vpp_display_format_t vpp_get_fb_field(vdo_framebuf_t *fb);
EXTERN void vpp_wait_vsync(void);
EXTERN int vpp_proc_flag(vpp_int_t type,int *flag,int wait);
EXTERN int vpp_proc_func(void (*func)(void *argc),void *arg,vpp_int_t type,	int wait);
EXTERN int vpp_get_gcd(int A, int B);
EXTERN unsigned int vpp_calculate_diff(unsigned int val1,unsigned int val2);
EXTERN void vpp_check_scale_ratio(int *src,int *dst,int max,int min);
EXTERN void vpp_calculate_timing(vpp_mod_t mod,unsigned int fps,vpp_clock_t *tmr);
EXTERN void vpp_fill_framebuffer(vdo_framebuf_t *fb,unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int color);
EXTERN vpp_csc_t vpp_check_csc_mode(vpp_csc_t mode,vdo_color_fmt src_fmt,vdo_color_fmt dst_fmt,unsigned int flags);
EXTERN void vpp_trans_timing(vpp_mod_t mod,vpp_timing_t *tmr,vpp_clock_t *hw_tmr,int to_hw);
EXTERN void vpp_fill_pattern(vpp_mod_t mod,int no,int arg);
EXTERN unsigned int vpp_get_vmode_pixel_clock(unsigned int resx,unsigned int resy,unsigned int fps);
EXTERN vpp_timing_t *vpp_get_video_mode(unsigned int resx,unsigned int resy,unsigned int pixel_clock);
EXTERN void vpp_set_video_quality(int mode);

#ifdef __KERNEL__
/* dev-vpp.c */
EXTERN void vpp_get_info(struct fb_var_screeninfo *var);
EXTERN int vpp_config(struct fb_info *info);
EXTERN int vpp_mmap(struct vm_area_struct *vma);
EXTERN int vpp_ioctl(unsigned int cmd,unsigned long arg);
EXTERN int vpp_dev_init(void);
EXTERN int vpp_i2c_write(unsigned int addr,unsigned int index,char *pdata,int len);
EXTERN int vpp_i2c_read(unsigned int addr,unsigned int index,char *pdata,int len);
EXTERN int vpp_suspend(int state);
EXTERN int vpp_resume(void);
EXTERN unsigned int *vpp_backup_reg(unsigned int addr,unsigned int size);
EXTERN int vpp_restore_reg(unsigned int addr,unsigned int size,unsigned int *reg_ptr);

#endif

#ifndef __KERNEL__
void vpp_initialization(int FunctionNumber);
#endif

#undef EXTERN

#ifdef __cplusplus
}
#endif
#endif				//VPPIF_H
