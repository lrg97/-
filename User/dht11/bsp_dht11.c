#include "./dht11/bsp_dht11.h"
#include "./dwt_delay/core_delay.h" 

//下面的宏定义函数可以随意更换，可以使用内核的计时，也可以使用systick的计时

#define DHT11_DELAY_US(us)  CPU_TS_Tmr_Delay_US(us)
#define DHT11_DELAY_MS(ms)  CPU_TS_Tmr_Delay_MS(ms)



 //DHT11初始化函数
void DHT11_Init ( void )
{
	DHT11_GPIO_Config ();
	
	DHT11_Dout_1;               // 拉高GPIOE6
}


//初始化DHT11通信使用的IO口   PE6
static void DHT11_GPIO_Config ( void )
{		
	  GPIO_InitTypeDef GPIO_InitStructure; 

	//开启DHT11_Dout_GPIO_PORT的外设时钟
    DHT11_Dout_SCK_APBxClock_FUN ( DHT11_Dout_GPIO_CLK, ENABLE );	
 
	//选择要控制的DHT11_Dout_GPIO_PORT引脚															   
  	GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;	

	//设置引脚模式为通用推挽输出
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	//设置引脚速率为50MHz  
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	//调用库函数，初始化DHT11_Dout_GPIO_PORT
  	GPIO_Init(DHT11_Dout_GPIO_PORT, &GPIO_InitStructure);		  
	
}


//使DHT11-DATA引脚变为上拉输入模式
static void DHT11_Mode_IPU(void)
{
 	  GPIO_InitTypeDef GPIO_InitStructure;
	  GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;
	  // 设置引脚模式为浮空输入模式
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
	  GPIO_Init(DHT11_Dout_GPIO_PORT, &GPIO_InitStructure);	 
}


//使DHT11-DATA引脚变为推挽输出模式
static void DHT11_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;										   
  GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;	
	//设置引脚模式为通用推挽输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT11_Dout_GPIO_PORT, &GPIO_InitStructure);	 	 
	
}


//从DHT11读取一个字节，高位先行
static uint8_t DHT11_ReadByte ( void )
{
	uint8_t i, temp=0;
	
	for(i=0;i<8;i++)    
	{	 
		//每bit以54us左右的低电平标置开始，轮询直到从机发出的54us低电平结束
		while(DHT11_Dout_IN()==Bit_RESET);

		//DHT11 以23~27us的高电平表示“0”，以68~74us高电平表示“1”    通过检测 x us后的电平即可区别这两个状态
		 
		DHT11_DELAY_US(50); //延时x us 这个延时需要大于数据0持续的时间即可	   	  

		if(DHT11_Dout_IN()==Bit_SET)//x us后仍为高电平表示数据“1”
		{
			//等待数据1的高电平结束
			while(DHT11_Dout_IN()==Bit_SET);
			//将数据写入对应的位
			temp|=(uint8_t)(0x01<<(7-i));  //把第7-i位   置1，高位先行 
		}
		else	 // x us后为低电平表示数据“0”
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //把第7-i位   置0，高位先行
		}
	}
	
	return temp;
	
}


//一次完整的数据传输为40bit，高位先出；8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和 
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data)
{  
	//主机为输出模式
	DHT11_Mode_Out_PP();
	//总线拉低  发送信号
	DHT11_Dout_0;
	//延时18ms
	DHT11_DELAY_MS(18);

	//总线拉高 主机延时30us  主机释放总线
	DHT11_Dout_1; 

	DHT11_DELAY_US(30); 

	// 主机设为输入模式 判断从机响应信号
	DHT11_Mode_IPU();

	//判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行
	if(DHT11_Dout_IN()==Bit_RESET)     
	{
		//循环查询输入电平直到从机发出 的83us 低电平 响应信号结束
		while(DHT11_Dout_IN()==Bit_RESET);

		//循环查询输入电平直到从机发出的 87us 高电平 标置信号结束，准备接收数据
		while(DHT11_Dout_IN()==Bit_SET);

		//开始接收数据 
		DHT11_Data->humi_int= DHT11_ReadByte();

		DHT11_Data->humi_deci= DHT11_ReadByte();

		DHT11_Data->temp_int= DHT11_ReadByte();

		DHT11_Data->temp_deci= DHT11_ReadByte();

		DHT11_Data->check_sum= DHT11_ReadByte();


		//读取结束，传感器继续输出54us低电平后释放总线，引脚改为输出模式
		DHT11_Mode_Out_PP();
		//总线拉高
		DHT11_Dout_1;

		//检查读取的数据是否正确
		if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci)
			return SUCCESS;
		else 
			return ERROR;
	}
	
	else
		return ERROR;
	
}

