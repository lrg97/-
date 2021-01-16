#include "./systick/bsp_SysTick.h"

static __IO u32 TimingDelay;
 
//启动系统滴答定时器
void SysTick_Init(void)
{
   
	if (SysTick_Config(SystemCoreClock / 1000000))	// 1微秒中断一次
	{ 

		while (1);
	}
		// 关闭滴答定时器  
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}

//微秒延时，10um为一个单位
void Delay_us(__IO u32 nTime)
{ 
	TimingDelay = nTime;	

	// 使能滴答定时器  
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay != 0);
}

//获取节拍程序，在 SysTick 中断函数 SysTick_Handler()调用，中断函数统一放在stm32f10x_it.c文件中
void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}

