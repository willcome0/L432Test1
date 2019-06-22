
#ifndef __flash_H
#define __flash_H
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32l4xx_hal.h"
#include "common.h"

/*
    L432KB FLASH��128KB��
    L432KC FLASH��256KB��

    һ��������2KB����2048B���ɴ�2048��u8�ͻ�256��u64�͡�

    �ǵ����ô������Ŀռ䣬��Ҫ�����ǡ�

    WillKang 2018.11.14
*/    
    
#define WK_FLASH_SIZE          128                                         // flash��С��KB��
#define WK_FLASH_PAGE_SIZE     2                                           // ��ҳ��С��KB��
#define WK_FLASH_PAGE_NUM      (WK_FLASH_SIZE/WK_FLASH_PAGE_SIZE)                // ��ҳ����

#define WK_FLASH_DATA_ADDR     (FLASH_BASE+(WK_FLASH_SIZE-WK_FLASH_PAGE_SIZE)*1024)  // ����������ʼ��ַ

    
    
/* ���� */
    
#define IS_FLASH_DATA_AREA(VALUE)       (((VALUE) >= 0)                         || \
                                         ((VALUE) <= WK_FLASH_PAGE_SIZE*1024-8))


    
void WK_Flash_DataWrite(uint16_t offsetAddr, uint64_t writeDate);
uint32_t WK_Flash_DataRead(uint16_t offsetAddr);


    
    
#ifdef __cplusplus
}
#endif
#endif /*__ mpu9250_H */

