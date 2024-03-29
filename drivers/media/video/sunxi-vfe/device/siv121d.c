/*
 * A V4L2 driver for GalaxyCore SIV121D cameras.
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
MODULE_DESCRIPTION("A low-level driver for GalaxyCore SIV121D sensors");
MODULE_LICENSE("GPL");



//for internel driver debug
#define DEV_DBG_EN   		0
#if(DEV_DBG_EN == 1)		
#define vfe_dev_dbg(x,arg...) printk("[CSI_DEBUG][SIV121D]"x,##arg)
#else
#define vfe_dev_dbg(x,arg...) 
#endif
#define vfe_dev_err(x,arg...) printk("[CSI_ERR][SIV121D]"x,##arg)
#define vfe_dev_print(x,arg...) printk("[CSI][SIV121D]"x,##arg)

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
#define V4L2_IDENT_SENSOR 0x121d

//define the voltage level of control signal
#define CSI_STBY_ON			1
#define CSI_STBY_OFF 		0
#define CSI_RST_ON			1
#define CSI_RST_OFF			0
#define CSI_PWR_ON			1
#define CSI_PWR_OFF			0

#define regval_list reg_list_a8_d8
#define REG_TERM 0xff
#define VAL_TERM 0xff
#define REG_DLY  0xff


/*
 * Our nominal (default) frame rate.
 */
#define SENSOR_FRAME_RATE 8

/*
 * The siv121d sits on i2c with ID 0x66
 */
#define I2C_ADDR 0x66
#define SENSOR_NAME "siv121d"

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



/*
 * The default register settings
 *
 */
static struct regval_list siv121d_sensor_default_regs[] = {
	// IO & Clock & Analog Setup    
	//Sensor On                   																				 
	// PMU
{0x00,0x01},
{0x03,0x0a},

{0x00,0x01},
{0x03,0x0a},

{0x00,0x00},  
{0x03,0x04},                                           
{0x10,0x85}, //85  
{0x11,0x74},// f1}},	  //0x11  0x61 davis20120514

{0x00,0x01},                             
{0x04,0x00},  //00                                         
{0x06,0x04},                                           
{0x10,0x46},                                           
{0x11,0x23},                                           
{0x12,0x21},                                           
{0x17,0x86},                                           
{0x18,0x00},                                           
{0x20,0x00},                                           
{0x21,0x05},                                           
{0x22,0x01},
{0x23,0x69}, //add 
{0x40,0x0F},                                        
{0x41,0x90},                                        
{0x42,0xd2},                                           
{0x43,0x00},

// AE
{0x00,0x02},                                          
{0x10,0x80},                                           
{0x11,0x0c},//14},//10}},   //��?�̨�???��                                        
{0x12,0x64},//5a},  // D65 target    60                                      
{0x14,0x63},//59},  // A target    5d                                     
{0x34,0x96},                                           
{0x40,0x35},//40}}, // 40                                         
{0x5f,0x01},                                           
{0x41,0x28},//20}},                                          
{0x42,0x28},//20}},                                           
{0x44,0x08},//00}},                                           
{0x45,0x08},//00}},                                           
{0x46,0x15},//0a}},                                           
{0x47,0x1c},//10}},                                           
{0x48,0x20},//13}},                                           
{0x49,0x21},//15}},                                           
{0x4a,0x23},//18}},                                           
{0x4b,0x24},//                                       
{0x4c,0x25},//                                      
{0x4d,0x28},//20}},                                           
{0x4e,0x1c},//10}},                                           
{0x4f,0x15},//0a}},  
    
{0x50,0x12},                                           
{0x51,0x10},//06}},                                           
{0x52,0x0e},//05}},                                           
{0x53,0x0c},//04}},                                           
{0x54,0x0b},//02}},                                           
{0x55,0x0a},//01}},                                           
{0x90,0x80},                                           
{0x91,0x80}, 

// AWB 
{0x00,0x03},                                          
{0x10,0xd0}, //                                           
{0x11,0xc1},//c0}},//c0                                           
{0x13,0x80},//7d}, //Cr target 0x7e                                                                
{0x14,0x7f},//7d}, //Cb target  7f                                                                 
{0x15,0xd8},//c0}}, // R gain Top   e0                                                             
{0x16,0x7c},//8c}}, // R gain bottom  8c    //                                                      
{0x17,0xe0}, // B gain Top   e0                                                             
{0x18,0x88},//70}}, // B gain bottom 0x80     //                                                  
{0x19,0x8c}, // Cr top value 8c        //                                                
{0x1a,0x64},	// Cr bottom value 0x64   60        //YCbYCr                                 
{0x1b,0x98}, // Cb top value 0x98                                                        
{0x1c,0x6c}, // Cb bottom value 0x6c           // 0x01        
{0x1d,0x94}, // 0xa0                                 
{0x1e,0x6c}, // 0x60                                                
{0x20,0xe8}, // AWB luminous top value                                                   
{0x21,0x30}, // AWB luminous bottom value 0x20                                           
{0x22,0xb8},                                           
{0x23,0x10},   
{0x25,0x20},    
{0x26,0x0f},    
{0x27,0x08},    // BRTSRT                                            
{0x28,0x0f},    // BRTEND                                           
{0x29,0xa0},    // BRTRGNBOT                                        
{0x2a,0x90},    // BRTBGNTOP 
    
{0x40,0x01},                                           
{0x41,0x03},//04}},                                           
{0x42,0x08},                                           
{0x43,0x10},                                           
{0x44,0x13},//12}}, // 0x12  
{0x45,0x8f},//35}},  //35                                 
{0x46,0xfc},  //  fc  

{0x60,0xa0},
{0x61,0xa0},
	                                     
{0x62,0x80},//84                                  
{0x63,0x90},  // R D30 to D20                                           
{0x64,0xd0},  // B D30 to D20                                          
{0x65,0x98},//90}},  // R D20 to D30                                          
{0x66,0xd0},  // B D20 to D30  

// IDP 
{0x00,0x04},                                          
{0x10,0xff},                                           
//{{0x11},{0x1d}}, 
{0x11,0x19}, // changed
{0x12,0x1d},//++ 


// DPCBNR                                            
{0x18,0xbe},//fe}},  // DPCNRCTRL                                               
{0x19,0x00},//04}},  // DPCTHV                                                  
{0x1A,0x00},//01}},  // DPCTHVSLP                                               
{0x1B,0x00},//08}},  // DPCTHVDIFSRT                                                                                                                                                                                                                                                                                                              
{0x1C,0x0f},//08}},  // DPCTHVDIFSLP                                            
{0x1d,0xFF},  // DPCTHVMAX      
                     
{0x1E,0x04},  // BNRTHV  0c                                              
{0x1F,0x08},//04}},  // BNRTHVSLPN 10                                           
{0x20,0x10},  // BNRTHVSLPD                                              
{0x21,0x00}, /// BNRNEICNT      / 0x08                                   
{0x22,0x10},  // STRTNOR        // 0x03                                  
{0x23,0x40},//40}},  // STRTDRK        
{0x24,0x00},

// Gamma                                          
{0x31,0x03},//0x080x31, 0x04 },//0x08   0x03
{0x32,0x0b},//0x100x32, 0x0b },//0x10   0x08
{0x33,0x1e},//0x1B0x33, 0x24 },//0x1B   0x16
{0x34,0x46},//0x370x34, 0x49 },//0x37   0x36
{0x35,0x62},//0x4D0x35, 0x66 },//0x4D   0x56
{0x36,0x78},//0x600x36, 0x7C },//0x60   0x77
{0x37,0x8b},//0x720x37, 0x8D },//0x72   0x88
{0x38,0x9b},//0x820x38, 0x9B },//0x82   0x9a
{0x39,0xa8},//0x910x39, 0xAA },//0x91   0xA9
{0x3a,0xb6},//0xA00x3a, 0xb6 },//0xA0   0xb5
{0x3b,0xcc},//0xBA0x3b, 0xca },//0xBA   0xca
{0x3c,0xdf},//0xD30x3c, 0xdc },//0xD3   0xdd
{0x3d,0xf0},//0xEA0x3d, 0xef },//0xEA   0xee

// Shading Register Setting                                          
{0x40,0x06},//04}},  //06                                         
{0x41,0x44},                                           
{0x42,0x44},//43}},                                           
{0x43,0x20},                                   
{0x44,0x22},  // left R gain[7:4], right R gain[3:0] 	 22					                                          
{0x45,0x22},  // top R gain[7:4], bottom R gain[3:0] 		22				                                          
{0x46,0x00},  // left G gain[7:4], right G gain[3:0] 	                                                    
{0x47,0x11},  // top G gain[7:4], bottom G gain[3:0] 	   11                                                   
{0x48,0x00},  // left B gain[7:4], right B gain[3:0] 	                                                   
{0x49,0x00},  // top B gain[7:4], bottom B gain[3:0] 	           //color matrix default                                          
{0x4a,0x04},  // X-axis center high[3:2], Y-axis center high[1:0]                                         
{0x4b,0x48},  // X-axis center low[7:0]				48		                                                        
{0x4c,0xe8},  // Y-axis center low[7:0]		 e8			                                                          
{0x4d,0x84},  // Shading Center Gain     80                                                                  
{0x4e,0x00},  // Shading R Offset                                                                   
{0x4f,0x00},  // Shading Gr Offset                                                                        
{0x50,0x00},  // Shading B Offset                                


// Interpolation                                           
{0x60,0x7f},                                     
{0x61,0x08},  
#if 0
// Color matrix (D65) - Daylight                                         
{0x71,0x34},		 //0x34 	  0x39							  
{0x72,0xCe},		 //0xCe 	  0xCc							 
{0x73,0xFe},         //0xFe       0xFb                           
{0x74,0x13},         //0x13       0x10                           
{0x75,0x25},         //0x25       0x21                           
{0x76,0x08},         //0x08       0x0f                           
{0x77,0xec},		 //0xec 	  0xf8							 
{0x78,0xcd},		 //0xd1 	  0xc0							 
{0x79,0x47},		 //0x47 	  0x48

// Color matrix (D20) - A                                           
{0x83,0x37},     //0x3c 	                                       
{0x84,0xd1},     //0xc6 	                                      
{0x85,0xf6},     //0xff                                         
{0x86,0x12},     //0x12                                         
{0x87,0x24},     //0x24 	                                  
{0x88,0x0a},     //0x0a 	                                      
{0x89,0xed},     //0xed                                         
{0x8a,0xbb},     //0xc2 	                                      
{0x8b,0x58},     //0x51   
 #else
 // Color matrix (D65) - Daylight                                         
{0x71,0x39},		 //0x34 	  0x39							  
{0x72,0xC8},		 //0xCe 	  0xCc							 
{0x73,0xFf},         //0xFe       0xFb                           
{0x74,0x13},         //0x13       0x10                           
{0x75,0x25},         //0x25       0x21                           
{0x76,0x08},         //0x08       0x0f                           
{0x77,0xf8},		 //0xec 	  0xf8							 
{0x78,0xc0},		 //0xd1 	  0xc0							 
{0x79,0x48},		 //0x47 	  0x48

// Color matrix (D20) - A                                           
{0x83,0x34},     //0x3c 	                                       
{0x84,0xd4},     //0xc6 	                                      
{0x85,0xf8},     //0xff                                         
{0x86,0x12},     //0x12                                         
{0x87,0x25},     //0x24 	                                  
{0x88,0x09},     //0x0a 	                                      
{0x89,0xf0},     //0xed                                         
{0x8a,0xbe},     //0xc2 	                                      
{0x8b,0x54},     //0x51 
#endif
                     
{0x8c,0x10},     //CMA select

//G Edge                                        
{0x90,0x30},//28},    //Upper gain                                                                 
{0x91,0x48},    //down gain                                                                  
{0x92,0x33},//22}},    //[7:4] upper coring [3:0] down coring                                       
{0x9a,0x40},                                           
{0x9b,0x40},                                           
{0x9c,0x38},    //edge suppress start   30                                      
{0x9d,0x30},    //edge suppress slope   

{0xa8,0x14},//10}},                                     
{0xa9,0x13},//10}},                                           
{0xaa,0x12},  

{0xb9,0x28},//30}},     // nightmode 38 at gain 0x48 5fps                                        
{0xba,0x41},//44}},     // nightmode 80 at gain 0x48 5fps                                       
{0xde,0x80},  
    
{0xe5,0x15},                                           
{0xe6,0x02},                                           
{0xe7,0x04}, 
                                                         
//Sensor On //Sensor On                                                             
{0x00,0x01},                                           
{0x03,0x01},  

{0x00,0x00},    

};


/*
 * The 121du default register settings
 *
 */
static struct regval_list siv121du_sensor_default_regs[] = 
{
	// IO & Clock & Analog Setup    
	//Sensor On                   																				 
	// PMU

  {0x00,0x01},  
	{0x03,0x08}, 
	{0x00,0x01},  
	{0x03,0x08}, 
	
	{0x00,0x00},  
	{0x03,0x04},                                           
	{0x10,0x86},//8d}}, //85  
	{0x11,0x74},// f1}},	  //0x11  0x61 davis20120514
	
	{0x00,0x01},                             
	{0x04,0x00},  //00                                         
	{0x06,0x04},                                           
	{0x10,0x11},//46}},                                           
	{0x11,0x25},//23}},                                           
	{0x12,0x21},                                           
	{0x17,0x94},//86}},                                           
	{0x18,0x00},                                           
	{0x20,0x00},                                           
	{0x21,0x05},                                           
	{0x22,0x01},
	{0x23,0x69}, //add 
	{0x40,0x0F},                                        
	{0x41,0x90},                                        
	{0x42,0xd2},                                           
	{0x43,0x00}, 

	// AE
	{0x00,0x02},                                          
	{0x10,0x84},                                           
	{0x11,0x0a},//0c}},//10}},                                           
	{0x12,0x71},//70}},  // D65 target    60                                      
	{0x14,0x68},  // A target    5d                                     
	{0x34,0x96},                                           
	{0x40,0x38},//48}},//40}}, // 40  
	{0x44,0x08},//00}},   
	
	
	                                   
	                                    
	                                    
	                                    
	                                    
	                                    
	                                    
	                                    
	                                    
	                                    
	                                    
	                                    
	                                    
	
	                
                                      
 	                                         
	                                         
	                                         
	                                         
	                                         
	                                  
	
	
	
	// AWB 
	{0x00,0x03},                                          
	{0x10,0xD0},//c3}}, //                                           
	{0x11,0xc1},//c0}},//c0  
	{0x12,0x80},                                        
	{0x13,0x80}, //Cr target 0x7e                                                                
	{0x14,0x80}, //Cb target  7f                                                                 
	{0x15,0xe0},//c0}}, // R gain Top   e0                                                             
	{0x16,0x7C},//88}},//8c}}, // R gain bottom  8c    //                                                      
	{0x17,0xe0}, // B gain Top   e0                                                             
	{0x18,0x80},//70}}, // B gain bottom 0x80     //                                                  
	{0x19,0x8C},//90}}, // Cr top value 8c        //                                                
	{0x1a,0x64},	// Cr bottom value 0x64   60        //YCbYCr                                 
	{0x1b,0x94}, // Cb top value 0x98                                                        
	{0x1c,0x6c}, // Cb bottom value 0x6c           // 0x01        
	{0x1d,0x94}, // 0xa0                                 
	{0x1e,0x6c}, // 0x60                                                
	{0x20,0xe8}, // AWB luminous top value                                                   
	{0x21,0x30}, // AWB luminous bottom value 0x20                                           
	{0x22,0xb8},                                           
	{0x23,0x10},   
	{0x25,0x08},//20}},    
	{0x26,0x0f},//0f}},    
	{0x27,0x60},    // BRTSRT                                            
	{0x28,0x70},    // BRTEND                                           
	{0x29,0xb7},    // BRTRGNBOT                                        
	{0x2a,0xa3},    // BRTBGNTOP 
	                                         
	{0x40,0x01},                                           
	{0x41,0x03},//04}},                                           
	{0x42,0x08},                                           
	{0x43,0x10},                                           
	{0x44,0x13},//12}}, // 0x12  
	{0x45,0x6a},//35}},  //35                                 
	{0x46,0xca},  //  fc   
	
	{0x62,0x80},//84}},   //84   //   
	
	{0x63,0x90},//9c}},  // R D30 to D20                                           
	{0x64,0xD0},//d4}},  // B D30 to D20                                          
	{0x65,0x98},//a0}},//90}},  // R D20 to D30                                          
	{0x66,0xd0},  // B D20 to D30  

	// IDP 
	{0x00,0x04},                                          
	{0x10,0x7f},//ff}},                                           
	//{{0x11},{0x1d}}, 
	{0x11,0x19}, // changed
   {0x12, 0xfd},//dd}},//1d,
	{0x13,0xfe},//++ 
	{0x14,0x01},//++ 
	
	
		// DPCBNR                                            
	{0x18,0xbf},//fe}},  // DPCNRCTRL                                               
	{0x19,0x00},//04}},  // DPCTHV                                                  
	{0x1A,0x00},//01}},  // DPCTHVSLP                                               
	{0x1B,0x00},//08}},//08}},  // DPCTHVDIFSRT                                                                                                                                                                                                                                                                                                              
	//{{0x1C},{0x0f}},//08}},  // DPCTHVDIFSLP                                            
	//{{0x1d},{0xFF}},  // DPCTHVMAX      
	                                                             
	{0x1E,0x04},  // BNRTHV  0c                                              
	{0x1F,0x08},//04}},  // BNRTHVSLPN 10                                           
	{0x20,0x20},  // BNRTHVSLPD                                              
	{0x21,0x00}, /// BNRNEICNT      / 0x08                                   
	{0x22,0x08},  // STRTNOR        // 0x03                                  
	{0x23,0x38},//40}},  // STRTDRK        
	{0x24,0x00},

	// Gamma 
	#if 0
	{0x31, 0x02},//0x080x31, 0x04 },//0x08   0x03
	{0x32, 0x09},//0x100x32, 0x0b },//0x10   0x08
	{0x33, 0x16},//0x1B0x33, 0x24 },//0x1B   0x16
	{0x34, 0x3b},//0x370x34, 0x49 },//0x37   0x36
	{0x35, 0x5e},//0x4D0x35, 0x66 },//0x4D   0x56
	{0x36, 0x7c},//0x600x36, 0x7C },//0x60   0x77
	{0x37, 0x8f},//0x720x37, 0x8D },//0x72   0x88
	{0x38, 0x9f},//0x820x38, 0x9B },//0x82   0x9a
	{0x39, 0xae},//0x910x39, 0xAA },//0x91   0xA9
	{0x3a, 0xba},//0xA00x3a, 0xb6 },//0xA0   0xb5
	{0x3b, 0xcf},//0xBA0x3b, 0xca },//0xBA   0xca
	{0x3c, 0xe1},//0xD30x3c, 0xdc },//0xD3   0xdd
	{0x3d, 0xf0},//0xEA0x3d, 0xef },//0xEA   0xee
	#else
	{0x31, 0x03},//0x080x31, 0x04 },//0x08   0x03
	{0x32, 0x0b},//0x100x32, 0x0b },//0x10   0x08
	{0x33, 0x1e},//0x1B0x33, 0x24 },//0x1B   0x16
	{0x34, 0x46},//0x370x34, 0x49 },//0x37   0x36
	{0x35, 0x62},//0x4D0x35, 0x66 },//0x4D   0x56
	{0x36, 0x78},//0x600x36, 0x7C },//0x60   0x77
	{0x37, 0x8b},//0x720x37, 0x8D },//0x72   0x88
	{0x38, 0x9b},//0x820x38, 0x9B },//0x82   0x9a
	{0x39, 0xa8},//0x910x39, 0xAA },//0x91   0xA9
	{0x3a, 0xb6},//0xA00x3a, 0xb6 },//0xA0   0xb5
	{0x3b, 0xcc},//0xBA0x3b, 0xca },//0xBA   0xca
	{0x3c, 0xdf},//0xD30x3c, 0xdc },//0xD3   0xdd
	{0x3d, 0xf0},//0xEA0x3d, 0xef },//0xEA   0xee
	#endif
	// Shading Register Setting                                          
	{0x40,0x55},//04}},  //06                                         
	{0x41,0x44},                                           
	{0x42,0x33},//43}},                                           
	{0x43,0x00},                                   
	{0x44,0x22},  // left R gain[7:4], right R gain[3:0] 	 22					                                          
	{0x45,0x22},  // top R gain[7:4], bottom R gain[3:0] 		22				                                          
	{0x46,0x00},  // left G gain[7:4], right G gain[3:0] 	                                                    
	{0x47,0x03},  // top G gain[7:4], bottom G gain[3:0] 	   11                                                   
	{0x48,0x01},  // left B gain[7:4], right B gain[3:0] 	                                                   
	{0x49,0x11},  // top B gain[7:4], bottom B gain[3:0] 	           //color matrix default                                          
	{0x4a,0x05},  // X-axis center high[3:2], Y-axis center high[1:0]                                         
	{0x4b,0x40},  // X-axis center low[7:0]				48		                                                        
	{0x4c,0x10},  // Y-axis center low[7:0]		 e8			                                                          
	{0x4d,0x80},  // Shading Center Gain     80                                                                  
	{0x4e,0x00},  // Shading R Offset                                                                   
	{0x4f,0x00},  // Shading Gr Offset                                                                        
	{0x50,0x00},  // Shading B Offset                                

	
	// Interpolation                                           
	{0x60,0x7f},                                     
	{0x61,0x08},  
	 
	 // Color matrix (D65) - Daylight    
	 #if 0
	{0x71,0x33},		 //0x34 	  0x39							  
	{0x72,0xCd},		 //0xCe 	  0xCc							 
	{0x73,0x00},         //0xFe       0xFb                           
	{0x74,0x0e},         //0x13       0x10                           
	{0x75,0x27},         //0x25       0x21                           
	{0x76,0x0b},         //0x08       0x0f                           
	{0x77,0xff},		 //0xec 	  0xf8							 
	{0x78,0xd0},		 //0xd1 	  0xc0							 
	{0x79,0x42},		 //0x47 	  0x48
	
	// Color matrix (D20) - A                                           
	{0x83,0x34},     //0x3c 	                                       
	{0x84,0xd4},     //0xc6 	                                      
	{0x85,0xf8},     //0xff                                         
	{0x86,0x12},     //0x12                                         
	{0x87,0x25},     //0x24 	                                  
	{0x88,0x09},     //0x0a 	                                      
	{0x89,0xf0},     //0xed                                         
	{0x8a,0xbe},     //0xc2 	                                      
	{0x8b,0x54},     //0x51   
	#else
	{0x71,0x39},		 //0x34 	  0x39							  
	{0x72,0xC8},		 //0xCe 	  0xCc							 
	{0x73,0xFf},         //0xFe       0xFb                           
	{0x74,0x13},         //0x13       0x10                           
	{0x75,0x25},         //0x25       0x21                           
	{0x76,0x08},         //0x08       0x0f                           
	{0x77,0xf8},		 //0xec 	  0xf8							 
	{0x78,0xc0},		 //0xd1 	  0xc0							 
	{0x79,0x48},		 //0x47 	  0x48
	
	// Color matrix (D20) - A                                           
	{0x83,0x34},     //0x3c 	                                       
	{0x84,0xd4},     //0xc6 	                                      
	{0x85,0xf8},     //0xff                                         
	{0x86,0x12},     //0x12                                         
	{0x87,0x25},     //0x24 	                                  
	{0x88,0x09},     //0x0a 	                                      
	{0x89,0xf0},     //0xed                                         
	{0x8a,0xbe},     //0xc2 	                                      
	{0x8b,0x54},     //0x51   
	#endif
	                                                          
	{0x8c,0x10},     //CMA select
	
	//G Edge                                        
	{0x90,0x35},    //Upper gain                                                                 
	{0x91,0x48},    //down gain                                                                  
	{0x92,0x33},//22}},    //[7:4] upper coring [3:0] down coring                                       
	{0x9a,0x40},                                           
	{0x9b,0x40},                                           
	{0x9c,0x38},    //edge suppress start   30                                      
	{0x9d,0x30},    //edge suppress slope   
	
	{0x9f,0x26},
	{0xa0,0x11},
	
	{0xa8,0x11},//10}},                                     
	{0xa9,0x11},//10}},                                           
	{0xaa,0x11},  
	
	{0xbf,0x20},
	
	{0xb9,0x28},//30}},     // nightmode 38 at gain 0x48 5fps                                        
	{0xba,0x41},//44}},     // nightmode 80 at gain 0x48 5fps                                       
	{0xde,0x80},  
	                                     
	{0xe5,0x15},                                           
	{0xe6,0x02},                                           
	{0xe7,0x04}, 
	                                                                                             
	//Sensor On //Sensor On                                                             
	{0x00,0x01},                                           
	{0x03,0x01},  
       
     {0x00,0x00},
};


/*
 * The white balance settings
 * Here only tune the R G B channel gain. 
 * The white balance enalbe bit is modified in sensor_s_autowb and sensor_s_wb
 */
static struct regval_list sensor_wb_manual[] = { 
//null
};

static struct regval_list sensor_wb_auto_regs[] = {
	{0x00,0x03}, 
	//{{0x72},{0xa8}},
	//{{0x73},{0xa8}},
	{0x10,0xd0},//d3 d0
	{0x60,0xa0},
	{0x61,0xa0}
};

static struct regval_list sensor_wb_incandescence_regs[] = {
	//bai re guang
	{0x00,0x03}, 
	//{{0x72},{0xa8}},
	//{{0x73},{0xa8}},
	{0x10,0x00},//d3 d0
	{0x60,0x80},
	{0x61,0xe0}
};

static struct regval_list sensor_wb_fluorescent_regs[] = {
	//ri guang deng
	{0x00,0x03}, 
	//{{0x72},{0xa8}},
	//{{0x73},{0xa8}},
	{0x10,0x00},//d3 d0
	{0x60,0xb8},
	{0x61,0xcc}
};

static struct regval_list sensor_wb_tungsten_regs[] = {
	//wu si deng
    {0x00,0x03}, 
	//{{0x72},{0xa8}},
	//{{0x73},{0xa8}},
	{0x10,0x00},//d3 d0
	{0x60,0x7b},
	{0x61,0xa0}
};

static struct regval_list sensor_wb_horizon[] = { 
//null
};

static struct regval_list sensor_wb_daylight_regs[] = {
	//tai yang guang
	{0x00,0x03}, 
	//{{0x72},{0xa8}},
	//{{0x73},{0xa8}},
	{0x10,0x00},//d3 d0
	{0x60,0xd8},
	{0x61,0x90}
};

static struct regval_list sensor_wb_flash[] = { 
//null
};


static struct regval_list sensor_wb_cloud_regs[] = {
	{0x00,0x03}, 
	//{{0x72},{0xa8}},
	//{{0x73},{0xa8}},
	{0x10,0x00},//d3 d0
	{0x60,0xb4},
	{0x61,0x74}
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
	{0x00,0x04},
	{0xb6,0x00},
};

static struct regval_list sensor_colorfx_bw_regs[] = {
	/*
	{0x23,0x02},
	{0x2d,0x0a},
	{0x20,0xff},
	{0xd2,0x90},
	{0x73,0x00},
	
	{0xb3,0x40},
	{0xb4,0x80},
	{0xba,0x00},
	{0xbb,0x00},
	*/
};

static struct regval_list sensor_colorfx_sepia_regs[] = {
	/*
	{0x23,0x02},
	{0x2d,0x0a},
	{0x20,0xff},
	{0xd2,0x90},
	{0x73,0x00},
	
	{0xb3,0x40},
	{0xb4,0x80},
	{0xba,0xd0},
	{0xbb,0x28},
	*/
};

static struct regval_list sensor_colorfx_negative_regs[] = {
	/*
	{0x23,0x01},
	{0x2d,0x0a},
	{0x20,0xff},
	{0xd2,0x90},
	{0x73,0x00},
	
	{0xb3,0x40},
	{0xb4,0x80},
	{0xba,0x00},
	{0xbb,0x00},
	*/
};

static struct regval_list sensor_colorfx_emboss_regs[] = {
	/*
	{0x23,0x02},
	{0x2d,0x0a},
	{0x20,0xbf},
	{0xd2,0x10},
	{0x73,0x01},
	
	{0x51,0x40},
	{0x52,0x40},
	
	{0xb3,0x40},
	{0xb4,0x80},
	{0xba,0x00},
	{0xbb,0x00},
	*/
};

static struct regval_list sensor_colorfx_sketch_regs[] = {
	/*
	{0x23,0x02},
	{0x2d,0x0a},
	{0x20,0xbf},
	{0xd2,0x10},
	{0x73,0x01},
	
	{0x51,0x40},
	{0x52,0x40},
	
	{0xb3,0x98},
	{0xb4,0x06},
	{0xba,0x00},
	{0xbb,0x00},
	*/
};

static struct regval_list sensor_colorfx_sky_blue_regs[] = {
	/*
	{0x23,0x02},
	{0x2d,0x0a},
	{0x20,0xff},
	{0xd2,0x90},
	{0x73,0x00},
	
	{0xb3,0x40},
	{0xb4,0x80},
	{0xba,0x50},
	{0xbb,0xe0},
	*/
};

static struct regval_list sensor_colorfx_grass_green_regs[] = {
	/*
	{0x23,0x02},
	{0x2d,0x0a},
	{0x20,0xff},
	{0xd2,0x90},
	{0x73,0x88},
	
	{0xb3,0x40},
	{0xb4,0x80},
	{0xba,0xc0},
	{0xbb,0xc0},
	*/
};

static struct regval_list sensor_colorfx_skin_whiten_regs[] = {
	/*
	{0x23,0x02},
	{0x2d,0x0a},
	{0x20,0xbf},
	{0xd2,0x10},
	{0x73,0x01},
	      
	{0x51,0x40}, 
	{0x52,0x40}, 
 
	{0xb3,0x60},
	{0xb4,0x40},
	{0xba,0x00},
	{0xbb,0x00},
	*/
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
	{0x00,0x04},  
	
	{0xab,0xc0}
};

static struct regval_list sensor_ev_neg3_regs[] = {
	{0x00,0x04},  
	
	{0xab,0xb0}
};

static struct regval_list sensor_ev_neg2_regs[] = {
	{0x00,0x04},  
	
	{0xab,0xa0}
};

static struct regval_list sensor_ev_neg1_regs[] = {
	{0x00,0x04},  
	
	{0xab,0x90}
};

static struct regval_list sensor_ev_zero_regs[] = {
	{0x00,0x04},  
	
	{0xab,0x00}
};

static struct regval_list sensor_ev_pos1_regs[] = {
	{0x00,0x04},  
	
	{0xab,0x10}
};

static struct regval_list sensor_ev_pos2_regs[] = {
	{0x00,0x04},  
	
	{0xab,0x20}
};

static struct regval_list sensor_ev_pos3_regs[] = {
	{0x00,0x04},  
	
	{0xab,0x30}
};

static struct regval_list sensor_ev_pos4_regs[] = {
	{0x00,0x04},  
	
	{0xab,0x40}
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
	{0x00,0x04},	//YCbYCr
	{0x12,0x3d}
};

static struct regval_list sensor_fmt_yuv422_yvyu[] = {
	{0x00,0x04},		//YCrYCb
	{0x12,0x2d}
};

static struct regval_list sensor_fmt_yuv422_vyuy[] = {
	{0x00,0x04},		//CrYCbY
	{0x12,0x0d}
};

static struct regval_list sensor_fmt_yuv422_uyvy[] = {
	{0x00,0x04},		//CbYCrY
	{0x12,0x1d}
};

/*static struct regval_list sensor_fmt_raw[] = {
	{0x24,0xb7},//raw
};
*/


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
	
	ret = sensor_write(sd, 0x00, 0x01);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_g_hflip!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x04, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_g_hflip!\n");
		return ret;
	}
	
	val &= (1<<0);
	val = val>>0;		//0x14 bit0 is mirror
		
	*value = val;

	info->hflip = *value;
	return 0;
}

static int sensor_s_hflip(struct v4l2_subdev *sd, int value)
{
	int ret;
	struct sensor_info *info = to_state(sd);
	unsigned char val;
	
	ret = sensor_write(sd, 0x00, 0x01);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_hflip!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x04, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_s_hflip!\n");
		return ret;
	}
	
	switch (value) {
		case 0:
		  val &= 0x00;
			break;
		case 1:
			val |= 0x01;
			break;
		default:
			return -EINVAL;
	}
	ret = sensor_write(sd, 0x04, val);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_hflip!\n");
		return ret;
	}
	
	msleep(10);
	
	info->hflip = value;
	return 0;
}

static int sensor_g_vflip(struct v4l2_subdev *sd, __s32 *value)
{
	int ret;
	struct sensor_info *info = to_state(sd);
	unsigned char val;
	
	ret = sensor_write(sd, 0x00, 0x01);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_g_vflip!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x04, &val);
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
	
	ret = sensor_write(sd, 0x00, 0x01);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_vflip!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x04, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_s_vflip!\n");
		return ret;
	}
	
	switch (value) {
		case 0:
		  val &= 0x00;
			break;
		case 1:
			val |= 0x02;
			break;
		default:
			return -EINVAL;
	}
	ret = sensor_write(sd, 0x04, val);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_vflip!\n");
		return ret;
	}
	
	msleep(10);
	
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
	
	ret = sensor_write(sd, 0x00, 0x02);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_g_autoexp!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x10, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_g_autoexp!\n");
		return ret;
	}

	val &= 0x80;
	if (val == 0x80) {
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
	
	ret = sensor_write(sd, 0x00, 0x02);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_autoexp!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x10, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_s_autoexp!\n");
		return ret;
	}

	switch (value) {
		case V4L2_EXPOSURE_AUTO:
		  val |= 0x80;
			break;
		case V4L2_EXPOSURE_MANUAL:
			val &= 0x7f;
			break;
		case V4L2_EXPOSURE_SHUTTER_PRIORITY:
			return -EINVAL;    
		case V4L2_EXPOSURE_APERTURE_PRIORITY:
			return -EINVAL;
		default:
			return -EINVAL;
	}
		
	ret = sensor_write(sd, 0x10, val);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_autoexp!\n");
		return ret;
	}
	
	msleep(10);

	info->autoexp = value;
	return 0;
}

static int sensor_g_autowb(struct v4l2_subdev *sd, int *value)
{
	int ret;
	struct sensor_info *info = to_state(sd);
	unsigned char val;
	
	ret = sensor_write(sd, 0x00, 0x03);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_g_autowb!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x10, &val);
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
/*
	ret = sensor_write(sd, 0xf0, 0x00);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_autowb!\n");
		return ret;
	}
*/
	ret = sensor_read(sd, 0x42, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_s_autowb!\n");
		return ret;
	}

	switch(value) {
	case 0:
		val &= 0xd0;
		break;
	case 1:
		val |= 0x00;
		break;
	default:
		break;
	}	
	ret = sensor_write(sd, 0x10, val);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_s_autowb!\n");
		return ret;
	}
	
	msleep(10);
	
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
  
  if(info->wb == value)
    return 0;
  
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
	//struct i2c_client *client = v4l2_get_subdevdata(sd);
	
  //make sure that no device can access i2c bus during sensor initial or power down
  //when using i2c_lock_adpater function, the following codes must not access i2c bus before calling i2c_unlock_adapter
  cci_lock(sd);

  //insure that clk_disable() and clk_enable() are called in pair 
  //when calling CSI_SUBDEV_STBY_ON/OFF and CSI_SUBDEV_PWR_ON/OFF  
  switch(on)
  {
    case CSI_SUBDEV_STBY_ON:
      vfe_dev_dbg("CSI_SUBDEV_STBY_ON\n");
      //standby on io
      vfe_gpio_write(sd,PWDN,CSI_STBY_ON);
      usleep_range(30000,31000);
      //inactive mclk after stadby in
      vfe_set_mclk(sd,OFF);

      break;
    case CSI_SUBDEV_STBY_OFF:
//----------------------
      //power supply
  //    vfe_gpio_write(sd,POWER_EN,CSI_PWR_ON);
  //    vfe_set_pmu_channel(sd,IOVDD,ON);
  //    vfe_set_pmu_channel(sd,AVDD,ON);
  //    vfe_set_pmu_channel(sd,DVDD,ON);
  //    vfe_set_pmu_channel(sd,AFVDD,ON);
  //    usleep_range(20000,22000);
//----------------------

      vfe_dev_dbg("CSI_SUBDEV_STBY_OFF\n");
      //active mclk before stadby out
      vfe_set_mclk_freq(sd,MCLK);
      vfe_set_mclk(sd,ON);
      usleep_range(30000,31000);
      //standby off io
      vfe_gpio_write(sd,PWDN,CSI_STBY_OFF);
      usleep_range(10000,12000);



      break;
    case CSI_SUBDEV_PWR_ON:
      
      vfe_dev_dbg("CSI_SUBDEV_PWR_ON\n");
      //power on reset
      vfe_gpio_set_status(sd,PWDN,1);//set the gpio to output
      vfe_gpio_set_status(sd,RESET,1);//set the gpio to output
      usleep_range(10000,12000);
      //standby off io
      vfe_gpio_write(sd,PWDN,CSI_STBY_ON);
			//reset on io
      vfe_gpio_write(sd,RESET,CSI_RST_ON);
      //power supply
      vfe_gpio_write(sd,POWER_EN,CSI_PWR_ON);
      vfe_set_pmu_channel(sd,IOVDD,ON);
      vfe_set_pmu_channel(sd,AVDD,ON);
      vfe_set_pmu_channel(sd,DVDD,ON);
      vfe_set_pmu_channel(sd,AFVDD,ON);
      usleep_range(20000,22000);
      //standby off io
      vfe_gpio_write(sd,PWDN,CSI_STBY_OFF);
      usleep_range(10000,12000);
			//active mclk
      vfe_set_mclk_freq(sd,MCLK);
      vfe_set_mclk(sd,ON);
      usleep_range(10000,12000);
			//reset on io
      vfe_gpio_write(sd,RESET,CSI_RST_ON);
			usleep_range(30000,31000);
			//reset off io
      vfe_gpio_write(sd,RESET,CSI_RST_OFF);
			usleep_range(30000,31000);
			break;
	case CSI_SUBDEV_PWR_OFF:
      vfe_dev_dbg("CSI_SUBDEV_PWR_OFF\n");
      //reset io
      usleep_range(10000,12000);
      vfe_gpio_write(sd,RESET,CSI_RST_ON);
			usleep_range(10000,12000);
			//inactive mclk after power off
      vfe_set_mclk(sd,OFF);
      //power supply off
      vfe_gpio_write(sd,POWER_EN,CSI_PWR_OFF);
      vfe_set_pmu_channel(sd,AFVDD,OFF);
      vfe_set_pmu_channel(sd,DVDD,OFF);
      vfe_set_pmu_channel(sd,AVDD,OFF);
      vfe_set_pmu_channel(sd,IOVDD,OFF);  
      usleep_range(10000,12000);
      //standby and reset io
			//standby of io
      vfe_gpio_write(sd,PWDN,CSI_STBY_ON);
      usleep_range(10000,12000);
      //set the io to hi-z
      vfe_gpio_set_status(sd,RESET,0);//set the gpio to input
      vfe_gpio_set_status(sd,PWDN,0);//set the gpio to input
			break;
		default:
			return -EINVAL;
	}		

	//remember to unlock i2c adapter, so the device can access the i2c bus again
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
	
	ret = sensor_write(sd, 0x00, 0x00);
	if (ret < 0) {
		vfe_dev_err("sensor_write err at sensor_detect!\n");
		return ret;
	}
	
	ret = sensor_read(sd, 0x01, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_detect!\n");
		return ret;
	}

	if(val != 0xde)
		return -ENODEV;

vfe_dev_err(",,,,,,,, 1 %x!\n", val);

	ret = sensor_read(sd, 0x02, &val);
	if (ret < 0) {
		vfe_dev_err("sensor_read err at sensor_detect!\n");
		return ret;
	}
	if (val==0x10)
		sensor_write_array(sd, siv121d_sensor_default_regs , ARRAY_SIZE(siv121d_sensor_default_regs));
	else
		sensor_write_array(sd, siv121du_sensor_default_regs , ARRAY_SIZE(siv121du_sensor_default_regs));
vfe_dev_err(",,,,,,,, 2 %x!\n", val);

	
	return 0;
}
/*
static int sensor_release_detect(struct v4l2_subdev *sd)
{
	int ret;
	unsigned char val;
	
	//usleep_range(10000,12000);
	
	ret = sensor_read(sd, 0x02, val);
	if (ret < 0) {
		vfe_dev_err(">>>>>>>>>>>>>>>> sensor_read err at sensor_detect!\n");
		return ret;
	}
	vfe_dev_err("2,,,,,,,, 2 %x!\n", val);
	if (val==0x10)//siv121d
	{
		vfe_dev_dbg(">>>>>>>>>>>>>>>> read sensor release is siv121d\n");
		return 0;
	}
	else if(val==0x15) //siv121du
	{
		vfe_dev_dbg(">>>>>>>>>>>>>>>> read sensor release is siv121du\n");
		return 1;
	}
	else
	{
		vfe_dev_dbg(">>>>>>>>>>>>>>>> read sensor release faild\n");
		return -ENODEV;
	}
	return -1;
}
*/
static int sensor_init(struct v4l2_subdev *sd, u32 val)
{
	int ret;
	vfe_dev_dbg("sensor_init\n");
	
	/*Make sure it is a target sensor*/
	ret = sensor_detect(sd);
	if (ret) {
		vfe_dev_err("chip found is not an target chip.\n");
		return ret;
	}
	return 0;
	//return sensor_write_array(sd, siv121du_sensor_default_regs , ARRAY_SIZE(siv121du_sensor_default_regs));
	/*
	ret = sensor_release_detect(sd);
	if(ret == 0){
		vfe_dev_dbg("##################### %s:  read sensor release is siv121d\n", __func__);
		return sensor_write_array(sd, siv121d_sensor_default_regs , ARRAY_SIZE(siv121d_sensor_default_regs));
	}else	
	{ 
        vfe_dev_dbg("#####################  %s: read sensor release is siv121du\n", __func__);
		return sensor_write_array(sd, siv121du_sensor_default_regs , ARRAY_SIZE(siv121du_sensor_default_regs));
	}
*/
}

static long sensor_ioctl(struct v4l2_subdev *sd, unsigned int cmd, void *arg)
{
	int ret=0;
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
//	{
//		.desc		= "Raw RGB Bayer",
//		.mbus_code	= V4L2_MBUS_FMT_SBGGR8_1X8,//linux-3.0
//		.regs 		= sensor_fmt_raw,
//		.regs_size = ARRAY_SIZE(sensor_fmt_raw),
//		.bpp		= 1
//	},
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

static int sensor_enum_size(struct v4l2_subdev *sd,
                            struct v4l2_frmsizeenum *fsize)
{
  if(fsize->index > N_WIN_SIZES-1)
  	return -EINVAL;
  
  fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
  fsize->discrete.width = sensor_win_sizes[fsize->index].width;
  fsize->discrete.height = sensor_win_sizes[fsize->index].height;
  
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
  .enum_framesizes = sensor_enum_size,
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

	//client->addr=0x60>>1;

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
