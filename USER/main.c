#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "dht11.h"
#include "oled.h"
#include "timer.h"
#include "key.h"
#include "exti.h"
#include "beep.h"
#include "esp8266.h"
#include "onenet.h"
#include "bh1750.h"
#include "jq8400.h"
#include "HW.h"
#include "wave.h"
#include <stdio.h>

uint8_t tempH, tempL;		  // �¶�������С��
uint8_t humiH, humiL;		  // ʪ��������С��
u8 alarmFlag = 0;			  // ������־
u8 alarm_is_free = 10;		  // �жϱ������Ƿ����
float light = 0;			  // ���ն�
_Bool LED_Status = 0;		  // ��״̬
char PUB_BUF[256];			  // �ϴ�����buf
unsigned short timeCount = 0; // ���ͼ������

u8 Strightline = 0; // ����ʱ��û���ϰ�����ڰ���ä�����飬����һ·��ͨ������ǰ��--������3����û���ϰ���������
u8 Whole_time = 0;	// ����ͳ��δ�԰����ҹ����ʱ�䣬��һ��ʱ��󣬶���������������0���Ա�ﵽ��ʱ���Ѱ����ҹ
u8 Disnum = 30;		// ��������������
u8 t = 20;			// trig���Ÿߵ�ƽ����ʱ��
u8 Distance;		// ��������õľ���
u8 buff[64];		// �����ַ���ƴ�ӣ����͵��ֻ�����ʾ
u8 disWarn = 0;
u8 time = 0;

const char *topics[] = {"/wonderfulhome/sub"};	 // ����
const char devPubtopic[] = "/wonderfulhome/pub"; // ����
static char SETVOLUME[] = {"AA 13 01 1C D2"};	 // ��������

int main(void)
{

	unsigned char *dataPtr = NULL;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();		 // ��ʱ������ʼ��
	Usart1_Init(9600);	 // Debug����
	Usart2_Init(115200); // esp8266ͨѶ����
	LED_Init();			 // ��ʼ����LED���ӵ�Ӳ���ӿ�
	DHT11_Init();
	BH1750_Init();
	TIM2_Int_Init(4999, 7199);
	TIM3_Int_Init(2499, 7199);
	OLED_Init();
	OLED_Clear();
	KEY_Init();
	EXTIX_Init();
	Beep_Init();
	infrared_INIT1();			// ��ǰ���ϰ�����ģ���ʼ��	OUT->PA6
	infrared_INIT2();			// ��ǰ���ϰ�����ģ���ʼ��	OUT->PA5
	Wave_SRD_Init();			// ��ʼ��������   Trig->PB7  Echo->PB6     Trig����ΪPE7      Echo����ΪPE8
	Timer_SRD_Init(9998, 7199); // ��ʱ����ʼ��
	ESP8266_Init();

	while (OneNet_DevLink())
		delay_ms(500);
	Beep_Set(BEEP_ON);
	delay_ms(250);
	Beep_Set(BEEP_OFF);
	UsartPrintf(USART1, SETVOLUME);

	OneNet_Subscribe(topics, 1);

	while (Wave_SRD_Strat(t))
	{
	}

	playsong(1);
	delay_ms(1000);
	delay_ms(1000);
	while (1)
	{

		// ��ȡ��ʪ��
		DHT11_Read_Data(&humiH, &humiL, &tempH, &tempL);
		// UsartPrintf(USART1,"��ǰ�¶�:%d.%d  ʪ��:%d.%d\r\n",tempH,tempL,humiH,humiL);

		// ��ȡ��ǿ
		if (!i2c_CheckDevice(BH1750_Addr))
		{
			light = LIght_Intensity();
			// UsartPrintf(USART1,"��ǰ����ǿ��:%.2f lx\r\n",light);
		}

		if (alarm_is_free >= 10) // �������Ƿ����
		{

			if (light > 3000 || humiH > 80 || tempH > 32)
				alarmFlag = 1;
			else
				alarmFlag = 0;
		}
		if (alarm_is_free < 10)
			alarm_is_free++;

		if (++timeCount % 5 == 0)
		{
			LED_Status = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
			// UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
			sprintf(PUB_BUF, "{\"Temp\":%d.%d, \"Hum\":%d.%d, \"Light\":%.2f, \"LED\":%d, \"Buzzer\":%d, \"Distance\":%d, \"disWarn\":%d}", tempH, tempL, humiH, humiL, light, LED_Status ? 0 : 1, alarmFlag, Distance, disWarn);

			OneNet_Publish(devPubtopic, PUB_BUF);

			timeCount = 0;
			ESP8266_Clear();
		}

		dataPtr = ESP8266_GetIPD(3);
		if (dataPtr != NULL)
			OneNet_RevPro(dataPtr);

		// ��������10Us�ĸߵ�ƽ��OLED����ʾ�������������
		Wave_SRD_Strat(t);
		Distance = TIM_GetCounter(TIM4) * 340 / 200.0;
		// �ϰ����ⱨ������
		if (timeCount <= 180) // ����ʱ��û���ϰ������ʵ����ʾ�����õ���3min��������ä��ǰ��һ·��ͨ
		{

			if (Distance > 0)
			{

				delay_ms(10);
				if (Distance < Disnum) // ��⵽�ϰ������С��30cmʱ����
				{

					disWarn = 1; // ���͵��ֻ���
					LED1 = 0;	 // ����ʾ��
					playsong(2); // ����������ǰ�����ϰ���
					delay_ms(1000);
					delay_ms(1000);
					delay_ms(1000);
					LED1 = 1; // �ص�
					//					BEEP=0;
					Strightline = 0; // ���ϰ��������0
				}
				else
				{
					Strightline++; // û���ϰ������
				}
			}
			if (Detect2() == 0) // ��ǰ���ϰ�����   PA6
			{
				LED1 = 0;
				//				BEEP=0;

				disWarn = 2; // ���͵��ֻ���
				delay_ms(10);
				playsong(3);
				delay_ms(1000);
				delay_ms(1000);
				delay_ms(1000);
				LED1 = 1;
				//				BEEP=1;
				Strightline = 0; // ���ϰ��������0
			}
			else if (Detect1() == 0) // ��ǰ���ϰ�����   PA5
			{
				LED1 = 0;
				//				BEEP=0;
				disWarn = 3; // ���͵��ֻ���
				playsong(4);
				delay_ms(1000);
				delay_ms(1000);
				delay_ms(1000);
				LED1 = 1;
				//				BEEP=0;
				Strightline = 0; // ���ϰ��������0
			}
			else
			{
				Strightline++; // û���ϰ������
			}
			delay_ms(1000);
		}
		else
		{

			disWarn = 0; // ���͵��ֻ���
			playsong(7); // ������������ǰ��һ·��ͨ
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			Strightline = 0;
		}
	}
}
