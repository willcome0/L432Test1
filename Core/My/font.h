
#ifndef __font_H
#define __font_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
    
    

    
   

    
    
/* Êý×Ö */
extern uint8_t NUM_FONT32[10*64];
extern uint8_t NUM_FONT48[10*144];
extern uint8_t NUM_FONT64[10*256];

/* ×Ö·û */
#define EN_FONT14_HEX_NUM   14
#define EN_FONT16_HEX_NUM   16

extern uint8_t EN_FONT14[95*EN_FONT14_HEX_NUM];
extern uint8_t EN_FONT16[95*EN_FONT16_HEX_NUM];

/* ºº×Ö */

#define ZH_FONT14_NUM       50
#define ZH_FONT14_HEX_NUM   28

#define ZH_FONT16_NUM       100
#define ZH_FONT16_HEX_NUM   32

extern char ZH_FONT14_TAB[ZH_FONT14_NUM];
extern char ZH_FONT14[ZH_FONT14_NUM*ZH_FONT14_HEX_NUM];

extern char ZH_FONT16_TAB[ZH_FONT16_NUM];
extern char ZH_FONT16[ZH_FONT16_NUM*ZH_FONT16_HEX_NUM];

/* Í¼Æ¬ */
extern const uint8_t IMG64[10][512];
extern const uint8_t IMG48[10][288];

#ifdef __cplusplus
}
#endif
#endif /*__ font_H */



