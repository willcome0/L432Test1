
#ifndef __oled_H
#define __oled_H
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32l4xx_hal.h"
#include "common.h"

#define ZH_SIZE_14	14

#define ZH_FONT14_NUM       50
#define ZH_FONT14_HEX_NUM   28

#define EN_FONT14_HEX_NUM   14


#define ZH_FONT16_NUM       100
#define ZH_FONT16_HEX_NUM   32

#define EN_FONT16_HEX_NUM   16

#define	OLED_X_MAX		88				// 定义液晶x轴的像素数
#define	OLED_Y_MAX		128 				// 定义液晶y轴的像素数
#define	PAGE_MAX		OLED_Y_MAX/8	// 页地址
extern uint8_t OLED_GRAM[OLED_X_MAX][PAGE_MAX];

#define I2C_Device		(&hi2c1)
#define OLED_Address	(0x78)
#define OLED_MODE	0
#define SIZE		8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF
#define X_WIDTH 	128
#define Y_WIDTH 	64
//-----------------OLED IIC端口定义----------------



enum CmdDataSlect
{
    OLED_CMD  = 0x00,
    OLED_DATA = 0x40
};
//#define OLED_CMD  0	// 写命令
//#define OLED_DATA 1	// 写数据

void OLED_Refresh_Gram(void);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t mode);
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode);
void OLED_Show_Char(uint8_t x, uint8_t y, char chr, uint8_t size, uint8_t mode);
void OLED_Show_Str(uint8_t x, uint8_t y, char *ch, uint8_t size, uint8_t mode);
void OLED_Show_ZH(uint8_t x, uint8_t y, uint8_t fontSize, char ch1, char ch2, uint8_t mode);
void OLED_Show_StrAll(uint8_t x, uint8_t y, uint8_t fontSize, char *str, uint8_t mode);

void IIC_Start();
void IIC_Stop();
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char IIC_Byte);
void IIC_Wait_Ack();

void OLED_WR_Byte(unsigned dat, unsigned cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Init(void);
void OLED_Clear(void);

//void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);
//void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot);
//void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size);
//void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);
//void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *p, uint8_t Char_Size);
void OLED_Set_Pos(unsigned char x, unsigned char y);
//void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no);
//void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);
//void Delay_50ms(unsigned int Del_50ms);
//void Delay_1ms(unsigned int Del_1ms);
//void fill_picture(unsigned char fill_Data);
//void Picture();
void OLED_Write(uint8_t data, enum CmdDataSlect state);

extern const unsigned char F6x8[92][6];
extern const unsigned char F8X16[1520];

extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];
extern const unsigned char asc2_2412[95][36];

extern const char IMG64[10][512];
extern const char IMG48[10][288];






#ifdef __cplusplus
}
#endif
#endif /*__ oled_H */

