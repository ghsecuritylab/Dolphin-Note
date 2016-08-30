#ifndef __PROME_STAT_H
#define __PROME_STAT_H
#include "stm32f10x.h"
#include <rtthread.h>
extern u8 WIFI_Socket_Flag;    //����״̬���� 0->��δ���ӣ�1->WiFi���ӡ��ֻ�δ���ӣ�2->WiFi���ӡ��ֻ�����
extern u8 SNP_STAT_Flag;       //����������� 0->��������, 1->�������� 2->������
extern u8 REC_STAT_Flag;       //¼��������� 0->����¼��  1->����¼�� 2->¼����
extern u8 STAT_STAT_Flag;      //Ӳ������״̬����    0->���� 1->Ӳ������ 2->���������� 3->WIFI����APģʽ
extern u8 BEEP_STAT_Flag;      //����״̬���� 0->���� 1-> ��1�� 2->��2�� 3->����3��
extern u8 ERR_Status;          //Ӳ������  00->���� D1-> �ϴζ������в������� E1->���ݶ������ E2->�������������� E4->���ݻ��治���� E8->�ⲿ��ѹ����
static unsigned char heart_beat_lose_cnt = 0;
#endif
