#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"


extern u8 alarmFlag;
extern u8 alarm_is_free; //�жϱ������Ƿ����
 
//�ⲿ�жϳ�ʼ������
void EXTIX_Init(void)
{
 
 	  EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // ʹ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    // ����PA8��PB15Ϊ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // �����ⲿ�ж���
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // �½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);
    EXTI_InitStructure.EXTI_Line = EXTI_Line15;
    EXTI_Init(&EXTI_InitStructure);

    // �����ⲿ�ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; // ��Ϊ PA8 �� EXTI9_5_IRQn ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; // ��Ϊ PA15 �� EXTI15_10_IRQn ��
		NVIC_Init(&NVIC_InitStructure);
}

 
void EXTI9_5_IRQHandler(void) 
{
    if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
      delay_ms(10);    //����
			if(KEY1==0)
			{	  
				LED0=!LED0;
			}

        EXTI_ClearITPendingBit(EXTI_Line8);
    }
}

void EXTI15_10_IRQHandler(void) 
{
    if (EXTI_GetITStatus(EXTI_Line15) != RESET) {
      delay_ms(10);   //����	
			if(KEY0==0)	
			{
				alarmFlag = !alarmFlag;
				alarm_is_free = 0;
			}
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}
