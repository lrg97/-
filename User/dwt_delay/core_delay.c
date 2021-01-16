 #include "./dwt_delay/core_delay.h"   


//使用内核振动频率进行时间计数，功能与SysTick延时函数一样


#if USE_DWT_DELAY


#define  DWT_CR      *(__IO uint32_t *)0xE0001000
#define  DWT_CYCCNT  *(__IO uint32_t *)0xE0001004
#define  DEM_CR      *(__IO uint32_t *)0xE000EDFC


#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)


//初始化时间戳,使用延时函数前，必须调用本函数
void CPU_TS_TmrInit(void)
{
    //使能DWT外设
    DEM_CR |= (uint32_t)DEM_CR_TRCENA;                

    //DWT CYCCNT寄存器计数清0
    DWT_CYCCNT = (uint32_t)0u;

    //使能Cortex-M DWT CYCCNT寄存器 
    DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;
}

//读取并返回当前时间戳
uint32_t CPU_TS_TmrRd(void)
{        
  return ((uint32_t)DWT_CYCCNT);
}


//使用CPU内核完成的延时函数，在使用该函数前一定要先调用函数CPU_TS_TmrInit()
void CPU_TS_Tmr_Delay_US(__IO uint32_t us)
{
  uint32_t ticks;
  uint32_t told,tnow,tcnt=0;

  //在函数内部初始化时间戳寄存器  
#if (CPU_TS_INIT_IN_DELAY_FUNCTION)  
  //初始化时间戳并清零
  CPU_TS_TmrInit();
#endif
  
  ticks = us * (GET_CPU_ClkFreq() / 1000000);  //需要的节拍数      
  tcnt = 0;
  told = (uint32_t)CPU_TS_TmrRd();         //刚进入时的计数器值

  while(1)
  {
    tnow = (uint32_t)CPU_TS_TmrRd();  
    if(tnow != told)
    { 
      //32位计数器是递增计数器
      if(tnow > told)
      {
        tcnt += tnow - told;  
      }
      //重新装载
      else 
      {
        tcnt += UINT32_MAX - told + tnow; 
      } 
      
      told = tnow;

      //时间超过或者等于要延迟的时间,则退出
      if(tcnt >= ticks)break;
    }  
  }
}

#endif
