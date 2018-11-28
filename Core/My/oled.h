
#ifndef __oled_H
#define __oled_H
#ifdef __cplusplus
extern "C" {
#endif

    
#include "font.h"

    
#define I2C_Device		(&hi2c1)
#define OLED_Address	(0x78)
    
#define	OLED_X_MAX		88				// 定义液晶x轴的像素数
#define	OLED_Y_MAX		128             // 定义液晶y轴的像素数
#define	PAGE_MAX		OLED_Y_MAX/8	// 页地址



enum CmdDataSlect
{
    OLED_CMD  = 0x00,
    OLED_DATA = 0x40
};

void OLED_Refresh_Gram(void);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t mode);
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode);
void OLED_Clear(void);

void OLED_Show_Num(uint8_t x, uint8_t y, uint8_t font_size, char data, uint8_t mode);
void OLED_Show_EN(uint8_t x, uint8_t y, uint8_t font_size, char ch, uint8_t mode);
void OLED_Show_ZH(uint8_t x, uint8_t y, uint8_t font_size, char ch1, char ch2, uint8_t mode);
void OLED_Show_StrAll(uint8_t x, uint8_t y, uint8_t font_size, char *str, uint8_t mode);
void  OLED_Show_Pic(uint8_t x, uint8_t y, char *pic, uint8_t x_num, uint8_t y_num, uint8_t mode);   

void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Init(void);


#ifdef __cplusplus
}
#endif

#endif /*__ oled_H */

