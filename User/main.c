#include "stm32f10x.h"
#include "./systick/bsp_SysTick.h"
#include "./dht11/bsp_dht11.h"
#include "./usart/bsp_usart.h"



int main(void)
{
	DHT11_Data_TypeDef DHT11_Data;
	
	SysTick_Init();

	USART_Config();

	DHT11_Init ();
	
	while(1)
	{	
			//����DHT11_Read_TempAndHumidity��ȡ��ʪ�ȣ����ɹ����������Ϣ,ÿ2s�ϱ�һ��
			if( DHT11_Read_TempAndHumidity ( & DHT11_Data ) == SUCCESS)
			{
				printf("\r\n��ȡDHT11�ɹ�!\r\n\r\nʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\n",
				DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
			}			
			else
			{
				printf("Read DHT11 ERROR!\r\n");
			}
			
		 Delay_ms(2000);
	}

	
}
