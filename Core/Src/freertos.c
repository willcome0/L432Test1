/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "stm32l4xx_hal.h"
#include "oled.h"
#include "rtc.h"
#include "snake.h"
#include "mpu9250.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
volatile uint8_t KeyValue = 0;

    /* 任务句柄定义 */
void ShowPose(void);
void AdjustBrightness(void);
void AdjustTime(void);

int16_t g_x, g_y, g_z, a_x, a_y, a_z;
osThreadId readKey_Handle;
osThreadId showUI_Handle;
osThreadId showTime_Handle;
osThreadId poseControl_Handle;

osThreadId snakeTimer_Handle;


    /* 信号量定义 */
osSemaphoreId keyPressed_BinSemHandle;
osSemaphoreId snake_BinSemHandle;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

    /* 任务定义 */
void readKey_Task(void const * argument);
void showUI_Task(void const * argument);
void showTime_Task(void const * argument);
void poseControl_Task(void const * argument);

void snakeTimer_Task(void const * argument);



/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
    return 0;
}
/* USER CODE END 1 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 32);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    osThreadDef(readKey,        readKey_Task,       osPriorityNormal,       0, 64);
    osThreadDef(showUI,         showUI_Task,        osPriorityNormal,       0, 160);
    osThreadDef(poseControl,    poseControl_Task,   osPriorityAboveNormal,       0, 80);
    
    osThreadDef(snakeTimer, snakeTimer_Task, osPriorityAboveNormal, 0, 128);
    
    osSemaphoreDef(keyPressed_BinSem);
    osSemaphoreDef(snake_BinSem);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    readKey_Handle      = osThreadCreate(osThread(readKey),     NULL);
    showUI_Handle       = osThreadCreate(osThread(showUI),      NULL);
    poseControl_Handle  = osThreadCreate(osThread(poseControl), NULL);
    
    snakeTimer_Handle = osThreadCreate(osThread(snakeTimer), NULL);
//    showTime_Handle = osThreadCreate(osThread(showTime),    NULL);
    osThreadSuspend(snakeTimer_Handle);
    
    keyPressed_BinSemHandle =   osSemaphoreCreate(osSemaphore(keyPressed_BinSem), 1);
    snake_BinSemHandle =        osSemaphoreCreate(osSemaphore(snake_BinSem), 1);
    
  /* USER CODE END RTOS_QUEUES */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    for (;;)
    {
        osDelay(500);
//        printf("中文\r\n");
//        HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
    }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void readKey_Task(void const * argument)
{
    for (;;)
    {
        if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port,      KEY_UP_Pin)     == GPIO_PIN_RESET ||
            HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port,    KEY_DOWN_Pin)   == GPIO_PIN_RESET ||
            HAL_GPIO_ReadPin(KEY_MID_GPIO_Port,     KEY_MID_Pin)    == GPIO_PIN_RESET ||
            HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port,    KEY_LEFT_Pin)   == GPIO_PIN_RESET ||
            HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port,   KEY_RIGHT_Pin)  == GPIO_PIN_RESET)
        {
            osDelay(10);
            if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
            {
                HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
                KeyValue = PressUp;
                
                
            }
            else if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
            {
                HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
                KeyValue = PressDown;
                
                
            }
            else if (HAL_GPIO_ReadPin(KEY_MID_GPIO_Port, KEY_MID_Pin) == GPIO_PIN_RESET)
            {
                HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
                KeyValue = PressMid;
                
                
            }
            else if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
            {
                HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
                KeyValue = PressLeft;
                
                
            }
            else if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
            {
                HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
                KeyValue = PressRight;
                
                
            }
            else
            {
                KeyValue = PressNull;
            }
            
            osSemaphoreRelease(keyPressed_BinSemHandle);
            while  (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port,      KEY_UP_Pin)     == GPIO_PIN_RESET ||
                    HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port,    KEY_DOWN_Pin)   == GPIO_PIN_RESET ||
                    HAL_GPIO_ReadPin(KEY_MID_GPIO_Port,     KEY_MID_Pin)    == GPIO_PIN_RESET ||
                    HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port,    KEY_LEFT_Pin)   == GPIO_PIN_RESET ||
                    HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port,   KEY_RIGHT_Pin)  == GPIO_PIN_RESET);
        }
        else
        {
            KeyValue = PressNull;
        }
    }
}

void showUI_Task(void const * argument)
{
    
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    
    int8_t uiCase = -1;
    
    for (;;)
    {
        osDelay(30);
        osSemaphoreWait(keyPressed_BinSemHandle, osWaitForever);
        switch (KeyValue)
        {
            case PressUp:
            {
                if (uiCase<0)
                {
                    uiCase--;
                }
                else if (uiCase<10)
                {
                    uiCase--;
                    uiCase = uiCase<1 ? 5:uiCase;
                }
                else if (uiCase<100)
                { 
                    if (uiCase<20)
                        uiCase = 10;
                    else if (uiCase<30)
                        uiCase = 20;
                    else if (uiCase<40)
                    {
                        if (uiCase>30)
                            uiCase--;
                    }
                    else if (uiCase<50)
                    {
                        if (uiCase>40)
                            uiCase--;
                    }
                    else if (uiCase<60)
                    {
                        if (uiCase>50)
                            uiCase--;
                    }
                }
                
                break;
            }
            
            case PressDown:
            {
                if (uiCase<0)
                {
                    uiCase++;
                }
                else if (uiCase<10)
                {
                    uiCase++;
                    uiCase = uiCase>5 ? 1:uiCase;
                }
                else if (uiCase<100)
                {
                    if (uiCase<20)
                        uiCase = 10;
                    else if (uiCase<30)
                        uiCase = 20;
                    else if (uiCase<40)
                    {
                        if (uiCase<31)
                            uiCase++;
                    }
                    else if (uiCase<50)
                    {
                        if (uiCase<41)
                            uiCase++;
                    }
                    else if (uiCase<60)
                    {
                        if (uiCase<53)
                            uiCase++;
                    }
                }
                
                break;
                
            }
                
            case PressMid:
            {
                switch (uiCase)
                {
                    case 1: case 2: case 3: case 4: case 5:
                        uiCase *= 10;
                        break;
                    
                    case 30:
                        ShowPose();
                        break;
                        
                    case 40:
                        Snake_Start();
                        break;
                    
                    case 50:
                        AdjustBrightness();
                        break;
                    
                    case 51:
                        AdjustTime();
                        break;
                }
                break;
            }
                
            case PressLeft:
            {
                if (uiCase<0)
                {
                    
                }
                else if (uiCase<10)
                {
                    uiCase = -uiCase;
                    
                }
                else if (uiCase<100)
                {
//                    if (uiCase%10==0)
                        uiCase /= 10;
//                    else
//                        uiCase--;
                }
                break;
            }
                
            case PressRight:
            {
                if (uiCase<0)
                {
                    uiCase = 1;
                }
                else if (uiCase<10)
                {
                    uiCase *= 10;
                }
                else if (uiCase<100)
                {
//                    uiCase++;
                }
                break;
            }
        }
        
        OLED_Clear();
        if (uiCase < 0)
        {
            UI_ShowTime(-uiCase);
            uiCase = 1;
        }
        
        switch (uiCase)
        {

            case 1:
                OLED_Show_StrAll(24, 95, 16, "拨 号", 1);
                OLED_Show_Pic(11, 25, IMG64[uiCase-1], 64, 64, 1);
                break;
            case 2:
                OLED_Show_StrAll(24, 95, 16, "影 音", 1);
                OLED_Show_Pic(11, 25, IMG64[uiCase-1], 64, 64, 1);
                break;
            case 3:
                OLED_Show_StrAll(24, 95, 16, "设 备", 1);
                OLED_Show_Pic(11, 25, IMG64[uiCase-1], 64, 64, 1);
                break;
            case 4:
                OLED_Show_StrAll(24, 95, 16, "游 戏", 1);
                OLED_Show_Pic(11, 25, IMG64[uiCase-1], 64, 64, 1);
                break;
            case 5:
                OLED_Show_StrAll(24, 95, 16, "设 置", 1);
                OLED_Show_Pic(11, 25, IMG64[uiCase-1], 64, 64, 1);
                break;
            


            case 10: case 20: case 31: case 41: case 53: 
                OLED_Show_Pic(20, 30, IMG48[4], 48, 48, 1);
                OLED_Show_StrAll(12, 95, 16, "待 开 发", 1);
                break;
                
            case 30:
                OLED_Show_StrAll(24, 0, 16, "设 备", 1);
                OLED_Show_Pic(20, 30, IMG48[5], 48, 48, 1);
                OLED_Show_StrAll(24, 95, 16, "姿 态", 1);
                break;
                
            case 40:
                OLED_Show_StrAll(24, 0, 16, "游 戏", 1);
                OLED_Show_Pic(20, 30, IMG48[0], 48, 48, 1);
                OLED_Show_StrAll(12, 95, 16, "贪 吃 蛇", 1);
                break;
                
            case 50:
                OLED_Show_StrAll(24, 0, 16, "设 置", 1);
                OLED_Show_Pic(20, 30, IMG48[1], 48, 48, 1);
                OLED_Show_StrAll(24, 95, 16, "亮 度", 1);
                break;
               
            case 51:
                OLED_Show_StrAll(24, 0, 16, "设 置", 1);
                OLED_Show_Pic(20, 30, IMG48[3], 48, 48, 1);
                OLED_Show_StrAll(24, 95, 16, "时 间", 1);
                break;
                
            case 52:
                OLED_Show_StrAll(24, 0, 16, "设 置", 1);
                OLED_Show_Pic(20, 30, IMG48[2], 48, 48, 1);
                OLED_Show_StrAll(24, 95, 16, "关 于", 1);
                break;
        }

        
        
        
        OLED_Refresh_Gram();
    }
}

void poseControl_Task(void const * argument)
{
    
    uint16_t immobileCount = 0;
    uint8_t immobile = 0;
    for (;;)
    {
        
        HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
        

//
//        if (g_x == 0 &&
//            g_y == 0 &&
//            g_y == 0
//            )
//        {
//            immobileCount++;
//            if (immobileCount > 20 && immobile == 0)
//            {
//                
//                osThreadSuspend(showUI_Handle);
//                OLED_Clear();
//                OLED_Refresh_Gram();
//                immobile = 1;
//            }
//        }
//        else if (immobile == 1)
//        {
//            osThreadResume(showUI_Handle);
//            immobile = 0;
//        }
        osDelay(50);
    }
}


void UI_ShowTime(uint8_t num)
{
    uint8_t i = 0;
    uint16_t immobileCount = 0;
    uint8_t immobile = 0;
    for (;;)
    {
        HAL_RTC_GetTime(&hrtc, &timeStructure, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &dateStructure, RTC_FORMAT_BIN);

        char str[20] = {0};
//        sprintf(str, "%4d.%02d.%02d", 2000 + dateStructure.Year, dateStructure.Month, dateStructure.Date);
//        OLED_Show_StrAll(0, 0, 16, str, 1);
//        sprintf(str, "%02d:%02d:%02d", timeStructure.Hours, timeStructure.Minutes, timeStructure.Seconds);
//        OLED_Show_StrAll(0, 16, 16, str, 1);
        
        OLED_Show_TimeNum(20, 4, 48, '0'+timeStructure.Hours/10, 1);
        OLED_Show_TimeNum(44, 4, 48, '0'+timeStructure.Hours%10, 1);
        
        OLED_Show_TimeNum(20, 46, 48, '0'+timeStructure.Minutes/10, 1);
        OLED_Show_TimeNum(44, 46, 48, '0'+timeStructure.Minutes%10, 1);
        
        sprintf(str, "%02d", timeStructure.Seconds);
        OLED_Show_StrAll(72, 72, 16, str, 1);
        
        char weekDay[2] = {0};
        switch (dateStructure.WeekDay)
        {
            case RTC_WEEKDAY_MONDAY:    sprintf(weekDay, "一"); break;
            case RTC_WEEKDAY_TUESDAY:   sprintf(weekDay, "二"); break;
            case RTC_WEEKDAY_WEDNESDAY: sprintf(weekDay, "三"); break;
            case RTC_WEEKDAY_THURSDAY:  sprintf(weekDay, "四"); break;
            case RTC_WEEKDAY_FRIDAY:    sprintf(weekDay, "五"); break;
            case RTC_WEEKDAY_SATURDAY:  sprintf(weekDay, "六"); break;
            case RTC_WEEKDAY_SUNDAY:    sprintf(weekDay, "日"); break;
        }
        sprintf(str, "%02d/%02d 周%s", dateStructure.Month, dateStructure.Date, weekDay);
        OLED_Show_StrAll(4, 104, 16, str, 1);
//        OLED_Write(0xAE, OLED_CMD);
//        
        i++;
        if (i == 250)
            i = 0;
//        OLED_Write(0xAE, OLED_CMD);

//        OLED_Write(0xAF, OLED_CMD);
        
//        OLED_Write(0xAF, OLED_CMD);
        if (KeyValue == PressRight)
        {
            break;
        }
//        switch (KeyValue)
//        {
//            case PressUp:
//                
//                break;
//                
//            case PressDown:
//                
//                break;
//                
//            case PressMid:
//                
//                break;
//                
//            case PressLeft:
//                
//                break;
//                
//            case PressRight:
//                return;
//                break;
//        }
        
//        char str[20] = {0};
        
        
//        READ_MPU9250_ACCEL();
//        READ_MPU9250_TEMP();
//        sprintf(str, "%4d %4.2f", (int)T_X, T_T);
//        OLED_Show_StrAll(0, 110, 16, str, 1);
        
        READ_MPU9250_GYRO();
        g_x = T_X;
        g_y = T_Y;
        g_z = T_Z; 
        READ_MPU9250_ACCEL();
        a_x = T_X;
        a_y = T_Y;
        a_z = T_Z; 
//        printf("%5d %5d %5d\r\n", g_x, g_y, g_z);
//        printf("%5d %5d %5d\r\n\r\n", a_x, a_y, a_z);
        
        if (
            immobile == 0 &&
            abs(g_x) < 30 &&
            abs(g_y) < 30 &&
            abs(g_z) < 30 
            )
        {
            immobileCount++;
            if (immobileCount > 30 && immobile == 0)
            {
                
//                osThreadSuspend(showUI_Handle);
                OLED_Clear();

                immobile = 1;
            }
        }
        else
        {
            immobileCount = 0;
        }
        
        if (immobile == 0 && (abs(a_x)>75 || abs(a_y) > 75))
        {
            immobile = 1;
        }
        
        if (immobile == 1)
        {
            // 进入睡眠模式
            OLED_Clear();
        }
        
        /* 唤醒 */
        if (immobile == 1 && 
            ((abs(g_x)>20 || abs(g_y)>20 || abs(g_z)>20) && abs(a_x)<50 && abs(a_y)<50) )
        {
//            osThreadResume(showUI_Handle);
            immobile = 0;
            immobileCount = 0;
        }
        OLED_Refresh_Gram();
    }

}

//uint8_t ABS(int8_t num)
//{
//    if (num >= 0)
//    {
//        return num;
//    }
//    else
//    {
//        return -num;
//    }
//}


void ShowPose(void)
{
    int16_t g_x, g_y, g_z, a_x, a_y, a_z;
    float t;
    char str[20] = {0};
    OLED_Clear();
    OLED_Show_StrAll(24, 0, 16, "姿 态", 1);
    for (;;)
    {
        READ_MPU9250_GYRO();
        g_x = T_X;
        g_y = T_Y;
        g_z = T_Z; 
        READ_MPU9250_ACCEL();
        a_x = T_X;
        a_y = T_Y;
        a_z = T_Z; 
//        READ_MPU9250_TEMP();
//        t = (float)T_T;
        
        sprintf(str, "X%5d %5d", g_x, a_x);
        OLED_Show_StrAll(0, 40, 14, str, 1);
        sprintf(str, "Y%5d %5d", g_y, a_y);
        OLED_Show_StrAll(0, 60, 14, str, 1);
        sprintf(str, "Z%5d %5d", g_z, a_z);
        OLED_Show_StrAll(0, 80, 14, str, 1);
        
//        sprintf(str, "Temp: %2.1f", t/10);
//        OLED_Show_StrAll(0, 100, 16, str, 1);
        
        OLED_Refresh_Gram();
        
        if (KeyValue == PressLeft)
        {
            break;
        }
    }
}


void AdjustBrightness(void)
{
    uint8_t Level = WK_Flash_DataRead(0);
    uint8_t levelTemp = Level;
    
    OLED_Clear();
    OLED_Show_StrAll(24, 0, 16, "亮 度", 1);
    for (;;)
    {
        switch (KeyValue)
        {
            case PressUp:
                Level = Level>8 ? 8:Level;
                Level++;
                
                break;
                
            case PressDown:
                Level = Level==0 ? 1:Level;
                Level--;
                
                break;
                
            case PressLeft:
                WK_Flash_DataWrite(0, (uint64_t)Level);
                return;
                
        }
        OLED_Show_TimeNum(32, 30, 64, '0' + Level, 1);
        
        if (levelTemp != Level)
        {
            levelTemp = Level;
            OLED_Write(0x81, OLED_CMD);  
            OLED_Write((uint8_t)Level*25, OLED_CMD);
        }
        
        OLED_Refresh_Gram();
    }

}

void AdjustTime(void)
{
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
                  
    HAL_RTC_GetTime(&hrtc, &timeStructure, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &dateStructure, RTC_FORMAT_BIN);
    
    uint8_t year = dateStructure.Year;
    uint8_t month = dateStructure.Month;
    uint8_t day = dateStructure.Date;
    uint8_t hour = timeStructure.Hours;
    uint8_t min = timeStructure.Minutes;
    uint8_t sec = timeStructure.Seconds;
    
    uint8_t *data;
    
    uint8_t select = 0;
    char str[20] = {0};
    KeyValue = 0;
    for (;;)
    {
        OLED_Clear();
        OLED_Show_StrAll(24, 0, 16, "时 间", 1);
 
        sprintf(str, "20%02d年", year);
        OLED_Show_StrAll(20, 35, 16, str, 1);
        
        sprintf(str, "%02d月%02d日", month, day);
        OLED_Show_StrAll(12, 60, 16, str, 1);
 
        sprintf(str, "%02d时%02d分", hour, min);
        OLED_Show_StrAll(12, 85, 16, str, 1);
        
        sprintf(str, "%02d秒", sec);
        OLED_Show_StrAll(44, 105, 16, str, 1);
        
        switch (select)
        {
            case 0: // 秒
                data = &sec;
                sprintf(str, "%02d", sec);
                OLED_Show_StrAll(44, 105, 16, str, 0);
                break;
            
            case 1: // 分
                data = &min;
                sprintf(str, "%02d", min);
                OLED_Show_StrAll(44, 85, 16, str, 0);
                break;
                
            case 2: // 时
                data = &hour;
                sprintf(str, "%02d", hour);
                OLED_Show_StrAll(12, 85, 16, str, 0);
                break;
                
            case 3: // 日
                data = &day;
                sprintf(str, "%02d", day);
                OLED_Show_StrAll(44, 60, 16, str, 0);
                break;
                
            case 4: // 月
                data = &month;
                sprintf(str, "%02d", month);
                OLED_Show_StrAll(12, 60, 16, str, 0);
                break;
                
            case 5: // 年
                data = &year;
                sprintf(str, "20%02d", year);
                OLED_Show_StrAll(20, 35, 16, str, 0);
                break;
        }
        
        
        switch (KeyValue)
        {
            case PressUp:
                switch (select)
                {
                    case 0: // 秒
                        (*data) = (*data)>59 ? 59:(*data);
                        break;
                
                    case 1: // 分
                        (*data) = (*data)>59 ? 59:(*data);
                        break;
                        
                    case 2: // 时
                        (*data) = (*data)>23 ? 23:(*data);
                        break;
                        
                    case 3: // 日
                        (*data) = (*data)>30 ? 30:(*data);
                        break;
                        
                    case 4: // 月
                        (*data) = (*data)>11 ? 11:(*data);
                        break;
                        
                    case 5: // 年
                        (*data) = (*data)>98 ? 98:(*data);
                        break;
                }
                (*data)++;
                break;
                
            case PressDown:
                switch (select)
                {
                    case 0: case 1: case 2: case 5:
                        (*data) = (*data)==0 ? 1:(*data);
                        break;
                        
                    case 3: case 4:
                        (*data) = (*data)==1 ? 2:(*data);
                        break;
                }
                (*data)--;
                break;
            
            case PressMid:


                  sTime.Hours = hour;
                  sTime.Minutes = min;
                  sTime.Seconds = sec;
                  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
                  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
                  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

                  switch (month)
                  {
                      case  1: sDate.Month = RTC_MONTH_JANUARY;   break;
                      case  2: sDate.Month = RTC_MONTH_FEBRUARY;  break;
                      case  3: sDate.Month = RTC_MONTH_MARCH;     break;
                      case  4: sDate.Month = RTC_MONTH_APRIL;     break;
                      case  5: sDate.Month = RTC_MONTH_MAY;       break;
                      case  6: sDate.Month = RTC_MONTH_JUNE;      break;
                      case  7: sDate.Month = RTC_MONTH_JULY;      break;
                      case  8: sDate.Month = RTC_MONTH_AUGUST;    break;
                      case  9: sDate.Month = RTC_MONTH_SEPTEMBER; break;
                      case 10: sDate.Month = RTC_MONTH_OCTOBER;   break;
                      case 11: sDate.Month = RTC_MONTH_NOVEMBER;  break;
                      case 12: sDate.Month = RTC_MONTH_DECEMBER;  break;
                  }
                  sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
                  sDate.Date = day;
                  sDate.Year = year;
                  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
                  HAL_RTCEx_SetWakeUpTimer(&hrtc, 0, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
                return;
                break;
                
            case PressLeft:
                select = select>4  ? 4:select;
                select++;
                break;
                
            case PressRight:
                select = select==0 ? 1:select;
                select--;
                break;
                
        }
        
        OLED_Refresh_Gram();
    }
}

PressKeyValue ReadKey(void)
{
//    if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port,      KEY_UP_Pin)     == GPIO_PIN_RESET ||
//        HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port,    KEY_DOWN_Pin)   == GPIO_PIN_RESET ||
//        HAL_GPIO_ReadPin(KEY_MID_GPIO_Port,     KEY_MID_Pin)    == GPIO_PIN_RESET ||
//        HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port,    KEY_LEFT_Pin)   == GPIO_PIN_RESET ||
//        HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port,   KEY_RIGHT_Pin)  == GPIO_PIN_RESET)
//    {
//        osDelay(10);
//        if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
//        {
//            HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
//            
//            
//            while (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET);
//            return PressUp;
//        }
//        else if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
//        {
//            HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
//            
//            
//            while (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET);
//            return PressDown;
//        }
//        else if (HAL_GPIO_ReadPin(KEY_MID_GPIO_Port, KEY_MID_Pin) == GPIO_PIN_RESET)
//        {
//            HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
//            
//            
//            while (HAL_GPIO_ReadPin(KEY_MID_GPIO_Port, KEY_MID_Pin) == GPIO_PIN_RESET);
//            return PressMid;
//        }
//        else if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
//        {
//            HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
//            
//            
//            while (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET);
//            return PressLeft;
//        }
//        else if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
//        {
//            HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
//            
//            
//            while (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET);
//            return PressRight;
//        }
//        else
//        {
//            return PressNull;
//        }
//    }
//    else
//    {
//        return PressNull;
//    }
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
