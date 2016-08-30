#ifndef __USART2_H
#define __USART2_H	 

#include <rtthread.h>
#include "stm32f10x.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����3��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 1

extern struct rt_semaphore Screen_seqSem;//����LED��˸�����ź��������ź�����FreeRTOS�е��ź�����������
extern struct rt_semaphore Net_complete_seqSem;
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���
extern unsigned char Screen_TxBuffer[50];
extern unsigned char Screen_RxBuffer[255];
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart1_init(u32 bound);
void uart2_init(u32 bound);
void Uart1_Put_Buf(unsigned char *DataToSend , unsigned char data_num);
void Uart2_Put_Buf(unsigned char *DataToSend , unsigned char data_num);

#define USART2_MAX_RECV_LEN		200					//�����ջ����ֽ���
#define USART2_MAX_SEND_LEN		200					//����ͻ����ֽ���
#define USART2_RX_EN 			1					//0,������;1,����.

extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
extern vu16 USART2_RX_STA;   						//��������״̬

void usart2_init(u32 bound);				//����2��ʼ�� 

void TIM7_Int_Init(u16 arr,u16 psc);

#endif

