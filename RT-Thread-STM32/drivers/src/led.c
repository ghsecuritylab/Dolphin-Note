/*
 * File      : led.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */
#include <stdbool.h>
//#include <rtthread.h>
#include <stm32f10x.h>
#include "led.h"

static bool isInit = false;

static GPIO_TypeDef* led_port[] = 	//�ض���LED�˿�
{
	[LED_RED] 	= LED_GPIO_PORT, 
	[LED_GREEN]	= LED_GPIO_PORT,
	[LED_NET] 	= LED_GPIO_PORT, 
	[LED_SNP]	= LED_GPIO_PORT,
	[LED_REC]	= LED_REC_GPIO_PORT,
	[BEEP]		= LED_REC_GPIO_PORT,
	
};
static unsigned int led_pin[] = 	//�ض���LED�õ���pin
{
	[LED_RED] 	= LED_GPIO_RED, 
	[LED_GREEN]	= LED_GPIO_GREEN,
	[LED_NET] 	= LED_GPIO_NET, 
	[LED_SNP]	= LED_GPIO_SNP,
	[LED_REC]	= LED_GPIO_REC,
	[BEEP]		= BEEP_GPIO,
};
static int led_polarity[] = 		//LED��������
{
	[LED_RED] 	= LED_POL_RED, 
	[LED_GREEN]	= LED_POL_GREEN,
	[LED_NET] 	= LED_POL_NET, 
	[LED_SNP]	= LED_POL_SNP,
	[LED_REC]	= LED_POL_REC,
	[BEEP]		= BEEP_POL,
};

void rt_hw_led_init(void)
{
	if(isInit)
		return;//�Ѿ���ʼ���ɹ�������ִ���������
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    RCC_APB2PeriphClockCmd(LED_GPIO_PERIF | LED_REC_GPIO_PERIF,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = LED_GPIO_RED | LED_GPIO_GREEN | LED_GPIO_NET | LED_GPIO_SNP;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = LED_GPIO_REC | BEEP_GPIO;
    GPIO_Init(LED_REC_GPIO_PORT, &GPIO_InitStructure);
	
//	GPIO_SetBits(LED1_GPIO_PORT, LED_GPIO_RED);//Ĭ�Ϲر����е�
//	GPIO_SetBits(LED234_GPIO_PORT, LED_GPIO_GREEN|LED_GPIO_BLUE| LED_GPIO_YELLOW);//Ĭ�Ϲر����е�
//	ledSet(LED_RED	 ,	0);
//	ledSet(LED_GREEN ,	0);
//	ledSet(LED_BLUE  ,	0);
//	ledSet(LED_YELLOW,	0);
	
	isInit = true;//��ʼ�����
}

bool ledTest(void)
{
  return isInit;
}


void rt_hw_led_off(rt_uint32_t n)
{
    switch (n)
    {
		case 0:
//			GPIO_SetBits(LED1_GPIO_PORT, LED_GPIO_RED);
			break;
		case 1:
//			GPIO_SetBits(LED234_GPIO_PORT, LED_GPIO_GREEN);
			break;
		default:
			break;
    }
}

void rt_hw_led_on(rt_uint32_t n)
{
    switch (n)
    {
		case 0:
//			GPIO_ResetBits(LED1_GPIO_PORT, LED_GPIO_RED);
			break;
		case 1:
//			GPIO_ResetBits(LED234_GPIO_PORT, LED_GPIO_GREEN);
			break;
		default:
			break;
    }
}
void ledSet(led_t led, bool value) 
{

	if(value)
		GPIO_SetBits(led_port[led], led_pin[led]);
	else
		GPIO_ResetBits(led_port[led], led_pin[led]); 
}

