#include "snake.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include <stdlib.h>
#include <string.h>

uint32_t g_RunTime = 0;
uint8_t g_PointerCoor[MAX_LENGTH][2] = {0};

uint8_t g_Head[2] = {0};
uint8_t g_Food[2] = {0};

uint8_t g_NowDirect = 0;
uint8_t g_Length = DEFAULT_LENGTH;
uint8_t Exit = 0;
int16_t index;

/******函数接口声明******/
inline void DrawPoint(uint8_t x, uint8_t y, uint8_t mode)
{
	OLED_DrawPoint(x, y, mode);
}
inline uint8_t GetKey(void)
{
	return KeyValue;
}




void Snake_Start(void)
{
    Exit  = 0;

    OLED_Clear();

	Snake_ShowFood();
    
    
    osThreadResume(xTaskGetHandle("snakeTimer"));
    for (;;)
    {
        Snake_Turn();
        
            OLED_Clear();
            
//            osSemaphoreWait(snake_BinSemHandle, osWaitForever);
            Snake_ShowBody(g_Head[X], g_Head[Y], 1, g_Length);
            Snake_DrawPoint(g_Food[X], g_Food[Y], 1);
            if ( g_Head[X] == g_Food[X] && g_Head[Y] == g_Food[Y] )
            {
                Snake_ShowFood();
                g_Length++;
            }
            
        OLED_Refresh_Gram();
        if (Exit)
            return;
    }
}


void Snake_ShowFood(void)
{
	enum food
	{
		foodOK = 0,
		foorNO = 1
	};
	uint8_t foodFlag = 0;
	uint8_t temp_x = 0;
	uint8_t temp_y = 0;
	int16_t index = g_RunTime%MAX_LENGTH;
	
	do
	{
		temp_x = rand()%SNAKE_X_MAX;
		temp_y = rand()%SNAKE_Y_MAX;

		foodFlag = foodOK;
		for (uint8_t i=0; i<g_Length; i++)
		{
			if ( temp_x == g_PointerCoor[Last(index, i)][X] 
				&& temp_y == g_PointerCoor[Last(index, i)][Y] )
			{
				foodFlag = foorNO;
			}
		}
		
	}while (foodFlag);
	
	g_Food[X] = temp_x;
	g_Food[Y] = temp_y;
}

void Snake_Turn(void)
{
	static uint8_t s_LastDirect = 0;
	
	s_LastDirect = g_NowDirect;
	switch ( GetKey() )
	{
		case TurnUp:
			g_NowDirect = s_LastDirect==TurnDown	? TurnDown	: TurnUp;
			break;
		
		case TurnDown:
			g_NowDirect = s_LastDirect==TurnUp		? TurnUp	: TurnDown;
			break;
		
		case TurnLeft:
			g_NowDirect = s_LastDirect==TurnRight   ? TurnRight	: TurnLeft;
			break;
		
		case TurnRight:
			g_NowDirect = s_LastDirect==TurnLeft	? TurnLeft	: TurnRight;
			break;
		
		default:	break;
	}
}


void Snake_ShowBody(uint8_t x, uint8_t y, uint8_t mode, uint8_t length)
{
//	int16_t index = g_RunTime%MAX_LENGTH;
//	g_PointerCoor[index][X] = x;
//	g_PointerCoor[index][Y] = y;
//	Snake_DrawPoint(g_PointerCoor[Last(index, 0)][X], g_PointerCoor[Last(index, 0)][Y], 1);
	
//	printf("DATA%2d  X:%3d\t Y:%3d \r\n", 0, g_PointerCoor[Last(g_RunTime%MAX_LENGTH, 0)][X], g_PointerCoor[Last(g_RunTime%MAX_LENGTH, 0)][Y]);
//	
//	
    uint8_t select = 0;
	for (uint8_t i=0; i<(length>g_RunTime?g_RunTime:length); i++)
	{
//		printf("DATA%2d  X:%3d\t Y:%3d \r\n", i, g_PointerCoor[Last(g_RunTime%MAX_LENGTH, i)][X], g_PointerCoor[Last(g_RunTime%MAX_LENGTH, i)][Y]);
		if ( i != 0 &&
             g_PointerCoor[Last(index, i)][X] == g_PointerCoor[Last(index, 0)][X] && 
			 g_PointerCoor[Last(index, i)][Y] == g_PointerCoor[Last(index, 0)][Y] )
		{
//			NVIC_DisableIRQ(PIT_CH0);
            
            
			g_NowDirect = 0;
			memset(g_PointerCoor, 0, sizeof(g_PointerCoor));
			g_Head[X] = 10;
			g_Head[Y] = 10;
			g_RunTime = 0;
			g_Length = DEFAULT_LENGTH;

            
//			OLED_Clear();
			OLED_Show_StrAll(8, 30, 16, "GAME OVER", 1);
            
            OLED_Show_StrAll(29, 60, 16, "继 续", 1);
            OLED_Show_StrAll(29, 90, 16, "退 出", 1);
            
            OLED_Refresh_Gram();
            for (;;)
            {
                switch (KeyValue)
                {
                    case PressUp:
                        select = 0;
                        break;
                    
                    case PressDown:
                        select = 1;
                        break;
                        
                    case PressMid:
                        switch (select)
                        {
                        case 0:
                            goto out;
                            break;
                            
                        case 1:
                            osThreadSuspend(xTaskGetHandle("snakeTimer"));
                            Exit = 1;
                            return;
                            break;
                        }
                        break;
                }
                
                if (!select)
                {
                    OLED_Show_StrAll(13, 60, 16, ">", 1);
                    OLED_Show_StrAll(13, 90, 16, " ", 1);
                }
                else
                {
                    OLED_Show_StrAll(13, 60, 16, "  ", 1);
                    OLED_Show_StrAll(13, 90, 16, ">", 1);
                }
                OLED_Refresh_Gram();
                
            }
            
//            OLED_Show_StrAll(16, 50, 16, "继 续", 1);
//            OLED_Show_StrAll(16, 70, 16, "退 出", 1);

			
//			Delay_ms(1000);
//			OLED_Init();
//			NVIC_EnableIRQ(PIT_CH0);
			return;
		}

		Snake_DrawPoint(g_PointerCoor[Last(index, i)][X], g_PointerCoor[Last(index, i)][Y], mode);
		
	}
    out: ;
}

void Snake_DrawPoint(uint8_t x, uint8_t y, uint8_t mode)
{

	uint8_t new_x = POINT_SIZE * x;
	uint8_t new_y = POINT_SIZE * y;
	
	for ( uint8_t i=0; i<POINT_SIZE; i++ )
		for ( uint8_t j=0; j<POINT_SIZE; j++ )
			DrawPoint(new_x + i, new_y + j, mode);
}


void snakeTimer_Task(void const * argument)
{
    g_Head[X] = 10;
	g_Head[Y] = 10;

    for (;;)
    {
        osDelay(100);
//        printf("中文\r\n");
        HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

//    PIT_CLR_Flag(PIT_CH0);


        switch ( g_NowDirect )
        {
            case Up:	g_Head[Y]==0 ? (g_Head[Y]=SNAKE_Y_MAX-1) : g_Head[Y]--; break;
            
            case Down:	g_Head[Y]==SNAKE_Y_MAX-1 ? (g_Head[Y]=0) : g_Head[Y]++; break;
                
            case Left:	g_Head[X]==0 ? (g_Head[X]=SNAKE_X_MAX-1) : g_Head[X]--; break;
            
            case Right:	g_Head[X]==SNAKE_X_MAX-1 ? (g_Head[X]=0) : g_Head[X]++; break;
            
            default: break;
        }
        
        
        if ( g_NowDirect != 0 )
        {
            
            index = g_RunTime%MAX_LENGTH;
//	g_PointerCoor[index][X] = x;
//	g_PointerCoor[index][Y] = y;
	
//	Snake_DrawPoint(g_PointerCoor[Last(g_RunTime%MAX_LENGTH, 0)][X], g_PointerCoor[Last(index, 0)][Y], 1);
    
            g_PointerCoor[index][X] = g_Head[X];
            g_PointerCoor[index][Y] = g_Head[Y];
            
            
//            OLED_Clear();
//            Snake_ShowBody(g_Head[X], g_Head[Y], 1, g_Length);
//            Snake_DrawPoint(g_Food[X], g_Food[Y], 1);
//            if ( g_Head[X] == g_Food[X] && g_Head[Y] == g_Food[Y] )
//            {
//                Snake_ShowFood();
//                g_Length++;
//            }
            
            g_RunTime++;
            printf("time:%d\r\n", g_RunTime);
        }
//        osSemaphoreRelease(snake_BinSemHandle);
    }
}


// 100ms任务

//void PIT_CH0_IRQHandler(void)
//{
//    PIT_CLR_Flag(PIT_CH0);
//
//    /*中断内容--开始*/
//
//	switch ( g_NowDirect )
//	{
//		case Up:	g_Head[Y]==0 ? (g_Head[Y]=SNAKE_Y_MAX-1) : g_Head[Y]--; break;
//		
//		case Down:	g_Head[Y]==SNAKE_Y_MAX-1 ? (g_Head[Y]=0) : g_Head[Y]++; break;
//			
//		case Left:	g_Head[X]==0 ? (g_Head[X]=SNAKE_X_MAX-1) : g_Head[X]--; break;
//		
//		case Right:	g_Head[X]==SNAKE_X_MAX-1 ? (g_Head[X]=0) : g_Head[X]++; break;
//		
//		default: break;
//	}
//	
//	
//	
//	
//
//
//	
//	if ( g_NowDirect != 0 )
//	{
//		
//		OLED_Clear();
//		Snake_ShowBody(g_Head[X], g_Head[Y], 1, g_Length);
//		Snake_DrawPoint(g_Food[X], g_Food[Y], 1);
//		if ( g_Head[X] == g_Food[X] && g_Head[Y] == g_Food[Y] )
//		{
//			Snake_ShowFood();
//			g_Length++;
//		}
//		
//		g_RunTime++;
////		for(uint8_t i=0; i<20; i++)
////			Variable[i] = g_PointerCoor[Last(g_RunTime%MAX_LENGTH, i)][X];
////		Send_Begin();
////		Send_Variable();
//	}
//
//    /*中断内容--结束*/
//}
