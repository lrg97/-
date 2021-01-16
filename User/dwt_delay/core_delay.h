#ifndef CORE_DELAY_H
#define CORE_DELAY_H

#include "stm32f10x.h"

#define USE_DWT_DELAY			1	//使用DWT内核精确延时

#define USE_TICK_DELAY		0		//不使用SysTick延时

//简单任务管理
#define TASK_ENABLE 0
#define NumOfTask 3


#if USE_DWT_DELAY

//获取内核时钟频率
#define GET_CPU_ClkFreq()       (SystemCoreClock)
#define SysClockFreq            (SystemCoreClock)

#define CPU_TS_INIT_IN_DELAY_FUNCTION   1


//函数声明
uint32_t CPU_TS_TmrRd(void);
void CPU_TS_TmrInit(void);

//使用以下函数前必须先调用CPU_TS_TmrInit函数使能计数器，或使能宏CPU_TS_INIT_IN_DELAY_FUNCTION
//最大延时值为60秒      60s=2的32次方/72000000
void CPU_TS_Tmr_Delay_US(uint32_t us);
#define CPU_TS_Tmr_Delay_MS(ms)     CPU_TS_Tmr_Delay_US(ms*1000)
#define CPU_TS_Tmr_Delay_S(s)       CPU_TS_Tmr_Delay_MS(s*1000)

#endif

#endif /* CORE_DELAY_H */
