#include "oled.h"
#include "i2c.h"
#include <string.h>



uint8_t OLED_GRAM[OLED_X_MAX][PAGE_MAX];    // �Դ�����



void OLED_Write(uint8_t data, enum CmdDataSlect reg_add)
{
    HAL_I2C_Mem_Write(I2C_Device, OLED_Address, reg_add, 1, &data, 1, 0xFFFF);
}





/**
  * @brief  ��ʾ�Դ�
  * @param  None
  * @retval None
  */
void OLED_Refresh_Gram(void)
{
    for (uint8_t i=0; i<PAGE_MAX; i++)
    {
        OLED_Write (0xB0 + i, OLED_CMD);    //����ҳ��ַ��0~11��
        OLED_Write (0x00, OLED_CMD);        //������ʾλ�á��е͵�ַ
        OLED_Write (0x10, OLED_CMD);        //������ʾλ�á��иߵ�ַ
        for (uint8_t n=0; n<OLED_X_MAX; n++)
        {
            OLED_Write(OLED_GRAM[n][i], OLED_DATA);
        }
    }
}


/**
  * @brief  ����
  * @param  x: ���Ͻǵ�һ�����x������
  * @param  y: ���Ͻǵ�һ�����y������
  * @param  pic: ͼƬָ��
  * @param  mode: ��ʾģʽ
  * @retval None
  * @author Kang
  * @ver    1.0
  */
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t mode)
{
    if (x >= OLED_X_MAX || y >= OLED_Y_MAX)
        return;
    
    uint8_t pos = y / 8;
    uint8_t bx = y % 8;
    uint8_t temp = 0x01 << bx;	// ��λ��ǰ
    
    if (mode)
        OLED_GRAM[x][pos] |= temp;	// ��Ӧ��1���಻�䣬Ϊ1����λ����
    else
        OLED_GRAM[x][pos] &= ~temp;	// ��Ӧ��0���಻��
}


/**
  * @brief  ���
  * @param  x1: ���Ͻ�x������
  * @param  y1: ���Ͻ�y������
  * @param  x1: ���½�x������
  * @param  y2: ���½�y������
  * @param  mode: ��ʾģʽ
  * @retval None
  * @author Kang
  * @ver    1.0
  */
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode)
{
    for (uint8_t x=x1; x<=x2; x++)
        for (uint8_t y=y1; y<=y2; y++)
            OLED_DrawPoint(x, y, mode);
}


/**
  * @brief  �����Դ�
  * @param  None
  * @retval None
  */
void OLED_Clear(void)
{
    memset(OLED_GRAM, 0x00, OLED_X_MAX*PAGE_MAX);
}


/**
  * @brief  ��ʾ����
  * @param  x: ���Ͻ�x������
  * @param  y: ���Ͻ�y������
  * @param  font_size: �����С
  * @param  data: ����
  * @param  mode: ��ʾģʽ
  * @retval None
  * @author Kang
  * @time   2018.9.8
  * @ver    1.0
  */
void OLED_Show_Num(uint8_t x, uint8_t y, uint8_t font_size, char data, uint8_t mode)
{
    uint8_t font_dat;
    
    char *zh_font;


    switch (font_size)
    {
        case 32:
            zh_font = NUM_FONT32;
            break;
            
        case 48:
            zh_font = NUM_FONT48;
            break;
            
        case 64:
            zh_font = NUM_FONT64;
            break;
            
        default:
            return;
    }

    for (uint8_t i=0; i<font_size/8; i++)    // ������
    {
        for (uint8_t j=0; j<font_size/2; j++)                      // ������
        {
            font_dat = zh_font[(data-'0')*font_size*font_size/2/8 + i*font_size/2+j];

            for (uint8_t k=0; k<8; k++)
            {
                OLED_DrawPoint(x+j, y+8*i+k, (font_dat&0x01)==0?!mode:mode);
                font_dat >>= 1;
            }

        }
    }
}


/**
  * @brief  ��ʾ�ַ�
  * @param  x: ���Ͻ�x������
  * @param  y: ���Ͻ�y������
  * @param  font_size: �����С
  * @param  ch: �ַ�
  * @param  mode: ��ʾģʽ
  * @retval None
  * @author Kang
  * @time   2018.9.8
  * @ver    1.0
  */
void OLED_Show_EN(uint8_t x, uint8_t y, uint8_t font_size, char ch, uint8_t mode)
{
    uint8_t font_dat;

    for (uint8_t i=0; i<font_size/8+(font_size%8!=0?1:0); i++)    // ����
    {
        for (uint8_t j=0; j<font_size/2; j++)                    // ����
        {
            switch (font_size)
            {
                case 14:
                    font_dat = EN_FONT14[(ch-' ')*EN_FONT14_HEX_NUM+i*font_size/2+j];
                    break;
                case 16:
                    font_dat = EN_FONT16[(ch-' ')*EN_FONT16_HEX_NUM+i*font_size/2+j];
                    break;
                    
                default:
                    return;
            }

            for (uint8_t k=0; k<8; k++)
            {
                OLED_DrawPoint(x+j, y+8*i+k, (font_dat&0x01)==0?!mode:mode);
                font_dat >>= 1;
            }
        }
    }
}


/**
  * @brief  ��ʾ���֣����ᵥ��ʹ��
  * @param  x: ���Ͻ�x������
  * @param  y: ���Ͻ�y������
  * @param  font_size: �����С
  * @param  ch1: ���ֵ��ֽ�λ
  * @param  ch2: ���ָ��ֽ�λ
  * @param  mode: ��ʾģʽ
  * @retval None
  * @author Kang
  * @time   2018.9.8
  * @ver    1.0
  */
void OLED_Show_ZH(uint8_t x, uint8_t y, uint8_t font_size, char ch1, char ch2, uint8_t mode)
{
    uint8_t font_dat;

    uint16_t zh_num;
    uint16_t zh_hex_num;
    char *zh_font_tab;
    char *zh_font;

    switch (font_size)
    {
        case 14:
            zh_num = ZH_FONT14_NUM;
            zh_hex_num = ZH_FONT14_HEX_NUM;
            zh_font_tab = ZH_FONT14_TAB;
            zh_font = ZH_FONT14;
            break;

        case 16:
            zh_num = ZH_FONT16_NUM;
            zh_hex_num = ZH_FONT16_HEX_NUM;
            zh_font_tab = ZH_FONT16_TAB;
            zh_font = ZH_FONT16;
            break;

        default:
            return;
    }


    for (uint8_t i=0; i<zh_num*2; i+=2) //һ���������ֿ�
    {
        //��ͬʱ�жϺ��ֵĵ��ֽں͸��ֽ�
        if (zh_font_tab[i]==ch1 && zh_font_tab[i+1]==ch2)
        {
            ch1 = i/2;
            break;
        }
    }

    for (uint8_t i=0; i<font_size/8+(font_size%8!=0?1:0); i++)    // ������
    {
        for (uint8_t j=0; j<font_size; j++)                      // ������
        {
            font_dat = zh_font[ch1*zh_hex_num+i*font_size+j];

            for (uint8_t k=0; k<8; k++)
            {
                OLED_DrawPoint(x+j, y+8*i+k, (font_dat&0x01)==0?!mode:mode);
                font_dat >>= 1;
            }
        }
    }
}


/**
  * @brief  ��ʾͼƬ
  * @param  x: ���Ͻǵ�һ�����x������
  * @param  y: ���Ͻǵ�һ�����y������
  * @param  pic: ͼƬָ��
  * @param  x_num: ��
  * @param  y_num: ��
  * @param  mode: ��ʾģʽ
  * @retval None
  * @author Kang
  * @time   2018.9.8
  * @ver    1.0
  */
void  OLED_Show_Pic(uint8_t x, uint8_t y, char *pic, uint8_t x_num, uint8_t y_num, uint8_t mode)
{
    uint8_t font_dat;
    
    for (uint8_t i=0; i<y_num/8; i++)
    {
        for (uint8_t j=0; j<x_num; j++)
        {
            font_dat = pic[x_num*i+j];
            for (uint8_t k=0; k<8; k++)
            {
                OLED_DrawPoint(x+j, y+8*i+k, (font_dat&0x01)==0?!mode:mode);
                font_dat >>= 1;
            }

        }
    }
}


/**
  * @brief  ��ʾ�ַ��ͺ���
  * @param  x: ���Ͻ�x������
  * @param  y: ���Ͻ�y������
  * @param  font_size: �����С
  * @param  str: �ַ���ָ��
  * @param  mode: ��ʾģʽ
  * @retval None
  * @author Kang
  * @time   2018.9.8
  * @ver    1.0
  */
void OLED_Show_StrAll(uint8_t x, uint8_t y, uint8_t font_size, char *str, uint8_t mode)
{
    while (*str != 0)
    {       
        if ((*str<='~')&&(*str>=' '))
        {
            OLED_Show_EN(x, y, font_size, *str, mode);
            x += font_size/2;
            str++;
        }
        else
        {
            OLED_Show_ZH(x, y, font_size, *str, *(str+1), mode);
            x += font_size;
            str += 2;
        }
    }
}




//void OLED_Set_Pos(uint8_t x, uint8_t y)
//{
//    OLED_Write(0xB0 + y, OLED_CMD);
//    OLED_Write(((x & 0xF0) >> 4) | 0x10, OLED_CMD);
//    OLED_Write((x & 0x0F), OLED_CMD);
//}

/**
  * @brief  ����OLED��ʾ
  * @param  None
  * @retval None
  */
void OLED_Display_On(void)
{
    OLED_Write(0X8D, OLED_CMD);  //SET DCDC����
    OLED_Write(0X14, OLED_CMD);  //DCDC ON
    OLED_Write(0XAF, OLED_CMD);  //DISPLAY ON
}


/**
  * @brief  �ر�OLED��ʾ
  * @param  None
  * @retval None
  */
void OLED_Display_Off(void)
{
    OLED_Write(0X8D, OLED_CMD);  //SET DCDC����
    OLED_Write(0X10, OLED_CMD);  //DCDC OFF
    OLED_Write(0XAE, OLED_CMD);  //DISPLAY OFF
}


/**
  * @brief  OLED��ʼ��
  * @param  None
  * @retval None
  */
void OLED_Init(void)
{
    OLED_Write(0xAE, OLED_CMD);////Display OFF
    
    OLED_Write(0x00, OLED_CMD);//---set low column address      1
    OLED_Write(0x10, OLED_CMD);//---set high column address     2
    OLED_Write(0xB0, OLED_CMD);//set page address               3
    OLED_Write(0xD5, OLED_CMD); // Set Dclk                     4n
    OLED_Write(0xB1, OLED_CMD);     //                          5       //��������Ƶ��
    OLED_Write(0x20, OLED_CMD);// Set row address               6       //
    OLED_Write(0x81, OLED_CMD);// Set contrast control///       7    
    OLED_Write((uint8_t)WK_Flash_DataRead(0)*25, OLED_CMD);//--set normal display           8       // ����
    OLED_Write(0xA0, OLED_CMD);// Segment remap                 9
    OLED_Write(0xA4, OLED_CMD);// Set Entire Display ON         10  
    OLED_Write(0xA6, OLED_CMD);// Normal display                11  // A6 ��ת A7
    OLED_Write(0xA8, OLED_CMD);//set duty                       12z
    OLED_Write(0x57, OLED_CMD);//                               13
    OLED_Write(0xDC, OLED_CMD);//set display start line
    OLED_Write(0x00, OLED_CMD);
    OLED_Write(0xD3, OLED_CMD);//set offset
    OLED_Write(0x6C, OLED_CMD);
    OLED_Write(0xAD, OLED_CMD);// Set external VPP
    OLED_Write(0x80, OLED_CMD);//Set VCOM Deselect Level
    OLED_Write(0xC0, OLED_CMD);// Set Common scan direction
    OLED_Write(0xD9, OLED_CMD);//Set phase leghth
    OLED_Write(0x22, OLED_CMD);
    OLED_Write(0xDB, OLED_CMD);// Set Vcomh voltage
    OLED_Write(0x3F, OLED_CMD);//0.834xvpp
    OLED_Write(0xAF, OLED_CMD);//--turn on oled panel /*display ON*/

    OLED_Clear();
}




