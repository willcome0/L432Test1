
#ifndef __flash_H
#define __flash_H
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32l4xx_hal.h"
#include "common.h"

/*
    L432KB FLASH　128KB。
    L432KC FLASH　256KB。

    一个扇区有2KB，即2048B。可存2048个u8型或256个u64型。

    记得设置代码区的空间，不要被覆盖。

    WillKang 2018.11.14
*/    
    
#define WK_FLASH_SIZE          128                                         // flash大小（KB）
#define WK_FLASH_PAGE_SIZE     2                                           // 扇页大小（KB）
#define WK_FLASH_PAGE_NUM      (WK_FLASH_SIZE/WK_FLASH_PAGE_SIZE)                // 扇页数量

#define WK_FLASH_DATA_ADDR     (FLASH_BASE+(WK_FLASH_SIZE-WK_FLASH_PAGE_SIZE)*1024)  // 数据区的起始地址

    
    
/* 断言 */
    
#define IS_FLASH_DATA_AREA(VALUE)       (((VALUE) >= 0)                         || \
                                         ((VALUE) <= WK_FLASH_PAGE_SIZE*1024-8))


    
void WK_Flash_DataWrite(uint16_t offsetAddr, uint64_t writeDate);
uint32_t WK_Flash_DataRead(uint16_t offsetAddr);


    
    
#ifdef __cplusplus
}
#endif
#endif /*__ mpu9250_H */

