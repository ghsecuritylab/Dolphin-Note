#include "key.h"
#include "usart2.h"
#include "cc3200.h"

u8 WIFI_Socket_Flag = 0;    //����״̬���� 0->��δ���ӣ�1->WiFi���ӡ��ֻ�δ���ӣ�2->WiFi���ӡ��ֻ�����
u8 SNP_STAT_Flag 	= 0;       //����������� 0->��������, 1->�������� 2->������
u8 REC_STAT_Flag 	= 0;       //¼��������� 0->����¼��  1->����¼�� 2->¼����
u8 STAT_STAT_Flag 	= 0;      //Ӳ������״̬����    0->���� 1->Ӳ������ 2->���������� 3->WIFI����APģʽ
u8 BEEP_STAT_Flag 	= 0;      //����״̬���� 0->���� 1-> ��1�� 2->��2�� 3->����3��
u8 ERR_Status 		= 0;          //Ӳ������  00->���� D1-> �ϴζ������в������� E1->���ݶ������ E2->�������������� E4->���ݻ��治���� E8->�ⲿ��ѹ����

#define T_NET	1
#define T_SNP 2
#define T_REC 4

#define N_key	0		// �޼�
#define S_key	1		// ����
#define L_key 	2		// ����

#define key_state_0	0		// ����δ����
#define key_state_1	1		// ����������ȷ��
#define key_state_2	2		// ����δ�ͷ�
#define key_state_3	3		// �ȴ������ͷ�


void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
//	TIM7_Int_Init(1000-1,7200-1);		//10ms�ж�
}
/*******************************************************************************
* ������  : key_scan
* ����    : ����ɨ��
* ����ֵ  : 0���ް���; 
* ˵��    : 
*******************************************************************************/
u8 KeyRead(void)
{
	u8 Trg=0;
	u8 Cont=0;
	u8 status;
	u8 ReadData;

		ReadData = ((GPIO_ReadInputData(GPIOB) & 0x7000) >> 12) ^ 0x07;
		Trg = ReadData & (ReadData ^ Cont);
		Cont = ReadData;
	
		rt_thread_delay(300);
		status = Trg + ReadData;
	
	return status;
}
/*******************************************************************************
* ������  : key_scan
* ����    : ����ɨ��
* ����ֵ  : 0���ް���; 
*						0x12: T_REC �̰�; 0x13: T_REC ����; 
*						0x22: T_SNP �̰�; 0x23: T_SNP ����; 
*						0x32: T_NET �̰�; 0x33: T_NET ����; 
* ˵��    : wait_time�����ӵȴ�ʱ��; over_time: �������ӳ�ʱʱ�䣬��λ 1S
*******************************************************************************/
u8 key_scan(void)
{
	u8 key_value;
	u8 key_flag;
	u8 key_press;
	u8 key_return;
	u8 i;
	key_flag = 0;

	key_value = KeyRead();
	if(key_value !=0 )
	{
		key_press = ((key_value>>2) ^ 0x03)<<4;
		for(i=0;i<10;i++)
		{			
			rt_thread_delay(1);
			key_return = KeyRead();
			if(key_return == 0)
			{
				key_flag = key_press | 0x02;
				break;	
			}
			if(i == 7)
			{
				key_flag = key_press | 0x03;
				break;
			}			
		}
		return key_flag;
	}
	else
		return key_flag;
}

void key_state_function(unsigned char _key)
{
	static unsigned char key_state = 0;
	static unsigned char key_num = 0;
	switch(key_state)
	{
		case 0:
			
			break;
		case 1:
			break;
		case 2:
			break;
		
	}
}













