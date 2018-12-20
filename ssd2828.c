//备注，2828与2825的初始化代码是一样的，把spi里的屏的代码换为163的代码就好了。

#include "unistd.h"
#include <stdio.h>
#include <stdlib.h>

#include <system.h>

#include <io.h>
#include "sys/alt_irq.h"
#include "altera_avalon_pio_regs.h"
#include "pic.h"
typedef volatile unsigned long  vu32;
typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;







#define     write_ctrl   W_C
#define     write_data   W_D

#define HSIZE        800
#define VSIZE        1280
#define FRAME_SIZE        (HSIZE*VSIZE*4)

#define BLACK             (0x00000000)
#define GRAY             (0x007F7F7F)
#define WHITE             (0xffffffff)
#define RED               (0xFF<<16)
#define GREEN             (0xFF<<8)
#define BLUE              (0xFF)
#define QING              (0x00FFFF) //绿+兰=青
#define YELLOW              (0xFFFF00)//红+绿=黄
#define FEN            (0xFF00FF) //红+蓝=粉



#define     N       3   //当前基准频率120M
/***********************
* 分频 计算     当N=1     如选取基准频率为100M的主频   则PCLK=100M
* 当N=2   表示2分频   则PCLK=50M   以此类推
*  SOF  提供多个基准频率   便于用户实现更精准的PCLK
*/
/*
#define  HBPD      80   //80
#define  HFPD      140  //140
#define  HSPW      8    //8

#define  VBPD      4	//4
#define  VFPD      8	//	8
#define  VSPW      8	//8
*/
#define  HBPD      10 //80
#define  HFPD      10  //140
#define  HSPW      10    //8

#define  VBPD      6//4
#define  VFPD      2//	8
#define  VSPW      10//8


unsigned int buffer[VSIZE][HSIZE];
extern unsigned char pic[];
//extern unsigned char pic1[];

void Delay(unsigned int s);
void Delayus(unsigned int s);
void Delayus1(unsigned int s);
void waitkey(void);
void SPI_INIT(void);
void W_C(int data);
void W_C1(int data);
void W_D(int data);
void PacketSize_b(unsigned char *Data1);
void PacketSize_a(unsigned char *Data1);
void PacketSize_c(unsigned char *Data1);

void packet_DCS(int  PacketDCS);
void packet_GEN(int  PacketSize);
void block(void);

void read_ID(int data);
unsigned char SPI_READ(void);
void show_ID(void);
void mainlcd_test_id(int xs,int xe,int ys,int ye,unsigned char *s);
unsigned char  s[10][1300];
void show_ADC_ID(void);
//获取指定通道的A/D转换结果,两个函数任选其一
unsigned char ReadADC(int chanel);
unsigned char get_value_ADC0832(unsigned char CH);

void PutPixel(int x,int y, int colour );
void area(int xs,int xe,int ys,int ye,int colour);
void side(void);

void  rgb_init(void);
void fill_frame ( long base, long length,  long colour);
void fill_picture (unsigned long base);
//void fill_picture1 (unsigned long base);
void show_color_h (void);

void show_color_v (void);
void GRAY_32(void);


//I/O define
//KEY
#define key_rd()  IORD_ALTERA_AVALON_PIO_DATA(KEY_BASE)
//LCM
//#define DEN      *(vu32 *)(DEN_BASE     | (1<<31))
#define RESET    *(vu32 *)(RESET_BASE   | (1<<31))
#define SCLK     *(vu32 *)(SCLK_BASE    | (1<<31))
#define CS       *(vu32 *)(CS_BASE      | (1<<31))
#define SDI      *(vu32 *)(SDA_BASE     | (1<<31))
//#define SDO      *(vu32 *)(SDO_BASE     | (1<<31))


int id;
int main()
{
int i,adc_id0,adc_id1;

 // printf("Hello from Nios II!\n");

rgb_init();Delay(100);
SPI_INIT();Delay(50);


  while(1)
  {
//fill_frame (( long)buffer, FRAME_SIZE, GRAY);
//Delay(100);waitkey();

	  fill_frame (( long)buffer, FRAME_SIZE, WHITE);
	  Delay(100);waitkey();

	  fill_frame (( long)buffer, FRAME_SIZE, BLACK);
	  //area(HSIZE/4,(HSIZE/4)*3,VSIZE/4,(VSIZE/4)*3,WHITE);
	  Delay(100);waitkey();

	  GRAY_32();
	  Delay(100);waitkey();Delay(100);
	  show_color_v ();
	  //show_color_h ();
	  Delay(100);waitkey();Delay(100);


	//  fill_frame (( long)buffer, FRAME_SIZE, (0X1F<<16)|(0X1F<<8)|0X1F);
	//  Delay(100);waitkey();Delay(100);

	//  fill_frame (( long)buffer, FRAME_SIZE, (0X2F<<16)|(0X2F<<8)|0X2F);
	 // Delay(100);//waitkey();Delay(100);

	//  fill_frame (( long)buffer, FRAME_SIZE, (0X3F<<16)|(0X3F<<8)|0X3F);
	//  Delay(100);waitkey();Delay(100);

	 // fill_frame (( long)buffer, FRAME_SIZE, (0X4F<<16)|(0X4F<<8)|0X4F);
	//  Delay(100);waitkey();Delay(100);

	 // fill_frame (( long)buffer, FRAME_SIZE, (0X5F<<16)|(0X5F<<8)|0X5F);
	//  Delay(100);waitkey();Delay(100);

fill_frame (( long)buffer, FRAME_SIZE, RED);
Delay(100);waitkey();Delay(100);

fill_frame (( long)buffer, FRAME_SIZE, GREEN);
Delay(100);waitkey();Delay(100);

fill_frame (( long)buffer, FRAME_SIZE, BLUE);
Delay(100);waitkey();Delay(100);






fill_picture (( long)buffer);
Delay(100);waitkey();
  }

  return 0;
}

void packet_DCS(int  PacketDCS)
{

    W_C(0xB7);
    W_D(0x50);
    W_D(0x02);
    W_C(0xBD);
    W_D(0x00);
    W_D(0x00);
    W_C(0xBC);
    W_D(PacketDCS);
    W_D(0x00);
	Delay(1);



}
void packet_GEN(int  PacketSize)
{
	W_C(0xB7);
	W_D(0x10);
	W_D(0x02);

    W_C(0xBD);
    W_D(0x00);
    W_D(0x00);

    W_C(0xBC);
    W_D(PacketSize);
    W_D(0x00);
	Delay(1);

	W_C(0xbf);

}



void SPI_INIT(void)
{
//DEN=1;Delay(10);
    // VCI=2.8V

    RESET=1;
    Delay(200); // Delay 1ms
    RESET=0;
    Delay(1000);// Delay 10ms // This Delay time is necessary
    RESET=1;
    Delay(1000); // Delay 50 ms

    //SSD2825_Initial
    				        W_C(0xb7);
    				        W_D(0x50);//50=TX_CLK 70=PCLK
    				        W_D(0x00);   //Configuration Register

    				        W_C(0xb8);
    				        W_D(0x00);
    				        W_D(0x00);   //VC(Virtual ChannelID) Control Register

    				        W_C(0xb9);
    				        W_D(0x00);//1=PLL disable
    				        W_D(0x00);
    	                               //TX_CLK/MS should be between 5Mhz to100Mhz
    				        W_C(0xBA);//PLL=(TX_CLK/MS)*NS 8228=480M 4428=240M  061E=120M 4214=240M 821E=360M 8219=300M
    				        W_D(0x14);//D7-0=NS(0x01 : NS=1)
    				        W_D(0x42);//D15-14=PLL范围 00=62.5-125 01=126-250 10=251-500 11=501-1000  DB12-8=MS(01:MS=1)

    				        W_C(0xBB);//LP Clock Divider LP clock = 400MHz / LPD / 8 = 240 / 8 / 4 = 7.5MHz
    				        W_D(0x03);//D5-0=LPD=0x1 – Divide by 2
    				        W_D(0x00);

    				        W_C(0xb9);
    				       	W_D(0x00);//1=PLL disable
    				       	W_D(0x00);
    				        //MIPI lane configuration
    				            	W_C(0xDE);//通道数
    				            	W_D(0x00);//11=4LANE 10=3LANE 01=2LANE 00=1LANE
    				            	W_D(0x00);

    				        W_C(0xc9);
    				        W_D(0x02);
    				        W_D(0x23);   //p1: HS-Data-zero  p2: HS-Data- prepare  --> 8031 issue
    				        Delay(100);








//    				        unsigned char tab0 []={0x01,0x01};
//    				        unsigned char tab1 []={0x03,0xF0,0x5A,0x5A};
//    				        unsigned char tab2 []={0x03,0xF1,0x5A,0x5A};
//    				        unsigned char tab3 []={0x03,0xFC,0xA5,0xA5};
//    				        unsigned char tab4 []={0x02,0xB1,0x10};
//    				        unsigned char tab5 []={0x05,0xB2,0x14,0x22,0x2F,0x04};
//    				        unsigned char tab6 []={0x03,0xD0,0x00,0x10};
//    				        unsigned char tab7 []={0x06,0xF2,0x02,0x08,0x08,0x90,0x10};
//    				        unsigned char tab8 []={0x02,0xB0,0x04};
//    				        unsigned char tab9 []={0x02,0xFD,0x09};
//    				        unsigned char tab10[]={0x0b,0xF3,0x01,0xC0,0xE0,0x62,0xD3,0x81,0x35,0x30,0x25,0x00};
//    				        unsigned char tab11[]={0x2E,0xF4,0x00,0x02,0x03,0x26,0x03,0x02,0x09,0x00,0x07,0x16,0x16,0x03,0x00,0x08,0x08,0x03,0x19,0x1A,0x12,0x1C,0x1D,0x1E,0x1A,0x09,0x01,0x04,0x02,0x61,0x74,0x75,0x72,0x83,0x80,0x80,0x00,0x00,0x01,0x01,0x28,0x04,0x03,0x28,0x01,0xD1,0x32};
//    				        unsigned char tab12[]={0x1b,0xF5,0x64,0x3d,0x3d,0x5F,0xAB,0x98,0x4F,0x0F,0x33,0x43,0x04,0x59,0x54,0x52,0x05,0x40,0x60,0x40,0x60,0x40,0x27,0x26,0x52,0x25,0x6D,0x18};
//    				        unsigned char tab13[]={0x09,0xEE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//    				        unsigned char tab14[]={0x09,0xEF,0x12,0x12,0x43,0x43,0x90,0x84,0x24,0x81};
//    				        unsigned char tab15[]={0x07,0xF6,0x63,0x21,0xA6,0x00,0x00,0x14};
//    				        unsigned char tab16[]={0x02,0xB0,0x22};
//    				        unsigned char tab17[]={0x12,0xFA,0x00,0x33,0x04,0x09,0x13,0x0D,0x13,0x19,0x1B,0x23,0x29,0x31,0x39,0x38,0x30,0x2D,0x2A};
//    				        unsigned char tab18[]={0x02,0xB0,0x22};
//    				        unsigned char tab19[]={0x12,0xFB,0x00,0x33,0x04,0x09,0x13,0x0D,0x13,0x19,0x1B,0x23,0x29,0x31,0x39,0x38,0x30,0x2D,0x2A};
//    				        unsigned char tab20[]={0x02,0xB0,0x11};
//    				        unsigned char tab21[]={0x12,0xFA,0x20,0x33,0x23,0x23,0x26,0x1A,0x1D,0x1F,0x1E,0x24,0x2A,0x31,0x3A,0x39,0x31,0x30,0x32};
//    				        unsigned char tab22[]={0x02,0xB0,0x11};
//    				        unsigned char tab23[]={0x12,0xFB,0x20,0x33,0x23,0x23,0x26,0x1A,0x1D,0x1F,0x1E,0x24,0x2A,0x31,0x3A,0x39,0x31,0x30,0x32};
//    				        unsigned char tab24[]={0x02,0xB0,0x00};
//    				        unsigned char tab25[]={0x12,0xFA,0x00,0x33,0x05,0x0A,0x13,0x0D,0x13,0x18,0x1A,0x22,0x29,0x30,0x38,0x38,0x31,0x2F,0x30};
//    				        unsigned char tab26[]={0x02,0xB0,0x00};
//    				        unsigned char tab27[]={0x12,0xFB,0x00,0x33,0x05,0x0A,0x13,0x0D,0x13,0x18,0x1A,0x22,0x29,0x30,0x38,0x38,0x31,0x2F,0x30};
//    				        unsigned char tab28[]={0x21,0xF7,0x0A,0x0A,0x08,0x08,0x0B,0x0B,0x09,0x09,0x04,0x05,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x0A,0x08,0x08,0x0B,0x0B,0x09,0x09,0x04,0x05,0x01,0x01,0x01,0x01,0x01,0x01};
//    				        unsigned char tab29[]={0x01,0x11};
//    				        unsigned char tab30[]={0x04,0xC3,0x40,0x00,0x28};//0x28};
//    				        unsigned char tab31[]={0x01,0x35};
//    				        unsigned char tab32[]={0x01,0x29};
//    				                                       // PacketSize_b (tab0 );Delay(1000);
//    				                                        PacketSize_b (tab1 );
//    				                				        PacketSize_b (tab2 );
//    				                				        PacketSize_b (tab3 );
//    				                				        PacketSize_b (tab4 );
//    				                				        PacketSize_b (tab5 );
//    				                				        PacketSize_b (tab6 );
//   				                				            PacketSize_b (tab7 );
//    				                				        PacketSize_b (tab8 );
//   				                				            PacketSize_b (tab9 );
//   				                				            PacketSize_b (tab10 );
//
//   				                				            PacketSize_b (tab11 );
//    				                				        PacketSize_b (tab12 );
//    				                				        PacketSize_b (tab13 );
//    				                				        PacketSize_b (tab14 );
//    				                				        PacketSize_b (tab15 );
//    				                				        PacketSize_b (tab16 );
//   				                				            PacketSize_b (tab17 );
//    				                				        PacketSize_b (tab18 );
//   				                				            PacketSize_b (tab19 );
//   				                				            PacketSize_b (tab20 );
//    				                                        PacketSize_b (tab21 );
//   				                				            PacketSize_b (tab22 );
//    				                				        PacketSize_b (tab23 );
//   				                				            PacketSize_b (tab24 );
//    				                				        PacketSize_b (tab25 );
//    				                				        PacketSize_b (tab26 );
//    				                				        PacketSize_b (tab27 );
//   				                				            PacketSize_b (tab28 );
//    				     	                                PacketSize_a (tab29 );Delay(100);
//    				     	        				        PacketSize_a (tab30 );Delay(100);
//    				     	        				        PacketSize_a (tab31 );Delay(100);
//    				     	        				        PacketSize_a (tab32 );Delay(100);



    				        unsigned char tab1[]={0x03,0xF0,0x5A,0x5A};
    				        unsigned char tab2[]={0x03,0xF1,0x5A,0x5A};
    				        unsigned char tab3[]={0x03,0xFC,0xA5,0xA5};
    				        unsigned char tab4[]={0xD0,0x00,0x10};
    				        unsigned char tab5[]={0x02,0xB1,0x10};
    				        unsigned char tab6[]={0x05,0xB2,0x14,0x22,0x2F,0x04};
    				        unsigned char tab7[]={0x06,0xF2,0x02,0x10,0x10,0x20,0x10};
    				        unsigned char tab8[]={0x02,0xB0,0x04};
    				        unsigned char tab9[]={0x02,0xFD,0x09};
    				        unsigned char tab10[]={0x0b,0xF3,0x01,0xD7,0xE2,0x62,0xF4,0xF7,0x77,0x3C,0x26,0x00};
    				        unsigned char tab11[]={0x2e,0xF4,0x00,0x02,0x03,0x26,0x03,0x02,0x09,0x00,0x07,0x16,0x16,0x03,0x00,0x08,0x08,0x03,0x0E,0x0F,0x12,0x1C,0x1D,0x1E,0x0C,0x09,0x01,0x04,0x02,0x61,0x74,0x75,0x72,0x83,0x80,0x80,0xB0,0x00,0x01,0x01,0x28,0x04,0x03,0x28,0x01,0xD1,0x32};
    				        unsigned char tab12[]={0x1b,0xF5,0x92,0x28,0x28,0x5F,0xAB,0x98,0x52,0x0F,0x33,0x43,0x04,0x59,0x54,0x52,0x05,0x40,0x60,0x4E,0x60,0x40,0x27,0x26,0x52,0x25,0x6D,0x18};
    				        unsigned char tab13[]={0x09,0xEE,0x25,0x00,0x25,0x00,0x25,0x00,0x25,0x00};
    				        unsigned char tab14[]={0x09,0xEF,0x12,0x12,0x43,0x43,0xA0,0x04,0x24,0x81};
    				        unsigned char tab15[]={0x21,0xF7,0x0A,0x0A,0x08,0x08,0x0B,0x0B,0x09,0x09,0x04,0x05,0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x0A,0x08,0x08,0x0B,0x0B,0x09,0x09,0x04,0x05,0x01,0x01,0x01,0x01,0x01,0x01};
    				        unsigned char tab16[]={0x04,0xBC,0x01,0x4E,0x08};
    				        unsigned char tab17[]={0x06,0xE1,0x03,0x10,0x1C,0xA0,0x10};
    				        unsigned char tab18[]={0x07,0xF6,0x60,0x25,0xA6,0x00,0x00,0x00};
    				        unsigned char tab19[]={0x34,0xFA,0x00,0x34,0x06,0x0D,0x04,0x0A,0x0F,0x0F,0x12,0x1B,0x1E,0x1D,0x1E,0x1D,0x1D,0x1D,0x25};
    				        unsigned char tab20[]={0x34,0xFB,0x00,0x34,0x06,0x0D,0x04,0x0A,0x0F,0x0F,0x12,0x1B,0x1E,0x1D,0x1E,0x1D,0x1D,0x1D,0x25};
    				        unsigned char tab21[]={0x07,0xFE,0x00,0x0D,0x03,0x21,0x00,0x08};
    				        unsigned char tab22[]={0x04,0xC3,0x40,0x00,0x28};
    				        unsigned char tab23[]={0x01,0x35};
    				        unsigned char tab24[]={0x01,0x11};
    				        unsigned char tab25[]={0x01,0x29};
    				        unsigned char tab26[]={0x05,0x2a,0x00,0x00,0x20,0x03};
    				        unsigned char tab27[]={0x05,0x2b,0x00,0x00,0x00,0x05};    				            				                				           				     PacketSize_b (tab1 );
    				            				         PacketSize_b (tab2 );
    				            				         PacketSize_b (tab3 );
    				            				         PacketSize_b (tab4 );
    				            				         PacketSize_b (tab5 );
    				            				         PacketSize_b (tab6 );
    				            				         PacketSize_b (tab7 );
    				            				         PacketSize_b (tab8 );
    				            				         PacketSize_b (tab9 );
    				            				         PacketSize_b (tab10 );

                                                         PacketSize_b (tab11 );
    				            				         PacketSize_b (tab12 );
    				            				         PacketSize_b (tab13 );
    				            				         PacketSize_b (tab14 );
    				            				         PacketSize_b (tab15 );
    				            				         PacketSize_b (tab16 );
    				            				         PacketSize_b (tab17 );
    				            				         PacketSize_b (tab18 );
    				        						//PacketSize_b (tabF5 );
    				          				             PacketSize_b (tab19 );
    				            				         PacketSize_b (tab20 );
    				            				         PacketSize_b (tab21 );
    				            				         PacketSize_b (tab22 );
    				            				         PacketSize_b (tab23 );
    				            				         PacketSize_b (tab24 );
    				            				         PacketSize_b (tab25 );
    				            				         PacketSize_b (tab26 );
    				            				         PacketSize_b (tab27 );



	//SSD2825_Initial
	W_C(0xb7);
	W_D(0x50);
	W_D(0x00);   //Configuration Register

	W_C(0xb8);
	W_D(0x00);
	W_D(0x00);   //VC(Virtual ChannelID) Control Register

	W_C(0xb9);
	W_D(0x00);//1=PLL disable
	W_D(0x00);

	W_C(0xBA);//PLL=(TX_CLK/MS)*NS 8228=480M 4428=240M  061E=120M 4214=240M 821E=360M 8219=300M 8225=444M 8224=432
	W_D(0x22);//D7-0=NS(0x01 : NS=1)//28
	W_D(0x82);//D15-14=PLL范围 00=62.5-125 01=126-250 10=251-500 11=501-1000  DB12-8=MS(01:MS=1)

	W_C(0xBB);//LP Clock Divider LP clock = 400MHz / LPD / 8 = 480 / 8/ 8 = 7MHz
	W_D(0x06);//D5-0=LPD=0x1 – Divide by 2
	W_D(0x00);

	W_C(0xb9);
	W_D(0x01);//1=PLL disable
	W_D(0x00);

	W_C(0xc9);
	W_D(0x02);
	W_D(0x23);   //p1: HS-Data-zero  p2: HS-Data- prepare  --> 8031 issue
	//Delay(100);

	W_C(0xCA);
	W_D(0x01);//CLK Prepare
	W_D(0x23);//Clk Zero

	W_C(0xCB); //local_write_reg(addr=0xCB,data=0x0510)
	W_D(0x10); //Clk Post
	W_D(0x05); //Clk Per

	W_C(0xCC); //local_write_reg(addr=0xCC,data=0x100A)
	W_D(0x05); //HS Trail
	W_D(0x10); //Clk Trail

	W_C(0xD0);
	W_D(0x00);
	W_D(0x00);

	//RGB interface configuration
	W_C(0xB1);
	W_D(HSPW);//HSPW 07
	W_D(VSPW);//VSPW 05

	W_C(0xB2);
	W_D(HBPD);//HBPD 0x64=100
	W_D(VBPD);//VBPD 8 减小下移

	W_C(0xB3);
	W_D(HFPD);//HFPD 8
	W_D(VFPD);//VFPD 10

	W_C(0xB4);//Horizontal active period 720=02D0
	W_D(0x20);//013F=319 02D0=720
	W_D(0x03);//HACT=0x01E0=480	,0x021c=540

	W_C(0xB5);//Vertical active period 1280=0500
	W_D(0x00);//01DF=479 0500=1280
	W_D(0x05);//VACT=0x0320=800	,0x03c0=960	 ,0x0356=854

	W_C(0xB6);//RGB CLK  16BPP=00 18BPP=01
	W_D(0x03);//D7=0 D6=0 D5=0  D1-0=11 – 24bpp
	W_D(0x20);//D15=VS D14=HS D13=CLK D12-9=NC D8=0=Video with blanking packet. 00-F0



	//MIPI lane configuration
	W_C(0xDE);//通道数
	W_D(0x03);//11=4LANE 10=3LANE 01=2LANE 00=1LANE
	W_D(0x00);

	W_C(0xD6);//  05=BGR  04=RGB
	W_D(0x05);//D0=0=RGB 1:BGR D1=1=Most significant byte sent first
	W_D(0x00);


	W_C(0xB7);
	W_D(0x7b);
	W_D(0x02);

//	PacketSize_b (tab31 );Delay(2000);
//	PacketSize_b (tab29 );Delay(2000);
//			PacketSize_b (tab30 );Delay(2000);
//
//	PacketSize_b (tab32 );Delay(2000);
/*
	   				                				        PacketSize_b (tab19 );Delay(2000);
	    				                				    PacketSize_b (tab20 );Delay(2000);
	   				                				        PacketSize_b (tab21 );Delay(2000);
	   				                				        PacketSize_b (tab22 );Delay(2000);
//*/
	W_C(0x2C);
	W_C(0x3C);

}
void PacketSize_c(unsigned char *Data1)
{
unsigned int i;
unsigned char  PCS;
PCS=*Data1;
        W_C(0xB7);
        W_D(0x4b);
        W_D(0x02);

        W_C(0xBD);
        W_D(0x00);
        W_D(0x00);

        W_C(0xBC);
        W_D(PCS);
        W_D(0x00);
		//delay1(1);

        W_C(0xbf);
        for(i=0;i<PCS;i++){W_D(*(Data1+i+1));}
}
void PacketSize_a(unsigned char *Data1)
{
unsigned int i;
unsigned char  PCS;
PCS=*Data1;
        W_C(0xB7);
        W_D(0x50);
        W_D(0x02);

        W_C(0xBD);
        W_D(0x00);
        W_D(0x00);

        W_C(0xBC);
        W_D(PCS);
        W_D(0x00);
		//delay1(1);

        W_C(0xbf);
        for(i=0;i<PCS;i++){W_D(*(Data1+i+1));}
}



void PacketSize_b(unsigned char *Data1)
{
unsigned int i;
unsigned char  PCS;
PCS=*Data1;
        W_C(0xB7);
        W_D(0x50);
        W_D(0x02);

        W_C(0xBD);
        W_D(0x00);
        W_D(0x00);

        W_C(0xBC);
        W_D(PCS);
        W_D(0x00);
		//delay1(1);

        W_C(0xbf);
        for(i=0;i<PCS;i++){W_D(*(Data1+i+1));}
}




void block(void)
{
W_C(0x2A);
W_D(0x00);
W_D(0x00);
W_D(0x01);
W_D(0xdf); //013f=320 01df=480 031f=800

W_C(0x2B);
W_D(0x00);
W_D(0x00);
W_D(0x03);
W_D(0x1f);   //01DF=479 */
W_C(0x2C); //display on
}

void Delay(unsigned int s)
{
    unsigned int a,b;
     for(a=0;a<s;a++)
         for(b=0;b<10000;b++);
}

void Delayus(unsigned int s)
{
    unsigned int a,b;
     for(a=0;a<s;a++)
         for(b=0;b<50;b++);
}

void Delayus1(unsigned int s)
{
    unsigned int a,b;
     for(a=0;a<s;a++)
         for(b=0;b<2;b++);
}


void waitkey(void)
{
    unsigned int a;
    a=key_rd();
    while(a==1)
    {
        a=key_rd();
        Delay(100);
    }
}
void W_C(int data)
{

// RS=0
   int i;
   CS=1;
  SDI=1;
   SCLK=1;

   Delayus1(5);

   CS=0;
   Delayus1(5);
   SDI=0;//BIT8=0 COMM
   Delayus1(5);
   SCLK=0;
   Delayus1(5);
   SCLK=1;//

   for(i=0;i<8;i++)
   {

   if (data & 0x80)
      SDI=1;
   else
      {SDI=0;}

    data<<= 1;
    SCLK=0;
    Delayus1(5);
    SCLK=1;
    Delayus1(5);

    }
  CS=1;
}



void W_D(int data)
{

//RS=1
   int i;
   CS=1;
  SDI=1;
   SCLK=1;
   Delayus1(5);

  CS=0;
  Delayus1(5);
   SDI=1;//BIT8=1 DATA
   Delayus1(5);
   SCLK=0;
   Delayus1(5);
   SCLK=1;   //

   for(i=0;i<8;i++)
   {

     if (data & 0x80)
   SDI=1;
    else
     {SDI=0;}
    data  <<= 1;
  SCLK=0;
  Delayus1(5);
    SCLK=1;
    Delayus1(5);

   }
   CS=1 ;
 }

void W_C1(int data)
{

// RS=0
   int i;
   CS=1;
  SDI=1;
   SCLK=1;

   Delayus1(10);

   CS=0;
   Delayus1(10);
   SDI=0;//BIT8=0 COMM
   Delayus1(10);
   SCLK=0;
   Delayus1(10);
   SCLK=1;//

   for(i=0;i<8;i++)
   {

   if (data & 0x80)
      SDI=1;
   else
      {SDI=0;}

    data<<= 1;
    SCLK=0;
    Delayus1(10);
    SCLK=1;
   Delayus1(10);

    }
  //CS=1;
}

//TFT LCD单个象素的显示数据输出 0-479 0-799
void PutPixel(int x,int y, int colour )
{
    long i;
    i=(y*HSIZE+x)*4;

    IOWR_32DIRECT(( long)buffer, i, colour);
}


//TFT LCD指定区域输出 0-479 0-799
void area(int xs,int xe,int ys,int ye,int colour)
{
    int i,j,m,n,k;
        m=ye-ys+1;
        n=xe-xs+1;

        for (i = 0; i < m; i++)
          for (j = 0; j < n; j++)
{
        k=((ys+i)*HSIZE+xs)*4+j*4;
     IOWR_32DIRECT(( long)buffer, k, colour);

}
}
 //       TFT LCD指定区域输出 0-479 0-799


//黑色加白色边框
void side(void)
{
int i,j;
for (i = 0; i < HSIZE*4; i += 4)
     IOWR_32DIRECT(( long)buffer, i, WHITE);

for (j = 0; j < (VSIZE-2); j++)
{
    IOWR_32DIRECT(( long)buffer, HSIZE*(j+1)*4, RED);

    for (i = 0; i < (HSIZE-2); i++)
    IOWR_32DIRECT(( long)buffer, (HSIZE*(j+1)*4+4*i+4), BLACK);

    IOWR_32DIRECT(( long)buffer, (HSIZE*(j+2)*4-4), BLUE);

}

for (i = FRAME_SIZE-HSIZE*4; i < FRAME_SIZE; i += 4)
     IOWR_32DIRECT(( long)buffer, i, WHITE);
}


//32灰阶

void GRAY_32(void)
{
    int i;
for(i=0;i<32;i++)
area(0,(HSIZE-1),(VSIZE/32)*i,(i+1)*(VSIZE/32),(i<<19)|(i<<11)|(i<<3));

}

////八色 红绿蓝青黄品红黑白-横显
//void show_color_h (void)
//{
//int k=VSIZE/8;
//
//area(0,HSIZE-1,0,k-1,RED);
//area(0,HSIZE-1,k,k*2-1,GREEN);
//area(0,HSIZE-1,k*2,k*3-1,BLUE);
//area(0,HSIZE-1,k*3,k*4-1,QING);
//area(0,HSIZE-1,k*4,k*5-1,YELLOW);
//area(0,HSIZE-1,k*5,k*6-1,FEN);
//area(0,HSIZE-1,k*6,k*7-1,BLACK);
//area(0,HSIZE-1,k*7,k*8-1,WHITE);
//
//}


////八色 红绿蓝青黄品红黑白-显
void show_color_v (void)
{
int i,k;
area(0,HSIZE-1,0,VSIZE-1,RED);
area(500,HSIZE-1,300,400,WHITE);
area(0,200,700,800,BLUE);
//area(300,400-1,0,VSIZE,QING);
//area(400,500-1,0,VSIZE,YELLOW);
//area(500,600-1,0,VSIZE,FEN);
//area(600,700-1,0,VSIZE,BLACK);
//area(700,800-1,0,VSIZE,GREEN);
//for(i=0;i<VSIZE;i++){
//for(k=0;k<HSIZE/8;k++)IOWR_32DIRECT(( long)buffer+i*k*4, HSIZE/2,RED);
//for(k=HSIZE*1/8;k<HSIZE*2/8;k++)IOWR_32DIRECT(( long)buffer+HSIZE/2+i*k*4, HSIZE/2,GREEN);
//for(k=HSIZE*2/8;k<HSIZE*3/8;k++)IOWR_32DIRECT(( long)buffer+HSIZE/2*2+i*k*4, HSIZE/2,BLUE);
//for(k=HSIZE*3/8;k<HSIZE*4/8;k++)IOWR_32DIRECT(( long)buffer+HSIZE/2*3+i*k*4, HSIZE/2,QING);
//for(k=HSIZE*4/8;k<HSIZE*5/8;k++)IOWR_32DIRECT(( long)buffer+HSIZE/2*4+i*k*4, HSIZE/2,YELLOW);
//for(k=HSIZE*5/8;k<HSIZE*6/8;k++)IOWR_32DIRECT(( long)buffer+HSIZE/2*5+i*k*4, HSIZE/2,FEN);
//for(k=HSIZE*6/8;k<HSIZE*7/8;k++)IOWR_32DIRECT(( long)buffer+HSIZE/2*6+i*k*4, HSIZE/2,BLACK);
//for(k=HSIZE*7/8;k<HSIZE*8/8;k++)IOWR_32DIRECT(( long)buffer+HSIZE/2*7+i*k*4, HSIZE/2,WHITE);
//}

}
//八色 红绿蓝青黄品红黑白-横显
void show_color_h (void)
{
int k=VSIZE/8;

area(0,HSIZE-1,0,k-1,RED);
area(0,HSIZE-1,k,k*2-1,GREEN);
area(0,HSIZE-1,k*2,k*3-1,BLUE);
area(0,HSIZE-1,k*3,k*4-1,QING);
area(0,HSIZE-1,k*4,k*5-1,YELLOW);
area(0,HSIZE-1,k*5,k*6-1,FEN);
area(0,HSIZE-1,k*6,k*7-1,BLACK);
area(0,HSIZE-1,k*7,k*8-1,WHITE);

}
void  rgb_init(void)
 {

	 IOWR(LCD_CON_0_BASE,0,( long)buffer);
	 IOWR(LCD_CON_0_BASE,2,VSIZE<<16  | HSIZE);

	 IOWR(LCD_CON_0_BASE,3,  HBPD<<20 | HFPD<<8  | HSPW);
	 IOWR(LCD_CON_0_BASE,4,  VBPD<<20 | VFPD<<8  | VSPW);
	 IOWR(LCD_CON_0_BASE,5,   N);

	 IOWR(LCD_CON_0_BASE,1,0x00000001);
	 }


void fill_frame ( long base,  long length,  long colour)
{
  int i;
  for (i = 0; i <length; i += 4)
  {
    IOWR_32DIRECT(base, i, colour);
    //nop();

  }
}



void fill_picture (unsigned long base)
{
  unsigned int i;
  unsigned int j=0;
  unsigned int color;
  for (i = 0; i <HSIZE*VSIZE*4; i += 4)
  { color =(pic[j+2])|(pic[j+1]<<8)|(pic[j]<<16);
  j=j+3;
    IOWR_32DIRECT(base, i, color);
    //nop();
  }
}



