#include "usart2.h"
#include "stdio.h"	 	 
#include "string.h"	
#include "Screen.h"
#include "wifi.h"
#include "cc3200.h"

//////////////////////////////////////////////////////////////////////////////////	 
////////////////////////////////////////////////////////////////////////////////// 	   
struct rt_semaphore Screen_seqSem;//����LED��˸�����ź��������ź�����FreeRTOS�е��ź�����������
struct rt_semaphore Net_complete_seqSem;//������������ź���
//���ڽ��ջ����� 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.
u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 			//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�

//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART2_RX_STA=0;   	




#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  

//��ʼ��IO ����1 
//bound:������
void uart1_init(u32 bound)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//USART_ClockInitTypeDef  USART_ClockInitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��UART3��GPIOBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
 	USART_DeInit(USART1);  //��λ����
	 //USART1_TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��
 
	//USART1_RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��

   //USART1 NVIC ����
//	NVIC_Configuration();
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	//����USART1
	//�жϱ�������
	USART_InitStructure.USART_BaudRate = bound;       //�����ʿ���ͨ������վ����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8λ����
	USART_InitStructure.USART_StopBits = USART_StopBits_1;   //��֡��β����1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;    //������żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //Ӳ��������ʧ��
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //���͡�����ʹ��
	//����USART1ʱ��
	USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;  //ʱ�ӵ͵�ƽ�
	USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;  //SLCK������ʱ������ļ���->�͵�ƽ
	USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;  //ʱ�ӵڶ������ؽ������ݲ���
	USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable; //���һλ���ݵ�ʱ�����岻��SCLK���
	
	USART_Init(USART1, &USART_InitStructure);
	USART_ClockInit(USART1, &USART_ClockInitStruct);

	//ʹ��USART1�����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	rt_sem_init(&Screen_seqSem , "ledseqSem", 0, RT_IPC_FLAG_FIFO);//��ʼ���ź�������ʼֵΪ1�����ź����ᱻ��һ�����е��߳����
//ʹ��USART1
	USART_Cmd(USART1, ENABLE); 
}

unsigned char Screen_TxBuffer[50] = {0};
unsigned char Screen_RxBuffer[255] = {0};
//unsigned int i = 0;
unsigned char count_1 = 0;
unsigned char TxCounter_1 = 0;
void Uart1_Put_Buf(unsigned char *DataToSend , unsigned char data_num)
{
	unsigned char i = 0;
	for(i = 0; i < data_num; i ++)
		Screen_TxBuffer[count_1++] = *(DataToSend + i);
	if(!(USART1->CR1 & USART_CR1_TXEIE))
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	static  unsigned char err_cnt = 0;
			unsigned char ResData;//���յ���һ���ֽ�		
	static 	unsigned char Data_Len = 0,//���ݳ���
						  Data_Count = 0;//���ݳ��ȼ���
	static 	unsigned char RxState = 0;//��������״̬����   �����л�״̬
//	u8 Res;
	
	/* enter interrupt */
    rt_interrupt_enter();
	
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)//
    {
        USART_ReceiveData(USART1);
    }		
	//�����ж�
	if((USART1->SR & (1 << 7))&&(USART1->CR1 & USART_CR1_TXEIE))
	{
		USART1->DR = Screen_TxBuffer[TxCounter_1++]; //дDR����жϱ�־          
		if(TxCounter_1 == count_1)
		{
			USART1->CR1 &= ~USART_CR1_TXEIE;		//�ر�TXE�ж�
		}
	}
	//�����ж� (���ռĴ����ǿ�) 
	if(USART1->SR & (1 << 5))  
	{
		ResData = USART1->DR;
		switch (RxState)
		{
			case 0:
				
			
				if(ResData == 0x68)
				{
					RxState = 1;
					Screen_RxBuffer[0] = ResData;
				}
			break;
			case 1:
				if(ResData < 50)
				{
					RxState = 2;
					Screen_RxBuffer[1] = ResData;
					Data_Len = ResData;
					Data_Count = 0;
				}
			break;
			case 2:
				if(Data_Len > 0)
				{
					Data_Len --;
					Screen_RxBuffer[2 + Data_Count ++] = ResData;
					if(Data_Len == 0)
					{
						RxState = 0;
//						flag = 1;
//						_num = Data_Count + 2;
						Screen_data_analize(Screen_RxBuffer,Data_Count + 2);
						
					}
				}
				else
				{
					err_cnt ++;
					if(err_cnt >= 1)
					{
						err_cnt = 0;
						RxState = 0;
					}
				}
			break;
			default:
				RxState = 0;
			break;	
		}
	}
	/* leave interrupt */
    rt_interrupt_leave();
} 

#endif



unsigned char WIFI_TxBuffer[256] = {0};
unsigned char WIFI_RxBuffer[50] = {0};
//unsigned char WIFI_RxBuffer2[39] = {0};
unsigned char count_2 = 0;
unsigned char TxCounter_2 = 0;
void Uart2_Put_Buf(unsigned char *DataToSend , unsigned char data_num)
{
	unsigned char i = 0;
	for(i = 0; i < data_num; i ++)
		WIFI_TxBuffer[count_2++] = *(DataToSend + i);
	if(!(USART2->CR1 & USART_CR1_TXEIE))
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

void USART2_IRQHandler(void)
{
	/* enter interrupt */
    rt_interrupt_enter();
			unsigned char 	ResData;//���յ���һ���ֽ�
	static 	unsigned int	Data_Len = 0,//���ݳ���
							Data_Count = 0;//���ݳ��ȼ���
	static 	unsigned char	RxState = 0;//��������״̬����   �����л�״̬
	static	unsigned char	ID_cnt = 0;
//	static	unsigned char	rescount = 0;

		
	
	/* enter interrupt */
	rt_interrupt_enter();
	u8 res;
	switch(socket_start)
	{
		case 0:
				if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����
				{	 
					res =USART_ReceiveData(USART2);		 
					if((USART2_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
					{ 
						if(USART2_RX_STA<USART2_MAX_RECV_LEN)	//�����Խ�������
						{
							TIM_SetCounter(TIM7,0);          		//���������
							if(USART2_RX_STA==0) 				//ʹ�ܶ�ʱ��7���ж� 
							{
								TIM_Cmd(TIM7,ENABLE);//ʹ�ܶ�ʱ��7
							}
							USART2_RX_BUF[USART2_RX_STA++]=res;	//��¼���յ���ֵ	 
						}
						else 
						{
							USART2_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
						} 
					}
				}
				break;			
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
		case 1:
				if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)//
				{
					USART_ReceiveData(USART2);
				}		
				//�����ж�
				if((USART2->SR & (1 << 7))&&(USART2->CR1 & USART_CR1_TXEIE))
				{
					USART2->DR = WIFI_TxBuffer[TxCounter_2++]; //дDR����жϱ�־          
					if(TxCounter_2 == count_2)
					{
						//TxCounter_2 = 0;
						//count_2 = 0;
						USART2->CR1 &= ~USART_CR1_TXEIE;		//�ر�TXE�ж�
					}
				}
				//�����ж� (���ռĴ����ǿ�) 
				if(USART2->SR & (1 << 5))  
				{
					
					ResData = USART2->DR;
//					WIFI_RxBuffer2[rescount++]=ResData;
//					if(rescount>38)
//						rescount = 0;
					
					switch (RxState)
					{
						case 0:
							if(ResData == 0xFA)
							{
								RxState = 1;
								WIFI_RxBuffer[0] = ResData;
							}
						break;
						case 1:
							if(ResData == 0x0A)
							{
								RxState = 2;
								WIFI_RxBuffer[1] = ResData;
							}
						break;
						case 2://FC
							RxState = 3;
							WIFI_RxBuffer[2] = ResData;
						break;
						case 3://ID
							WIFI_RxBuffer[3 + ID_cnt] = ResData;
							ID_cnt ++;
							if(ID_cnt > 1)
							{
								ID_cnt = 0;
								RxState = 4;
								break;
							}
							
						break;
						case 4:
							WIFI_RxBuffer[5 + ID_cnt] = ResData;
							ID_cnt ++;
							if(ID_cnt > 1)
							{
								ID_cnt = 0;
								Data_Len = ((unsigned int)WIFI_RxBuffer[6])<<8 | WIFI_RxBuffer[5];
								
								Data_Count = 0;
								if(Data_Len == 0)
								{
									RxState = 6;
									break;
								}
								RxState = 5;
								break;
							}
						break;
						case 5:
							if(Data_Len > 0)
							{
								Data_Len --;
								WIFI_RxBuffer[7 + Data_Count ++] = ResData;
								if(Data_Len == 0)
								{
									RxState = 6;
									break;
								}
							}
//							else if(Data_Len == 0)
//							{
//								RxState = 0;
//							}
						break;
						case 6:
							WIFI_RxBuffer[7 + Data_Count ++] = ResData;
							RxState = 7;
						break;
						case 7:
							WIFI_RxBuffer[7 + Data_Count] = ResData;
							wifi_data_deal(WIFI_RxBuffer,8 + Data_Count);
							RxState = 0;
						break;
						default:
							RxState = 0;
						break;	
					}		
				}	
				break;
	}
	
	/* leave interrupt */
	rt_interrupt_leave();
}   


//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void usart2_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); //����2ʱ��ʹ��

 	USART_DeInit(USART2);  //��λ����2
		 //USART2_TX   PA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PB10

	//USART2_RX	  PA3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PB11
	
	USART_InitStructure.USART_BaudRate = bound;//������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(USART2, &USART_InitStructure); //��ʼ������	3
  

	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
	
	//ʹ�ܽ����ж�
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�   
	
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	
	TIM7_Int_Init(1000-1,7200-1);		//10ms�ж�
	USART2_RX_STA=0;		//����
	TIM_Cmd(TIM7,DISABLE);			//�رն�ʱ��7

}

//��ʱ��7�жϷ������		    
void TIM7_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		USART2_RX_STA|=1<<15;	//��ǽ������
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //���TIM7�����жϱ�־    
		TIM_Cmd(TIM7, DISABLE);  //�ر�TIM7 
	}	    
}
 
//ͨ�ö�ʱ��7�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ42M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz 
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM7_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//TIM7ʱ��ʹ��    
	
	//��ʱ��TIM7��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM7�ж�,��������ж�
	
	TIM_Cmd(TIM7,ENABLE);//������ʱ��7
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}















