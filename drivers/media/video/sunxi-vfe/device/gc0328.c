/*
 * A V4L2 driver for GalaxyCore GC0328 cameras.
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/videodev2.h>
#include <linux/clk.h>
#include <media/v4l2-device.h>
#include <media/v4l2-chip-ident.h>
#include <media/v4l2-mediabus.h>
#include <linux/io.h>
#include "camera.h"
MODULE_AUTHOR("raymonxiu");
MODULE_DESCRIPTION("A low-level driver for GalaxyCore GC0328 sensors");
MODULE_LICENSE("GPL");
//for internel driver debug
#define DEV_DBG_EN   		0
#if(DEV_DBG_EN == 1)		
#define vfe_dev_dbg(x,arg...) printk("[CSI_DEBUG][GC0328]"x,##arg)
#else
#define vfe_dev_dbg(x,arg...) 
#endif
#define vfe_dev_err(x,arg...) printk("[CSI_ERR][GC0328]"x,##arg)
#define vfe_dev_print(x,arg...) printk("[CSI][GC0328]"x,##arg)

#define LOG_ERR_RET(x)  { \
                          int ret;  \
                          ret = x; \
                          if(ret < 0) {\
                            vfe_dev_err("error at %s\n",__func__);  \
                            return ret; \
                          } \
                        }

//define module timing
#define MCLK (24*1000*1000)
#define VREF_POL          V4L2_MBUS_VSYNC_ACTIVE_HIGH
#define HREF_POL          V4L2_MBUS_HSYNC_ACTIVE_HIGH
#define CLK_POL           V4L2_MBUS_PCLK_SAMPLE_RISING
#define V4L2_IDENT_SENSOR 0x0328

//define the voltage level of control signal
#define CSI_STBY_ON			1
#define CSI_STBY_OFF 		0
#define CSI_RST_ON			0
#define CSI_RST_OFF			1
#define CSI_PWR_ON			1
#define CSI_PWR_OFF			0

#define regval_list reg_list_a8_d8
#define REG_TERM 0xff
#define VAL_TERM 0xff
#define REG_DLY  0xffff


/*
 * Our nominal (default) frame rate.
 */
#define SENSOR_FRAME_RATE 10

/*
 * The gc0328 sits on i2c with ID 0x42
 */
#define I2C_ADDR 0x42
#define SENSOR_NAME "gc0328"
/*
 * Information we maintain about a known sensor.
 */
struct sensor_format_struct;  /* coming later */

struct cfg_array { /* coming later */
	struct regval_list * regs;
	int size;
};

static inline struct sensor_info *to_state(struct v4l2_subdev *sd)
{
  return container_of(sd, struct sensor_info, sd);
}



/** The default register settings**/
static struct regval_list sensor_default_regs[] = {
	{0xfe , 0x80},    
	{0xfe , 0x80},    
	{0xfc , 0x16},     
	{0xfc , 0x16},     
	{0xfc , 0x16},     
	{0xfc , 0x16},     
	
	{0xfe , 0x00},   
	{0x4f , 0x00},  
	{0x42 , 0x00},  
	{0x03 , 0x00},  
	{0x04 , 0xc0},  
	{0x77 , 0x62},  
	{0x78 , 0x40},  
	{0x79 , 0x4d},  
	
	
	{0xfe , 0x01},    
	{0x4f , 0x00},    
	{0x4c , 0x01},     
	{0xfe , 0x00},    
	//////////////////////////////
	 ///////////AWB///////////
	////////////////////////////////
#if 1	
	{0xfe , 0x01},
	{0x51 , 0x80}, //20
	{0x52 , 0x12}, //16 1f
	{0x53 , 0x80}, //40
	{0x54 , 0x60}, //9f
	{0x55 , 0x04}, //01
	{0x56 , 0x0e}, //00
	{0x5b , 0x02},//02
	{0x61 , 0xdc}, //R2G_stand0[70]
	{0x62 , 0xca},  //B2G_stand0[70]

	{0x70 , 0xf5},
	{0x71 , 0x0a},
	{0x72 , 0x18},  // y2c
	{0x73 , 0x30}, //28 20  AWB_C_inter
	{0x74 , 0x58}, //40 20   AWB_C_max
	{0x7c , 0x71}, //AWB speed,AWB margin
	{0x7d , 0x00}, //10 AWB every N
	{0x76 , 0x8f}, //move mode en,Move mode TH
	{0x79 , 0x00}, //00
	{0x4f , 0x00},
	{0x4d , 0x34},
	{0x4e , 0x04},
	{0x4e , 0x02},
	{0x4e , 0x02},
	{0x4d , 0x43},
	{0x4e , 0x04},
	{0x4e , 0x04},//D50
	{0x4e , 0x02}, //d65 add
	{0x4e , 0x02}, //d65 add
	{0x4d , 0x53},
	{0x4e , 0x08},  //cwf
	{0x4e , 0x04},
	{0x4e , 0x02},  //D65
	{0x4e , 0x02},  //D65
	{0x4d , 0x63},
	{0x4e , 0x10},  //tl84
	{0x4d , 0x72},
	{0x4e , 0x20},
	{0x4e , 0x20},
	{0x4d , 0x82},
	{0x4e , 0x20},  //A 
	{0x4e , 0x20},
	{0x4d , 0x92},   
	{0x4e , 0x20},  //A
	{0x4e , 0x20},
	{0x4d , 0xa0},
	{0x4e , 0x40},  //  H
	{0x4e , 0x40},  
	{0x4e , 0x40},
	{0x4f , 0x01},    
	{0x50 , 0x84}, //80
	{0xfe , 0x00}, //page0

#else   //  mtk  awb
	{0xfe , 0x01},   
	{0x51 , 0x80},   
	{0x52 , 0x12},   
	{0x53 , 0x80},   
	{0x54 , 0x60},   
	{0x55 , 0x01},   
	{0x56 , 0x06},   
	{0x5b , 0x02},   
	{0x61 , 0xdc},   
	{0x62 , 0xdc},   
	{0x7c , 0x71},   
	{0x7d , 0x00},   
	{0x76 , 0x00},   
	{0x79 , 0x20},   
	{0x7b , 0x00},   
	{0x70 , 0xFF}, 
	{0x71 , 0x00},   
	{0x72 , 0x10},   
	{0x73 , 0x40},   
	{0x74 , 0x40},   
	////AWB//
	{0x50 , 0x00},    
	{0xfe , 0x01},    
	{0x4f , 0x00},     
	{0x4c , 0x01},   
	{0x4f , 0x00},   
	{0x4f , 0x00},  
	{0x4f , 0x00},  
	{0x4d , 0x36},  
	{0x4e , 0x02},  
	{0x4d , 0x46},  
	{0x4e , 0x02},  
	{0x4e , 0x02},  
	{0x4d , 0x53},  
	{0x4e , 0x08},  
	{0x4e , 0x04},  
	{0x4e , 0x04},  
	{0x4d , 0x63},  
	{0x4e , 0x08},  
	{0x4e , 0x08},  
	{0x4d , 0x82},  
	{0x4e , 0x20},  
	{0x4e , 0x20},  
	{0x4d , 0x92},  
	{0x4e , 0x40},  
	{0x4d , 0xa2},  
	{0x4e , 0x40},  
	{0x4f , 0x01},  
	
	{0x50 , 0x88},    
	{0xfe , 0x00},  

#endif
	////////////////////////////////////////////////
	////////////     BLK      //////////////////////
	////////////////////////////////////////////////
	{0x27 , 0x00},   
	{0x2a , 0x40},  
	{0x2b , 0x40},  
	{0x2c , 0x40},  
	{0x2d , 0x40},  
	
	
	//////////////////////////////////////////////
	////////// page  0    ////////////////////////
	//////////////////////////////////////////////
	{0xfe , 0x00},   
	
	{0x0d , 0x01},    
	{0x0e , 0xe8},    
	{0x0f , 0x02},    
	{0x10 , 0x88},    
	{0x09 , 0x00},    
	{0x0a , 0x00},    
	{0x0b , 0x00},    
	{0x0c , 0x00},    
	{0x16 , 0x00},    
	{0x17 , 0x14},    
	{0x18 , 0x0e},    
	{0x19 , 0x06},    
	
	{0x1b , 0x48},    
	{0x1f , 0xC8},    
	{0x20 , 0x01},    
	{0x21 , 0x78},    
	{0x22 , 0xb0},    
	{0x23 , 0x04},    
	{0x24 , 0x3f},    
	{0x26 , 0x00},    
	
	{0x50 , 0x01}, //crop mode
	                
	//global gain for range 
	{0x70 , 0x85},   
	
	
	////////////////////////////////////////////////
	////////////     block enable      /////////////
	////////////////////////////////////////////////
	{0x40 , 0x7f},   
	{0x41 , 0x24},   
	{0x42 , 0xff},
	{0x45 , 0x00},   
	{0x44 , 0x02},   // yuv format
	{0x46 , 0x03},   // hs  vs
	
	{0x4b , 0x01},   
	{0x50 , 0x01},  
	
	//DN & EEINTP
	{0x7e , 0x0a},    
	{0x7f , 0x03},    
	{0x81 , 0x15},    
	{0x82 , 0x85},    
	{0x83 , 0x02},    
	{0x84 , 0xe5},    
	{0x90 , 0xac},    
	{0x92 , 0x02},    
	{0x94 , 0x02},    
	{0x95 , 0x32},    
	
	///////YCP
	{0xd1 , 0x28},
	{0xd2 , 0x28},
	{0xdd , 0x58},
	{0xde , 0x36},
	{0xe4 , 0x88},
	{0xe5 , 0x40},    
	{0xd7 , 0x0e},    
	                      
	///////////////////////////// 
	//////////////// GAMMA ////// 
	///////////////////////////// 
	//rgb gamma                  
	{0xfe , 0x00},

	{0xBF , 0x0E},
	{0xc0 , 0x1C},
	{0xc1 , 0x34},
	{0xc2 , 0x48},
	{0xc3 , 0x5A},
	{0xc4 , 0x6B},
	{0xc5 , 0x7B},
	{0xc6 , 0x95},
	{0xc7 , 0xAB},
	{0xc8 , 0xBF},
	{0xc9 , 0xCE},
	{0xcA , 0xD9},
	{0xcB , 0xE4},
	{0xcC , 0xEC},
	{0xcD , 0xF7},
	{0xcE , 0xFD},
	{0xcF , 0xFF},
	
#if 0
//case GC0328_RGB_Gamma_m1:						//smallest gamma curve
	{0xfe , 0x00},
	{0xbf , 0x06},
	{0xc0 , 0x12},
	{0xc1 , 0x22},
	{0xc2 , 0x35},
	{0xc3 , 0x4b},
	{0xc4 , 0x5f},
	{0xc5 , 0x72},
	{0xc6 , 0x8d},
	{0xc7 , 0xa4},
	{0xc8 , 0xb8},
	{0xc9 , 0xc8},
	{0xca , 0xd4},
	{0xcb , 0xde},
	{0xcc , 0xe6},
	{0xcd , 0xf1},
	{0xce , 0xf8},
	{0xcf , 0xfd},
//case GC0328_RGB_Gamma_m2:
	{0xBF , 0x08},
	{0xc0 , 0x0F},
	{0xc1 , 0x21},
	{0xc2 , 0x32},
	{0xc3 , 0x43},
	{0xc4 , 0x50},
	{0xc5 , 0x5E},
	{0xc6 , 0x78},
	{0xc7 , 0x90},
	{0xc8 , 0xA6},
	{0xc9 , 0xB9},
	{0xcA , 0xC9},
	{0xcB , 0xD6},
	{0xcC , 0xE0},
	{0xcD , 0xEE},
	{0xcE , 0xF8},
	{0xcF , 0xFF},
//case GC0328_RGB_Gamma_m3:			
	{0xBF , 0x0B},
	{0xc0 , 0x16},
	{0xc1 , 0x29},
	{0xc2 , 0x3C},
	{0xc3 , 0x4F},
	{0xc4 , 0x5F},
	{0xc5 , 0x6F},
	{0xc6 , 0x8A},
	{0xc7 , 0x9F},
	{0xc8 , 0xB4},
	{0xc9 , 0xC6},
	{0xcA , 0xD3},
	{0xcB , 0xDD},
	{0xcC , 0xE5},
	{0xcD , 0xF1},
	{0xcE , 0xFA},
	{0xcF , 0xFF},
//case GC0328_RGB_Gamma_m4:
	{0xBF , 0x0E},
	{0xc0 , 0x1C},
	{0xc1 , 0x34},
	{0xc2 , 0x48},
	{0xc3 , 0x5A},
	{0xc4 , 0x6B},
	{0xc5 , 0x7B},
	{0xc6 , 0x95},
	{0xc7 , 0xAB},
	{0xc8 , 0xBF},
	{0xc9 , 0xCE},
	{0xcA , 0xD9},
	{0xcB , 0xE4},
	{0xcC , 0xEC},
	{0xcD , 0xF7},
	{0xcE , 0xFD},
	{0xcF , 0xFF},
//case GC0328_RGB_Gamma_m5:
	{0xBF , 0x10},
	{0xc0 , 0x20},
	{0xc1 , 0x38},
	{0xc2 , 0x4E},
	{0xc3 , 0x63},
	{0xc4 , 0x76},
	{0xc5 , 0x87},
	{0xc6 , 0xA2},
	{0xc7 , 0xB8},
	{0xc8 , 0xCA},
	{0xc9 , 0xD8},
	{0xcA , 0xE3},
	{0xcB , 0xEB},
	{0xcC , 0xF0},
	{0xcD , 0xF8},
	{0xcE , 0xFD},
	{0xcF , 0xFF},
//case GC0328_RGB_Gamma_m6:										// largest gamma curve
	{0xBF , 0x14},
	{0xc0 , 0x28},
	{0xc1 , 0x44},
	{0xc2 , 0x5D},
	{0xc3 , 0x72},
	{0xc4 , 0x86},
	{0xc5 , 0x95},
	{0xc6 , 0xB1},
	{0xc7 , 0xC6},
	{0xc8 , 0xD5},
	{0xc9 , 0xE1},
	{0xcA , 0xEA},
	{0xcB , 0xF1},
	{0xcC , 0xF5},
	{0xcD , 0xFB},
	{0xcE , 0xFE},
	{0xcF , 0xFF},
//case GC0328_RGB_Gamma_night:									//Gamma for night mode
	{0xBF , 0x0B},
	{0xc0 , 0x16},
	{0xc1 , 0x29},
	{0xc2 , 0x3C},
	{0xc3 , 0x4F},
	{0xc4 , 0x5F},
	{0xc5 , 0x6F},
	{0xc6 , 0x8A},
	{0xc7 , 0x9F},
	{0xc8 , 0xB4},
	{0xc9 , 0xC6},
	{0xcA , 0xD3},
	{0xcB , 0xDD},
	{0xcC , 0xE5},
	{0xcD , 0xF1},
	{0xcE , 0xFA},
	{0xcF , 0xFF},


#endif
	///Y gamma           
	{0xfe , 0x00},    
	{0x63 , 0x00},    
	{0x64 , 0x05},    
	{0x65 , 0x0b},    
	{0x66 , 0x19},    
	{0x67 , 0x2e},    
	{0x68 , 0x40},    
	{0x69 , 0x54},    
	{0x6a , 0x66},    
	{0x6b , 0x86},    
	{0x6c , 0xa7},    
	{0x6d , 0xc6},    
	{0x6e , 0xe4},    
	{0x6f , 0xFF},
	               
	//////ASDE             
	{0xfe , 0x01},    
	{0x18 , 0x02},    
	{0xfe , 0x00},    
	{0x98 , 0x00},    
	{0x9b , 0x20},    
	{0x9c , 0x80},    
	{0xa4 , 0x10},    
	{0xa8 , 0xB0},    
	{0xaa , 0x40},    
	{0xa2 , 0x23},    
	{0xad , 0x01},    
	
	//////////////////////////////////////////////
	////////// AEC    ////////////////////////
	//////////////////////////////////////////////
	{0xfe , 0x01},   
	{0x9c , 0x02},   
	{0x08 , 0xa0},   
	{0x09 , 0xe8},   
	
	{0x10 , 0x00},  
	{0x11 , 0x11},   
	{0x12 , 0x10},   
	{0x13 , 0x98},   
	{0x15 , 0xfc},   
	{0x18 , 0x03},
	{0x21 , 0xc0},   
	{0x22 , 0x60},   
	{0x23 , 0x30},   
	{0x25 , 0x00},   
	{0x24 , 0x14},   

 	{0xfe , 0x00}, 	
 	{0x05 , 0x02}, 	
 	{0x06 , 0x2c}, 
 	{0x07 , 0x00},
 	{0x08 , 0xb8},
 
 	{0xfe , 0x01},   
 	{0x29 , 0x00},   //anti-flicker step [118]
 	{0x2a , 0x60},   //anti-flicker step [70]
 
 	{0x2b , 0x02},   //exp level 0  14.28fps
 	{0x2c , 0xa0}, 
 	{0x2d , 0x03},   //exp level 1  12.50fps
 	{0x2e , 0x00}, 
 	{0x2f , 0x05},   //exp level 2  10.00fps
 	{0x30 , 0x40}, 
 	{0x31 , 0x07},   //exp level 3  7.14fps
 	{0x32 , 0x80}, 
 	{0x33 , 0x20}, 

	
	//////////////////////////////////////
	////////////LSC//////////////////////
	//////////////////////////////////////
	//gc0328 Alight lsc reg setting list
	////Record date: 2013-04-01 15:59:05
	{0xfe , 0x01},
	{0xc0 , 0x0d},
	{0xc1 , 0x05},
	{0xc2 , 0x00},
	{0xc6 , 0x07},
	{0xc7 , 0x03},
	{0xc8 , 0x01},
	{0xba , 0x19},
	{0xbb , 0x10},
	{0xbc , 0x0a},
	{0xb4 , 0x19},
	{0xb5 , 0x0d},
	{0xb6 , 0x09},
	{0xc3 , 0x00},
	{0xc4 , 0x00},
	{0xc5 , 0x0e},
	{0xc9 , 0x00},
	{0xca , 0x00},
	{0xcb , 0x00},
	{0xbd , 0x07},
	{0xbe , 0x00},
	{0xbf , 0x0e},
	{0xb7 , 0x09},
	{0xb8 , 0x00},
	{0xb9 , 0x0d},
	{0xa8 , 0x01},
	{0xa9 , 0x00},
	{0xaa , 0x03},
	{0xab , 0x02},
	{0xac , 0x05},
	{0xad , 0x0c},
	{0xae , 0x03},
	{0xaf , 0x00},
	{0xb0 , 0x04},
	{0xb1 , 0x04},
	{0xb2 , 0x03},
	{0xb3 , 0x08},
	{0xa4 , 0x00},
	{0xa5 , 0x00},
	{0xa6 , 0x00},
	{0xa7 , 0x00},
	{0xa1 , 0x3c},
	{0xa2 , 0x50},
	{0xfe , 0x00},
	              
	///cct       
	{0xB1 , 0x00},   
	{0xB2 , 0x02},   
	{0xB3 , 0x07},   
	{0xB4 , 0xf0},   
	{0xB5 , 0x05},   
	{0xB6 , 0xf8},   
	
	
	{0xfe , 0x00},   
	{0x27 , 0xf7},  
	{0x28 , 0x7F},   
	{0x29 , 0x20},   
	{0x33 , 0x20},   
	{0x34 , 0x20},   
	{0x35 , 0x20},   
	{0x36 , 0x20},   
	{0x32 , 0x08},   
	
	{0x47 , 0x00},   
	{0x48 , 0x00},   
	
	{0xfe , 0x01},  
	{0x79 , 0x00},  
	{0x7d , 0x00},   
	{0x50 , 0x88},   
	{0x5b , 0x04}, 
	{0x76 , 0x8f},   
	{0x80 , 0x70},
	{0x81 , 0x70},
	{0x82 , 0xb0},
	{0x70 , 0xff}, 
	{0x71 , 0x00}, 
	{0x72 , 0x10}, 
	{0x73 , 0x40}, 
	{0x74 , 0x40}, 
	
	{0xfe , 0x00},  
	{0x70 , 0x45},  
	{0x4f , 0x01},  
	{0xf1 , 0x07},  
	
	{0xf2 , 0x01},  
};

/*
 * The white balance settings
 * Here only tune the R G B channel gain. 
 * The white balance enalbe bit is modified in sensor_s_autowb and sensor_s_wb
 */
static struct regval_list sensor_wb_manual[] = { 
	{0xfe,0x00},
};

static struct regval_list sensor_wb_auto_regs[] = {
	{0xfe,0x00},
	{0x77,0x62},//57
	{0x78,0x40},//4
	{0x79,0x4d},//45
	{0x42,0xff}
};

static struct regval_list sensor_wb_incandescence_regs[] = {
	//bai re guang
	{0xfe,0x00},
	{0x42,0xfd},

	{0x77,0x48},
	{0x78,0x40},
	{0x79,0x5c}
};

static struct regval_list sensor_wb_fluorescent_regs[] = {
	//ri guang deng
	{0xfe,0x00},
	{0x42,0xfd},

	{0x77,0x40},
	{0x78,0x42},
	{0x79,0x50}
};

static struct regval_list sensor_wb_tungsten_regs[] = {
//wu si deng
	{0xfe,0x00},
	{0x42,0xfd},

	{0x77,0x40},
	{0x78,0x54},
	{0x79,0x70}
};

static struct regval_list sensor_wb_horizon[] = { 
//null
};

static struct regval_list sensor_wb_daylight_regs[] = {
	//tai yang guang
	{0xfe,0x00},
	{0x42,0xfd},

	{0x77,0x74},
	{0x78,0x52},
	{0x79,0x40}
};

static struct regval_list sensor_wb_flash[] = { 
//null
};


static struct regval_list sensor_wb_cloud_regs[] = {
	{0xfe,0x00},
	{0x42,0xfd},

	{0x77,0x8c},
	{0x78,0x50},
	{0x79,0x40}
};

static struct regval_list sensor_wb_shade[] = { 
//null
};

static struct cfg_array sensor_wb[] = {
  { 
  	.regs = sensor_wb_manual,             //V4L2_WHITE_BALANCE_MANUAL       
    .size = ARRAY_SIZE(sensor_wb_manual),
  },
  {
  	.regs = sensor_wb_auto_regs,          //V4L2_WHITE_BALANCE_AUTO      
    .size = ARRAY_SIZE(sensor_wb_auto_regs),
  },
  {
  	.regs = sensor_wb_incandescence_regs, //V4L2_WHITE_BALANCE_INCANDESCENT 
    .size = ARRAY_SIZE(sensor_wb_incandescence_regs),
  },
  {
  	.regs = sensor_wb_fluorescent_regs,   //V4L2_WHITE_BALANCE_FLUORESCENT  
    .size = ARRAY_SIZE(sensor_wb_fluorescent_regs),
  },
  {
  	.regs = sensor_wb_tungsten_regs,      //V4L2_WHITE_BALANCE_FLUORESCENT_H
    .size = ARRAY_SIZE(sensor_wb_tungsten_regs),
  },
  {
  	.regs = sensor_wb_horizon,            //V4L2_WHITE_BALANCE_HORIZON    
    .size = ARRAY_SIZE(sensor_wb_horizon),
  },  
  {
  	.regs = sensor_wb_daylight_regs,      //V4L2_WHITE_BALANCE_DAYLIGHT     
    .size = ARRAY_SIZE(sensor_wb_daylight_regs),
  },
  {
  	.regs = sensor_wb_flash,              //V4L2_WHITE_BALANCE_FLASH        
    .size = ARRAY_SIZE(sensor_wb_flash),
  },
  {
  	.regs = sensor_wb_cloud_regs,         //V4L2_WHITE_BALANCE_CLOUDY       
    .size = ARRAY_SIZE(sensor_wb_cloud_regs),
  },
  {
  	.regs = sensor_wb_shade,              //V4L2_WHITE_BALANCE_SHADE  
    .size = ARRAY_SIZE(sensor_wb_shade),
  },
//  {
//  	.regs = NULL,
//    .size = 0,
//  },
};
                                          

/*
 * The color effect settings
 */
static struct regval_list sensor_colorfx_none_regs[] = {
	{0x43,0x00}	
};

static struct regval_list sensor_colorfx_bw_regs[] = {

	{0x43,0x02},
	{0xda,0xd0},
	{0xdb,0x28}
};

static struct regval_list sensor_colorfx_sepia_regs[] = {
	{0x43,0x02},
	{0xda,0xc0},
	{0xdb,0xc0}
};

static struct regval_list sensor_colorfx_negative_regs[] = {

	{0x43,0x01}
};

static struct regval_list sensor_colorfx_emboss_regs[] = {
	{0x43,0x02},
	{0xda,0x50},
	{0xdb,0xe0}
};

static struct regval_list sensor_colorfx_sketch_regs[] = {
	{0x43,0x02},
	{0xda,0x00},
	{0xdb,0x00}
};

static struct regval_list sensor_colorfx_sky_blue_regs[] = {
//NULL
};

static struct regval_list sensor_colorfx_grass_green_regs[] = {
	
};

static struct regval_list sensor_colorfx_skin_whiten_regs[] = {
	
};

static struct regval_list sensor_colorfx_vivid_regs[] = {
//NULL
};

static struct regval_list sensor_colorfx_aqua_regs[] = {
//null
};

static struct regval_list sensor_colorfx_art_freeze_regs[] = {
//null
};

static struct regval_list sensor_colorfx_silhouette_regs[] = {
//null
};

static struct regval_list sensor_colorfx_solarization_regs[] = {
//null
};

static struct regval_list sensor_colorfx_antique_regs[] = {
//null
};

static struct regval_list sensor_colorfx_set_cbcr_regs[] = {
//null
};

static struct cfg_array sensor_colorfx[] = {
  {
  	.regs = sensor_colorfx_none_regs,         //V4L2_COLORFX_NONE = 0,         
    .size = ARRAY_SIZE(sensor_colorfx_none_regs),
  },
  {
  	.regs = sensor_colorfx_bw_regs,           //V4L2_COLORFX_BW   = 1,  
    .size = ARRAY_SIZE(sensor_colorfx_bw_regs),
  },
  {
  	.regs = sensor_colorfx_sepia_regs,        //V4L2_COLORFX_SEPIA  = 2,   
    .size = ARRAY_SIZE(sensor_colorfx_sepia_regs),
  },
  {
  	.regs = sensor_colorfx_negative_regs,     //V4L2_COLORFX_NEGATIVE = 3,     
    .size = ARRAY_SIZE(sensor_colorfx_negative_regs),
  },
  {
  	.regs = sensor_colorfx_emboss_regs,       //V4L2_COLORFX_EMBOSS = 4,       
    .size = ARRAY_SIZE(sensor_colorfx_emboss_regs),
  },
  {
  	.regs = sensor_colorfx_sketch_regs,       //V4L2_COLORFX_SKETCH = 5,       
    .size = ARRAY_SIZE(sensor_colorfx_sketch_regs),
  },
  {
  	.regs = sensor_colorfx_sky_blue_regs,     //V4L2_COLORFX_SKY_BLUE = 6,     
    .size = ARRAY_SIZE(sensor_colorfx_sky_blue_regs),
  },
  {
  	.regs = sensor_colorfx_grass_green_regs,  //V4L2_COLORFX_GRASS_GREEN = 7,  
    .size = ARRAY_SIZE(sensor_colorfx_grass_green_regs),
  },
  {
  	.regs = sensor_colorfx_skin_whiten_regs,  //V4L2_COLORFX_SKIN_WHITEN = 8,  
    .size = ARRAY_SIZE(sensor_colorfx_skin_whiten_regs),
  },
  {
  	.regs = sensor_colorfx_vivid_regs,        //V4L2_COLORFX_VIVID = 9,        
    .size = ARRAY_SIZE(sensor_colorfx_vivid_regs),
  },
  {
  	.regs = sensor_colorfx_aqua_regs,         //V4L2_COLORFX_AQUA = 10,        
    .size = ARRAY_SIZE(sensor_colorfx_aqua_regs),
  },
  {
  	.regs = sensor_colorfx_art_freeze_regs,   //V4L2_COLORFX_ART_FREEZE = 11,  
    .size = ARRAY_SIZE(sensor_colorfx_art_freeze_regs),
  },
  {
  	.regs = sensor_colorfx_silhouette_regs,   //V4L2_COLORFX_SILHOUETTE = 12,  
    .size = ARRAY_SIZE(sensor_colorfx_silhouette_regs),
  },
  {
  	.regs = sensor_colorfx_solarization_regs, //V4L2_COLORFX_SOLARIZATION = 13,
    .size = ARRAY_SIZE(sensor_colorfx_solarization_regs),
  },
  {
  	.regs = sensor_colorfx_antique_regs,      //V4L2_COLORFX_ANTIQUE = 14,     
    .size = ARRAY_SIZE(sensor_colorfx_antique_regs),
  },
  {
  	.regs = sensor_colorfx_set_cbcr_regs,     //V4L2_COLORFX_SET_CBCR = 15, 
    .size = ARRAY_SIZE(sensor_colorfx_set_cbcr_regs),
  },
};



/*
 * The brightness setttings
 */
static struct regval_list sensor_brightness_neg4_regs[] = {
//NULL
};

static struct regval_list sensor_brightness_neg3_regs[] = {
//NULL
};

static struct regval_list sensor_brightness_neg2_regs[] = {
//NULL
};

static struct regval_list sensor_brightness_neg1_regs[] = {
//NULL
};

static struct regval_list sensor_brightness_zero_regs[] = {
//NULL
};

static struct regval_list sensor_brightness_pos1_regs[] = {
//NULL
};

static struct regval_list sensor_brightness_pos2_regs[] = {
//NULL
};

static struct regval_list sensor_brightness_pos3_regs[] = {
//NULL
};

static struct regval_list sensor_brightness_pos4_regs[] = {
//NULL
};

static struct cfg_array sensor_brightness[] = {
  {
  	.regs = sensor_brightness_neg4_regs,
  	.size = ARRAY_SIZE(sensor_brightness_neg4_regs),
  },
  {
  	.regs = sensor_brightness_neg3_regs,
  	.size = ARRAY_SIZE(sensor_brightness_neg3_regs),
  },
  {
  	.regs = sensor_brightness_neg2_regs,
  	.size = ARRAY_SIZE(sensor_brightness_neg2_regs),
  },
  {
  	.regs = sensor_brightness_neg1_regs,
  	.size = ARRAY_SIZE(sensor_brightness_neg1_regs),
  },
  {
  	.regs = sensor_brightness_zero_regs,
  	.size = ARRAY_SIZE(sensor_brightness_zero_regs),
  },
  {
  	.regs = sensor_brightness_pos1_regs,
  	.size = ARRAY_SIZE(sensor_brightness_pos1_regs),
  },
  {
  	.regs = sensor_brightness_pos2_regs,
  	.size = ARRAY_SIZE(sensor_brightness_pos2_regs),
  },
  {
  	.regs = sensor_brightness_pos3_regs,
  	.size = ARRAY_SIZE(sensor_brightness_pos3_regs),
  },
  {
  	.regs = sensor_brightness_pos4_regs,
  	.size = ARRAY_SIZE(sensor_brightness_pos4_regs),
  },
};

/*
 * The contrast setttings
 */
static struct regval_list sensor_contrast_neg4_regs[] = {
//NULL
};

static struct regval_list sensor_contrast_neg3_regs[] = {
//NULL
};

static struct regval_list sensor_contrast_neg2_regs[] = {
//NULL
};

static struct regval_list sensor_contrast_neg1_regs[] = {
//NULL
};

static struct regval_list sensor_contrast_zero_regs[] = {
//NULL
};

static struct regval_list sensor_contrast_pos1_regs[] = {
//NULL
};

static struct regval_list sensor_contrast_pos2_regs[] = {
//NULL
};

static struct regval_list sensor_contrast_pos3_regs[] = {
//NULL
};

static struct regval_list sensor_contrast_pos4_regs[] = {
};

static struct cfg_array sensor_contrast[] = {
  {
  	.regs = sensor_contrast_neg4_regs,
  	.size = ARRAY_SIZE(sensor_contrast_neg4_regs),
  },
  {
  	.regs = sensor_contrast_neg3_regs,
  	.size = ARRAY_SIZE(sensor_contrast_neg3_regs),
  },
  {
  	.regs = sensor_contrast_neg2_regs,
  	.size = ARRAY_SIZE(sensor_contrast_neg2_regs),
  },
  {
  	.regs = sensor_contrast_neg1_regs,
  	.size = ARRAY_SIZE(sensor_contrast_neg1_regs),
  },
  {
  	.regs = sensor_contrast_zero_regs,
  	.size = ARRAY_SIZE(sensor_contrast_zero_regs),
  },
  {
  	.regs = sensor_contrast_pos1_regs,
  	.size = ARRAY_SIZE(sensor_contrast_pos1_regs),
  },
  {
  	.regs = sensor_contrast_pos2_regs,
  	.size = ARRAY_SIZE(sensor_contrast_pos2_regs),
  },
  {
  	.regs = sensor_contrast_pos3_regs,
  	.size = ARRAY_SIZE(sensor_contrast_pos3_regs),
  },
  {
  	.regs = sensor_contrast_pos4_regs,
  	.size = ARRAY_SIZE(sensor_contrast_pos4_regs),
  },
};

/*
 * The saturation setttings
 */
static struct regval_list sensor_saturation_neg4_regs[] = {
//NULL
};

static struct regval_list sensor_saturation_neg3_regs[] = {
//NULL
};

static struct regval_list sensor_saturation_neg2_regs[] = {
//NULL
};

static struct regval_list sensor_saturation_neg1_regs[] = {
//NULL
};

static struct regval_list sensor_saturation_zero_regs[] = {
//NULL
};

static struct regval_list sensor_saturation_pos1_regs[] = {
//NULL
};

static struct regval_list sensor_saturation_pos2_regs[] = {
//NULL
};

static struct regval_list sensor_saturation_pos3_regs[] = {
//NULL
};

static struct regval_list sensor_saturation_pos4_regs[] = {
//NULL
};

static struct cfg_array sensor_saturation[] = {
  {
  	.regs = sensor_saturation_neg4_regs,
  	.size = ARRAY_SIZE(sensor_saturation_neg4_regs),
  },
  {
  	.regs = sensor_saturation_neg3_regs,
  	.size = ARRAY_SIZE(sensor_saturation_neg3_regs),
  },
  {
  	.regs = sensor_saturation_neg2_regs,
  	.size = ARRAY_SIZE(sensor_saturation_neg2_regs),
  },
  {
  	.regs = sensor_saturation_neg1_regs,
  	.size = ARRAY_SIZE(sensor_saturation_neg1_regs),
  },
  {
  	.regs = sensor_saturation_zero_regs,
  	.size = ARRAY_SIZE(sensor_saturation_zero_regs),
  },
  {
  	.regs = sensor_saturation_pos1_regs,
  	.size = ARRAY_SIZE(sensor_saturation_pos1_regs),
  },
  {
  	.regs = sensor_saturation_pos2_regs,
  	.size = ARRAY_SIZE(sensor_saturation_pos2_regs),
  },
  {
  	.regs = sensor_saturation_pos3_regs,
  	.size = ARRAY_SIZE(sensor_saturation_pos3_regs),
  },
  {
  	.regs = sensor_saturation_pos4_regs,
  	.size = ARRAY_SIZE(sensor_saturation_pos4_regs),
  },
};

/*
 * The exposure target setttings
 */
static struct regval_list sensor_ev_neg4_regs[] = {
	{0xfe,0x00},
	{0xd5,0xb8},
	{0xfe,0x01},
	{0x13,0x70},
	{0xfe,0x00}
};

static struct regval_list sensor_ev_neg3_regs[] = {
	{0xfe,0x00},
	{0xd5,0xd0},
	{0xfe,0x01},
	{0x13,0x78},
	{0xfe,0x00}
};

static struct regval_list sensor_ev_neg2_regs[] = {
	{0xfe,0x00},
	{0xd5,0xe0},
	{0xfe,0x01},
	{0x13,0x80},
	{0xfe,0x00}
};

static struct regval_list sensor_ev_neg1_regs[] = {
	{0xfe,0x00},
	{0xd5,0xf0},
	{0xfe,0x01},
	{0x13,0x88},
	{0xfe,0x00}
};

static struct regval_list sensor_ev_zero_regs[] = {
	{0xfe,0x00},
	{0xd5,0x00},
	{0xfe,0x01},
	{0x13,0x98},
	{0xfe,0x00}
};

static struct regval_list sensor_ev_pos1_regs[] = {
	{0xfe,0x00},
	{0xd5,0x10},
	{0xfe,0x01},
	{0x13,0xa0},
	{0xfe,0x00}
};

static struct regval_list sensor_ev_pos2_regs[] = {
	{0xfe,0x00},
	{0xd5,0x20},
	{0xfe,0x01},
	{0x13,0xa8},
	{0xfe,0x00}
};

static struct regval_list sensor_ev_pos3_regs[] = {
	{0xfe,0x00},
	{0xd5,0x30},
	{0xfe,0x01},
	{0x13,0xb0},
	{0xfe,0x00}
};

static struct regval_list sensor_ev_pos4_regs[] = {
	{0xfe,0x00},
	{0xd5,0x48},
	{0xfe,0x01},
	{0x13,0xb8},
	{0xfe,0x00}
};

static struct cfg_array sensor_ev[] = {
  {
  	.regs = sensor_ev_neg4_regs,
  	.size = ARRAY_SIZE(sensor_ev_neg4_regs),
  },
  {
  	.regs = sensor_ev_neg3_regs,
  	.size = ARRAY_SIZE(sensor_ev_neg3_regs),
  },
  {
  	.regs = sensor_ev_neg2_regs,
  	.size = ARRAY_SIZE(sensor_ev_neg2_regs),
  },
  {
  	.regs = sensor_ev_neg1_regs,
  	.size = ARRAY_SIZE(sensor_ev_neg1_regs),
  },
  {
  	.regs = sensor_ev_zero_regs,
  	.size = ARRAY_SIZE(sensor_ev_zero_regs),
  },
  {
  	.regs = sensor_ev_pos1_regs,
  	.size = ARRAY_SIZE(sensor_ev_pos1_regs),
  },
  {
  	.regs = sensor_ev_pos2_regs,
  	.size = ARRAY_SIZE(sensor_ev_pos2_regs),
  },
  {
  	.regs = sensor_ev_pos3_regs,
  	.size = ARRAY_SIZE(sensor_ev_pos3_regs),
  },
  {
  	.regs = sensor_ev_pos4_regs,
  	.size = ARRAY_SIZE(sensor_ev_pos4_regs),
  },
};

/*
 * Here we'll try to encapsulate the changes for just the output
 * video format.
 * 
 */


static struct regval_list sensor_fmt_yuv422_yuyv[] = {
	{0x44,0x02}	//YCbYCr
};

static struct regval_list sensor_fmt_yuv422_yvyu[] = {
	{0x44,0x03}	//YCrYCb
};

static struct regval_list sensor_fmt_yuv422_vyuy[] = {
	{0x44,0x01}	//CrYCbY
};

static struct regval_list sensor_fmt_yuv422_uyvy[] = {
	{0x44,0x00}	//CbYCrY
};

static struct regval_list sensor_fmt_raw[] = {
	{0x44,0x17}//raw
};



/*
 * Low-level register I/O.
 *
 */


/*
 * On most platforms, we'd rather do straight i2c I/O.
 */
static int sensor_read(struct v4l2_subdev *sd, unsigned char reg,
    unsigned char *value) //!!!!be careful of the para type!!!
{
	int ret=0;
	int cnt=0;
	
  ret = cci_read_a8_d8(sd,reg,value);
  while(ret!=0&&cnt<2)
  {
  	ret = cci_read_a8_d8(sd,reg,value);
  	cnt++;
  }
  if(cnt>0)
  	vfe_dev_dbg("sensor read retry=%d\n",cnt);
  
  return ret;
}

static int sensor_write(struct v4l2_subdev *sd, unsigned char reg,
    unsigned char value)
{
	int ret=0;
	int cnt=0;
	
  ret = cci_write_a8_d8(sd,reg,value);
  while(ret!=0&&cnt<2)
  {
  	ret = cci_write_a8_d8(sd,reg,value);
  	cnt++;
  }
  if(cnt>0)
  	vfe_dev_dbg("sensor write retry=%d\n",cnt);
  
  return ret;
}

/*
 * Write a list of register settings;
 */
static int sensor_write_array(struct v4l2_subdev *sd, struct regval_list *regs, int array_size)
{
	int i=0;
	
  if(!regs)
  	return 0;
  	//return -EINVAL;
  
  while(i<array_size)
  {
    if(regs->addr == REG_DLY) {
      msleep(regs->data);
    } 
    else {
    	//printk("write 0x%x=0x%x\n", regs->addr, regs->data);
      LOG_ERR_RET(sensor_write(sd, regs->addr, regs->data))
    }
    i++;
    regs++;
  }
  return 0;
}

static int sensor_g_hflip(struct v4l2_subdev *sd, __s32 *value)
{
	int ret;
	struct sensor_info *info = to_state(sd);
	unsigned char val;
	
	ret = sensor_write(sd, 0xfe, 0x00);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_g_hflip!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x17, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_g_hflip!\n");
		return ret;
	}
	
	val &= (1<<0);
	val = val>>0;		//0x17 bit0 is mirror
		
	*value = val;

	info->hflip = *value;
	return 0;
}

static int sensor_s_hflip(struct v4l2_subdev *sd, int value)
{
	int ret;
	struct sensor_info *info = to_state(sd);
	unsigned char val;
	
	ret = sensor_write(sd, 0xfe, 0x00);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_hflip!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x17, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_s_hflip!\n");
		return ret;
	}
	
	switch (value) {
		case 0:
		  val &= 0xfc;
			break;
		case 1:
			val |= (0x01|(info->vflip<<1));
			break;
		default:
			return -EINVAL;
	}
	ret = sensor_write(sd, 0x17, val);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_hflip!\n");
		return ret;
	}
	
	usleep_range(10000,12000);
	
	info->hflip = value;
	return 0;
}

static int sensor_g_vflip(struct v4l2_subdev *sd, __s32 *value)
{
	int ret;
	struct sensor_info *info = to_state(sd);
	unsigned char val;
	
	ret = sensor_write(sd, 0xfe, 0x00);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_g_vflip!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x17, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_g_vflip!\n");
		return ret;
	}
	
	val &= (1<<1);
	val = val>>1;		//0x14 bit1 is upsidedown
		
	*value = val;

	info->vflip = *value;
	return 0;
}

static int sensor_s_vflip(struct v4l2_subdev *sd, int value)
{
	int ret;
	struct sensor_info *info = to_state(sd);
	unsigned char val;
	
	ret = sensor_write(sd, 0xfe, 0x00);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_vflip!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x17, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_s_vflip!\n");
		return ret;
	}
	
	switch (value) {
		case 0:
		  val &= 0xfc;
			break;
		case 1:
			val |= (0x02|info->hflip);
			break;
		default:
			return -EINVAL;
	}
	ret = sensor_write(sd, 0x17, val);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_vflip!\n");
		return ret;
	}
	
	usleep_range(10000,12000);
	
	info->vflip = value;
	return 0;
}

static int sensor_g_autogain(struct v4l2_subdev *sd, __s32 *value)
{
	return -EINVAL;
}

static int sensor_s_autogain(struct v4l2_subdev *sd, int value)
{
	return -EINVAL;
}

static int sensor_g_autoexp(struct v4l2_subdev *sd, __s32 *value)
{
	int ret;
	struct sensor_info *info = to_state(sd);
	unsigned char val;
	
	ret = sensor_write(sd, 0xfe, 0x00);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_g_autoexp!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x4f, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_g_autoexp!\n");
		return ret;
	}

	val &= 0x01;
	if (val == 0x01) {
		*value = V4L2_EXPOSURE_AUTO;
	}
	else
	{
		*value = V4L2_EXPOSURE_MANUAL;
	}
	
	info->autoexp = *value;
	return 0;
}

static int sensor_s_autoexp(struct v4l2_subdev *sd,
		enum v4l2_exposure_auto_type value)
{
	int ret;
	struct sensor_info *info = to_state(sd);
	unsigned char val;
	
	ret = sensor_write(sd, 0xfe, 0x00);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_autoexp!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x4f, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_s_autoexp!\n");
		return ret;
	}

	switch (value) {
		case V4L2_EXPOSURE_AUTO:
		  val |= 0x01;
			break;
		case V4L2_EXPOSURE_MANUAL:
			val &= 0xfe;
			break;
		case V4L2_EXPOSURE_SHUTTER_PRIORITY:
			return -EINVAL;    
		case V4L2_EXPOSURE_APERTURE_PRIORITY:
			return -EINVAL;
		default:
			return -EINVAL;
	}
		
	ret = sensor_write(sd, 0x4f, val);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_autoexp!\n");
		return ret;
	}
	
	usleep_range(10000,12000);
	
	info->autoexp = value;
	return 0;
}

static int sensor_g_autowb(struct v4l2_subdev *sd, int *value)
{
	int ret;
	struct sensor_info *info = to_state(sd);
	unsigned char val;
	
	ret = sensor_write(sd, 0xfe, 0x00);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_g_autowb!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x4f, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_g_autowb!\n");
		return ret;
	}

	val &= (1<<1);
	val = val>>1;		//0x22 bit1 is awb enable
		
	*value = val;
	info->autowb = *value;
	
	return 0;
}

static int sensor_s_autowb(struct v4l2_subdev *sd, int value)
{
	int ret;
	struct sensor_info *info = to_state(sd);
	unsigned char val;
	
	ret = sensor_write_array(sd, sensor_wb_auto_regs, ARRAY_SIZE(sensor_wb_auto_regs));
	if (ret < 0) {
		vfe_dev_err("sensor_write_array err at sensor_s_autowb!\n");
		return ret;
	}
	
	ret = sensor_write(sd, 0xfe, 0x00);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_autowb!\n");
		return ret;
	}
	ret = sensor_read(sd, 0x42, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_s_autowb!\n");
		return ret;
	}

	switch(value) {
	case 0:
		val &= 0xfd;
		break;
	case 1:
		val |= 0x02;
		break;
	default:
		break;
	}	
	ret = sensor_write(sd, 0x42, val);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_autowb!\n");
		return ret;
	}
	
	usleep_range(10000,12000);
	
	info->autowb = value;
	return 0;
}

static int sensor_g_hue(struct v4l2_subdev *sd, __s32 *value)
{
	return -EINVAL;
}

static int sensor_s_hue(struct v4l2_subdev *sd, int value)
{
	return -EINVAL;
}

static int sensor_g_gain(struct v4l2_subdev *sd, __s32 *value)
{
	return -EINVAL;
}

static int sensor_s_gain(struct v4l2_subdev *sd, int value)
{
	return -EINVAL;
}
/* *********************************************end of ******************************************** */

static int sensor_g_brightness(struct v4l2_subdev *sd, __s32 *value)
{
  struct sensor_info *info = to_state(sd);
  
  *value = info->brightness;
  return 0;
}

static int sensor_s_brightness(struct v4l2_subdev *sd, int value)
{
  struct sensor_info *info = to_state(sd);
  
  if(info->brightness == value)
    return 0;
  
  if(value < -4 || value > 4)
    return -ERANGE;
  
  LOG_ERR_RET(sensor_write_array(sd, sensor_brightness[value+4].regs, sensor_brightness[value+4].size))

  info->brightness = value;
  return 0;
}

static int sensor_g_contrast(struct v4l2_subdev *sd, __s32 *value)
{
  struct sensor_info *info = to_state(sd);
  
  *value = info->contrast;
  return 0;
}

static int sensor_s_contrast(struct v4l2_subdev *sd, int value)
{
  struct sensor_info *info = to_state(sd);
  
  if(info->contrast == value)
    return 0;
  
  if(value < -4 || value > 4)
    return -ERANGE;
    
  LOG_ERR_RET(sensor_write_array(sd, sensor_contrast[value+4].regs, sensor_contrast[value+4].size))
  
  info->contrast = value;
  return 0;
}

static int sensor_g_saturation(struct v4l2_subdev *sd, __s32 *value)
{
  struct sensor_info *info = to_state(sd);
  
  *value = info->saturation;
  return 0;
}

static int sensor_s_saturation(struct v4l2_subdev *sd, int value)
{
  struct sensor_info *info = to_state(sd);
  
  if(info->saturation == value)
    return 0;

  if(value < -4 || value > 4)
    return -ERANGE;
      
  LOG_ERR_RET(sensor_write_array(sd, sensor_saturation[value+4].regs, sensor_saturation[value+4].size))

  info->saturation = value;
  return 0;
}

static int sensor_g_exp_bias(struct v4l2_subdev *sd, __s32 *value)
{
  struct sensor_info *info = to_state(sd);
  
  *value = info->exp_bias;
  return 0;
}

static int sensor_s_exp_bias(struct v4l2_subdev *sd, int value)
{
  struct sensor_info *info = to_state(sd);

  if(info->exp_bias == value)
    return 0;

  if(value < -4 || value > 4)
    return -ERANGE;
      
  LOG_ERR_RET(sensor_write_array(sd, sensor_ev[value+4].regs, sensor_ev[value+4].size))

  info->exp_bias = value;
  return 0;
}

static int sensor_g_wb(struct v4l2_subdev *sd, int *value)
{
  struct sensor_info *info = to_state(sd);
  enum v4l2_auto_n_preset_white_balance *wb_type = (enum v4l2_auto_n_preset_white_balance*)value;
  
  *wb_type = info->wb;
  
  return 0;
}

static int sensor_s_wb(struct v4l2_subdev *sd,
    enum v4l2_auto_n_preset_white_balance value)
{
  struct sensor_info *info = to_state(sd);
  
  if(info->capture_mode == V4L2_MODE_IMAGE)
    return 0;
  
  //if(info->wb == value)
    //return 0;
  
  LOG_ERR_RET(sensor_write_array(sd, sensor_wb[value].regs ,sensor_wb[value].size) )
  
  if (value == V4L2_WHITE_BALANCE_AUTO) 
    info->autowb = 1;
  else
    info->autowb = 0;
	
	info->wb = value;
	return 0;
}

static int sensor_g_colorfx(struct v4l2_subdev *sd,
		__s32 *value)
{
	struct sensor_info *info = to_state(sd);
	enum v4l2_colorfx *clrfx_type = (enum v4l2_colorfx*)value;
	
	*clrfx_type = info->clrfx;
	return 0;
}

static int sensor_s_colorfx(struct v4l2_subdev *sd,
    enum v4l2_colorfx value)
{
  struct sensor_info *info = to_state(sd);

  if(info->clrfx == value)
    return 0;
  
  LOG_ERR_RET(sensor_write_array(sd, sensor_colorfx[value].regs, sensor_colorfx[value].size))

  info->clrfx = value;
  return 0;
}

static int sensor_g_flash_mode(struct v4l2_subdev *sd,
    __s32 *value)
{
  struct sensor_info *info = to_state(sd);
  enum v4l2_flash_led_mode *flash_mode = (enum v4l2_flash_led_mode*)value;
  
  *flash_mode = info->flash_mode;
  return 0;
}

static int sensor_s_flash_mode(struct v4l2_subdev *sd,
    enum v4l2_flash_led_mode value)
{
  struct sensor_info *info = to_state(sd);
//  struct vfe_dev *dev=(struct vfe_dev *)dev_get_drvdata(sd->v4l2_dev->dev);
//  int flash_on,flash_off;
//  
//  flash_on = (dev->flash_pol!=0)?1:0;
//  flash_off = (flash_on==1)?0:1;
//  
//  switch (value) {
//  case V4L2_FLASH_MODE_OFF:
//    os_gpio_write(&dev->flash_io,flash_off);
//    break;
//  case V4L2_FLASH_MODE_AUTO:
//    return -EINVAL;
//    break;  
//  case V4L2_FLASH_MODE_ON:
//    os_gpio_write(&dev->flash_io,flash_on);
//    break;   
//  case V4L2_FLASH_MODE_TORCH:
//    return -EINVAL;
//    break;
//  case V4L2_FLASH_MODE_RED_EYE:   
//    return -EINVAL;
//    break;
//  default:
//    return -EINVAL;
//  }
  
  info->flash_mode = value;
  return 0;
}

//static int sensor_s_sw_stby(struct v4l2_subdev *sd, int on_off)
//{
//	int ret=0;
////	unsigned char rdval;
////	
////	ret=sensor_read(sd, 0x00, &rdval);
////	if(ret!=0)
////		return ret;
////	
////	if(on_off==CSI_STBY_ON)//sw stby on
////	{
////		ret=sensor_write(sd, 0x00, rdval&0x7f);
////	}
////	else//sw stby off
////	{
////		ret=sensor_write(sd, 0x00, rdval|0x80);
////	}
//	return ret;
//}

/*
 * Stuff that knows about the sensor.
 */
 
static int sensor_power(struct v4l2_subdev *sd, int on)
{
	cci_lock(sd);
	switch(on)
	{
		case CSI_SUBDEV_STBY_ON:
			vfe_dev_dbg("CSI_SUBDEV_STBY_ON\n");
			vfe_gpio_write(sd,PWDN,CSI_GPIO_HIGH);
			usleep_range(5000,12000);
			vfe_set_mclk(sd,OFF);
			break;
		case CSI_SUBDEV_STBY_OFF:
			vfe_dev_dbg("CSI_SUBDEV_STBY_OFF\n");
			vfe_set_mclk_freq(sd,MCLK);
			vfe_set_mclk(sd,ON);
			usleep_range(5000,12000);
			vfe_gpio_write(sd,PWDN,CSI_GPIO_LOW);
			usleep_range(5000,12000);
			break;
		case CSI_SUBDEV_PWR_ON:
			vfe_dev_dbg("CSI_SUBDEV_PWR_ON\n");
			vfe_gpio_set_status(sd,PWDN,1);//set the gpio to output
			vfe_gpio_set_status(sd,RESET,1);//set the gpio to output
			vfe_gpio_write(sd,PWDN,CSI_GPIO_HIGH);
			vfe_gpio_write(sd,RESET,CSI_GPIO_LOW);
			vfe_gpio_write(sd,POWER_EN,CSI_PWR_ON);
			usleep_range(10000,12000);
			vfe_set_mclk_freq(sd,MCLK);
			vfe_set_mclk(sd,ON);
			usleep_range(10000,12000);

			vfe_set_pmu_channel(sd,IOVDD,ON);
			vfe_set_pmu_channel(sd,AVDD,ON);
			vfe_set_pmu_channel(sd,DVDD,ON);
			vfe_set_pmu_channel(sd,AFVDD,ON);
			usleep_range(10000,12000);
			vfe_gpio_write(sd,PWDN,CSI_GPIO_LOW);
			usleep_range(10000,12000);
			vfe_gpio_write(sd,RESET,CSI_GPIO_HIGH);
			usleep_range(10000,12000);
			break;
		case CSI_SUBDEV_PWR_OFF:
			vfe_dev_dbg("CSI_SUBDEV_PWR_OFF\n");
			vfe_gpio_write(sd,PWDN,CSI_GPIO_HIGH);
			usleep_range(10000,12000);
			vfe_gpio_write(sd,RESET,CSI_GPIO_LOW);
			usleep_range(10000,12000);

			vfe_gpio_write(sd,POWER_EN,CSI_PWR_OFF);
			vfe_set_pmu_channel(sd,AFVDD,OFF);
			vfe_set_pmu_channel(sd,DVDD,OFF);
			vfe_set_pmu_channel(sd,AVDD,OFF);
			vfe_set_pmu_channel(sd,IOVDD,OFF);
			usleep_range(5000,12000);
			vfe_set_mclk(sd,OFF);
			usleep_range(5000,12000);
			vfe_gpio_set_status(sd,RESET,0);//set the gpio to input
			vfe_gpio_set_status(sd,PWDN,0);//set the gpio to input
			break;
		default:
			return -EINVAL;
	}		
	cci_unlock(sd);	
	return 0;
}
 
static int sensor_reset(struct v4l2_subdev *sd, u32 val)
{
  switch(val)
  {
    case 0:
      vfe_gpio_write(sd,RESET,CSI_RST_OFF);
      usleep_range(10000,12000);
      break;
    case 1:
      vfe_gpio_write(sd,RESET,CSI_RST_ON);
      usleep_range(10000,12000);
      break;
    default:
      return -EINVAL;
  }
    
  return 0;
}

static int sensor_detect(struct v4l2_subdev *sd)
{
	int ret;
	unsigned char val;
	
	ret = sensor_write(sd, 0xfe, 0x00);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_detect!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0xf0, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_detect!\n");
		return ret;
	}

	if(val != 0x9D)
		return -ENODEV;
	
	return 0;
}

static int sensor_init(struct v4l2_subdev *sd, u32 val)
{
	int ret;
	vfe_dev_dbg("sensor_init\n");
	
	/*Make sure it is a target sensor*/
	printk("gc0328------------------------------------------james--16-47");
	ret = sensor_detect(sd);
	if (ret) {
		vfe_dev_err("chip found is not an target chip.\n");
		return ret;
	}
	/*return*/ sensor_write_array(sd, sensor_default_regs , ARRAY_SIZE(sensor_default_regs));
	msleep(500);
	return 0;
}

static int sensor_g_exif(struct v4l2_subdev *sd, struct sensor_exif_attribute *exif)
{
	int ret = 0;//, gain_val, exp_val;
	unsigned  int  temp=0,shutter=0, gain = 0;
	unsigned char val;

	sensor_write(sd, 0xfe, 0x00);
	//sensor_write(sd, 0xb6, 0x02);

	/*read shutter */
	sensor_read(sd, 0x03, &val);
	temp |= (val<< 8);
	sensor_read(sd, 0x04, &val);
	temp |= (val & 0xff);
	shutter=temp;

	sensor_read(sd, 0x71, &val);
	gain = val;
	exif->fnumber = 280;
	exif->focal_length = 425;
	exif->brightness = 125;
	exif->flash_fire = 0;
	exif->iso_speed = 50*((50 + CLIP(gain-0x40, 0, 0xff)*5)/50);
	exif->exposure_time_num = 1;
	exif->exposure_time_den = 8000/shutter;
	return ret;
}

static long sensor_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
	int ret=0;
	//struct sensor_info *info = to_state(sd);
	switch(cmd) {
		case GET_SENSOR_EXIF:
			sensor_g_exif(sd, (struct sensor_exif_attribute *)arg);
			break;
		default:
			return -EINVAL;
	}
	return ret;
}


/*
 * Store information about the video data format. 
 */
static struct sensor_format_struct {
	__u8 *desc;
	//__u32 pixelformat;
	enum v4l2_mbus_pixelcode mbus_code;//linux-3.0
	struct regval_list *regs;
	int	regs_size;
	int bpp;   /* Bytes per pixel */
} sensor_formats[] = {
	{
		.desc		= "YUYV 4:2:2",
		.mbus_code	= V4L2_MBUS_FMT_YUYV8_2X8,//linux-3.0
		.regs 		= sensor_fmt_yuv422_yuyv,
		.regs_size = ARRAY_SIZE(sensor_fmt_yuv422_yuyv),
		.bpp		= 2,
	},
	{
		.desc		= "YVYU 4:2:2",
		.mbus_code	= V4L2_MBUS_FMT_YVYU8_2X8,//linux-3.0
		.regs 		= sensor_fmt_yuv422_yvyu,
		.regs_size = ARRAY_SIZE(sensor_fmt_yuv422_yvyu),
		.bpp		= 2,
	},
	{
		.desc		= "UYVY 4:2:2",
		.mbus_code	= V4L2_MBUS_FMT_UYVY8_2X8,//linux-3.0
		.regs 		= sensor_fmt_yuv422_uyvy,
		.regs_size = ARRAY_SIZE(sensor_fmt_yuv422_uyvy),
		.bpp		= 2,
	},
	{
		.desc		= "VYUY 4:2:2",
		.mbus_code	= V4L2_MBUS_FMT_VYUY8_2X8,//linux-3.0
		.regs 		= sensor_fmt_yuv422_vyuy,
		.regs_size = ARRAY_SIZE(sensor_fmt_yuv422_vyuy),
		.bpp		= 2,
	},
	{
		.desc		= "Raw RGB Bayer",
		.mbus_code	= V4L2_MBUS_FMT_SBGGR8_1X8,//linux-3.0
		.regs 		= sensor_fmt_raw,
		.regs_size = ARRAY_SIZE(sensor_fmt_raw),
		.bpp		= 1
	},
};
#define N_FMTS ARRAY_SIZE(sensor_formats)


/*
 * Then there is the issue of window sizes.  Try to capture the info here.
 */


static struct sensor_win_size 
sensor_win_sizes[] = {
  /* VGA */
  {
    .width      = VGA_WIDTH,
    .height     = VGA_HEIGHT,
    .hoffset    = 0,
    .voffset    = 0,
    .regs       = NULL,
    .regs_size  = 0,
    .set_size   = NULL,
  },
};

#define N_WIN_SIZES (ARRAY_SIZE(sensor_win_sizes))

static int sensor_enum_fmt(struct v4l2_subdev *sd, unsigned index,
                 enum v4l2_mbus_pixelcode *code)
{
  if (index >= N_FMTS)
    return -EINVAL;

  *code = sensor_formats[index].mbus_code;
  return 0;
}


static int sensor_try_fmt_internal(struct v4l2_subdev *sd,
    struct v4l2_mbus_framefmt *fmt,
    struct sensor_format_struct **ret_fmt,
    struct sensor_win_size **ret_wsize)
{
  int index;
  struct sensor_win_size *wsize;

  for (index = 0; index < N_FMTS; index++)
    if (sensor_formats[index].mbus_code == fmt->code)
      break;

  if (index >= N_FMTS) 
    return -EINVAL;
  
  if (ret_fmt != NULL)
    *ret_fmt = sensor_formats + index;
    
  /*
   * Fields: the sensor devices claim to be progressive.
   */
  
  fmt->field = V4L2_FIELD_NONE;
  
  /*
   * Round requested image size down to the nearest
   * we support, but not below the smallest.
   */
  for (wsize = sensor_win_sizes; wsize < sensor_win_sizes + N_WIN_SIZES;
       wsize++)
    if (fmt->width >= wsize->width && fmt->height >= wsize->height)
      break;
    
  if (wsize >= sensor_win_sizes + N_WIN_SIZES)
    wsize--;   /* Take the smallest one */
  if (ret_wsize != NULL)
    *ret_wsize = wsize;
  /*
   * Note the size we'll actually handle.
   */
  fmt->width = wsize->width;
  fmt->height = wsize->height;
  //pix->bytesperline = pix->width*sensor_formats[index].bpp;
  //pix->sizeimage = pix->height*pix->bytesperline;

  return 0;
}

static int sensor_try_fmt(struct v4l2_subdev *sd, 
             struct v4l2_mbus_framefmt *fmt)//linux-3.0
{
	return sensor_try_fmt_internal(sd, fmt, NULL, NULL);
}

static int sensor_g_mbus_config(struct v4l2_subdev *sd,
           struct v4l2_mbus_config *cfg)
{
  cfg->type = V4L2_MBUS_PARALLEL;
  cfg->flags = V4L2_MBUS_MASTER | VREF_POL | HREF_POL | CLK_POL ;
  
  return 0;
}

/*
 * Set a format.
 */
static int sensor_s_fmt(struct v4l2_subdev *sd, 
             struct v4l2_mbus_framefmt *fmt)//linux-3.0
{
	int ret;
	struct sensor_format_struct *sensor_fmt;
	struct sensor_win_size *wsize;
	struct sensor_info *info = to_state(sd);
	vfe_dev_dbg("sensor_s_fmt\n");
	ret = sensor_try_fmt_internal(sd, fmt, &sensor_fmt, &wsize);
	if (ret)
		return ret;
	
		
	sensor_write_array(sd, sensor_fmt->regs , sensor_fmt->regs_size);
	
	ret = 0;
	if (wsize->regs)
	{
		ret = sensor_write_array(sd, wsize->regs , wsize->regs_size);
		if (ret < 0)
			return ret;
	}
	
	if (wsize->set_size)
	{
		ret = wsize->set_size(sd);
		if (ret < 0)
			return ret;
	}
	
	info->fmt = sensor_fmt;
	info->width = wsize->width;
	info->height = wsize->height;
	
	return 0;
}

/*
 * Implement G/S_PARM.  There is a "high quality" mode we could try
 * to do someday; for now, we just do the frame rate tweak.
 */
static int sensor_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
	struct v4l2_captureparm *cp = &parms->parm.capture;
	//struct sensor_info *info = to_state(sd);

	if (parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	memset(cp, 0, sizeof(struct v4l2_captureparm));
	cp->capability = V4L2_CAP_TIMEPERFRAME;
	cp->timeperframe.numerator = 1;
	cp->timeperframe.denominator = SENSOR_FRAME_RATE;
	
	return 0;
}

static int sensor_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
//	struct v4l2_captureparm *cp = &parms->parm.capture;
	//struct v4l2_fract *tpf = &cp->timeperframe;
	//struct sensor_info *info = to_state(sd);
	//int div;

//	if (parms->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
//		return -EINVAL;
//	if (cp->extendedmode != 0)
//		return -EINVAL;

//	if (tpf->numerator == 0 || tpf->denominator == 0)
//		div = 1;  /* Reset to full rate */
//	else
//		div = (tpf->numerator*SENSOR_FRAME_RATE)/tpf->denominator;
//		
//	if (div == 0)
//		div = 1;
//	else if (div > CLK_SCALE)
//		div = CLK_SCALE;
//	info->clkrc = (info->clkrc & 0x80) | div;
//	tpf->numerator = 1;
//	tpf->denominator = sensor_FRAME_RATE/div;
//sensor_write(sd, REG_CLKRC, info->clkrc);
	return 0;
}


/* 
 * Code for dealing with controls.
 * fill with different sensor module
 * different sensor module has different settings here
 * if not support the follow function ,retrun -EINVAL
 */

/* *********************************************begin of ******************************************** */
static int sensor_queryctrl(struct v4l2_subdev *sd,
		struct v4l2_queryctrl *qc)
{
	/* Fill in min, max, step and default value for these controls. */
	/* see include/linux/videodev2.h for details */
	/* see sensor_s_parm and sensor_g_parm for the meaning of value */
	
	switch (qc->id) {
//	case V4L2_CID_BRIGHTNESS:
//		return v4l2_ctrl_query_fill(qc, -4, 4, 1, 1);
//	case V4L2_CID_CONTRAST:
//		return v4l2_ctrl_query_fill(qc, -4, 4, 1, 1);
//	case V4L2_CID_SATURATION:
//		return v4l2_ctrl_query_fill(qc, -4, 4, 1, 1);
//	case V4L2_CID_HUE:
//		return v4l2_ctrl_query_fill(qc, -180, 180, 5, 0);
	case V4L2_CID_VFLIP:
	case V4L2_CID_HFLIP:
		return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
//	case V4L2_CID_GAIN:
//		return v4l2_ctrl_query_fill(qc, 0, 255, 1, 128);
//	case V4L2_CID_AUTOGAIN:
//		return v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);
	case V4L2_CID_EXPOSURE:
  case V4L2_CID_AUTO_EXPOSURE_BIAS:
		return v4l2_ctrl_query_fill(qc, -4, 4, 1, 0);
	case V4L2_CID_EXPOSURE_AUTO:
		return v4l2_ctrl_query_fill(qc, 0, 1, 1, 0);
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
    return v4l2_ctrl_query_fill(qc, 0, 9, 1, 1);
	case V4L2_CID_AUTO_WHITE_BALANCE:
		return v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);
	case V4L2_CID_COLORFX:
    return v4l2_ctrl_query_fill(qc, 0, 15, 1, 0);
  case V4L2_CID_FLASH_LED_MODE:
	  return v4l2_ctrl_query_fill(qc, 0, 4, 1, 0);	
	}
	return -EINVAL;
}


static int sensor_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		return sensor_g_brightness(sd, &ctrl->value);
	case V4L2_CID_CONTRAST:
		return sensor_g_contrast(sd, &ctrl->value);
	case V4L2_CID_SATURATION:
		return sensor_g_saturation(sd, &ctrl->value);
	case V4L2_CID_HUE:
		return sensor_g_hue(sd, &ctrl->value);	
	case V4L2_CID_VFLIP:
		return sensor_g_vflip(sd, &ctrl->value);
	case V4L2_CID_HFLIP:
		return sensor_g_hflip(sd, &ctrl->value);
	case V4L2_CID_GAIN:
		return sensor_g_gain(sd, &ctrl->value);
	case V4L2_CID_AUTOGAIN:
		return sensor_g_autogain(sd, &ctrl->value);
	case V4L2_CID_EXPOSURE:
  case V4L2_CID_AUTO_EXPOSURE_BIAS:
    return sensor_g_exp_bias(sd, &ctrl->value);
  case V4L2_CID_EXPOSURE_AUTO:
    return sensor_g_autoexp(sd, &ctrl->value);
  case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
    return sensor_g_wb(sd, &ctrl->value);
  case V4L2_CID_AUTO_WHITE_BALANCE:
    return sensor_g_autowb(sd, &ctrl->value);
  case V4L2_CID_COLORFX:
    return sensor_g_colorfx(sd, &ctrl->value);
  case V4L2_CID_FLASH_LED_MODE:
    return sensor_g_flash_mode(sd, &ctrl->value);
	}
	return -EINVAL;
}

static int sensor_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
  struct v4l2_queryctrl qc;
  int ret;
  
//  vfe_dev_dbg("sensor_s_ctrl ctrl->id=0x%8x\n", ctrl->id);
  qc.id = ctrl->id;
  ret = sensor_queryctrl(sd, &qc);
  if (ret < 0) {
    return ret;
  }

	if (qc.type == V4L2_CTRL_TYPE_MENU ||
		qc.type == V4L2_CTRL_TYPE_INTEGER ||
		qc.type == V4L2_CTRL_TYPE_BOOLEAN)
	{
	  if (ctrl->value < qc.minimum || ctrl->value > qc.maximum) {
	    return -ERANGE;
	  }
	}
	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		return sensor_s_brightness(sd, ctrl->value);
	case V4L2_CID_CONTRAST:
		return sensor_s_contrast(sd, ctrl->value);
	case V4L2_CID_SATURATION:
		return sensor_s_saturation(sd, ctrl->value);
	case V4L2_CID_HUE:
		return sensor_s_hue(sd, ctrl->value);		
	case V4L2_CID_VFLIP:
		return sensor_s_vflip(sd, ctrl->value);
	case V4L2_CID_HFLIP:
		return sensor_s_hflip(sd, ctrl->value);
	case V4L2_CID_GAIN:
		return sensor_s_gain(sd, ctrl->value);
	case V4L2_CID_AUTOGAIN:
		return sensor_s_autogain(sd, ctrl->value);
	case V4L2_CID_EXPOSURE:
    case V4L2_CID_AUTO_EXPOSURE_BIAS:
      return sensor_s_exp_bias(sd, ctrl->value);
    case V4L2_CID_EXPOSURE_AUTO:
      return sensor_s_autoexp(sd,
          (enum v4l2_exposure_auto_type) ctrl->value);
    case V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE:
  		return sensor_s_wb(sd,
          (enum v4l2_auto_n_preset_white_balance) ctrl->value); 
    case V4L2_CID_AUTO_WHITE_BALANCE:
      return sensor_s_autowb(sd, ctrl->value);
    case V4L2_CID_COLORFX:
      return sensor_s_colorfx(sd,
          (enum v4l2_colorfx) ctrl->value);
    case V4L2_CID_FLASH_LED_MODE:
      return sensor_s_flash_mode(sd,
          (enum v4l2_flash_led_mode) ctrl->value);
	}
	return -EINVAL;
}


static int sensor_g_chip_ident(struct v4l2_subdev *sd,
		struct v4l2_dbg_chip_ident *chip)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	return v4l2_chip_ident_i2c_client(client, chip, V4L2_IDENT_SENSOR, 0);
}


/* ----------------------------------------------------------------------- */

static const struct v4l2_subdev_core_ops sensor_core_ops = {
	.g_chip_ident = sensor_g_chip_ident,
	.g_ctrl = sensor_g_ctrl,
	.s_ctrl = sensor_s_ctrl,
	.queryctrl = sensor_queryctrl,
	.reset = sensor_reset,
	.init = sensor_init,
	.s_power = sensor_power,
	.ioctl = sensor_ioctl,
};

static const struct v4l2_subdev_video_ops sensor_video_ops = {
  .enum_mbus_fmt = sensor_enum_fmt,
  .try_mbus_fmt = sensor_try_fmt,
  .s_mbus_fmt = sensor_s_fmt,
  .s_parm = sensor_s_parm,
  .g_parm = sensor_g_parm,
  .g_mbus_config = sensor_g_mbus_config,
};

static const struct v4l2_subdev_ops sensor_ops = {
	.core = &sensor_core_ops,
	.video = &sensor_video_ops,
};

/* ----------------------------------------------------------------------- */
static struct cci_driver cci_drv = {
	.name = SENSOR_NAME,
	.addr_width = CCI_BITS_8,
	.data_width = CCI_BITS_8,
};

static int sensor_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct v4l2_subdev *sd;
	struct sensor_info *info;
//	int ret;

	info = kzalloc(sizeof(struct sensor_info), GFP_KERNEL);
	if (info == NULL)
		return -ENOMEM;
	sd = &info->sd;
	cci_dev_probe_helper(sd, client, &sensor_ops, &cci_drv);

	info->fmt = &sensor_formats[0];
	info->brightness = 0;
	info->contrast = 0;
	info->saturation = 0;
	info->hue = 0;
	info->hflip = 0;
	info->vflip = 0;
	info->gain = 0;
	info->autogain = 1;
	info->exp = 0;
	info->autoexp = 0;
	info->autowb = 1;
	info->wb = 0;
	info->clrfx = 0;
	
	return 0;
}


static int sensor_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd;

	sd = cci_dev_remove_helper(client, &cci_drv);
	kfree(to_state(sd));
	return 0;
}

static const struct i2c_device_id sensor_id[] = {
	{ SENSOR_NAME, 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, sensor_id);
//linux-3.0
static struct i2c_driver sensor_driver = {
	.driver = {
		.owner = THIS_MODULE,
	  .name = SENSOR_NAME,
	},
	.probe = sensor_probe,
	.remove = sensor_remove,
	.id_table = sensor_id,
};

static __init int init_sensor(void)
{
	return cci_dev_init_helper(&sensor_driver);
}

static __exit void exit_sensor(void)
{
	cci_dev_exit_helper(&sensor_driver);
}

module_init(init_sensor);
module_exit(exit_sensor);
