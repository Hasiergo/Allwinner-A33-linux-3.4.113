/* drivers/input/touchscreen/gt813_827_828.h
 * 
 * 2010 - 2012 Goodix Technology.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be a reference 
 * to you, when you are integrating the GOODiX's CTP IC into your system, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 * General Public License for more details.
 * 
 * Version:1.0
 *      V1.0:2012/08/31,first release.
 */

#ifndef _LINUX_GOODIX_TOUCH_H
#define	_LINUX_GOODIX_TOUCH_H

#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/interrupt.h>
#include <linux/io.h>

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif


#include <linux/device.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <linux/async.h>
#include <linux/ioport.h>
#include <asm/irq.h>
#include <asm/delay.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <mach/irqs.h>
#include <mach/hardware.h>
#include <mach/sys_config.h>
#include <linux/init-input.h>

#include <linux/pinctrl/consumer.h>
#include <linux/pinctrl/pinconf-sunxi.h>


struct goodix_ts_data {
    spinlock_t irq_lock;
    struct i2c_client *client;
    struct input_dev  *input_dev;
    struct hrtimer timer;
    struct work_struct  work;

#ifdef CONFIG_HAS_EARLYSUSPEND
    struct early_suspend early_suspend;
#endif

    s32 irq_is_free;
    s32 irq_is_disable;
    s32 use_irq;
    u16 abs_x_max;
    u16 abs_y_max;
    u8  max_touch_num;
    u8  int_trigger_type;
    u8  green_wake_mode;
    u8  chip_type;
    u8  enter_update;
    u8  gtp_is_suspend;
    u8  gtp_rawdiff_mode;
    u8  gtp_cfg_len;
};

extern u16 show_len;
extern u16 total_len;
extern struct ctp_config_info config_info;

//***************************PART1:ON/OFF define*******************************
#define GTP_CUSTOM_CFG        0
#define GTP_DRIVER_SEND_CFG   1 
#define GTP_HAVE_TOUCH_KEY    1
#define GTP_POWER_CTRL_SLEEP  1
#define GTP_AUTO_UPDATE       1
#define GTP_CHANGE_X2Y        0
#define GTP_ESD_PROTECT       0
#define GTP_CREATE_WR_NODE    1
#define GTP_ICS_SLOT_REPORT   0

#define GTP_HEADER_FW_UPDATE  0     
#define GTP_SLIDING_WAKEUP    0     

#define GTP_DEBUG_ON          0
#define GTP_DEBUG_ARRAY_ON    0
#define GTP_DEBUG_FUNC_ON     0

//***************************PART2:TODO define**********************************

//~~~~~~~~~~D978 begin~~~~~~~~~~~
//D978_SG
#define CTP_CFG_GROUP1 {\
0x41,0x00,0x04,0x00,0x03,0x0A,0x3D,0x00,0x01,0x0A,0x23,0x09,0x50,0x3C,0x03,0x05,0x00,0x00,0x00,0x00,0x11,0x11,0x04,0x16,0x18,0x1C,0x14,0x90,0x30,0xAA,0x1F,0x21,0x39,0x13,0x00,0x00,0x00,0x9A,0x02,0x2D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x32,0x94,0xC5,0x02,0x08,0x00,0x00,0x04,0xE6,0x16,0x00,0xC9,0x1A,0x00,0xB0,0x1F,0x00,0x99,0x26,0x00,0x8A,0x2D,0x00,0x8A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x18,0x17,0x16,0x15,0x14,0x11,0x10,0x0F,0x0E,0x0D,0x0C,0x09,0x08,0x07,0x06,0x05,0x04,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x04,0x06,0x07,0x08,0x0A,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x19,0x1B,0x1C,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x29,0x01}
															

#define CTP_CFG_GROUP2 {\
}

#define CTP_CFG_GROUP3 {\
}

//D978_RSS
#define CTP_CFG_GROUP4 {\
0x41,0x00,0x04,0x00,0x03,0x0A,0x3D,0x01,0x02,0x0F,0x23,0x09,0x64,0x3C,0x03,0x05,0x00,0x00,0x00,0x00,0x11,0x11,0x04,0x16,0x18,0x1C,0x14,0x90,0x30,0xAA,0x52,0x54,0xF7,0x0C,0x00,0x00,0x00,0x9B,0x02,0x45,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x5A,0x94,0xC5,0x02,0x08,0x00,0x00,0x04,0x9A,0x21,0x00,0x8E,0x29,0x00,0x85,0x34,0x00,0x81,0x41,0x00,0x80,0x51,0x00,0x80,0x08,0x00,0x00,0x00,0xF0,0x4A,0x3A,0x50,0x00,0x27,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x18,0x17,0x16,0x15,0x14,0x11,0x10,0x0F,0x0E,0x0D,0x0C,0x09,0x08,0x07,0x06,0x05,0x04,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2A,0x29,0x28,0x27,0x26,0x25,0x24,0x23,0x22,0x21,0x20,0x1F,0x1E,0x1C,0x1B,0x19,0x14,0x13,0x12,0x11,0x10,0x0F,0x0E,0x0D,0x0C,0x0A,0x08,0x07,0x06,0x04,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x23,0x01}

#define CTP_CFG_GROUP5 {\
}

//s906_opd_gt911_1024*600
#define CTP_CFG_GROUP6 {\
}
//~~~~~~~~~~D978 end~~~~~~~~~~~



//#define GTP_RST_PORT    S5PV210_GPJ3(6)
//#define GTP_INT_PORT    S5PV210_GPH1(3)
//#define GTP_INT_IRQ     SW_INT_IRQNO_PIO
//#define GTP_INT_CFG     S3C_GPIO_SFN(0xF)
//#ifdef CONFIG_ARCH_SUN4I
//#define CTP_IRQ_NO			(IRQ_EINT21)
//#elif defined CONFIG_ARCH_SUN5I
//#define CTP_IRQ_NO			(IRQ_EINT9)
//#endif
//#define CTP_IRQ_MODE			(TRIG_EDGE_NEGATIVE)

#define GTP_GPIO_AS_INPUT(pin)          do{\
                                            gpio_direction_input(pin);\
                                            s3c_gpio_setpull(pin, S3C_GPIO_PULL_NONE);\
                                        }while(0)
#define GTP_GPIO_AS_INT(pin)            do{\
                                            GTP_GPIO_AS_INPUT(pin);\
                                            s3c_gpio_cfgpin(pin, GTP_INT_CFG);\
                                        }while(0)
#define GTP_GPIO_GET_VALUE(pin)         gpio_get_value(pin)
#define GTP_GPIO_OUTPUT(pin,level)      gpio_direction_output(pin,level)
#define GTP_GPIO_REQUEST(pin, label)    gpio_request(pin, label)
#define GTP_GPIO_FREE(pin)              gpio_free(pin)
#define GTP_IRQ_TAB                     {IRQ_TYPE_EDGE_RISING, IRQ_TYPE_EDGE_FALLING, IRQ_TYPE_LEVEL_LOW, IRQ_TYPE_LEVEL_HIGH}

//STEP_3(optional):Custom set some config by themself,if need.
#if GTP_CUSTOM_CFG
  #define GTP_MAX_HEIGHT   1024
  #define GTP_MAX_WIDTH    768
  #define GTP_INT_TRIGGER  1    //0:Rising 1:Falling
#else
  #define GTP_MAX_HEIGHT   4096
  #define GTP_MAX_WIDTH    4096
  #define GTP_INT_TRIGGER  1
#endif
#define GTP_MAX_TOUCH         10
#define GTP_ESD_CHECK_CIRCLE  2000

#define KEY_HOME 172
//STEP_4(optional):If this project have touch key,Set touch key config.                                    
#if GTP_HAVE_TOUCH_KEY
    //#define GTP_KEY_TAB	 {KEY_MENU, KEY_HOME, KEY_BACK, KEY_SEND}
    #define GTP_KEY_TAB	 {KEY_HOME}
#endif

//***************************PART3:OTHER define*********************************
#define GTP_DRIVER_VERSION    "V1.4<2012/12/12>"
#define GTP_I2C_NAME          "gt9xx_ts"
#define GTP_POLL_TIME         10
#define GTP_ADDR_LENGTH       2
#define GTP_CONFIG_MAX_LENGTH 240
#define FAIL                  0
#define SUCCESS               1

//Register define
#define GTP_READ_COOR_ADDR    0x814E
#define GTP_REG_SLEEP         0x8040
#define GTP_REG_SENSOR_ID     0x814A
#define GTP_REG_CONFIG_DATA   0x8047
#define GTP_REG_VERSION       0x8140

#define RESOLUTION_LOC        3
#define TRIGGER_LOC           8

//Log define
#define GTP_INFO(fmt,arg...)           printk("<<-GTP-INFO->> "fmt"\n",##arg)
#define GTP_ERROR(fmt,arg...)          printk("<<-GTP-ERROR->> "fmt"\n",##arg)
#define GTP_DEBUG(fmt,arg...)          do{\
                                         if(GTP_DEBUG_ON)\
                                         printk("<<-GTP-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                       }while(0)
#define GTP_DEBUG_ARRAY(array, num)    do{\
                                         s32 i;\
                                         u8* a = array;\
                                         if(GTP_DEBUG_ARRAY_ON)\
                                         {\
                                            printk("<<-GTP-DEBUG-ARRAY->>\n");\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                printk("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printk("\n");\
                                                }\
                                            }\
                                            printk("\n");\
                                        }\
                                       }while(0)
#define GTP_DEBUG_FUNC()               do{\
                                         if(GTP_DEBUG_FUNC_ON)\
                                         printk("<<-GTP-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)
#define GTP_SWAP(x, y)                 do{\
                                         typeof(x) z = x;\
                                         x = y;\
                                         y = z;\
                                       }while (0)

//****************************PART4:UPDATE define*******************************
//Error no
#define ERROR_NO_FILE           2   //ENOENT
#define ERROR_FILE_READ         23  //ENFILE
#define ERROR_FILE_TYPE         21  //EISDIR
#define ERROR_GPIO_REQUEST      4   //EINTR
#define ERROR_I2C_TRANSFER      5   //EIO
#define ERROR_NO_RESPONSE       16  //EBUSY
#define ERROR_TIMEOUT           110 //ETIMEDOUT

//*****************************End of Part III********************************

#endif /* _LINUX_GOODIX_TOUCH_H */
