#include "./systick/bsp_SysTick.h"

static __IO u32 TimingDelay;
 
//����ϵͳ�δ�ʱ��
void SysTick_Init(void)
{
   
	if (SysTick_Config(SystemCoreClock / 1000000))	// 1΢���ж�һ��
	{ 

		while (1);
	}
		// �رյδ�ʱ��  
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}

//΢����ʱ��10umΪһ����λ
void Delay_us(__IO u32 nTime)
{ 
	TimingDelay = nTime;	

	// ʹ�ܵδ�ʱ��  
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay != 0);
}

//��ȡ���ĳ����� SysTick �жϺ��� SysTick_Handler()���ã��жϺ���ͳһ����stm32f10x_it.c�ļ���
void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}

