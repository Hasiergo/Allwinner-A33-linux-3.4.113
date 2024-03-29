/*
 * drivers/input/touchscreen/gslX680.c
 *
 * Copyright (c) 2012 Shanghai Basewin
 *	Guan Yuwei<guanyuwei@basewin.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/pm_runtime.h>
#if defined(CONFIG_HAS_EARLYSUSPEND)
#include <linux/earlysuspend.h>
#endif
#include <linux/input/mt.h>

#include <linux/i2c.h>
#include <linux/input.h>

#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/async.h>
#include <linux/hrtimer.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/device.h>
#include <mach/irqs.h>
#include <mach/hardware.h>
#include <mach/sys_config.h>
#include <linux/gpio.h>

#include <linux/init-input.h>

#include "gslX680NEW.h"
#include "GslChipAjust.h"

//D71 OGS
#include "D71_GSL1688E_OGS_DZ_70L1.h"
//#include "D71_GSL1688E_OGS_DZ_70E2.h"
#include "D71_GSL168XX_OGS_6001024_DZ_70E2.h"

#include "D71_GSL1688E_OGS_DZ_70L1_800480.h"
//#include "D71_GSL1688E_OGS_DZ_70E2_800480.h"
#include "D71_GSL168XX_OGS_480800_DZ_70E2.h"

//D71 PG
//#include "D71_GSL1688E_PG_INET_1024600.h"
//#include "D71_GSL1688E_PG_INET_800480.h"
#include "D71_GSL168XX_PG_6001024_INET.h"
#include "D71_GSL168XX_PG_480800_INET.h"


//D791 OGS
#include "D791_GSL915_OGS_DZ_79D2.h"
#include "D791_GSL915_OGS_DZ_79D4.h"
#include "D791_GSL915_1024768_GF_QSD.h"
#include "D791_GSL915_1024768_OGS_JND.h"
#include "D791_GSL915_1024768_OGS_SG5908A.h"

//D72E OGS
#include "D72E_GSL1688E_OGS_DZ_70L1.h"
#include "D72E_GSL1688E_OGS_DZ_70E2.h"
#include "D72E_GSL1688E_OGS_DZ_70L1_800480.h"
#include "D72E_GSL1688E_OGS_DZ_70E2_800480.h"

//D72E PG
#include "D72E_GSL1688E_PG_CTD_1024600.h"
#include "D72E_GSL1688E_PG_CTD_800480.h"


//D102
#include "D102_GSL3675B_GG_DH_1024600.h"
#include "D102_GSL3675B_GG_QSD_1024600.h"
#include "D102_GSL3675B_PG_DH_1024600.h"
#include "D102_GSL3675B_PG_QSD_1024600.h"
#include "D102_GSL3675B_PG_LHJ_1024600.h"
#include "D102_GSL3675B_PG_DPT_1024600.h"
#include "D102_GSL3675B_OGS_SG_1024600.h"
#include "D102_GSL3675B_PG_XLL_1024600.h"
#include "U102C_GSL3675B_1024600_GG_FC.h"
//#include "A791_gsl2682_1280800_PG_ZJX_79A40.h"
#include "U791_gsl2682_7681024_PG_ZJX_79A40.h"


//D92
#include "D92_GSL3675B_GG_QSD_800480.h"
#include "D92_GSL3675B_PG_800480_DH.h"
#include "D92_GSL3675B_GG_800480_DH.h"
#include "D92_GSL3675B_PG_800480_QSD.h"
#include "D92_GSL3675B_PG_800480_LHJ.h"
#include "D92_GSL3675B_PG_FC_800480.h"

#include "D92_GSL3675B_GG_DH_1024600.h"
#include "D92_GSL3675B_PG_DH_1024600.h"
#include "D92_GSL3675B_GG_1024600_QSD.h"
#include "D92_GSL3675B_PG_1024600_QSD.h"
#include "D92_GSL3675B_PG_LHJ_1024600.h"
#include "D92_GSL3675B_PG_FC_1024600.h"
#include "U92_GSL3675B_PG_XLL_1024600.h"

#include "U92C_GSL3675B_GG_6001024_QSD.h"

//U92
#include "U92_GSL3675B_PG_DH_1024600.h"

//D69
#include "D69_GSL2682B_OGS_DZ.h"

//D703
#include "D703HC_GSL1688E_1024600_PG_CTD.h"
#include "D703C_GSL1688E_800480_PG_CTD.h"

//D70T
#include "D70T_GSL1680E_PG_SG_1024600.h"
#include "D70T_GSL1680E_PG_SG_800480.h"

//D103
#include "D100_GSL3675B_GG_DH_1024600.h"
#include "D100_GSL3675B_PG_YTG_1024600.h"
#include "D100_GSL3675B_GG_QSD_1024600.h"
#include "D100_GSL3675B_PG_QSD_1024600.h"
#include "D100_GSL3675B_PG_DPT_1024600.h"


//D100Z
#include "D100Z_GSL3675B.h"

//U108C
#include "U108C_GSL3675B_8001280_GG_RSS.h"
#include "U108C_GSL3675B_8001280_PG_XLL.h"


//U100
#include "U100_GSL3675B_6001024_GG_QLT.h"


//D970
#include "D970_GSL3675B_GG_QSD_1024768.h"

//D705
#include "D705_GSL1681E_PG_SG_800480.h"
#include "D705_GSL1681E_PG_CTD_800480.h"
#include "D705_GSL1681E_PG_DPT_800480.h"
#include "D705_GSL1681E_PG_QLT_800480.h"

//D78
#include "D78_GSL1680E_PG_QLT_800480.h"
#include "D78_GSL1680E_PG_6001024_GT_70Q88001V2.h"
#include "U78_GSL1686F_1024600_PG_QCY070157.h"

//D81
#include "D81_GSL3675B_PG_800480_CLGD.h"

//D90 800480
#include "D90_GSL3675B_PG_800480_DPT.h"
#include "D90_GSL3675B_PG_800480_FC.h"

//D90 1024600
#include "D90_GSL3675B_PG_1024600_DPT.h"

//D702
#include "D702C_GSL1688E_PG_DPT_1024600.h"

//86DEZ_D701EB2C
#include "D701EB2C_GSL1680E_PG_INET_800480.h"

//D101
#include "D101_GSL3675B_1024600.h"

//D798
#include "D798_GSL915_7681024_OGS_DZ.h"
#include "U798_GSL915_7681024_OGS_XCL.h"
#include "U798_GSL915_7681024_OGS_SG.h"

//D791C-A33

//PG
#include "D791C_GSL915_1024768_PG_QSD.h"
#include "D791C_GSL915_7681024_CTD_PG.h"
#include "D791C_GSL915_1024768_PG_XLL.h"
#include "D791C_GSL915_1024768_PG_YCGD.h"


//GG
#include "D791C_GSL915_1024768_GG_CTD.h"



//OGS
#include "D791C_GSL915_1024768_OGS_DZ_79D4.h"
#include "D791C_GSL915_1024768_OGS_SG5908A.h"

//U70
//#include "U70_GSL1680E_480800_PG_INET.h"
#include "U701C_GSL168XX_PG_480800_INET.h"
//#include "U70_GSL1680E_6001024_PG_INET.h"
#include "U701C_GSL168XX_PG_6001024_INET.h"

//U906C-A33
#include "U906C_GSL3675B_8001280_GG_RSS.h"

//U680C-A33
#include "U680C_GSL3676B_1280800_OGS_DZ.h"
#include "U680_GSL3676B_1280800_OGS_XCL.h"
#include "U680_GSL3676B_1280800_OGS_SG.h"
#include "U680C_GSL3676B_PG_FC80J115_8001280_FUWOTE.h"

#include "A791_gsl2682_1280800_PG_getu.h"
//U430-A33
#include "U430_GSL1680E_480800_PG_YJ.h"

//U106-A83-10.6 1366x768
#include "N106_GSL3692_1366768_PG_XLL_XN_1572.h"
#include "U106_GSL3692_1366768_GG_FC_FC101S124.h"
#include "N106_GSL3692_1280800_PG_FC_FC101S123.h"
#include "N106_GSL3692_8001280_GG_FC_FC101S123.h"
#include "N106_GSL3692_8001280_PG_FC_FC101S123.h"
#include "N106_GSL3692_8001280_PG_FC_FC101S124.h"
#include "N106_GSL3692_8001280_GG_HK_10DR2590.h"
#include "N106_GSL3692_1280800_PG_ZYD10151V02.h"
#include "N106_GSL3692_1280800_PG_XLL_XN1582AB.h"
//==============Customer============
#include "Customer_info.h"
//#include "Customer_D71_GSL1688E_OGS_SG.h"
//#include "Customer_D71_GSL1688E_OGS_SG_800480.h"
//#include "Customer_D71_GSL1688E_PG_DPT_1024600.h"
//#include "Customer_D71_GSL1688E_PG_DX_1024600.h"
//#include "Customer_D71_GSL1688E_PG_DX_800480.h"
//#include "Customer_D71_GSL1688E_OGS_CTD_1024600.h"
//#include "Customer_D71_GSL1688E_1024600_OGS_JND.h"
//#include "Customer_D71_GSL1688E_800480_OGS_JND.h"
//#include "Customer_D71_GSL1688E_PG_OPD_800480_JUBAOSHI.h"
//#include "Customer_D71_GSL1688E_PG_OPD_1024600.h"
//#include "Customer_D71_GSL1688E_1024600_OGS_XCL_GERUIBANG.h"
//#include "Customer_D71_GSL1688E_PG_RUIFU_HANGTAI_1024600.h"
//#include "Customer_D71_GSL1688E_PG_RUIFU_FEICHU_1024600.h"
//#include "Customer_U71_GSL1680E_PG_PB_800480_SHUOYING.h"
//#include "Customer_U100_GSL3675B_6001024_GG_JX_GERUIBANG.h"
//#include "Customer_U100_GSL3675B_6001024_GG_MR_YAZHONGXUN.h"
//#include "Customer_U102_GSL3675B_GG_FC_1024600_RUIFULIANKE.h"
//#include "Customer_U102_GSL3675B_PG_WT_1024600_MINGZHI.h"
//#include "Customer_U98_GSL3675B_6001024_PG_MR_KAIMEISHUMA.h"
//#include "Customer_U98_GSL3675B_6001024_PG_YC_KAIMEISHUMA.h"
//#include "Customer_D92_GSL3675B_PG_DH_1024600_JURENKEJI.h"
//#include "Customer_U92_GSL3675B_PG_FC_1024600_SHUOYING.h"
//#include "Customer_U100Z_GSL3675B_6001024_GG_YTG_FUWOTE.h"
//#include "Customer_U100Z_GSL3675B_6001024_GG_YLD_DAHUADIANQI.h"
//#include "Customer_U100Z_GSL3675B_6001024_GG_YTG_FUWOTE_SSQ_20140923.h"
//#include "Customer_U90_GSL3675B_PG_1024600_CTD_YIPUDA.h"
//#include "Customer_U701C_GSL1680E_PG_JX_480800_GERUIBANG.h"
//#include "Customer_U701C_GSL1680E_PG_FXDZ_6001024_LIANXINTAI.h"
//#include "Customer_U791C_GSL915_1024768_PG_YC_MINGZHI.h"
//#include "Customer_U791_GSL915_1024768_OGS_DZ_79D4_PUNAIER.h"
//#include "Customer_U680C_GSL3676B_PG_FC_8001280_PUNAIER.h"

extern void tp_light_ctrl(int i);

#ifdef GSL_ID_VERSION_INFO
extern unsigned int gsl_mask_tiaoping(void);
extern unsigned int gsl_version_id(void);
extern void gsl_alg_id_main(struct gsl_touch_info *cinfo);
extern void gsl_DataInit(int *ret);
extern void gsl_ReportPressure(unsigned int *p);

#define GSL_DATAINIT			gsl_DataInit
#define GSL_VERSION_ID			gsl_version_id
#define GSL_ALG_ID_MAIN			gsl_alg_id_main
#define GSL_MASK_TIAOPING		gsl_mask_tiaoping
#else
extern unsigned int gsl_mask_tiaoping(void);
extern unsigned int gsl_version_id(void);
extern void gsl_alg_id_main(struct gsl_touch_info *cinfo);
extern void gsl_DataInit(int *ret);

#define GSL_DATAINIT			gsl_DataInit
#define GSL_VERSION_ID			gsl_version_id
#define GSL_ALG_ID_MAIN			gsl_alg_id_main
#define GSL_MASK_TIAOPING		gsl_mask_tiaoping

#endif

//mbg ++ 20131230
//#define GSL_MONITOR //This is for ESD

#ifdef GSL_MONITOR
static struct delayed_work gsl_monitor_work;
static struct workqueue_struct *gsl_monitor_workqueue = NULL;
static char int_1st[4] = {0};
static char int_2nd[4] = {0};
static char dac_counter = 0;
static struct i2c_client *gsl_client = NULL;
#endif 

//mbg --

static int ctp_cob_gslX680_old = 0;
#define FOR_TSLIB_TEST
//#define GSL_TIMER
//#define PRINT_POINT_INFO 
//#define REPORT_DATA_ANDROID_4_0
static u32 gslX680_debug_mask = 1;

#define HAVE_TOUCH_KEY

//#define gsl_resume_wq_mode

#define SCREEN_MAX_X		(screen_max_x)
#define SCREEN_MAX_Y		(screen_max_y)

#define GSLX680_I2C_NAME 	"gslX680"
//#define GSLX680_I2C_ADDR 	0x40

#define TPD_PROC_DEBUG
#ifdef TPD_PROC_DEBUG
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
static struct proc_dir_entry *gsl_config_proc = NULL;
#define GSL_CONFIG_PROC_FILE "gsl_config"
#define CONFIG_LEN 31
static char gsl_read[CONFIG_LEN];
static u8 gsl_data_proc[8] = {0};
static u8 gsl_proc_flag = 0;
#endif



#define GSL_DATA_REG		0x80
#define GSL_STATUS_REG		0xe0
#define GSL_PAGE_REG		0xf0

#define PRESS_MAX    			255
#define MAX_FINGERS 		10//5 //最大手指个数
#define MAX_CONTACTS 		10
#define DMA_TRANS_LEN		0x20

#define PHO_CFG2_OFFSET	(0X104)
#define PHO_DAT_OFFSET		(0X10C)
#define PHO_PULL1_OFFSET	(0X11C)
#define GPIOF_CON			0x7f0080a0
#define GPIOF_DAT			0x7f0080a4
#define GPIOF_PUD			0x7f0080a8

#define GSL_NOID_VERSION
static char cfg_adjust_flag = 0;
static int cfg_adjust_used_id = 0;

#ifdef HAVE_TOUCH_KEY       
static u16 key = 0;
static int key_state_flag = 0;
struct key_data {
	u16 key;
	u16 x_min;
	u16 x_max;
	u16 y_min;
	u16 y_max;	
};

#define KEY_BACK	158
#define KEY_HOME	172
#define KEY_MENU	139
#define KEY_SEARCH	217

const u16 key_array[]={
                                      KEY_BACK,
                                      KEY_HOME,
                                      KEY_MENU,
                                      KEY_SEARCH,
                                     }; 
#define MAX_KEY_NUM     (sizeof(key_array)/sizeof(key_array[0]))

struct key_data gsl_key_data[MAX_KEY_NUM] = {
	{KEY_BACK, 2048, 2048, 2048, 2048},
	{KEY_HOME, 250, 350, 1025, 1050},	
	{KEY_MENU, 2048, 2048, 2048, 2048},
	{KEY_SEARCH, 2048, 2048, 2048, 2048},
};
#endif

struct gsl_ts_data {
	u8 x_index;
	u8 y_index;
	u8 z_index;
	u8 id_index;
	u8 touch_index;
	u8 data_reg;
	u8 status_reg;
	u8 data_size;
	u8 touch_bytes;
	u8 update_data;
	u8 touch_meta_data;
	u8 finger_size;
};

static struct gsl_ts_data devices[] = {
	{
		.x_index = 6,
		.y_index = 4,
		.z_index = 5,
		.id_index = 7,
		.data_reg = GSL_DATA_REG,
		.status_reg = GSL_STATUS_REG,
		.update_data = 0x4,
		.touch_bytes = 4,
		.touch_meta_data = 4,
		.finger_size = 70,
	},
};

struct gsl_ts {
	struct i2c_client *client;
	struct input_dev *input;
	struct work_struct work;
	struct workqueue_struct *wq;
	struct gsl_ts_data *dd;
	u8 *touch_data;
	u8 device_id;
	u8 prev_touches;
	bool is_suspended;
	bool int_pending;
	struct mutex sus_lock;
	int irq;
#if defined(CONFIG_HAS_EARLYSUSPEND)
	struct early_suspend early_suspend;
#endif
#ifdef GSL_TIMER
	struct timer_list gsl_timer;
#endif

};



static u32 id_sign[MAX_CONTACTS+1] = {0};
static u8 id_state_flag[MAX_CONTACTS+1] = {0};
static u8 id_state_old_flag[MAX_CONTACTS+1] = {0};
static u16 x_old[MAX_CONTACTS+1] = {0};
static u16 y_old[MAX_CONTACTS+1] = {0};
static u16 x_new = 0;
static u16 y_new = 0;


///////////////////////////////////////////////
//specific tp related macro: need be configured for specific tp

#define CTP_IRQ_NUMBER                  (config_info.int_number)
#define CTP_IRQ_MODE			(IRQF_TRIGGER_FALLING)
#define CTP_NAME			GSLX680_I2C_NAME
#define SCREEN_MAX_X		        (screen_max_x)
#define SCREEN_MAX_Y		        (screen_max_y)

#define GSLX680_I2C_ADDR 0x40
						 
#define GSLX680_USED     "\n \
												  \n+++++++++++++++++++++++++++++++++ \
												  \n++++++ GSLX680 new used +++++++++ \
												  \n+++++++++++++++++++++++++++++++++ \
												  \n"
													 
#define GSLX680_IC_INFO  "\n============================================================== \
												  \nIC	 :GSLX680 \
												  \nAUTHOR :mbgalex@163.com \
												  \nVERSION:2014-02-10_17:13\n"

												  

//static int gsl_chipType_new = 0;
static int ctp_cob_gslX680 = 0;
static int screen_max_x = 0;
static int screen_max_y = 0;
static int revert_x_flag = 0;
static int revert_y_flag = 0;
static int exchange_x_y_flag = 0;

//mbg ++ 20131126
static int fw_index = -1;
struct gslX680_fw_array {
	unsigned int size;
	const struct fw_data *fw;
} gslx680_fw_grp[] = {
	{ARRAY_SIZE(Inet_Customer),Inet_Customer},//0

	{ARRAY_SIZE(D71_GSL1688E_OGS_DZ_70L1),D71_GSL1688E_OGS_DZ_70L1},// 1
	{ARRAY_SIZE(D71_GSL1688E_OGS_DZ_70L1_800480),D71_GSL1688E_OGS_DZ_70L1_800480},// 2
	
	//{ARRAY_SIZE(D71_GSL1688E_OGS_DZ_70E2),D71_GSL1688E_OGS_DZ_70E2},// 3
	{ARRAY_SIZE(D71_GSL168XX_OGS_6001024_DZ_70E2),D71_GSL168XX_OGS_6001024_DZ_70E2},// 3
	//{ARRAY_SIZE(D71_GSL1688E_OGS_DZ_70E2_800480),D71_GSL1688E_OGS_DZ_70E2_800480}, // 4
	{ARRAY_SIZE(D71_GSL168XX_OGS_480800_DZ_70E2),D71_GSL168XX_OGS_480800_DZ_70E2}, // 4
	
	{ARRAY_SIZE(D72E_GSL1688E_OGS_DZ_70L1),D72E_GSL1688E_OGS_DZ_70L1},// 5
	{ARRAY_SIZE(D72E_GSL1688E_OGS_DZ_70L1_800480),D72E_GSL1688E_OGS_DZ_70L1_800480}, // 6
	
	{ARRAY_SIZE(D72E_GSL1688E_OGS_DZ_70E2),D72E_GSL1688E_OGS_DZ_70E2},// 7
	{ARRAY_SIZE(D72E_GSL1688E_OGS_DZ_70E2_800480),D72E_GSL1688E_OGS_DZ_70E2_800480},// 8
	
	//{ARRAY_SIZE(D71_GSL1688E_PG_INET),D71_GSL1688E_PG_INET},// 9
	{ARRAY_SIZE(D71_GSL168XX_PG_6001024_INET),D71_GSL168XX_PG_6001024_INET},// 9
	//{ARRAY_SIZE(D71_GSL1688E_PG_INET_800480),D71_GSL1688E_PG_INET_800480}, // 10
	{ARRAY_SIZE(D71_GSL168XX_PG_480800_INET),D71_GSL168XX_PG_480800_INET}, // 10

	{ARRAY_SIZE(D791_GSL915_OGS_DZ_79D2),D791_GSL915_OGS_DZ_79D2},// 11
	{ARRAY_SIZE(FW_D791C_GSL915_1024768_OGS_DZ_79D4),FW_D791C_GSL915_1024768_OGS_DZ_79D4}, // 12
	{ARRAY_SIZE(FW_D791C_GSL915_1024768_OGS_SG5908A),FW_D791C_GSL915_1024768_OGS_SG5908A}, // 13
	{ARRAY_SIZE(FW_D791_GSL915_1024768_OGS_JND),FW_D791_GSL915_1024768_OGS_JND}, // 14
	{ARRAY_SIZE(D791_GSL915_1024768_GF_QSD),D791_GSL915_1024768_GF_QSD}, // 15

	{ARRAY_SIZE(D702C_GSL1688E_DPT_PG),D702C_GSL1688E_DPT_PG},// 16

	{ARRAY_SIZE(D72E_GSL1688E_PG_CTD_1024600),D72E_GSL1688E_PG_CTD_1024600},// 17
	{ARRAY_SIZE(D72E_GSL1688E_PG_CTD_800480),D72E_GSL1688E_PG_CTD_800480},// 18

	{ARRAY_SIZE(D102_GSL3675B_GG_QSD_1024600),D102_GSL3675B_GG_QSD_1024600}, // 19
	{ARRAY_SIZE(D102_GSL3675B_GG_DH_1024600),D102_GSL3675B_GG_DH_1024600},// 20
	{ARRAY_SIZE(D102_GSL3675B_PG_DH_1024600),D102_GSL3675B_PG_DH_1024600},// 21
	{ARRAY_SIZE(D102_GSL3675B_PG_QSD_1024600),D102_GSL3675B_PG_QSD_1024600},// 22
	{ARRAY_SIZE(D102_GSL3675B_PG_LHJ_1024600),D102_GSL3675B_PG_LHJ_1024600},// 23
	{ARRAY_SIZE(D102_GSL3675B_PG_DPT_1024600),D102_GSL3675B_PG_DPT_1024600},// 24
	{ARRAY_SIZE(D102_GSL3675B_OGS_SG_1024600),D102_GSL3675B_OGS_SG_1024600},// 25


	
	{ARRAY_SIZE(D92_GSL3675B_GG_QSD_800480),D92_GSL3675B_GG_QSD_800480}, // 26
	{ARRAY_SIZE(D92_GSL3675B_PG_800480_DH),D92_GSL3675B_PG_800480_DH},//27
	{ARRAY_SIZE(FW_D92_GSL3675B_GG_800480_DH),FW_D92_GSL3675B_GG_800480_DH},// 28
	{ARRAY_SIZE(FW_D92_GSL3675B_PG_800480_QSD),FW_D92_GSL3675B_PG_800480_QSD},// 29
	{ARRAY_SIZE(FW_D92_GSL3675B_PG_800480_LHJ),FW_D92_GSL3675B_PG_800480_LHJ},// 30

	{ARRAY_SIZE(D92_GSL3675B_GG_DH_1024600),D92_GSL3675B_GG_DH_1024600}, // 31
	{ARRAY_SIZE(D92_GSL3675B_PG_DH_1024600),D92_GSL3675B_PG_DH_1024600}, // 32
	{ARRAY_SIZE(FW_D92_GSL3675B_GG_1024600_QSD),FW_D92_GSL3675B_GG_1024600_QSD},//33
	{ARRAY_SIZE(FW_D92_GSL3675B_PG_1024600_QSD),FW_D92_GSL3675B_PG_1024600_QSD},//34


	{ARRAY_SIZE(D69_GSL2682B_OGS_DZ),D69_GSL2682B_OGS_DZ}, // 35

	{ARRAY_SIZE(D703HC_GSL1688E_1024600_PG_CTD),D703HC_GSL1688E_1024600_PG_CTD}, // 36
	{ARRAY_SIZE(D703C_GSL1688E_800480_PG_CTD),D703C_GSL1688E_800480_PG_CTD}, // 37

	{ARRAY_SIZE(D100_GSL3675B_GG_DH_1024600),D100_GSL3675B_GG_DH_1024600},// 38
	{ARRAY_SIZE(D100_GSL3675B_PG_YTG_1024600),D100_GSL3675B_PG_YTG_1024600},// 39
	{ARRAY_SIZE(D100_GSL3675B_GG_QSD_1024600),D100_GSL3675B_GG_QSD_1024600},// 40
	{ARRAY_SIZE(D100_GSL3675B_PG_QSD_1024600),D100_GSL3675B_PG_QSD_1024600},// 41
	{ARRAY_SIZE(D100_GSL3675B_PG_DPT_1024600),D100_GSL3675B_PG_DPT_1024600},// 42
	
	{ARRAY_SIZE(D970_GSL3675B_GG_QSD_1024768),D970_GSL3675B_GG_QSD_1024768},// 43

	{ARRAY_SIZE(D705_GSL1681E_PG_SG_800480),D705_GSL1681E_PG_SG_800480},// 44
	{ARRAY_SIZE(D705_GSL1681E_PG_CTD_800480),D705_GSL1681E_PG_CTD_800480},// 45
	{ARRAY_SIZE(D705_GSL1681E_PG_DPT_800480),D705_GSL1681E_PG_DPT_800480},// 46
	{ARRAY_SIZE(D705_GSL1681E_PG_QLT_800480),D705_GSL1681E_PG_QLT_800480},// 47

	{ARRAY_SIZE(FW_D81_GSL3675B_PG_800480_CLGD),FW_D81_GSL3675B_PG_800480_CLGD},// 48

	{ARRAY_SIZE(FW_D90_GSL3675B_PG_800480_DPT),FW_D90_GSL3675B_PG_800480_DPT},// 49

	{ARRAY_SIZE(FW_D90_GSL3675B_PG_1024600_DPT),FW_D90_GSL3675B_PG_1024600_DPT},// 50

	{ARRAY_SIZE(D70T_GSL1680E_PG_SG_1024600),D70T_GSL1680E_PG_SG_1024600},// 51
	{ARRAY_SIZE(D70T_GSL1680E_PG_SG_800480),D70T_GSL1680E_PG_SG_800480},// 52

	{ARRAY_SIZE(D100Z_GSL3675B),D100Z_GSL3675B},// 53

	{ARRAY_SIZE(D78_GSL1680E_PG_QLT_800480),D78_GSL1680E_PG_QLT_800480},// 54

	{ARRAY_SIZE(D701EB2C_GSL1680E_PG_INET_800480),D701EB2C_GSL1680E_PG_INET_800480},// 55

	{ARRAY_SIZE(FW_D90_GSL3675B_PG_800480_FC),FW_D90_GSL3675B_PG_800480_FC},// 56

	{ARRAY_SIZE(D101_GSL3675B_1024600),D101_GSL3675B_1024600},// 57

	{ARRAY_SIZE(D92_GSL3675B_PG_FC_800480),D92_GSL3675B_PG_FC_800480},// 58
	{ARRAY_SIZE(D92_GSL3675B_PG_FC_1024600),D92_GSL3675B_PG_FC_1024600},// 59
	{ARRAY_SIZE(D92_GSL3675B_PG_LHJ_1024600),D92_GSL3675B_PG_LHJ_1024600},// 60
	
	{ARRAY_SIZE(FW_798_GSL915_7681024_OGS_DZ),FW_798_GSL915_7681024_OGS_DZ},// 61

	{ARRAY_SIZE(D102_GSL3675B_PG_XLL_1024600),D102_GSL3675B_PG_XLL_1024600},// 62

	{ARRAY_SIZE(D791C_GSL915_1024768_PG_QSD),D791C_GSL915_1024768_PG_QSD},// 63

	{ARRAY_SIZE(D791C_GSL915_7681024_CTD_PG),D791C_GSL915_7681024_CTD_PG},// 64

	{ARRAY_SIZE(D791C_GSL915_1024768_PG_XLL),D791C_GSL915_1024768_PG_XLL},// 65

	{ARRAY_SIZE(D791C_GSL915_1024768_PG_YCGD),D791C_GSL915_1024768_PG_YCGD},// 66

	{ARRAY_SIZE(D791C_GSL915_1024768_GG_CTD),D791C_GSL915_1024768_GG_CTD},// 67

	{ARRAY_SIZE(U100_GSL3675B_6001024_GG_QLT),U100_GSL3675B_6001024_GG_QLT},// 68

	//{ARRAY_SIZE(U70_GSL1680E_480800_PG_INET),U70_GSL1680E_480800_PG_INET},// 69
	{ARRAY_SIZE(U701C_GSL168XX_PG_480800_INET),U701C_GSL168XX_PG_480800_INET},// 69

	//{ARRAY_SIZE(U70_GSL1680E_6001024_PG_INET),U70_GSL1680E_6001024_PG_INET},// 70
	{ARRAY_SIZE(U701C_GSL168XX_PG_6001024_INET),U701C_GSL168XX_PG_6001024_INET},// 70

	{ARRAY_SIZE(U906C_GSL3675B_8001280_GG_RSS),U906C_GSL3675B_8001280_GG_RSS},// 71

	{ARRAY_SIZE(U680C_GSL3676B_1280800_OGS_DZ),U680C_GSL3676B_1280800_OGS_DZ},// 72

	{ARRAY_SIZE(U680_GSL3676B_1280800_OGS_XCL),U680_GSL3676B_1280800_OGS_XCL},// 73

	{ARRAY_SIZE(FW_U680_GSL3676B_1280800_OGS_SG),FW_U680_GSL3676B_1280800_OGS_SG},// 74

	{ARRAY_SIZE(U798_GSL915_7681024_OGS_XCL),U798_GSL915_7681024_OGS_XCL},// 75

	{ARRAY_SIZE(U108C_GSL3675B_8001280_GG_RSS),U108C_GSL3675B_8001280_GG_RSS},// 76

	{ARRAY_SIZE(U798_GSL915_7681024_OGS_SG),U798_GSL915_7681024_OGS_SG},// 77

	{ARRAY_SIZE(FW_U430_GSL1680E_480800_PG_YJ),FW_U430_GSL1680E_480800_PG_YJ},// 78

	{ARRAY_SIZE(U92C_GSL3675B_GG_6001024_QSD),U92C_GSL3675B_GG_6001024_QSD},// 79
	
	{ARRAY_SIZE(U108C_GSL3675B_8001280_PG_XLL),U108C_GSL3675B_8001280_PG_XLL},// 80

	{ARRAY_SIZE(N106_GSL3692_1366768_PG_XLL_XN_15721572),N106_GSL3692_1366768_PG_XLL_XN_15721572},// 81
	
	{ARRAY_SIZE(U106_GSL3692_1366768_GG_FC_FC101S124),U106_GSL3692_1366768_GG_FC_FC101S124},// 82
	
	{ARRAY_SIZE(N106_GSL3692_1280800_PG_FC_FC101S123),N106_GSL3692_1280800_PG_FC_FC101S123},// 83
	
	{ARRAY_SIZE(U102C_GSL3675B_1024600_GG_FC),U102C_GSL3675B_1024600_GG_FC},// 84
	
	{ARRAY_SIZE(U92_GSL3675B_PG_DH_1024600),U92_GSL3675B_PG_DH_1024600},// 85
	
	{ARRAY_SIZE(U92_GSL3675B_PG_DH_1024600),U92_GSL3675B_PG_XLL_1024600},// 86
	
	{ARRAY_SIZE(D78_GSL1680E_PG_6001024_GT_70Q88001V2),D78_GSL1680E_PG_6001024_GT_70Q88001V2},// 87
	
	{ARRAY_SIZE(N106_GSL3692_8001280_PG_FC_FC101S123),N106_GSL3692_8001280_PG_FC_FC101S123},// 88
	
	{ARRAY_SIZE(N106_GSL3692_8001280_GG_FC_FC101S123),N106_GSL3692_8001280_GG_FC_FC101S123},// 89
	
	{ARRAY_SIZE(U680C_GSL3676B_PG_FC80J115_8001280_FUWOTE),U680C_GSL3676B_PG_FC80J115_8001280_FUWOTE},// 90
	
{ARRAY_SIZE(N106_GSL3692_8001280_PG_FC_FC101S124),N106_GSL3692_8001280_PG_FC_FC101S124},// 91
	{ARRAY_SIZE(U78_GSL1686F_1024600_PG_QCY070157),U78_GSL1686F_1024600_PG_QCY070157},// 92
		{ARRAY_SIZE(N106_GSL3692_8001280_GG_HK_10DR2590),N106_GSL3692_8001280_GG_HK_10DR2590},// 93
	{ARRAY_SIZE(N106_GSL3692_1280800_PG_ZYD10151V02),N106_GSL3692_1280800_PG_ZYD10151V02},// 94
		{ARRAY_SIZE(N106_GSL3692_1280800_PG_XLL_XN1582AB),N106_GSL3692_1280800_PG_XLL_XN1582AB},// 95
				{ARRAY_SIZE(A791_gsl2682_1280800_PG_getu),A791_gsl2682_1280800_PG_getu},// 96
{ARRAY_SIZE(U791_gsl2682_7681024_PG_ZJX_79A40),U791_gsl2682_7681024_PG_ZJX_79A40},// 97
};

unsigned int *gslX680_config_data[] = {
	gsl_config_data_id_Customer,// 0
	
	gsl_config_data_id_D71_GSL1688E_OGS_DZ_70L1,// 1
	gsl_config_data_id_D71_GSL1688E_OGS_DZ_70L1_800480, // 2
	
	//gsl_config_data_id_D71_GSL1688E_OGS_DZ_70E2,// 3
	gsl_config_data_id_D71_GSL168XX_OGS_6001024_DZ_70E2,// 3
	//gsl_config_data_id_D71_GSL1688E_OGS_DZ_70E2_800480,// 4
	gsl_config_data_id_D71_GSL168XX_OGS_480800_DZ_70E2,// 4
	
	gsl_config_data_id_D72E_GSL1688E_OGS_DZ_70L1, // 5
	gsl_config_data_id_D72E_GSL1688E_OGS_DZ_70L1_800480, // 6
	
	gsl_config_data_id_D72E_GSL1688E_OGS_DZ_70E2,// 7
	gsl_config_data_id_D72E_GSL1688E_OGS_DZ_70E2_800480,// 8
	
	//gsl_config_data_id_D71_GSL1688E_PG_INET,// 9
	gsl_config_data_id_D71_GSL168XX_PG_6001024_INET,// 9
	//gsl_config_data_id_D71_GSL1688E_PG_INET_800480,// 10
	gsl_config_data_id_D71_GSL168XX_PG_480800_INET,// 10

	gsl_config_data_id_D791_GSL915_OGS_DZ_79D2,// 11
	gsl_config_data_id_D791C_GSL915_1024768_OGS_DZ_79D4,// 12
	gsl_config_data_id_D791C_GSL915_1024768_OGS_SG5908A, //13
	gsl_config_data_id_JND_OGS_D791_GSL915_1024768_OGS_JND, //14
	gsl_config_data_id_D791_GSL915_1024768_GF_QSD, //15

	gsl_config_data_id_D702C_GSL1688E_DPT_PG,// 16

	gsl_config_data_id_D72E_GSL1688E_PG_CTD_1024600,// 17
	gsl_config_data_id_D72E_GSL1688E_PG_CTD_800480, //18

	gsl_config_data_id_D102_GSL3675B_GG_QSD_1024600, // 19
	gsl_config_data_id_D102_GSL3675B_GG_DH_1024600, //20
	gsl_config_data_id_D102_GSL3675B_PG_DH_1024600, //21
	gsl_config_data_id_D102_GSL3675B_PG_QSD_1024600, //22
	gsl_config_data_id_D102_GSL3675B_PG_LHJ_1024600, //23
	gsl_config_data_id_D102_GSL3675B_PG_DPT_1024600, //24
	gsl_config_data_id_D102_GSL3675B_OGS_SG_1024600, //25


	gsl_config_data_id_D92_GSL3675B_GG_QSD_800480, // 26
	gsl_config_data_id_D92_GSL3675B_PG_800480_DH,//27
	gsl_config_data_id_D92_GSL3675B_GG_800480_DH, //28
	gsl_config_data_id_D92_GSL3675B_PG_800480_QSD, //29
	gsl_config_data_id_D92_GSL3675B_PG_800480_LHJ, //30

	gsl_config_data_id_D92_GSL3675B_GG_DH_1024600, //31
	gsl_config_data_id_D92_GSL3675B_PG_DH_1024600, //32
	gsl_config_data_id_D92_GSL3675B_GG_1024600_QSD, // 33
	gsl_config_data_id_D92_GSL3675B_PG_1024600_QSD, // 34

	gsl_config_data_id_D69_GSL2682B_OGS_DZ, //35

	gsl_config_data_id_D703HC_GSL1688E_1024600_PG_CTD, //36
    gsl_config_data_id_D703C_GSL1688E_800480_PG_CTD, //37

	gsl_config_data_id_D100_GSL3675B_GG_DH_1024600, //38
	gsl_config_data_id_D100_GSL3675B_PG_YTG_1024600, // 39
	gsl_config_data_id_D100_GSL3675B_GG_QSD_1024600, // 40
	gsl_config_data_id_D100_GSL3675B_PG_QSD_1024600, //41
	gsl_config_data_id_D100_GSL3675B_PG_DPT_1024600, //42

	gsl_config_data_id_D970_GSL3675B_GG_QSD_1024768, //43

	
	gsl_config_data_id_D705_GSL1681E_PG_SG_800480, //44
	gsl_config_data_id_D705_GSL1681E_PG_CTD_800480, //45
	gsl_config_data_id_D705_GSL1681E_PG_DPT_800480, //46
	gsl_config_data_id_D705_GSL1681E_PG_QLT_800480, //47
	
	gsl_config_data_id_D81_GSL3675B_PG_800480_CLGD, //48

	gsl_config_data_id_D90_GSL3675B_PG_800480_DPT, //49

	gsl_config_data_id_D90_GSL3675B_PG_1024600_DPT, //50

	gsl_config_data_id_D70T_GSL1688E_PG_SG_1024600, //51
	gsl_config_data_id_D70T_GSL1688E_PG_SG_800480, //52

	gsl_config_data_id_D100Z_GSL3675B, //53

	gsl_config_data_id_D78_GSL1680E_PG_QLT_800480, //54

	gsl_config_data_id_D701EB2C_GSL1680E_PG_INET_800480, //55

	gsl_config_data_id_D90_GSL3675B_PG_800480_FC, //56

	gsl_config_data_id_D101_GSL3675B_1024600, //57

	gsl_config_data_id_D92_GSL3675B_PG_FC_800480, //58
	gsl_config_data_id_D92_GSL3675B_PG_FC_1024600, //59
	gsl_config_data_id_D92_GSL3675B_PG_LHJ_1024600, //60
	
	gsl_config_data_id_798_GSL915_7681024_OGS_DZ, //61

	gsl_config_data_id_D102_GSL3675B_PG_XLL_1024600, //62

	gsl_config_data_id_D791C_GSL915_1024768_PG_QSD, //63

	gsl_config_data_id_D791C_GSL915_7681024_CTD_PG, //64

	gsl_config_data_id_D791C_GSL915_1024768_PG_XLL, //65

	gsl_config_data_id_D791C_GSL915_1024768_PG_YCGD, //66

	gsl_config_data_id_D791C_GSL915_1024768_GG_CTD, //67

	gsl_config_data_id_U100_GSL3675B_6001024_GG_QLT, //68

	//gsl_config_data_id_U70_GSL1680E_480800_PG_INET, //69
	gsl_config_data_id_U701C_GSL168XX_PG_480800_INET, //69

	//gsl_config_data_id_U70_GSL1680E_6001024_PG_INET, //70
	gsl_config_data_id_U701C_GSL168XX_PG_6001024_INET, //70

	gsl_config_data_id_U906C_GSL3675B_8001280_GG_RSS, //71

	gsl_config_data_id_U680C_GSL3676B_1280800_OGS_DZ, //72

	gsl_config_data_id_U680_GSL3676B_1280800_OGS_XCL, //73

	gsl_config_data_id_U680_GSL3676B_1280800_OGS_SG, //74

	gsl_config_data_id_U798_GSL915_7681024_OGS_XCL, //75

	gsl_config_data_id_U108C_GSL3675B_8001280_GG_RSS, //76

	gsl_config_data_id_U798_GSL915_7681024_OGS_SG, //77

	gsl_config_data_id_U430_GSL1680E_480800_PG_YJ, //78

	gsl_config_data_id_U92C_GSL3675B_GG_6001024_QSD, //79
	
	gsl_config_data_id_U108C_GSL3675B_8001280_PG_XLL, //80
	
	gsl_config_data_id_N106_GSL3692_1366768_PG_XLL_XN_1572, //81
	
	gsl_config_data_id_U106_GSL3692_1366768_GG_FC_FC101S124, //82
	
	gsl_config_data_id_N106_GSL3692_1280800_PG_FC_FC101S123, //83
	
	gsl_config_data_id_U102C_GSL3675B_1024600_GG_FC, //84
	
	gsl_config_data_id_U92_GSL3675B_PG_DH_1024600, //85
	
	gsl_config_data_id_U92_GSL3675B_PG_XLL_1024600, //86
	
	gsl_config_data_id_D78_GSL1680E_PG_6001024_GT_70Q88001V2, //87
	
	gsl_config_data_id_N106_GSL3692_8001280_PG_FC_FC101S123, //88
	
	gsl_config_data_id_N106_GSL3692_8001280_GG_FC_FC101S123, //89
	
	gsl_config_data_id_U680C_GSL3676B_PG_FC80J115_8001280_FUWOTE, //90	
	
gsl_config_data_id_N106_GSL3692_8001280_PG_FC_FC101S124, //91
		gsl_config_data_id_U78_GSL1686F_1024600_PG_QCY070157, //92
			gsl_config_data_id_N106_GSL3692_8001280_GG_HK_10DR2590, //93
	gsl_config_data_id_N106_GSL3692_1280800_PG_ZYD10151V02, //94
		gsl_config_data_id_N106_GSL3692_1280800_PG_XLL_XN1582AB, //95
		gsl_config_data_id_A791_gsl2682_1280800_PG_getu,//96
	gsl_config_data_id_U791_gsl2682_7681024_PG_ZJX_79A40, //97s
};

static int getIndex(int num)
{
	int index = 0;
	printk("++++++++++++++++++++++++\n");
 	switch(num)
 	{

 		case 999:
			index = 0;
			printk("Inet_Customer\n");
			printk("%s\n",CUSTOMER_INFO);
			break;

		
 		case 7101:
			if(screen_max_x == 1024 && screen_max_y == 600)
			{
				index = 1;
				printk("D71_GSL1688E_OGS_DZ_70L1\n");
			}
			else
			{
				index = 2;
				printk("D71_GSL1688E_OGS_DZ_70L1_800480\n");
			}
			break;
		case 7102:
			if(screen_max_x == 1024 && screen_max_y == 600)
			{
				index = 3;
				printk("D71_GSL168XX_OGS_6001024_DZ_70E2\n");
			}
			else
			{
				index = 4;
				//printk("D71_GSL1688E_OGS_DZ_70E2_800480\n");
				printk("D71_GSL168XX_OGS_480800_DZ_70E2\n");
			}
			break;
		case 7103:
			if(screen_max_x == 1024 && screen_max_y == 600)
			{
				index = 5;
				printk("D72E_GSL1688E_OGS_DZ_70L1\n");
			}
			else
			{
				index = 6;
				printk("D72E_GSL1688E_OGS_DZ_70L1_800480\n");
			}
			break;
		case 7104:
			if(screen_max_x == 1024 && screen_max_y == 600)
			{
				index = 7;
				printk("D72E_GSL1688E_OGS_DZ_70E2\n");
			}
			else
			{
				index = 8;
				printk("D72E_GSL1688E_OGS_DZ_70E2_800480\n");
			}
			break;
		case 92:
			if(screen_max_x == 1024 && screen_max_y == 600)
			{
				index = 9;
				//printk("D71_GSL1688E_PG_INET\n");
				printk("D71_GSL168XX_PG_6001024_INET\n");
			}
			else
			{
				index = 10;
				//printk("D71_GSL1688E_PG_INET_800480\n");
				printk("D71_GSL168XX_PG_480800_INET\n");
			}
			break;
		case 9203:
			index = 85;
			printk("U92_GSL3675B_PG_DH_1024600\n");
			break;
		case 7910:
			//index = 11;
			printk("D791C_GSL915_OGS TP ajust\n");
			break;
		case 7911:
			//index = 12;
			printk("D791C_GSL915_PG TP ajust\n");
			break;
		case 7912:
			index = 63;
			printk("D791C_GSL915_1024768_PG_QSD\n");
			break;
		case 7913:
			index = 66;
			printk("D791C_GSL915_1024768_PG_YCGD\n");
			break;
		case 7914:
			index = 67;
			printk("D791C_GSL915_1024768_GG_CTD\n");
			break;
		case 7915:
			index = 97;
			printk("U791_gsl2682_7681024_PG_ZJX_79A40\n");
			break;
		case 79101:
			index = 96;
			printk("A791_gsl2682_1280800_PG_getu\n");
			break;	
		case 8602:
			index = 16;
			printk("D702C_GSL1688E_DPT_PG\n");
			break;
		case 7105:
			if(screen_max_x == 1024 && screen_max_y == 600)
			{
				index = 17;
				printk("D72E_GSL1688E_PG_CTD_1024600\n");
			}
			else
			{
				index = 18;
				printk("D72E_GSL1688E_PG_CTD_800480\n");
			}
			break;
		case 102:
			//index = 18;
			printk("D102 1024 600 tp ajust\n");
			break;
		case 921:
			//index = 19;
			printk("D92 800 480 tp ajust\n");
			break;
		case 922:
			//index = 20;
			printk("D92 1024 600 tp ajust\n");
			break;
		case 69:
			index = 35;
			printk("D69_GSL2682B_OGS_DZ\n");
			break;
		case 8606:
			index = 36;
			printk("D703HC_GSL1688E_1024600_PG_CTD\n");
			break;	
		case 8605:
			index = 37;
			printk("D703C_GSL1688E_800480_PG_CTD\n");
			break;
		case 103:
			//index = 37;
			printk("D103 1024 600 tp ajust\n");
			break;
		case 970:
			index = 43;
			printk("D970_GSL3675B_GG_QSD_1024768\n");
			break;
		case 8601:
			if(screen_max_x == 1024 && screen_max_y == 600)
				{
					index = 70;
					//printk("U70_GSL1680E_6001024_PG_INET\n");
					printk("U701C_GSL168XX_PG_6001024_INET\n");
				}
			else
				{
					index = 69;
					//printk("U70_GSL1680E_480800_PG_INET\n");
					printk("U701C_GSL168XX_PG_480800_INET\n");
				}
			break;
		case 705:
			//index = 42;
			printk("D705_TP_AJUST\n");
			break;
		case 78:
			index = 54;
			printk("D78_GSL1680E_PG_QLT_800480\n");
			break;
		case 7801:
			index = 87;
			printk("D78_GSL1680E_PG_6001024_GT_70Q88001V2_ajust-ffc07807\n");			
			break;		
		case 7803:
			index = 92;
			printk("U78_GSL1686F_1024600_PG_QCY070157\n");			
			break;
		case 81:
			//index = 49;
			printk("D81 TP Ajust\n");
			break;
		case 90:
			//index = 49;
			printk("D90_GSL3675B_800480 TP AJUST\n");
			break;
		case 91:
			index = 50;
			printk("FW_D90_GSL3675B_PG_1024600_DPT\n");
			break;
		case 9204:
			if(screen_max_x == 1024 && screen_max_y == 600)
			{
				index = 51;
				printk("D70T_GSL1680E_PG_SG_1024600\n");
			}
			else
			{
				index = 52;
				printk("D70T_GSL1680E_PG_SG_800480\n");
			}
			break;	
		case 100:
			index = 53;
			printk("D100 IENT BOM DPT\n");
			break;
		case 1001:
			index = 68;
			printk("U100_GSL3675B_6001024_GG_QLT\n");
			break;
		case 1002:
			//index = 76;
			printk("U108C 8001280 TP Ajust\n");
			break;
		case 70112:
			index = 55;
			printk("D701EB2C_GSL1680E_PG_DPT_800480\n");
			break;	
		case 101:
			index = 57;
			printk("D101_GSL3675B_1024600\n");
			break;
		case 9060:
			index = 71;
			printk("U906C_GSL3675B_8001280_GG_RSS\n");
			break;
		case 6808:
			//index = 72;
			printk("U680C_GSL3676B_1280800_OGS ajust\n");
			break;	
		case 680:
			index = 90;
			printk("U680C_GSL3676B_PG_FC80J115_8001280_FUWOTE\n");
			break;
		case 798:
			printk("D798 TP AJUST\n");
			break;
		case 106:
			//index = 81;
			printk("N106_GSL3692_1366768_PG_XLL_XN_15721572\n");
			break;
		case 1061:
			index = 89;
			printk("N106_GSL3692_8001280_GG_FC_FC101S123\n");
			break;
		case 1063:
			index = 91;
			printk("N106_GSL3692_8001280_PG_FC_FC101S124\n");
			break;
		case 1062:
			index = 93;
			printk("N106_GSL3692_8001280_GG_HK_10DR2590\n");
			break;
		case 1064:
			index = 94;
			printk("N106_GSL3692_1280800_PG_ZYD10151V02\n");
			break;
		case 1069:
			index = 95;
			printk("N106_GSL3692_1280800_PG_XLL_XN1582AB\n");
			break;
		case 4301:
			index = 78;
			printk("FW_U430_GSL1680E_480800_PG_YJ\n");
			break;
		default:
			index = 16;
			printk("D702C_GSL1688E_DPT_PG\n");
			break;	
 	}
	printk("++++++++++++++++++++++++\n");
	return index;
}
//--


struct ctp_config_info config_info = {
	.input_type = CTP_TYPE,
	.name = NULL,
	.int_number = 0,
};


static __u32 twi_id = 0;

static u32 debug_mask = 0;
enum{
	DEBUG_INIT = 1U << 0,
	DEBUG_SUSPEND = 1U << 1,
	DEBUG_INT_INFO = 1U << 2,
	DEBUG_X_Y_INFO = 1U << 3,
	DEBUG_KEY_INFO = 1U << 4,
	DEBUG_WAKEUP_INFO = 1U << 5,
	DEBUG_OTHERS_INFO = 1U << 6,
};

#define dprintk(level_mask,fmt,arg...)    if(unlikely(debug_mask & level_mask)) \
        printk("***CTP***"fmt, ## arg)


/* Addresses to scan */
static const unsigned short normal_i2c[2] = {GSLX680_I2C_ADDR, I2C_CLIENT_END};


static void glsX680_resume_events(struct work_struct *work);
struct workqueue_struct *gslX680_resume_wq;
static DECLARE_WORK(glsX680_resume_work, glsX680_resume_events);
struct i2c_client *glsX680_i2c;

int ctp_i2c_write_bytes(struct i2c_client *client, uint8_t *data, uint16_t len)
{
	struct i2c_msg msg;
	int ret=-1;
	
	msg.flags = !I2C_M_RD;
	msg.addr = client->addr;
	msg.len = len;
	msg.buf = data;		
	
	ret=i2c_transfer(client->adapter, &msg,1);
	return ret;
}

bool ctp_i2c_test(struct i2c_client * client)
{
	int ret,retry;
	uint8_t test_data[1] = { 0 };	//only write a data address.

	for(retry=0; retry < 2; retry++)
	{
		ret =ctp_i2c_write_bytes(client, test_data, 1);	//Test i2c.
		if (ret == 1)
			break;
		msleep(50);
	}

	return ret==1 ? true : false;
}

static int ctp_detect(struct i2c_client *client, struct i2c_board_info *info)
{
	struct i2c_adapter *adapter = client->adapter;
	int ret;

        if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
                return -ENODEV;
    
	if(twi_id == adapter->nr){
                pr_info("%s: addr= %x\n",__func__,client->addr);
                ret = ctp_i2c_test(client);
                if(!ret){
        		pr_info("%s:I2C connection might be something wrong \n",__func__);
        		return -ENODEV;
        	}else{      
            	        pr_info("I2C connection sucess!\n");
            	        strlcpy(info->type, CTP_NAME, I2C_NAME_SIZE);
						pr_info("%s", GSLX680_USED);
    		    return 0;	
	             }

	}else{
		return -ENODEV;
	}
}

/**
 * ctp_print_info - sysconfig print function
 * return value:
 *
 */
void ctp_print_info(struct ctp_config_info info,int debug_level)
{
	if(debug_level == DEBUG_INIT)
	{
		dprintk(DEBUG_INIT,"info.ctp_used:%d\n",info.ctp_used);
		dprintk(DEBUG_INIT,"info.ctp_name:%s\n",info.name);
		dprintk(DEBUG_INIT,"info.twi_id:%d\n",info.twi_id);
		dprintk(DEBUG_INIT,"info.screen_max_x:%d\n",info.screen_max_x);
		dprintk(DEBUG_INIT,"info.screen_max_y:%d\n",info.screen_max_y);
		dprintk(DEBUG_INIT,"info.revert_x_flag:%d\n",info.revert_x_flag);
		dprintk(DEBUG_INIT,"info.revert_y_flag:%d\n",info.revert_y_flag);
		dprintk(DEBUG_INIT,"info.exchange_x_y_flag:%d\n",info.exchange_x_y_flag);
		dprintk(DEBUG_INIT,"info.irq_gpio_number:%d\n",info.irq_gpio.gpio);
		dprintk(DEBUG_INIT,"info.wakeup_gpio_number:%d\n",info.wakeup_gpio.gpio);
	}
}

/**
 * ctp_wakeup - function
 *
 */
int ctp_wakeup(int status,int ms)
{
	dprintk(DEBUG_INIT,"***CTP*** %s:status:%d,ms = %d\n",__func__,status,ms);

	if (status == 0) {

		if(ms == 0) {
			__gpio_set_value(config_info.wakeup_gpio.gpio, 0);
		}else {
			__gpio_set_value(config_info.wakeup_gpio.gpio, 0);
			msleep(ms);
			__gpio_set_value(config_info.wakeup_gpio.gpio, 1);
		}
	}
	if (status == 1) {
		if(ms == 0) {
			__gpio_set_value(config_info.wakeup_gpio.gpio, 1);
		}else {
			__gpio_set_value(config_info.wakeup_gpio.gpio, 1);
			msleep(ms);
			__gpio_set_value(config_info.wakeup_gpio.gpio, 0);
		}
	}
	msleep(5);

	return 0;
}


static ssize_t gslX680_debug_enable_show(
	struct device *dev,
	struct device_attribute *attr,
	char *buf)
{
	return sprintf(buf, "0x%x", gslX680_debug_mask);
}

static ssize_t gslX680_debug_enable_store(
	struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count)
{
	if (buf[0] >= '0' && buf[0] <= '9')
	{
		gslX680_debug_mask = (buf[0] - '0');
	}
	else if (buf[0] >= 'a' && buf[0] <= 'f')
	{
		gslX680_debug_mask = 0x0A + (buf[0] - 'a');
	}
	else
	{
		gslX680_debug_mask = 0;
	}
	return count;
}

static DEVICE_ATTR(debug_enable, 0666, gslX680_debug_enable_show, gslX680_debug_enable_store);


static int gslX680_chip_init(void)
{	
         ctp_wakeup(1,0);
         msleep(20);
         return 0;   
}

static int gslX680_shutdown_low(void)
{
        ctp_wakeup(0,0);
	return 0;
}

static int gslX680_shutdown_high(void)
{
        ctp_wakeup(1,0);
	return 0;
}

static inline u16 join_bytes(u8 a, u8 b)
{
	u16 ab = 0;
	ab = ab | a;
	ab = ab << 8 | b;
	return ab;
}

static u32 gsl_read_interface(struct i2c_client *client, u8 reg, u8 *buf, u32 num)
{
	struct i2c_msg xfer_msg[2];

	xfer_msg[0].addr = client->addr;
	xfer_msg[0].len = 1;
	xfer_msg[0].flags = client->flags & I2C_M_TEN;
	xfer_msg[0].buf = &reg;

	xfer_msg[1].addr = client->addr;
	xfer_msg[1].len = num;
	xfer_msg[1].flags |= I2C_M_RD;
	xfer_msg[1].buf = buf;

	if (reg < 0x80) {
		i2c_transfer(client->adapter, xfer_msg, ARRAY_SIZE(xfer_msg));
		msleep(5);
	}

	return i2c_transfer(client->adapter, xfer_msg, ARRAY_SIZE(xfer_msg)) == ARRAY_SIZE(xfer_msg) ? 0 : -EFAULT;
}
static u32 gsl_write_interface(struct i2c_client *client, const u8 reg, u8 *buf, u32 num)
{
	struct i2c_msg xfer_msg[1];

	buf[0] = reg;

	xfer_msg[0].addr = client->addr;
	xfer_msg[0].len = num + 1;
	xfer_msg[0].flags = client->flags & I2C_M_TEN;
	xfer_msg[0].buf = buf;

	return i2c_transfer(client->adapter, xfer_msg, 1) == 1 ? 0 : -EFAULT;
}

static int gsl_ts_write(struct i2c_client *client, u8 addr, u8 *pdata, int datalen)
{
	int ret = 0;
	u8 tmp_buf[128];
	unsigned int bytelen = 0;
	if (datalen > 125){
		printk("%s too big datalen = %d!\n", __func__, datalen);
		return -1;
	}
	
	tmp_buf[0] = addr;
	bytelen++;
	
	if (datalen != 0 && pdata != NULL){
		memcpy(&tmp_buf[bytelen], pdata, datalen);
		bytelen += datalen;
	}
	
	ret = i2c_master_send(client, tmp_buf, bytelen);
	return ret;
}

static int gsl_ts_read(struct i2c_client *client, u8 addr, u8 *pdata, unsigned int datalen)
{
	int ret = 0;

	if (datalen > 126){
		printk("%s too big datalen = %d!\n", __func__, datalen);
		return -1;
	}

	ret = gsl_ts_write(client, addr, NULL, 0);
	if (ret < 0){
		printk("%s set data address fail!\n", __func__);
		return ret;
	}
	
	return i2c_master_recv(client, pdata, datalen);
}


static __inline__ void fw2buf(u8 *buf, const u32 *fw)
{
	u32 *u32_buf = (int *)buf;
	*u32_buf = *fw;
}


static void gsl_load_fw(struct i2c_client *client)
{
	//printk("[FUN]%s\n", __func__);
	
	u8 buf[DMA_TRANS_LEN*4 + 1] = {0};
	u8 send_flag = 1;
	u8 *cur = buf + 1;
	u32 source_line = 0;
	u32 source_len;
	struct fw_data *ptr_fw;
	
	u8 read_buf[4] = {0};

	gsl_ts_read(client, 0xfc, read_buf, 4);
	printk("read 0xfc = %x %x %x %x\n", read_buf[3], read_buf[2], read_buf[1], read_buf[0]);
	if(read_buf[2] != 0x82 && read_buf[2] != 0x88 &&  read_buf[2] != 0x91 && read_buf[2] != 0x36)
	{
		msleep(100);
		gsl_ts_read(client, 0xfc, read_buf, 4);
		printk("read 0xfc = %x %x %x %x\n", read_buf[3], read_buf[2], read_buf[1], read_buf[0]);
	}
	
	
	printk("=============gsl_load_fw start==============\n");

	switch(ctp_cob_gslX680)
	{
		case 798:
			if(cfg_adjust_flag)
			{
				switch(cfg_adjust_used_id)
				{
					case 1:
						fw_index = 61;
						printk("FW_798_GSL915_7681024_OGS_DZ\n");
						break;
					case 2:
						fw_index = 75;
						printk("U798_GSL915_7681024_OGS_XCL\n");
						break;
					case 3:
						fw_index = 77;
						printk("U798_GSL915_7681024_OGS_SG\n");
						break;	
					default:
						fw_index = 61;
						printk("FW_798_GSL915_7681024_OGS_DZ\n");
						break;
						
				}
				ptr_fw = gslx680_fw_grp[fw_index].fw;
				source_len = gslx680_fw_grp[fw_index].size;	
				break;
				
			}
			else
			{
				cfg_adjust_flag = 1 ;
				printk("GSLX680_FW_TEST\n");
				ptr_fw = GSLX680_FW_TEST;
				source_len = ARRAY_SIZE(GSLX680_FW_TEST);
				break;
			}
		case 1002:
			if(cfg_adjust_flag)
			{
				switch(cfg_adjust_used_id)
				{
					case 1:
						fw_index = 80;
						printk("U108C_GSL3675B_8001280_PG_XLL\n");
						break;
					case 2:
						fw_index = 76;
						printk("U108C_GSL3675B_8001280_GG_RSS\n");
						break;
					case 3:
						fw_index = 83;
						printk("N106_GSL3692_1280800_PG_FC_FC101S123\n");
						break;
					default:
						fw_index = 80;
						printk("U108C_GSL3675B_8001280_PG_XLL\n");
						break;
						
				}
				ptr_fw = gslx680_fw_grp[fw_index].fw;
				source_len = gslx680_fw_grp[fw_index].size;	
				break;
				
			}
			else
			{
				cfg_adjust_flag = 1 ;
				printk("GSLX680_FW_TEST\n");
				ptr_fw = GSLX680_FW_TEST;
				source_len = ARRAY_SIZE(GSLX680_FW_TEST);
				break;
			}
		case 81:
			if(cfg_adjust_flag)
			{
				switch(cfg_adjust_used_id)
				{
					case 1:
						fw_index = 48;
						printk("FW_D81_GSL3675B_PG_800480_CLGD\n");
						break;
					default:
						fw_index = 48;
						printk("FW_D81_GSL3675B_PG_800480_CLGD\n");
						break;
						
				}
				ptr_fw = gslx680_fw_grp[fw_index].fw;
				source_len = gslx680_fw_grp[fw_index].size;	
				break;
				
			}
			else
			{
				cfg_adjust_flag = 1 ;
				printk("GSLX680_FW_TEST\n");
				ptr_fw = GSLX680_FW_TEST;
				source_len = ARRAY_SIZE(GSLX680_FW_TEST);
				break;
			}
		case 90:
			if(cfg_adjust_flag)
			{
				switch(cfg_adjust_used_id)
				{
					case 1:
						fw_index = 56;
						printk("FW_D90_GSL3675B_PG_800480_FC\n");
						break;
					default:
						fw_index = 49;
						printk("FW_D90_GSL3675B_PG_800480_DPT\n");
						break;
						
				}
				ptr_fw = gslx680_fw_grp[fw_index].fw;
				source_len = gslx680_fw_grp[fw_index].size;	
				break;
				
			}
			else
			{
				cfg_adjust_flag = 1 ;
				printk("GSLX680_FW_TEST\n");
				ptr_fw = GSLX680_FW_TEST;
				source_len = ARRAY_SIZE(GSLX680_FW_TEST);
				break;
			}	
		case 7910:
			if(cfg_adjust_flag)
			{
				switch(cfg_adjust_used_id)
				{
					case 1:
						fw_index = 12;
						printk("FW_D791C_GSL915_1024768_OGS_DZ_79D4\n");
						break;
					case 2:
						fw_index = 13;
						printk("FW_D791C_GSL915_1024768_OGS_SG5908A\n");
						break;
					default:
						fw_index = 12;
						printk("FW_D791C_GSL915_1024768_OGS_DZ_79D4\n");
						break;
						
				}
				ptr_fw = gslx680_fw_grp[fw_index].fw;
				source_len = gslx680_fw_grp[fw_index].size;	
				break;
				
			}
			else
			{
				cfg_adjust_flag = 1 ;
				printk("GSLX680_FW_TEST\n");
				ptr_fw = GSLX680_FW_TEST;
				source_len = ARRAY_SIZE(GSLX680_FW_TEST);
				break;
			}
		case 7911:
			if(cfg_adjust_flag)
			{
				switch(cfg_adjust_used_id)
				{
					case 1:
						fw_index = 64;
						printk("D791C_GSL915_1024768_PG_CTD\n");
						break;
					case 2:
						fw_index = 65;
						printk("D791C_GSL915_1024768_PG_XLL\n");
						break;
					default:
						fw_index = 64;
						printk("D791C_GSL915_1024768_PG_CTD\n");
						break;
						
				}
				ptr_fw = gslx680_fw_grp[fw_index].fw;
				source_len = gslx680_fw_grp[fw_index].size;	
				break;
				
			}
			else
			{
				cfg_adjust_flag = 1 ;
				printk("GSLX680_FW_TEST\n");
				ptr_fw = GSLX680_FW_TEST;
				source_len = ARRAY_SIZE(GSLX680_FW_TEST);
				break;
			}
		case 921:
			if(cfg_adjust_flag)
			{
				//printk("[FUN]%s\n", __func__);
				//printk("fw_index = %d\n",fw_index);
				switch(cfg_adjust_used_id)
				{
					case 1:
						fw_index = 26;
						printk("D92_GSL3675B_GG_QSD_800480\n");
						break;
					case 2:
						fw_index = 27;
						printk("D92_GSL3675B_PG_DH_800480\n");
						break;
					case 3:
						fw_index = 28;
						printk("D92_GSL3675B_PG_DH_800480\n");
						break;
					case 4:
						fw_index = 29;
						printk("FW_D92_GSL3675B_PG_800480_QSD\n");
						break;
					case 5:
						fw_index = 30;
						printk("FW_D92_GSL3675B_PG_800480_LHJ\n");
						break;
					case 6:
						fw_index = 58;
						printk("D92_GSL3675B_PG_FC_800480\n");
						break;
					default:
						fw_index = 26;
						printk("D92_GSL3675B_GG_QSD_800480\n");
						break;
						
				}
				ptr_fw = gslx680_fw_grp[fw_index].fw;
				source_len = gslx680_fw_grp[fw_index].size;	
				break;
				
			}
			else
			{
				cfg_adjust_flag = 1 ;
				printk("GSLX680_FW_TEST\n");
				ptr_fw = GSLX680_FW_TEST;
				source_len = ARRAY_SIZE(GSLX680_FW_TEST);
				break;
			}	
		case 922:
			if(cfg_adjust_flag)
			{
				//printk("[FUN]%s\n", __func__);
				//printk("fw_index = %d\n",fw_index);
				switch(cfg_adjust_used_id)
				{
					case 1:
						fw_index = 31;
						printk("D92_GSL3675B_GG_DH_1024600\n");
						break;
					case 2:
						fw_index = 32;
						printk("D92_GSL3675B_PG_DH_1024600\n");
						break;
					case 3:
						fw_index = 33;
						printk("D92_GSL3675B_GG_QSD_1024600\n");
						break;
					case 4:
						fw_index = 34;
						printk("FW_D92_GSL3675B_PG_1024600_QSD\n");
						break;
					case 5:
						fw_index = 60;
						printk("D92_GSL3675B_PG_LHJ_1024600\n");
						break;
					case 6:
						fw_index = 59;
						printk("D92_GSL3675B_PG_FC_1024600\n");
						break;
					case 7:
						fw_index = 79;
						printk("U92C_GSL3675B_GG_6001024_QSD\n");
						break;
					case 8:
						fw_index = 86;
						printk("U92_GSL3675B_PG_XLL_1024600\n");
						break;
					default:
						fw_index = 31;
						printk("D92_GSL3675B_PG_DH_1024600\n");
						break;
						
				}
				ptr_fw = gslx680_fw_grp[fw_index].fw;
				source_len = gslx680_fw_grp[fw_index].size;	
				break;
				
			}
			else
			{
				cfg_adjust_flag = 1 ;
				printk("GSLX680_FW_TEST\n");
				ptr_fw = GSLX680_FW_TEST;
				source_len = ARRAY_SIZE(GSLX680_FW_TEST);
				break;
			}
			case 102:
				if(cfg_adjust_flag)
				{
					//printk("[FUN]%s\n", __func__);
					//printk("fw_index = %d\n",fw_index);
					switch(cfg_adjust_used_id)
					{
						case 1:
							fw_index = 19;
							printk("D102_GSL3675B_GG_QSD_1024600\n");
							break;
						case 2:
							fw_index = 20;
							printk("D102_GSL3675B_GG_DH_1024600\n");
							break;
						case 3:
							fw_index = 21;
							printk("D102_GSL3675B_PG_DH_1024600\n");
							break;
						case 4:
							fw_index = 22;
							printk("D102_GSL3675B_PG_QSD_1024600\n");
							break;
						case 5:
							fw_index = 23;
							printk("D102_GSL3675B_PG_LHJ_1024600\n");
							break;
						case 6:
							fw_index = 24;
							printk("D102_GSL3675B_PG_DPT_1024600\n");
							break;
						case 7:
							fw_index = 25;
							printk("D102_GSL3675B_OGS_SG_1024600\n");
							break;							
						case 8:
							fw_index = 62;
							printk("D102_GSL3675B_PG_XLL_1024600\n");
							break;	
						case 9:
							fw_index = 84;
							printk("U102C_GSL3675B_1024600_GG_FC\n");
							break;	
						default:
							fw_index = 19;
							printk("D102_GSL3675B_GG_DH_1024600\n");
							break;
							
					}
					ptr_fw = gslx680_fw_grp[fw_index].fw;
					source_len = gslx680_fw_grp[fw_index].size; 
					break;
					
				}
				else
				{
					cfg_adjust_flag = 1 ;
					printk("GSLX680_FW_TEST\n");
					ptr_fw = GSLX680_FW_TEST;
					source_len = ARRAY_SIZE(GSLX680_FW_TEST);
					break;
				}
		case 103:
				if(cfg_adjust_flag)
				{
					switch(cfg_adjust_used_id)
					{
						case 1:
							fw_index = 38;
							printk("D100_GSL3675B_GG_DH_1024600\n");
							break;
						case 2:
							fw_index = 39;
							printk("D100_GSL3675B_PG_YTG_1024600\n");
							break;
						case 3:
							fw_index = 40;
							printk("D100_GSL3675B_GG_QSD_1024600\n");
							break;
						case 4:
							fw_index = 41;
							printk("D100_GSL3675B_PG_QSD_1024600\n");
							break;
						case 5:
							fw_index = 42;
							printk("D100_GSL3675B_PG_DPT_1024600\n");
							break;
						default:
							fw_index = 38;
							printk("D100_GSL3675B_GG_DH_1024600\n");
							break;
							
					}
					ptr_fw = gslx680_fw_grp[fw_index].fw;
					source_len = gslx680_fw_grp[fw_index].size; 
					break;
					
				}
				else
				{
					cfg_adjust_flag = 1 ;
					printk("GSLX680_FW_TEST\n");
					ptr_fw = GSLX680_FW_TEST;
					source_len = ARRAY_SIZE(GSLX680_FW_TEST);
					break;
				}
		case 6808:
			if(cfg_adjust_flag)
			{
				switch(cfg_adjust_used_id)
				{
					case 1:
						fw_index = 72;
						printk("FW_U680_GSL3676B_1280800_OGS_DZ\n");
						break;
					case 2:
						fw_index = 74;
						printk("FW_U680_GSL3676B_1280800_OGS_SG\n");
						break;
					case 3:
						fw_index = 73;
						printk("U680_GSL3676B_1280800_OGS_XCL\n");
						break;	
					default:
						fw_index = 72;
						printk("FW_U680_GSL3676B_1280800_OGS_DZ\n");
						break;
						
				}
				ptr_fw = gslx680_fw_grp[fw_index].fw;
				source_len = gslx680_fw_grp[fw_index].size;	
				break;
				
			}
			else
			{
				cfg_adjust_flag = 1 ;
				ptr_fw = GSLX680_FW_TEST;
				source_len = ARRAY_SIZE(GSLX680_FW_TEST);
				break;
			}
				case 705:
				if(cfg_adjust_flag)
				{
					switch(cfg_adjust_used_id)
					{
						case 1:
							fw_index = 44;
							printk("D705_GSL1681E_PG_SG_800480\n");
							break;
						case 2:
							fw_index = 45;
							printk("D705_GSL1681E_PG_CTD_800480\n");
							break;
						case 3:
							fw_index = 46;
							printk("D705_GSL1681E_PG_CTD_800480\n");
							break;
						case 4:
							fw_index = 47;
							printk("D705_GSL1681E_PG_QLT_800480\n");
							break;							
						default:
							fw_index = 44;
							printk("D705_GSL1681E_PG_SG_800480\n");
							break;
							
					}
					ptr_fw = gslx680_fw_grp[fw_index].fw;
					source_len = gslx680_fw_grp[fw_index].size; 
					break;
					
				}
				else
				{
					cfg_adjust_flag = 1 ;
					printk("GSLX680_FW_TEST\n");
					ptr_fw = GSLX680_FW_TEST;
					source_len = ARRAY_SIZE(GSLX680_FW_TEST);
					break;
				}
	
				case 106:
				if(cfg_adjust_flag)
				{
					switch(cfg_adjust_used_id)
					{
						case 1:
							fw_index = 81;
							printk("U106_GSL3692_PG_XXL_1366600\n");
							break;
						case 2:
							fw_index = 82;
							printk("U106_GSL3692_PG_FC_1366600\n");
							break;							
						default:
							fw_index = 81;
							printk("U106_GSL3692_PG_XXL_1366600 default\n");
							break;
							
					}
					ptr_fw = gslx680_fw_grp[fw_index].fw;
					source_len = gslx680_fw_grp[fw_index].size; 
					break;
					
				}
				else
				{
					cfg_adjust_flag = 1 ;
					printk("GSLX680_FW_TEST\n");
					ptr_fw = GSLX680_FW_TEST;
					source_len = ARRAY_SIZE(GSLX680_FW_TEST);
					break;
				}
				
		default:
			ptr_fw = gslx680_fw_grp[fw_index].fw;
			source_len = gslx680_fw_grp[fw_index].size;	
			break;
	}
	
	for (source_line = 0; source_line < source_len; source_line++) 
	{
		/* init page trans, set the page val */
		if (GSL_PAGE_REG == ptr_fw[source_line].offset)
		{
			fw2buf(cur, &ptr_fw[source_line].val);
			gsl_write_interface(client, GSL_PAGE_REG, buf, 4);
			send_flag = 1;
		}
		else 
		{
			if (1 == send_flag % (DMA_TRANS_LEN < 0x20 ? DMA_TRANS_LEN : 0x20))
	    			buf[0] = (u8)ptr_fw[source_line].offset;

			fw2buf(cur, &ptr_fw[source_line].val);
			cur += 4;

			if (0 == send_flag % (DMA_TRANS_LEN < 0x20 ? DMA_TRANS_LEN : 0x20)) 
			{
	    			gsl_write_interface(client, buf[0], buf, cur - buf - 1);
	    			cur = buf + 1;
			}

			send_flag++;
		}
	}

	printk("=============gsl_load_fw end==============\n");

}

static void startup_chip(struct i2c_client *client)
{
	u8 tmp = 0x00;
#if 0
	u8 buf[4] = {0x00};
	buf[3] = 0x01;
	buf[2] = 0xfe;
	buf[1] = 0x10;
	buf[0] = 0x00;	
	gsl_ts_write(client, 0xf0, buf, sizeof(buf));
	buf[3] = 0x00;
	buf[2] = 0x00;
	buf[1] = 0x00;
	buf[0] = 0x0f;	
	gsl_ts_write(client, 0x04, buf, sizeof(buf));
	msleep(20);	
#endif
	gsl_ts_write(client, 0xe0, &tmp, 1);
	msleep(10);	
}

static void reset_chip(struct i2c_client *client)
{
	u8 tmp = 0x88;
	u8 buf[4] = {0x00};
	
	gsl_ts_write(client, 0xe0, &tmp, sizeof(tmp));
	msleep(20);
	tmp = 0x04;
	gsl_ts_write(client, 0xe4, &tmp, sizeof(tmp));
	msleep(10);
	gsl_ts_write(client, 0xbc, buf, sizeof(buf));
	msleep(10);
}

static void clr_reg(struct i2c_client *client)
{
	u8 write_buf[4]	= {0};

	write_buf[0] = 0x88;
	gsl_ts_write(client, 0xe0, &write_buf[0], 1); 	
	msleep(20);
	write_buf[0] = 0x03;//mbg modify for GSL3675 ic ajust 20140107
	gsl_ts_write(client, 0x80, &write_buf[0], 1); 	
	msleep(5);
	write_buf[0] = 0x04;
	gsl_ts_write(client, 0xe4, &write_buf[0], 1); 	
	msleep(5);
	write_buf[0] = 0x00;
	gsl_ts_write(client, 0xe0, &write_buf[0], 1); 	
	msleep(20);
}

static int cfg_adjust(struct i2c_client *client)
{
	
	int i,j,ret;
	u8 read_buf[4]  = {0};
	u8 temp[4]={0};
      u32 GSL_TP_ID_TEMP=0;

	switch(ctp_cob_gslX680)
	{
		case 81:
		case 7910:
		case 7911:
		case 90:
		case 921:	
		case 922:
		case 102:
		case 103:
		case 705:
		case 798:
		case 6808:
		case 1002:
		case 106:
			ret = 1;
			break;
		default:
			ret = 0;
			break;
			
	 }

	if(ret==0)
	{
		return 0;
	}
	
	printk("=============gsl_load_cfg_adjust check start==============\n");
	
	msleep(200);
	//i2c_smbus_read_i2c_block_data(client,0xb8, sizeof(read_buf), read_buf);
	gsl_ts_read(client,0xb8, read_buf, sizeof(read_buf));
    printk("fuc:cfg_adjust, b8: %x %x %x %x\n", read_buf[3], read_buf[2], read_buf[1], read_buf[0]);
	//GSL_TP_ID_TEMP=(u32)(read_buf[0]<<24+read_buf[1]<<16+read_buf[2]<<8+read_buf[3]);
	//GSL_TP_ID_TEMP=GSL_MX790_TP_ID[0];
	printk("fuc:cfg_adjust, GSL_TP_ID_TEMP: %x \n", GSL_TP_ID_TEMP);

	
	switch(ctp_cob_gslX680)
	{
		case 798:
			for(i=0;i<D798_7681024_TP_NUM;i++)
			  {
				for(j=0;j<4;j++)
				{
				temp[j]=D798_7681024_TP[i][j];
				}
				printk("temp[] %x %x %x %x\n", temp[0],temp[1],temp[2],temp[3]);
				if((read_buf[3]==temp[0])&&(read_buf[2]==temp[1])&&(read_buf[1]==temp[2])&&(read_buf[0]==temp[3]))
					{
					cfg_adjust_used_id=i+1;
					break;
					}
			  }
			break;
		case 1002:
			for(i=0;i<U108C_8001280_TP_NUM;i++)
			  {
				for(j=0;j<4;j++)
				{
				temp[j]=U108C_8001280_TP[i][j];
				}
				printk("temp[] %x %x %x %x\n", temp[0],temp[1],temp[2],temp[3]);
				if((read_buf[3]==temp[0])&&(read_buf[2]==temp[1])&&(read_buf[1]==temp[2])&&(read_buf[0]==temp[3]))
					{
					cfg_adjust_used_id=i+1;
					break;
					}
			  }
			break;
		case 6808:
			for(i=0;i<U680_8inch_TP_NUM;i++)
			  {
				for(j=0;j<4;j++)
				{
				temp[j]=U680_8inch_TP[i][j];
				}
				printk("temp[] %x %x %x %x\n", temp[0],temp[1],temp[2],temp[3]);
				if((read_buf[3]==temp[0])&&(read_buf[2]==temp[1])&&(read_buf[1]==temp[2])&&(read_buf[0]==temp[3]))
					{
					cfg_adjust_used_id=i+1;
					break;
					}
			  }
			break;
		case 81:
			for(i=0;i<D81_800480_TP_NUM;i++)
			  {
				for(j=0;j<4;j++)
				{
				temp[j]=D81_800480_TP[i][j];
				}
				printk("temp[] %x %x %x %x\n", temp[0],temp[1],temp[2],temp[3]);
				if((read_buf[3]==temp[0])&&(read_buf[2]==temp[1])&&(read_buf[1]==temp[2])&&(read_buf[0]==temp[3]))
					{
					cfg_adjust_used_id=i+1;
					break;
					}
			  }
			break;
		case 90:
			for(i=0;i<D90_800480_TP_NUM;i++)
			  {
				for(j=0;j<4;j++)
				{
				temp[j]=D90_800480_TP[i][j];
				}
				printk("temp[] %x %x %x %x\n", temp[0],temp[1],temp[2],temp[3]);
				if((read_buf[3]==temp[0])&&(read_buf[2]==temp[1])&&(read_buf[1]==temp[2])&&(read_buf[0]==temp[3]))
					{
					cfg_adjust_used_id=i+1;
					break;
					}
			  }
			break;
		case 7910:
			for(i=0;i<D791C_OGS_TP_NUM;i++)
			  {
				for(j=0;j<4;j++)
				{
				temp[j]=D791C_OGS_TP[i][j];
				}
				printk("temp[] %x %x %x %x\n", temp[0],temp[1],temp[2],temp[3]);
				if((read_buf[3]==temp[0])&&(read_buf[2]==temp[1])&&(read_buf[1]==temp[2])&&(read_buf[0]==temp[3]))
					{
					cfg_adjust_used_id=i+1;
					break;
					}
			  }
			break;
		case 7911:
			for(i=0;i<D791C_PG_TP_NUM;i++)
			  {
				for(j=0;j<4;j++)
				{
				temp[j]=D791C_PG_TP[i][j];
				}
				printk("temp[] %x %x %x %x\n", temp[0],temp[1],temp[2],temp[3]);
				if((read_buf[3]==temp[0])&&(read_buf[2]==temp[1])&&(read_buf[1]==temp[2])&&(read_buf[0]==temp[3]))
					{
					cfg_adjust_used_id=i+1;
					break;
					}
			  }
			break;
		case 921:
			for(i=0;i<D92_800480_TP_NUM;i++)
			  {
				for(j=0;j<4;j++)
				{
				temp[j]=D92_800480_TP[i][j];
				}
				printk("temp[] %x %x %x %x\n", temp[0],temp[1],temp[2],temp[3]);
				if((read_buf[3]==temp[0])&&(read_buf[2]==temp[1])&&(read_buf[1]==temp[2])&&(read_buf[0]==temp[3]))
					{
					cfg_adjust_used_id=i+1;
					break;
					}
			  }
			break;	
		case 922:
			for(i=0;i<D92_1024600_TP_NUM;i++)
			  {
				for(j=0;j<4;j++)
				{
				temp[j]=D92_1024600_TP[i][j];
				}
				printk("temp[] %x %x %x %x\n", temp[0],temp[1],temp[2],temp[3]);
				if((read_buf[3]==temp[0])&&(read_buf[2]==temp[1])&&(read_buf[1]==temp[2])&&(read_buf[0]==temp[3]))
					{
					cfg_adjust_used_id=i+1;
					break;
					}
			  }
			break;
		case 102:
				for(i=0;i<D102_1024600_TP_NUM;i++)
				  {
					for(j=0;j<4;j++)
					{
					temp[j]=D102_1024600_TP[i][j];
					}
					printk("temp[] %x %x %x %x\n", temp[0],temp[1],temp[2],temp[3]);
					if((read_buf[3]==temp[0])&&(read_buf[2]==temp[1])&&(read_buf[1]==temp[2])&&(read_buf[0]==temp[3]))
						{
						cfg_adjust_used_id=i+1;
						break;
						}
				  }
				break;
		case 103:
				for(i=0;i<D100_1024600_TP_NUM;i++)
				  {
					for(j=0;j<4;j++)
					{
					temp[j]=D100_1024600_TP[i][j];
					}
					printk("temp[] %x %x %x %x\n", temp[0],temp[1],temp[2],temp[3]);
					if((read_buf[3]==temp[0])&&(read_buf[2]==temp[1])&&(read_buf[1]==temp[2])&&(read_buf[0]==temp[3]))
						{
						cfg_adjust_used_id=i+1;
						break;
						}
				  }
				break;
		case 705:
				for(i=0;i<D705_800480_TP_NUM;i++)
				  {
					for(j=0;j<4;j++)
					{
					temp[j]=D705_800480_TP[i][j];
					}
					printk("temp[] %x %x %x %x\n", temp[0],temp[1],temp[2],temp[3]);
					if((read_buf[3]==temp[0])&&(read_buf[2]==temp[1])&&(read_buf[1]==temp[2])&&(read_buf[0]==temp[3]))
						{
						cfg_adjust_used_id=i+1;
						break;
						}
				  }
				break;	
		case 106:
				for(i=0;i<U106_PG_TP_NUM;i++)
				  {
					for(j=0;j<4;j++)
					{
					temp[j]=U106_PG_TP[i][j];
					}
					printk("temp[] %x %x %x %x\n", temp[0],temp[1],temp[2],temp[3]);
					if((read_buf[3]==temp[0])&&(read_buf[2]==temp[1])&&(read_buf[1]==temp[2])&&(read_buf[0]==temp[3]))
						{
						cfg_adjust_used_id=i+1;
						break;
						}
				  }
				break;
			
	}
	printk("fuc:cfg_adjust, cfg_adjust_used_id: %x \n", cfg_adjust_used_id);
	printk("=============gsl_load_cfg_adjust check end==============\n");
	      clr_reg(client);
		reset_chip(client);
		gsl_load_fw(client);
		startup_chip(client);
		reset_chip(client);
		startup_chip(client);	

	return 1;
}
//#endif



static void init_chip(struct i2c_client *client)
{
	gslX680_shutdown_low();	
	msleep(50); 	
	gslX680_shutdown_high();	
	msleep(30); 		
	//test_i2c(client);
	clr_reg(client);
	reset_chip(client);
	gsl_load_fw(client);			
	startup_chip(client);
	reset_chip(client);
	startup_chip(client);	
}

static void check_mem_data(struct i2c_client *client)
{
	u8 read_buf[4]  = {0};

	//if(gsl_chipType_new == 1)	
	{
		msleep(30);
		gsl_ts_read(client,0xb0, read_buf, sizeof(read_buf));
		printk("#########check mem read 0xb0 = %x %x %x %x #########\n", read_buf[3], read_buf[2], read_buf[1], read_buf[0]);
	
		if (read_buf[3] != 0x5a || read_buf[2] != 0x5a || read_buf[1] != 0x5a || read_buf[0] != 0x5a)
		{	
			init_chip(client);
		}
	}
}
#ifdef STRETCH_FRAME
static void stretch_frame(u16 *x, u16 *y)
{
	u16 temp_x = *x;
	u16 temp_y = *y;
	u16 temp_0, temp_1, temp_2;

	if(temp_x < X_STRETCH_MAX + X_STRETCH_CUST)
	{
		temp_0 = temp_1 = temp_2 = 0;
		temp_0 = X_STRETCH_MAX + X_STRETCH_CUST - temp_x;
		temp_0 = temp_0 > X_STRETCH_CUST ? X_STRETCH_CUST : temp_0;
		temp_0 = temp_0*(100 + X_RATIO_CUST)/100;
		if(temp_x < X_STRETCH_MAX)
		{
			temp_1 = X_STRETCH_MAX - temp_x;
			temp_1 = temp_1 > X_STRETCH_MAX/4 ? X_STRETCH_MAX/4 : temp_1;
			temp_1 = temp_1*(100 + 2*XL_RATIO_1)/100;
		}	
		if(temp_x < 3*X_STRETCH_MAX/4)
		{
			temp_2 = 3*X_STRETCH_MAX/4 - temp_x;
			temp_2 = temp_2*(100 + 2*XL_RATIO_2)/100;
		}
		*x = (temp_0 + temp_1 +temp_2) < (X_STRETCH_MAX + X_STRETCH_CUST) ? ((X_STRETCH_MAX + X_STRETCH_CUST) - (temp_0 + temp_1 +temp_2)) : 1;
	}
	else if(temp_x > (CTP_MAX_X -X_STRETCH_MAX - X_STRETCH_CUST))
	{
		temp_0 = temp_1 = temp_2 = 0;
		temp_0 = temp_x - (CTP_MAX_X -X_STRETCH_MAX - X_STRETCH_CUST);
		temp_0 = temp_0 > X_STRETCH_CUST ? X_STRETCH_CUST : temp_0;
		temp_0 = temp_0*(100 + X_RATIO_CUST)/100;
		if(temp_x > (CTP_MAX_X -X_STRETCH_MAX))
		{
			temp_1 = temp_x - (CTP_MAX_X -X_STRETCH_MAX);
			temp_1 = temp_1 > X_STRETCH_MAX/4 ? X_STRETCH_MAX/4 : temp_1;
			temp_1 = temp_1*(100 + 2*XR_RATIO_1)/100;
		}	
		if(temp_x > (CTP_MAX_X -3*X_STRETCH_MAX/4))
		{
			temp_2 = temp_x - (CTP_MAX_X -3*X_STRETCH_MAX/4);
			temp_2 = temp_2*(100 + 2*XR_RATIO_2)/100;
		}
		*x = (temp_0 + temp_1 +temp_2) < (X_STRETCH_MAX + X_STRETCH_CUST) ? ((CTP_MAX_X -X_STRETCH_MAX - X_STRETCH_CUST) + (temp_0 + temp_1 +temp_2)) : (CTP_MAX_X - 1);
	}
		
	if(temp_y < Y_STRETCH_MAX + Y_STRETCH_CUST)
	{
		temp_0 = temp_1 = temp_2 = 0;
		temp_0 = Y_STRETCH_MAX + Y_STRETCH_CUST - temp_y;
		temp_0 = temp_0 > Y_STRETCH_CUST ? Y_STRETCH_CUST : temp_0;
		temp_0 = temp_0*(100 + Y_RATIO_CUST)/100;
		if(temp_y < Y_STRETCH_MAX)
		{
			temp_1 = Y_STRETCH_MAX - temp_y;
			temp_1 = temp_1 > Y_STRETCH_MAX/4 ? Y_STRETCH_MAX/4 : temp_1;
			temp_1 = temp_1*(100 + 2*YL_RATIO_1)/100;
		}	
		if(temp_y < 3*Y_STRETCH_MAX/4)
		{
			temp_2 = 3*Y_STRETCH_MAX/4 - temp_y;
			temp_2 = temp_2*(100 + 2*YL_RATIO_2)/100;
		}
		*y = (temp_0 + temp_1 +temp_2) < (Y_STRETCH_MAX + Y_STRETCH_CUST) ? ((Y_STRETCH_MAX + Y_STRETCH_CUST) - (temp_0 + temp_1 +temp_2)) : 1;
	}
	else if(temp_y > (CTP_MAX_Y -Y_STRETCH_MAX - Y_STRETCH_CUST))
	{
		temp_0 = temp_1 = temp_2 = 0;	
		temp_0 = temp_y - (CTP_MAX_Y -Y_STRETCH_MAX - Y_STRETCH_CUST);
		temp_0 = temp_0 > Y_STRETCH_CUST ? Y_STRETCH_CUST : temp_0;
		temp_0 = temp_0*(100 + Y_RATIO_CUST)/100;
		if(temp_y > (CTP_MAX_Y -Y_STRETCH_MAX))
		{
			temp_1 = temp_y - (CTP_MAX_Y -Y_STRETCH_MAX);
			temp_1 = temp_1 > Y_STRETCH_MAX/4 ? Y_STRETCH_MAX/4 : temp_1;
			temp_1 = temp_1*(100 + 2*YR_RATIO_1)/100;
		}	
		if(temp_y > (CTP_MAX_Y -3*Y_STRETCH_MAX/4))
		{
			temp_2 = temp_y - (CTP_MAX_Y -3*Y_STRETCH_MAX/4);
			temp_2 = temp_2*(100 + 2*YR_RATIO_2)/100;
		}
		*y = (temp_0 + temp_1 +temp_2) < (Y_STRETCH_MAX + Y_STRETCH_CUST) ? ((CTP_MAX_Y -Y_STRETCH_MAX - Y_STRETCH_CUST) + (temp_0 + temp_1 +temp_2)) : (CTP_MAX_Y - 1);
	}
}
#endif

#ifdef FILTER_POINT
static void filter_point(u16 x, u16 y , u8 id)
{
	u16 x_err =0;
	u16 y_err =0;
	u16 filter_step_x = 0, filter_step_y = 0;
	
	id_sign[id] = id_sign[id] + 1;
	if(id_sign[id] == 1)
	{
		x_old[id] = x;
		y_old[id] = y;
	}
	
	x_err = x > x_old[id] ? (x -x_old[id]) : (x_old[id] - x);
	y_err = y > y_old[id] ? (y -y_old[id]) : (y_old[id] - y);

	if( (x_err > FILTER_MAX && y_err > FILTER_MAX/3) || (x_err > FILTER_MAX/3 && y_err > FILTER_MAX) )
	{
		filter_step_x = x_err;
		filter_step_y = y_err;
	}
	else
	{
		if(x_err > FILTER_MAX)
			filter_step_x = x_err; 
		if(y_err> FILTER_MAX)
			filter_step_y = y_err;
	}

	if(x_err <= 2*FILTER_MAX && y_err <= 2*FILTER_MAX)
	{
		filter_step_x >>= 2; 
		filter_step_y >>= 2;
	}
	else if(x_err <= 3*FILTER_MAX && y_err <= 3*FILTER_MAX)
	{
		filter_step_x >>= 1; 
		filter_step_y >>= 1;
	}	
	else if(x_err <= 4*FILTER_MAX && y_err <= 4*FILTER_MAX)
	{
		filter_step_x = filter_step_x*3/4; 
		filter_step_y = filter_step_y*3/4;
	}	
	
	x_new = x > x_old[id] ? (x_old[id] + filter_step_x) : (x_old[id] - filter_step_x);
	y_new = y > y_old[id] ? (y_old[id] + filter_step_y) : (y_old[id] - filter_step_y);

	x_old[id] = x_new;
	y_old[id] = y_new;
}
#else

static void record_point(u16 x, u16 y , u8 id)
{
	u16 x_err =0;
	u16 y_err =0;

	id_sign[id]=id_sign[id]+1;
	
	if(id_sign[id]==1){
		x_old[id]=x;
		y_old[id]=y;
	}

	x = (x_old[id] + x)/2;
	y = (y_old[id] + y)/2;
		
	if(x>x_old[id]){
		x_err=x -x_old[id];
	}
	else{
		x_err=x_old[id]-x;
	}

	if(y>y_old[id]){
		y_err=y -y_old[id];
	}
	else{
		y_err=y_old[id]-y;
	}

	if( (x_err > 3 && y_err > 1) || (x_err > 1 && y_err > 3) ){
		x_new = x;     x_old[id] = x;
		y_new = y;     y_old[id] = y;
	}
	else{
		if(x_err > 3){
			x_new = x;     x_old[id] = x;
		}
		else
			x_new = x_old[id];
		if(y_err> 3){
			y_new = y;     y_old[id] = y;
		}
		else
			y_new = y_old[id];
	}

	if(id_sign[id]==1){
		x_new= x_old[id];
		y_new= y_old[id];
	}
	
}
#endif
#ifdef TPD_PROC_DEBUG
static int char_to_int(char ch)
{
	if(ch>='0' && ch<='9')
		return (ch-'0');
	else
		return (ch-'a'+10);
}

static int gsl_config_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	char *ptr = page;
	//char temp_data[4] = {0};
	char temp_data[5] = {0};
	//int i;
	unsigned int tmp=0;
	if('v'==gsl_read[0]&&'s'==gsl_read[1])
	{
#ifdef GSL_NOID_VERSION
		tmp=GSL_VERSION_ID();
#else 
		tmp=0x20121215;
#endif
		ptr += sprintf(ptr,"version:%x\n",tmp);
	}
	else if('r'==gsl_read[0]&&'e'==gsl_read[1])
	{
		if('i'==gsl_read[3])
		{
#ifdef GSL_NOID_VERSION 
			tmp=(gsl_data_proc[5]<<8) | gsl_data_proc[4];
			ptr +=sprintf(ptr,"gsl_config_data_id[%d] = ",tmp);
			if(tmp>=0&&tmp<256)
				//ptr +=sprintf(ptr,"%d\n",gsl_config_data_id[tmp]); 
				ptr +=sprintf(ptr,"%d\n",gslX680_config_data[fw_index][tmp]); 
#endif
		}
		else 
		{
			gsl_ts_write(glsX680_i2c,0xf0,&gsl_data_proc[4],4);
			gsl_ts_read(glsX680_i2c,gsl_data_proc[0],temp_data,4);
			gsl_ts_read(glsX680_i2c,gsl_data_proc[0],temp_data,4);
			ptr +=sprintf(ptr,"offset : {0x%02x,0x",gsl_data_proc[0]);
			ptr +=sprintf(ptr,"%02x",temp_data[3]);
			ptr +=sprintf(ptr,"%02x",temp_data[2]);
			ptr +=sprintf(ptr,"%02x",temp_data[1]);
			ptr +=sprintf(ptr,"%02x};\n",temp_data[0]);
		}
	}
	*eof = 1;
	return (ptr - page);
}
static int gsl_config_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	u8 buf[8] = {0};
	//u8 addr = 0;
	int tmp = 0;
	int tmp1 = 0;
	//print_info("[tp-gsl][%s] \n",__func__);
	
	if(count > CONFIG_LEN)
	{
		//print_info("size not match [%d:%ld]\n", CONFIG_LEN, count);
        	return -EFAULT;
	}
	
	if(copy_from_user(gsl_read, buffer, (count<CONFIG_LEN?count:CONFIG_LEN)))
	{
		//print_info("copy from user fail\n");
        	return -EFAULT;
	}
	//print_info("[tp-gsl][%s][%s]\n",__func__,gsl_read);

	buf[3]=char_to_int(gsl_read[14])<<4 | char_to_int(gsl_read[15]);	
	buf[2]=char_to_int(gsl_read[16])<<4 | char_to_int(gsl_read[17]);
	buf[1]=char_to_int(gsl_read[18])<<4 | char_to_int(gsl_read[19]);
	buf[0]=char_to_int(gsl_read[20])<<4 | char_to_int(gsl_read[21]);
	
	buf[7]=char_to_int(gsl_read[5])<<4 | char_to_int(gsl_read[6]);
	buf[6]=char_to_int(gsl_read[7])<<4 | char_to_int(gsl_read[8]);
	buf[5]=char_to_int(gsl_read[9])<<4 | char_to_int(gsl_read[10]);
	buf[4]=char_to_int(gsl_read[11])<<4 | char_to_int(gsl_read[12]);
	if('v'==gsl_read[0]&& 's'==gsl_read[1])//version //vs
	{
		printk("gsl version\n");
	}
	else if('s'==gsl_read[0]&& 't'==gsl_read[1])//start //st
	{
		gsl_proc_flag = 1;
		reset_chip(glsX680_i2c);
		/*msleep(20);
		mt_set_gpio_out(GPIO_CTP_RST_PIN, 0);
		msleep(20);
		mt_set_gpio_out(GPIO_CTP_RST_PIN, 1);
		msleep(20);
		*/
		//gsl_start_core(ddata->client);
	}
	else if('e'==gsl_read[0]&&'n'==gsl_read[1])//end //en
	{
		msleep(20);
		reset_chip(glsX680_i2c);
		startup_chip(glsX680_i2c);
#ifdef GSL_NOID_VERSION
		//GSL_DATAINIT(gsl_config_data_id);
		GSL_DATAINIT(gslX680_config_data[fw_index]);	
	
#endif
		gsl_proc_flag = 0;
	}
	else if('r'==gsl_read[0]&&'e'==gsl_read[1])//read buf //
	{
		memcpy(gsl_data_proc,buf,8);
	}
	else if('w'==gsl_read[0]&&'r'==gsl_read[1])//write buf
	{
		gsl_ts_write(glsX680_i2c,buf[4],buf,4);
	}
	
#ifdef GSL_NOID_VERSION
	else if('i'==gsl_read[0]&&'d'==gsl_read[1])//write id config //
	{
		tmp1=(buf[7]<<24)|(buf[6]<<16)|(buf[5]<<8)|buf[4];
		tmp=(buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|buf[0];
		if(tmp1>=0 && tmp1<256)
		{
			//gsl_config_data_id[tmp1] = tmp;	
			gslX680_config_data[fw_index][tmp1] = tmp;
		}
	}
#endif
	return count;
}
#endif
#ifdef HAVE_TOUCH_KEY
static void report_key(struct gsl_ts *ts, u16 x, u16 y)
{
	u16 i = 0;
	for(i = 0; i < MAX_KEY_NUM; i++) {
		if((gsl_key_data[i].x_min < x) && (x < gsl_key_data[i].x_max)&&(gsl_key_data[i].y_min < y) &&\
		  (y < gsl_key_data[i].y_max)){
			key = gsl_key_data[i].key;	
			//printk("key=%d\n",key);
			input_report_key(ts->input, key, 1);
			input_sync(ts->input); 		
			key_state_flag = 1;
			break;
		}
	}
}
#endif

static void report_data(struct gsl_ts *ts, u16 x, u16 y, u8 pressure, u8 id)
{
	switch(ctp_cob_gslX680)
	{
		case 7101:
		case 7102:
		case 7107:
		case 7108:
		case 92:
		case 798:
			break;
		default:
			x = SCREEN_MAX_X - x;		
			y = SCREEN_MAX_Y - y;
			break;
	}
			
	dprintk(DEBUG_X_Y_INFO,"#####id=%d,x=%d,y=%d######\n",id,x,y);
	//printk("#####id=%d,x=%d,y=%d######\n",id,x,y);
	
	tp_light_ctrl(1);

	if(x>SCREEN_MAX_X||y>SCREEN_MAX_Y)
	{
	#ifdef HAVE_TOUCH_KEY
		switch(ctp_cob_gslX680)
		{
			case 798:
				report_key(ts,x,y);
				break;
			default:
				break;
		}
	#endif
		return;
	}
	
#ifdef REPORT_DATA_ANDROID_4_0
	input_mt_slot(ts->input, id);		
	input_report_abs(ts->input, ABS_MT_TRACKING_ID, id);
	input_report_abs(ts->input, ABS_MT_TOUCH_MAJOR, pressure);
	input_report_abs(ts->input, ABS_MT_POSITION_X, x);
	input_report_abs(ts->input, ABS_MT_POSITION_Y, y);	
	input_report_abs(ts->input, ABS_MT_WIDTH_MAJOR, 1);
#else
	input_report_abs(ts->input, ABS_MT_TRACKING_ID, id);
	input_report_abs(ts->input, ABS_MT_TOUCH_MAJOR, pressure);
	input_report_abs(ts->input, ABS_MT_POSITION_X,x);
	input_report_abs(ts->input, ABS_MT_POSITION_Y, y);
	input_report_abs(ts->input, ABS_MT_WIDTH_MAJOR, 1);
	input_mt_sync(ts->input);
#endif
}


static void process_gslX680_data(struct gsl_ts *ts)
{
	u8 id, touches;
	u16 x, y;
	int i = 0;
	int tmp1 = 0;
	u8 buf[4]={0};
	touches = ts->touch_data[ts->dd->touch_index];
#ifdef GSL_NOID_VERSION
    struct gsl_touch_info cinfo;
	cinfo.finger_num = touches;
	//printk("tp-gsl  finger_num = %d\n",cinfo.finger_num);
	for(i = 0; i < (touches < MAX_CONTACTS ? touches : MAX_CONTACTS); i ++)
	{
		cinfo.x[i] = join_bytes( ( ts->touch_data[ts->dd->x_index  + 4 * i + 1] & 0xf),
				ts->touch_data[ts->dd->x_index + 4 * i]);
		cinfo.y[i] = join_bytes(ts->touch_data[ts->dd->y_index + 4 * i + 1],
				ts->touch_data[ts->dd->y_index + 4 * i ]);
		cinfo.id[i] = ((ts->touch_data[ts->dd->x_index  + 4 * i + 1] & 0xf0)>>4);
		//printk("tp-gsl  before: x[%d] = %d, y[%d] = %d, id[%d] = %d \n",i,cinfo.x[i],i,cinfo.y[i],i,cinfo.id[i]);
	}
	cinfo.finger_num = ts->touch_data[0] | (ts->touch_data[1]<<8)|(ts->touch_data[2]<<16)|
		(ts->touch_data[3]<<24);
	GSL_ALG_ID_MAIN(&cinfo);
	//printk("tp-gsl  finger_num = %d\n",cinfo.finger_num);
	tmp1=GSL_MASK_TIAOPING();
	//printk("[tp-gsl] tmp1=%x\n",tmp1);
	if(tmp1>0&&tmp1<0xffffffff)
	{
		buf[0]=0xa;
		buf[1]=0;
		buf[2]=0;
		buf[3]=0;
		gsl_ts_write(ts->client,0xf0,buf,4);
		buf[0]=(u8)(tmp1 & 0xff);
		buf[1]=(u8)((tmp1>>8) & 0xff);
		buf[2]=(u8)((tmp1>>16) & 0xff);
		buf[3]=(u8)((tmp1>>24) & 0xff);
		printk("tmp1=%08x,buf[0]=%02x,buf[1]=%02x,buf[2]=%02x,buf[3]=%02x\n",
			tmp1,buf[0],buf[1],buf[2],buf[3]);
		//gsl_write_interface(ts->client,0x8,buf,4);
		gsl_ts_write(ts->client,0x8,buf,4);//mbg modify 20131128
	}
	touches = cinfo.finger_num;

#endif
	for(i=1;i<=MAX_CONTACTS;i++){
		if(touches == 0)
			id_sign[i] = 0;	
		id_state_flag[i] = 0;
	}
	for(i= 0;i < (touches > MAX_FINGERS ? MAX_FINGERS : touches);i++)
	{
	#ifdef GSL_NOID_VERSION
		id = cinfo.id[i];
		x =  cinfo.x[i];
		y =  cinfo.y[i];	
	#else
		x = join_bytes( ( ts->touch_data[ts->dd->x_index  + 4 * i + 1] & 0xf),
				ts->touch_data[ts->dd->x_index + 4 * i]);
		y = join_bytes(ts->touch_data[ts->dd->y_index + 4 * i + 1],
				ts->touch_data[ts->dd->y_index + 4 * i ]);
		id = ts->touch_data[ts->dd->id_index + 4 * i] >> 4;
    #endif
		if(1 <=id && id <= MAX_CONTACTS)
		{

		#ifdef STRETCH_FRAME
			stretch_frame(&x, &y);
		
		#endif
		#ifdef FILTER_POINT
			filter_point(x, y ,id);
		#else
			record_point(x, y , id);
		#endif
			report_data(ts, x_new, y_new, 10, id);		
			id_state_flag[id] = 1;
		}
	}
	
	
	for(i = 1;i <= MAX_CONTACTS ; i++)
	{
		if( (0 == touches) || ((0 != id_state_old_flag[i]) && (0 == id_state_flag[i])) ){
		#ifdef REPORT_DATA_ANDROID_4_0
			input_mt_slot(ts->input, i);
			input_report_abs(ts->input, ABS_MT_TRACKING_ID, -1);
			input_mt_report_slot_state(ts->input, MT_TOOL_FINGER, false);
		#endif
			input_report_abs(ts->input, ABS_MT_TOUCH_MAJOR, 0);//for dragonboard
			id_sign[i]=0;
		}
		id_state_old_flag[i] = id_state_flag[i];
	}
#ifndef REPORT_DATA_ANDROID_4_0
	if(0 == touches){	
		input_mt_sync(ts->input);
	#ifdef HAVE_TOUCH_KEY
		if(key_state_flag){
        		input_report_key(ts->input, key, 0);
			input_sync(ts->input);
			key_state_flag = 0;
		}
	#endif			
	}
#endif
	input_sync(ts->input);
	ts->prev_touches = touches;
}


static void gsl_ts_xy_worker(struct work_struct *work)
{
	int rc;
	u8 read_buf[4] = {0};
	struct gsl_ts *ts = container_of(work, struct gsl_ts,work);
	int ret =0;

	dprintk(DEBUG_X_Y_INFO,"---gsl_ts_xy_worker---\n");				 
#ifdef TPD_PROC_DEBUG
	if(gsl_proc_flag == 1){
		goto schedule;
	}
#endif
	/* read data from DATA_REG */
	rc = gsl_ts_read(ts->client, 0x80, ts->touch_data, ts->dd->data_size);
	dprintk(DEBUG_X_Y_INFO,"---touches: %d ---\n",ts->touch_data[0]);			
	if (rc < 0) {
		dev_err(&ts->client->dev, "read failed\n");
		goto schedule;
	}

	if (ts->touch_data[ts->dd->touch_index] == 0xff) {
		goto schedule;
	}

	rc = gsl_ts_read( ts->client, 0xbc, read_buf, sizeof(read_buf));
	if (rc < 0) {
		dev_err(&ts->client->dev, "read 0xbc failed\n");
		goto schedule;
	}
	dprintk(DEBUG_X_Y_INFO,"reg %x : %x %x %x %x\n",0xbc, read_buf[3], read_buf[2], read_buf[1], read_buf[0]);		
	if (read_buf[3] == 0 && read_buf[2] == 0 && read_buf[1] == 0 && read_buf[0] == 0){
		process_gslX680_data(ts);
	}
	else
	{
		reset_chip(ts->client);
		startup_chip(ts->client);
	}
	
schedule:
    ret = input_set_int_enable(&(config_info.input_type), 1);
	if (ret < 0)
		dprintk(DEBUG_SUSPEND,"%s irq enable failed\n", __func__);

}

#ifdef GSL_MONITOR
static void gsl_monitor_worker(void)
{
	char write_buf[4] = {0};
	char read_buf[4]  = {0};
	
	printk("----------------gsl_monitor_worker-----------------\n");	

	gsl_ts_read(gsl_client, 0xb4, read_buf, 4);	
	int_2nd[3] = int_1st[3];
	int_2nd[2] = int_1st[2];
	int_2nd[1] = int_1st[1];
	int_2nd[0] = int_1st[0];
	int_1st[3] = read_buf[3];
	int_1st[2] = read_buf[2];
	int_1st[1] = read_buf[1];
	int_1st[0] = read_buf[0];

	if (int_1st[3] == int_2nd[3] && int_1st[2] == int_2nd[2] &&int_1st[1] == int_2nd[1] && int_1st[0] == int_2nd[0]) 
	{
		printk("======int_1st: %x %x %x %x , int_2nd: %x %x %x %x ======\n",int_1st[3], int_1st[2], int_1st[1], int_1st[0], int_2nd[3], int_2nd[2],int_2nd[1],int_2nd[0]);
		init_chip(gsl_client);
	}

	write_buf[3] = 0x01;
	write_buf[2] = 0xfe;
	write_buf[1] = 0x10;
	write_buf[0] = 0x00;
	gsl_ts_write(gsl_client, 0xf0, write_buf, 4);
	gsl_ts_read(gsl_client, 0x10, read_buf, 4);
	gsl_ts_read(gsl_client, 0x10, read_buf, 4);	
	//printk("======read DAC1_0: %x %x %x %x ======\n",read_buf[3], read_buf[2], read_buf[1], read_buf[0]);
	
	if(read_buf[3] < 10 && read_buf[2] < 10 && read_buf[1] < 10 && read_buf[0] < 10)
		dac_counter ++;
	else
		dac_counter = 0;

	if(dac_counter > 2) 
	{
		printk("======read DAC1_0: %x %x %x %x ======\n",read_buf[3], read_buf[2], read_buf[1], read_buf[0]);
		init_chip(gsl_client);
		dac_counter = 0;
	}
	
	queue_delayed_work(gsl_monitor_workqueue, &gsl_monitor_work, 100);
}
#endif


irqreturn_t gsl_ts_irq(int irq, void *dev_id)
{
	struct gsl_ts *ts = (struct gsl_ts *)dev_id;
	int ret =0 ;
	dprintk(DEBUG_INT_INFO,"==========GSLX680 Interrupt============\n");

	ret = input_set_int_enable(&(config_info.input_type), 0);
	if (ret < 0)
		dprintk(DEBUG_SUSPEND,"%s irq enable failed\n", __func__);
	
	queue_work(ts->wq, &ts->work);
#ifdef GSL_TIMER
	mod_timer(&ts->gsl_timer, jiffies + msecs_to_jiffies(30));
#endif
	return IRQ_HANDLED;
}


#ifdef GSL_TIMER
static void gsl_timer_handle(unsigned long data)
{
	struct gsl_ts *ts = (struct gsl_ts *)data;

#ifdef GSL_DEBUG	
	printk("----------------gsl_timer_handle-----------------\n");	
#endif
    ret = input_set_int_enable(&(config_info.input_type), 1);
	if (ret < 0)
		dprintk(DEBUG_SUSPEND,"%s irq disable failed\n", __func__);
	check_mem_data(ts->client);
	ts->gsl_timer.expires = jiffies + 3 * HZ;
	add_timer(&ts->gsl_timer);
	//enable_irq(ts->irq);
	
}
#endif

static int gsl_ts_init_ts(struct i2c_client *client, struct gsl_ts *ts)
{
	struct input_dev *input_device;
	int  rc = 0;
	int i= 0;
	printk("[GSLX680] Enter %s\n", __func__);
	ts->dd = &devices[ts->device_id];

	if (ts->device_id == 0) {
		ts->dd->data_size = MAX_FINGERS * ts->dd->touch_bytes + ts->dd->touch_meta_data;
		ts->dd->touch_index = 0;
	}

	ts->touch_data = kzalloc(ts->dd->data_size, GFP_KERNEL);
	if (!ts->touch_data) {
		pr_err("%s: Unable to allocate memory\n", __func__);
		return -ENOMEM;
	}

	ts->prev_touches = 0;

	input_device = input_allocate_device();
	if (!input_device) {
		rc = -ENOMEM;
		goto error_alloc_dev;
	}

	ts->input = input_device;
	input_device->name = GSLX680_I2C_NAME;
	input_device->id.bustype = BUS_I2C;
	input_device->dev.parent = &client->dev;
	input_set_drvdata(input_device, ts);

#ifdef REPORT_DATA_ANDROID_4_0
	__set_bit(EV_ABS, input_device->evbit);
	__set_bit(EV_KEY, input_device->evbit);
	__set_bit(EV_REP, input_device->evbit);
	__set_bit(INPUT_PROP_DIRECT, input_device->propbit);
	input_mt_init_slots(input_device, (MAX_CONTACTS+1));
#else
	input_set_abs_params(input_device,ABS_MT_TRACKING_ID, 0, (MAX_CONTACTS+1), 0, 0);
	set_bit(EV_ABS, input_device->evbit);
	set_bit(EV_KEY, input_device->evbit);
	__set_bit(INPUT_PROP_DIRECT, input_device->propbit);
	input_device->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
#endif
#ifdef HAVE_TOUCH_KEY
		//input_device->evbit[0] = BIT_MASK(EV_KEY);
		for (i = 0; i < MAX_KEY_NUM; i++)
		{
			set_bit(key_array[i] & KEY_MAX, input_device->keybit);
		}
		
#endif

	set_bit(ABS_MT_POSITION_X, input_device->absbit);
	set_bit(ABS_MT_POSITION_Y, input_device->absbit);
	set_bit(ABS_MT_TOUCH_MAJOR, input_device->absbit);
	set_bit(ABS_MT_WIDTH_MAJOR, input_device->absbit);

	input_set_abs_params(input_device,ABS_MT_POSITION_X, 0, SCREEN_MAX_X, 0, 0);
	input_set_abs_params(input_device,ABS_MT_POSITION_Y, 0, SCREEN_MAX_Y, 0, 0);
	input_set_abs_params(input_device,ABS_MT_TOUCH_MAJOR, 0, PRESS_MAX, 0, 0);
	input_set_abs_params(input_device,ABS_MT_WIDTH_MAJOR, 0, 200, 0, 0);

	ts->wq = create_singlethread_workqueue("kworkqueue_ts");
	if (!ts->wq) {
		dev_err(&client->dev, "Could not create workqueue\n");
		goto error_wq_create;
	}
	flush_workqueue(ts->wq);	

	INIT_WORK(&ts->work, gsl_ts_xy_worker);

	rc = input_register_device(input_device);
	if (rc)
		goto error_unreg_device;

	return 0;

error_unreg_device:
	destroy_workqueue(ts->wq);
error_wq_create:
	input_free_device(input_device);
error_alloc_dev:
	kfree(ts->touch_data);
	return rc;
}
#ifdef gsl_resume_wq_mode
static void glsX680_resume_events (struct work_struct *work)
{
    int ret =0;
	ctp_wakeup(1,0);
	startup_chip(glsX680_i2c);
	check_mem_data(glsX680_i2c);
	ret = input_set_int_enable(&(config_info.input_type), 1);
	if (ret < 0)
		dprintk(DEBUG_SUSPEND,"%s irq enable failed\n", __func__);
}

static int gsl_ts_suspend(struct i2c_client *client, pm_message_t mesg)
{
        struct gsl_ts *ts = i2c_get_clientdata(client);
        dprintk(DEBUG_SUSPEND,"%s,start\n",__func__);
        int ret =0;
        cancel_work_sync(&glsX680_resume_work);
  	    flush_workqueue(gslX680_resume_wq);
  	
#ifndef CONFIG_HAS_EARLYSUSPEND
        ts->is_suspended = true;
#endif

#ifdef GSL_TIMER
	dprintk(DEBUG_SUSPEND,"gsl_ts_suspend () : delete gsl_timer\n");
	del_timer(&ts->gsl_timer);
#endif
        if(ts->is_suspended == true ){
        ret = input_set_int_enable(&(config_info.input_type), 0);
	    if (ret < 0)
		dprintk(DEBUG_SUSPEND,"%s irq disable failed\n", __func__);
        	flush_workqueue(gslX680_resume_wq);
        	cancel_work_sync(&ts->work);
        	flush_workqueue(ts->wq);
        	gslX680_shutdown_low(); 
        }
        
        ts->is_suspended = true;
        return 0;
	
}

static int gsl_ts_resume(struct i2c_client *client)
{
	//printk("[FUN]%s\n", __func__);
	
	struct gsl_ts *ts = i2c_get_clientdata(client);
	
	ts->is_suspended = true;
  	dprintk(DEBUG_SUSPEND,"I'am in gsl_ts_resume() start\n");
  	cancel_work_sync(&ts->work);
	flush_workqueue(ts->wq);
	queue_work(gslX680_resume_wq, &glsX680_resume_work);
	
#ifdef GSL_TIMER
	dprintk(DEBUG_SUSPEND, "gsl_ts_resume () : add gsl_timer\n");
	init_timer(&ts->gsl_timer);
	ts->gsl_timer.expires = jiffies + 3 * HZ;
	ts->gsl_timer.function = &gsl_timer_handle;
	ts->gsl_timer.data = (unsigned long)ts;
	add_timer(&ts->gsl_timer);
#endif
	return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void gsl_ts_early_suspend(struct early_suspend *h)
{
	struct gsl_ts *ts = container_of(h, struct gsl_ts, early_suspend);
	int ret = 0;
	dprintk(DEBUG_SUSPEND,"[GSL1680] Enter %s\n", __func__);
	cancel_work_sync(&glsX680_resume_work);
	flush_workqueue(gslX680_resume_wq);
        ts->is_suspended = false;
    ret = input_set_int_enable(&(config_info.input_type), 0);
	if (ret < 0)
		dprintk(DEBUG_SUSPEND,"%s irq disable failed\n", __func__);

	cancel_work_sync(&ts->work);
	flush_workqueue(ts->wq);
	gslX680_shutdown_low(); 
}

static void gsl_ts_late_resume(struct early_suspend *h)
{
	struct gsl_ts *ts = container_of(h, struct gsl_ts, early_suspend);
	dprintk(DEBUG_SUSPEND,"[GSL1680] Enter %s\n", __func__);
	cancel_work_sync(&ts->work);
	flush_workqueue(ts->wq);
	
#ifndef CONFIG_PM
        gsl_ts_resume(ts->client);
#endif
      if(ts->is_suspended == false){
                queue_work(gslX680_resume_wq, &glsX680_resume_work);
      }
	dprintk(DEBUG_SUSPEND,"ts->is_suspended:%d\n",ts->is_suspended);
}
#endif
#else
static void glsX680_resume_events (struct work_struct *work)
{ 
	reset_chip(glsX680_i2c);
	startup_chip(glsX680_i2c);
	check_mem_data(glsX680_i2c);
}

static int gsl_ts_suspend(struct device *dev)
{
	struct gsl_ts *ts = dev_get_drvdata(dev);
	int ret =0;
	dprintk(DEBUG_SUSPEND,"%s,start\n",__func__);
#ifdef TPD_PROC_DEBUG
	if(gsl_proc_flag == 1){
		return;
	}
#endif
	ret = input_set_int_enable(&(config_info.input_type), 0);
	if (ret < 0)
		dprintk(DEBUG_SUSPEND,"%s irq disable failed\n", __func__);

#ifdef GSL_MONITOR
		printk( "gsl_ts_suspend () : cancel gsl_monitor_work\n");
		cancel_delayed_work_sync(&gsl_monitor_work);
#endif
	gslX680_shutdown_low();

#ifdef SLEEP_CLEAR_POINT
	msleep(10); 		
	#ifdef REPORT_DATA_ANDROID_4_0
	for(i = 1; i <= MAX_CONTACTS ;i ++)
	{	
		input_mt_slot(ts->input, i);
		input_report_abs(ts->input, ABS_MT_TRACKING_ID, -1);
		input_mt_report_slot_state(ts->input, MT_TOOL_FINGER, false);
	}
	#else	
	input_mt_sync(ts->input);
	#endif
	input_sync(ts->input);
	msleep(10); 	
	report_data(ts, 1, 1, 10, 1);		
	input_sync(ts->input);	
#endif	

	return 0;
}

static int gsl_ts_resume(struct device *dev)
{
	struct gsl_ts *ts = dev_get_drvdata(dev);
	int ret =0 ;
  	dprintk(DEBUG_SUSPEND,"%s,start\n",__func__);
#ifdef TPD_PROC_DEBUG
	if(gsl_proc_flag == 1){
		return;
	}
#endif
	gslX680_shutdown_high();
	msleep(20); 	
	//queue_work(gslX680_resume_wq, &glsX680_resume_work);
	reset_chip(glsX680_i2c);
	startup_chip(glsX680_i2c);
	check_mem_data(glsX680_i2c);
	ret = input_set_int_enable(&(config_info.input_type), 1);
	if (ret < 0)
		dprintk(DEBUG_SUSPEND,"%s irq enable failed\n", __func__);
#ifdef SLEEP_CLEAR_POINT
	#ifdef REPORT_DATA_ANDROID_4_0
	for(i =1;i<=MAX_CONTACTS;i++)
	{	
		input_mt_slot(ts->input, i);
		input_report_abs(ts->input, ABS_MT_TRACKING_ID, -1);
		input_mt_report_slot_state(ts->input, MT_TOOL_FINGER, false);
	}
	#else	
	input_mt_sync(ts->input);
	#endif
	input_sync(ts->input);	
#endif
#ifdef GSL_MONITOR
	printk( "gsl_ts_resume () : queue gsl_monitor_work\n");
	queue_delayed_work(gsl_monitor_workqueue, &gsl_monitor_work, 300);
#endif	
	
	return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void gsl_ts_early_suspend(struct early_suspend *h)
{
	struct gsl_ts *ts = container_of(h, struct gsl_ts, early_suspend);
	//tp light
	tp_light_ctrl(0);
	dprintk(DEBUG_SUSPEND,"CONFIG_HAS_EARLYSUSPEND:Enter %s\n", __func__);
	gsl_ts_suspend(&ts->client->dev);
}

static void gsl_ts_late_resume(struct early_suspend *h)
{
	struct gsl_ts *ts = container_of(h, struct gsl_ts, early_suspend);
	//tp light
	tp_light_ctrl(1);	
	dprintk(DEBUG_SUSPEND,"CONFIG_HAS_EARLYSUSPEND: Enter %s\n", __func__);
	gsl_ts_resume(&ts->client->dev);
}
#endif
#endif
static int __devinit gsl_ts_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct gsl_ts *ts;
	int rc;
	int ret =0 ;

	printk("GSLX680 Enter %s\n", __func__);
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "I2C functionality not supported\n");
		return -ENODEV;
	}
 
	ts = kzalloc(sizeof(*ts), GFP_KERNEL);
	if (!ts){
	        printk("allocate data fail!\n");
		return -ENOMEM;
	}
        
	gslX680_resume_wq = create_singlethread_workqueue("gslX680_resume");
	if (gslX680_resume_wq == NULL) {
		printk("create gslX680_resume_wq fail!\n");
		return -ENOMEM;
	}
	
        glsX680_i2c = client;
	ts->client = client;
	i2c_set_clientdata(client, ts);
	ts->device_id = id->driver_data;

	ts->is_suspended = false;
	ts->int_pending = false;
	mutex_init(&ts->sus_lock);
	
	rc = gsl_ts_init_ts(client, ts);
	if (rc < 0) {
		dev_err(&client->dev, "GSLX680 init failed\n");
		goto error_mutex_destroy;
	}
#ifdef GSL_MONITOR
	gsl_client = client;
#endif
	gslX680_chip_init();    	
	init_chip(ts->client);
	cfg_adjust(ts->client);
	check_mem_data(ts->client);
	
	device_enable_async_suspend(&client->dev);
	//rc=  request_irq(client->irq, gsl_ts_irq, IRQF_TRIGGER_RISING | IRQF_SHARED, client->name, ts);
	config_info.dev = &(ts->input->dev);
	ret = input_request_int(&(config_info.input_type), gsl_ts_irq,
				CTP_IRQ_MODE, ts);
	if (ret) {
		printk( "gsl_probe: request irq failed\n");
	}

#ifdef GSL_TIMER
	printk( "gsl_ts_probe () : add gsl_timer\n");

	init_timer(&ts->gsl_timer);
	ts->gsl_timer.expires = jiffies + 3 * HZ;	//定时3  秒钟
	ts->gsl_timer.function = &gsl_timer_handle;
	ts->gsl_timer.data = (unsigned long)ts;
	add_timer(&ts->gsl_timer);
#endif
#ifdef TPD_PROC_DEBUG
	gsl_config_proc = create_proc_entry(GSL_CONFIG_PROC_FILE, 0666, NULL);
	if (gsl_config_proc == NULL)
	{
		//print_info("create_proc_entry %s failed\n", GSL_CONFIG_PROC_FILE);
	}
	else
	{
		gsl_config_proc->read_proc = gsl_config_read_proc;
		gsl_config_proc->write_proc = gsl_config_write_proc;
	}
	gsl_proc_flag = 0;
#endif
	/* create debug attribute */
	rc = device_create_file(&ts->input->dev, &dev_attr_debug_enable);

	gslX680_debug_mask = 0;

#ifdef CONFIG_HAS_EARLYSUSPEND
	ts->early_suspend.level = EARLY_SUSPEND_LEVEL_DISABLE_FB + 1;
	ts->early_suspend.suspend = gsl_ts_early_suspend;
	ts->early_suspend.resume = gsl_ts_late_resume;
	register_early_suspend(&ts->early_suspend);
#endif
#ifdef GSL_NOID_VERSION
	//GSL_DATAINIT(gsl_config_data_id);
	GSL_DATAINIT(gslX680_config_data[fw_index]);

#endif
	
#ifdef GSL_MONITOR
		printk( "gsl_ts_probe () : queue gsl_monitor_workqueue\n");
	
		INIT_DELAYED_WORK(&gsl_monitor_work, gsl_monitor_worker);
		gsl_monitor_workqueue = create_singlethread_workqueue("gsl_monitor_workqueue");
		queue_delayed_work(gsl_monitor_workqueue, &gsl_monitor_work, 1000);
#endif

	printk("[GSLX680] End %s\n", __func__);

	return 0;

	
error_req_irq_fail:
    input_free_int(&(config_info.input_type), ts);  	
error_mutex_destroy:
	mutex_destroy(&ts->sus_lock);
	input_free_device(ts->input);
	kfree(ts);
	return rc;
}

static int __devexit gsl_ts_remove(struct i2c_client *client)
{
	struct gsl_ts *ts = i2c_get_clientdata(client);
	printk("==gsl_ts_remove=\n");

#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&ts->early_suspend);
#endif

#ifdef GSL_MONITOR
		cancel_delayed_work_sync(&gsl_monitor_work);
		destroy_workqueue(gsl_monitor_workqueue);
#endif


	device_init_wakeup(&client->dev, 0);
	cancel_work_sync(&ts->work);
	cancel_work_sync(&glsX680_resume_work);
	input_free_int(&(config_info.input_type), ts);
	destroy_workqueue(ts->wq);
	input_unregister_device(ts->input);
	destroy_workqueue(gslX680_resume_wq);
	mutex_destroy(&ts->sus_lock);
	device_remove_file(&ts->input->dev, &dev_attr_debug_enable);
	kfree(ts->touch_data);
	kfree(ts);

	return 0;
}

static const struct i2c_device_id gsl_ts_id[] = {
	{GSLX680_I2C_NAME, 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, gsl_ts_id);

static struct i2c_driver gsl_ts_driver = {
	.class = I2C_CLASS_HWMON,
	.driver = {
		.name = GSLX680_I2C_NAME,
		.owner = THIS_MODULE,
	},
	.probe		= gsl_ts_probe,
	.remove		= __devexit_p(gsl_ts_remove),
	.id_table		= gsl_ts_id,
	.address_list	= normal_i2c,
#ifdef gsl_resume_wq_mode
	.suspend        =  gsl_ts_suspend,
	.resume         =  gsl_ts_resume,
#else
#ifndef CONFIG_HAS_EARLYSUSPEND
	.suspend = gsl_ts_suspend,
	.resume	= gsl_ts_resume,
#endif
#endif
};
static int ctp_get_system_config(void)
{   
			
    script_item_u   val;
	if(SCIRPT_ITEM_VALUE_TYPE_INT != script_get_item("ctp_para", "ctp_cob_gslX680", &val))
	{
		ctp_cob_gslX680=0;
	}
	else
	{
		ctp_cob_gslX680 = val.val;
	}
		
    ctp_print_info(config_info,DEBUG_INIT);
  // ctp_cob_gslX680=78;
    twi_id = config_info.twi_id;
    screen_max_x = config_info.screen_max_x;
    screen_max_y = config_info.screen_max_y;
    revert_x_flag = config_info.revert_x_flag;
    revert_y_flag = config_info.revert_y_flag;
    exchange_x_y_flag = config_info.exchange_x_y_flag;

	
	#ifdef CUSTOMER_USED
	ctp_cob_gslX680 = 999;
	#endif
//	ctp_cob_gslX680 = 1064;
	printk("ctp_cob_gslX680=%d\n",ctp_cob_gslX680);
	fw_index = getIndex(ctp_cob_gslX680);
	printk("fw_index=%d\n",fw_index);
	if (fw_index == -1) {
		printk("gslx680: no matched TP firmware!\n");
		return 0;
	}
	
    if((screen_max_x == 0) || (screen_max_y == 0)){
            printk("%s:read config error!\n",__func__);
            return 0;
    }
    return 1;
}
static int __init gsl_ts_init(void)
{
	

	script_item_u   val;
	if(SCIRPT_ITEM_VALUE_TYPE_INT != script_get_item("ctp_para", "ctp_cob_gslX680_old", &val))
	{
		ctp_cob_gslX680_old=0;
	}
	else
	{
		ctp_cob_gslX680_old = val.val;
	}
	//ctp_cob_gslX680_old = 0;
	if(ctp_cob_gslX680_old!=0)
	{
		printk("gslX680 old algorithm\n");
		return -ENODEV;
	}
	
	printk("%s\n",GSLX680_IC_INFO);

	int ret = -1;
	dprintk(DEBUG_INIT,"****************************************************************\n");
	if (input_fetch_sysconfig_para(&(config_info.input_type))) {
		printk("%s: ctp_fetch_sysconfig_para err.\n", __func__);
		//return 0;
	} else {
		ret = input_init_platform_resource(&(config_info.input_type));
		if (0 != ret) {
			printk("%s:ctp_ops.init_platform_resource err. \n", __func__);    
		}
	}

	if (config_info.ctp_used == 0) {
		printk("*** ctp_used set to 0 !\n");
		printk("*** if use ctp,please put the sys_config.fex ctp_used set to 1. \n");
		return 0;
	}
	if (!ctp_get_system_config()) {
		printk("%s:read config fail!\n",__func__);
		return ret;
	}
	
	input_set_power_enable(&(config_info.input_type), 1);
	msleep(10);	
	ctp_wakeup(0, 10);
	//ctp_wakeup(1,0);
	gsl_ts_driver.detect = ctp_detect;
	ret = i2c_add_driver(&gsl_ts_driver);
	printk("****************************************************************\n");
	return ret;
}

static void __exit gsl_ts_exit(void)
{
	printk("==gsl_ts_exit==\n");
	i2c_del_driver(&gsl_ts_driver);
	input_free_platform_resource(&(config_info.input_type));
	return;
}

module_init(gsl_ts_init);
module_exit(gsl_ts_exit);
module_param_named(debug_mask,debug_mask,int,S_IRUGO | S_IWUSR | S_IWGRP);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GSLX680 touchscreen controller driver");
MODULE_AUTHOR("Guan Yuwei, guanyuwei@basewin.com");
MODULE_ALIAS("platform:gsl_ts");

