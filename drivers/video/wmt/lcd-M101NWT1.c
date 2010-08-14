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
#define LCD_CHILIN_LW700AT9003_C

/*=== lcd-oem.c =============================================================
*
* MODULE       : lcd-CHILIN-LW700AT9003.c
* AUTHOR       : Sam Shen
* DATE         : 2009/4/28
*-----------------------------------------------------------------------------*/

/*--------------------- History -----------------------------------------------* 
Version 0.01 , Sam Shen, 2009/4/28
	First version

------------------------------------------------------------------------------*/
/*----------------------- DEPENDENCE -----------------------------------------*/
#include <linux/kernel.h>
#include <linux/slab.h>
#include "lcd.h"

/*----------------------- PRIVATE MACRO --------------------------------------*/
/* #define  LCD_LW700AT9003_XXXX  xxxx    *//*Example*/

#define DEBUG
#ifdef __KERNEL__
#define DPRINT		printk
#else
#define DPRINT		printf
#endif

#ifdef DEBUG
#define DBGMSG(fmt, args...)   DPRINT("[LW700AT9003] %s: " fmt, __FUNCTION__ , ## args)
#else
#define DBGMSG(fmt, args...)	do {} while(0)
#endif
/*----------------------- PRIVATE CONSTANTS ----------------------------------*/
/* #define LCD_LW700AT9003_XXXX    1     *//*Example*/

/*----------------------- PRIVATE TYPE --------------------------------------*/
/* typedef  xxxx lcd_xxx_t; *//*Example*/

/*----------EXPORTED PRIVATE VARIABLES are defined in lcd.h  -------------*/
static void lcd_m101nwt1_initial(void);
static void lcd_m101nwt1_uninitial(void);

/*----------------------- INTERNAL PRIVATE VARIABLES ------------------------*/
/* int  lcd_xxx;        *//*Example*/
lcd_parm_t lcd_m101nwt1_parm = {
	.name = "CHILIN M101NWT1",
	.fps = 48,						/* frame per second */
	.bits_per_pixel = 16,
	.capability = 0,
	.timing = {
		.pixel_clock = 25000000,	/* pixel clock */
		.option = 0,				/* option flags */

		.hsync = 159,				/* horizontal sync pulse */
		.hbp = 159,					/* horizontal back porch */
		.hpixel = 1024,				/* horizontal pixel */
		.hfp = 2,					/* horizontal front porch */

		.vsync = 18,					/* vertical sync pulse */
		.vbp = 18,					/* vertical back porch */
		.vpixel = 576,				/* vertical pixel */
		.vfp = 2,					/* vertical front porch */
	},
	
	.initial = lcd_m101nwt1_initial,
	.uninitial = lcd_m101nwt1_uninitial,		
};

/*--------------------- INTERNAL PRIVATE FUNCTIONS ---------------------------*/
/* void lcd_xxx(void); *//*Example*/

/*----------------------- Function Body --------------------------------------*/
static void lcd_m101nwt1_initial(void)
{	
	DPRINT("lcd_m101nwt1_initial\n");
	/*REG32_VAL(0xd8110064) |= 0x00000004;
	REG32_VAL(0xd811008c) |= 0x00000004;
	REG32_VAL(0xd81100B4) |= 0x00000004;*/
	
	/* TODO */
}

static void lcd_m101nwt1_uninitial(void)
{	
	DPRINT("lcd_m101nwt1_initial\n");
	/*REG32_VAL(0xd8110064) |= 0x00000004;
	REG32_VAL(0xd811008c) |= 0x00000004;
	REG32_VAL(0xd81100B4) |= 0x00000004;*/
	
	/* TODO */
}

lcd_parm_t *lcd_m101nwt1_get_parm(int arg) 
{	
	return &lcd_m101nwt1_parm;
}

static int lcd_m101nwt1_init(void){	
	int ret;	

	ret = lcd_panel_register(LCD_M101NWT1,(void *) lcd_m101nwt1_get_parm);	
	return ret;
} /* End of lcd_oem_init */
module_init(lcd_m101nwt1_init);

/*--------------------End of Function Body -----------------------------------*/
#undef LCD_CHILIN_LW700AT9003_C

