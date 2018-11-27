#include "flash.h"


void WK_Flash_DataWrite(uint16_t offsetAddr, uint64_t writeDate)
{
    assert_param(IS_FLASH_DATA_AREA(offsetAddr));
    
    HAL_FLASH_Unlock(); // 解锁

    FLASH_EraseInitTypeDef f;

    f.TypeErase = FLASH_TYPEERASE_PAGES;
    f.Page = WK_FLASH_PAGE_NUM-1;      // 选择最后一个扇页
    f.NbPages = 1;                  // 只擦除一页
    
    uint32_t PageError = 0;
    HAL_FLASHEx_Erase(&f, &PageError);  // 擦除
    
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, WK_FLASH_DATA_ADDR+offsetAddr, writeDate); // 写入

    HAL_FLASH_Lock();   // 锁住
}

uint32_t WK_Flash_DataRead(uint16_t offsetAddr)
{
    assert_param(IS_FLASH_DATA_AREA(offsetAddr));
    
    return *(__IO uint32_t*)(WK_FLASH_DATA_ADDR+offsetAddr);
}

