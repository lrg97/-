#include "./dht11/bsp_dht11.h"
#include "./dwt_delay/core_delay.h" 

//����ĺ궨�庯�������������������ʹ���ں˵ļ�ʱ��Ҳ����ʹ��systick�ļ�ʱ

#define DHT11_DELAY_US(us)  CPU_TS_Tmr_Delay_US(us)
#define DHT11_DELAY_MS(ms)  CPU_TS_Tmr_Delay_MS(ms)



 //DHT11��ʼ������
void DHT11_Init ( void )
{
	DHT11_GPIO_Config ();
	
	DHT11_Dout_1;               // ����GPIOE6
}


//��ʼ��DHT11ͨ��ʹ�õ�IO��   PE6
static void DHT11_GPIO_Config ( void )
{		
	  GPIO_InitTypeDef GPIO_InitStructure; 

	//����DHT11_Dout_GPIO_PORT������ʱ��
    DHT11_Dout_SCK_APBxClock_FUN ( DHT11_Dout_GPIO_CLK, ENABLE );	
 
	//ѡ��Ҫ���Ƶ�DHT11_Dout_GPIO_PORT����															   
  	GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;	

	//��������ģʽΪͨ���������
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	//������������Ϊ50MHz  
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	//���ÿ⺯������ʼ��DHT11_Dout_GPIO_PORT
  	GPIO_Init(DHT11_Dout_GPIO_PORT, &GPIO_InitStructure);		  
	
}


//ʹDHT11-DATA���ű�Ϊ��������ģʽ
static void DHT11_Mode_IPU(void)
{
 	  GPIO_InitTypeDef GPIO_InitStructure;
	  GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;
	  // ��������ģʽΪ��������ģʽ
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
	  GPIO_Init(DHT11_Dout_GPIO_PORT, &GPIO_InitStructure);	 
}


//ʹDHT11-DATA���ű�Ϊ�������ģʽ
static void DHT11_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;										   
  GPIO_InitStructure.GPIO_Pin = DHT11_Dout_GPIO_PIN;	
	//��������ģʽΪͨ���������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT11_Dout_GPIO_PORT, &GPIO_InitStructure);	 	 
	
}


//��DHT11��ȡһ���ֽڣ���λ����
static uint8_t DHT11_ReadByte ( void )
{
	uint8_t i, temp=0;
	
	for(i=0;i<8;i++)    
	{	 
		//ÿbit��54us���ҵĵ͵�ƽ���ÿ�ʼ����ѯֱ���ӻ�������54us�͵�ƽ����
		while(DHT11_Dout_IN()==Bit_RESET);

		//DHT11 ��23~27us�ĸߵ�ƽ��ʾ��0������68~74us�ߵ�ƽ��ʾ��1��    ͨ����� x us��ĵ�ƽ��������������״̬
		 
		DHT11_DELAY_US(50); //��ʱx us �����ʱ��Ҫ��������0������ʱ�伴��	   	  

		if(DHT11_Dout_IN()==Bit_SET)//x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1��
		{
			//�ȴ�����1�ĸߵ�ƽ����
			while(DHT11_Dout_IN()==Bit_SET);
			//������д���Ӧ��λ
			temp|=(uint8_t)(0x01<<(7-i));  //�ѵ�7-iλ   ��1����λ���� 
		}
		else	 // x us��Ϊ�͵�ƽ��ʾ���ݡ�0��
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //�ѵ�7-iλ   ��0����λ����
		}
	}
	
	return temp;
	
}


//һ�����������ݴ���Ϊ40bit����λ�ȳ���8bit ʪ������ + 8bit ʪ��С�� + 8bit �¶����� + 8bit �¶�С�� + 8bit У��� 
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data)
{  
	//����Ϊ���ģʽ
	DHT11_Mode_Out_PP();
	//��������  �����ź�
	DHT11_Dout_0;
	//��ʱ18ms
	DHT11_DELAY_MS(18);

	//�������� ������ʱ30us  �����ͷ�����
	DHT11_Dout_1; 

	DHT11_DELAY_US(30); 

	// ������Ϊ����ģʽ �жϴӻ���Ӧ�ź�
	DHT11_Mode_IPU();

	//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������
	if(DHT11_Dout_IN()==Bit_RESET)     
	{
		//ѭ����ѯ�����ƽֱ���ӻ����� ��83us �͵�ƽ ��Ӧ�źŽ���
		while(DHT11_Dout_IN()==Bit_RESET);

		//ѭ����ѯ�����ƽֱ���ӻ������� 87us �ߵ�ƽ �����źŽ�����׼����������
		while(DHT11_Dout_IN()==Bit_SET);

		//��ʼ�������� 
		DHT11_Data->humi_int= DHT11_ReadByte();

		DHT11_Data->humi_deci= DHT11_ReadByte();

		DHT11_Data->temp_int= DHT11_ReadByte();

		DHT11_Data->temp_deci= DHT11_ReadByte();

		DHT11_Data->check_sum= DHT11_ReadByte();


		//��ȡ�������������������54us�͵�ƽ���ͷ����ߣ����Ÿ�Ϊ���ģʽ
		DHT11_Mode_Out_PP();
		//��������
		DHT11_Dout_1;

		//����ȡ�������Ƿ���ȷ
		if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci)
			return SUCCESS;
		else 
			return ERROR;
	}
	
	else
		return ERROR;
	
}

