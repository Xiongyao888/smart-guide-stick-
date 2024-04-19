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

uint8_t tempH, tempL;		  // 温度整数，小数
uint8_t humiH, humiL;		  // 湿度整数，小数
u8 alarmFlag = 0;			  // 报警标志
u8 alarm_is_free = 10;		  // 判断报警器是否空闲
float light = 0;			  // 光照度
_Bool LED_Status = 0;		  // 灯状态
char PUB_BUF[256];			  // 上传数据buf
unsigned short timeCount = 0; // 发送间隔变量

u8 Strightline = 0; // 若长时间没有障碍物，用于安抚盲人心情，提醒一路畅通，继续前行--》比如3分钟没有障碍物则提醒
u8 Whole_time = 0;	// 用于统计未对白天黑夜播报时间，到一定时间后，对语音播报次数清0，以便达到定时提醒白天黑夜
u8 Disnum = 30;		// 超声波报警距离
u8 t = 20;			// trig引脚高电平持续时间
u8 Distance;		// 超声波测得的距离
u8 buff[64];		// 用于字符串拼接，发送到手机端显示
u8 disWarn = 0;
u8 time = 0;

const char *topics[] = {"/wonderfulhome/sub"};	 // 接受
const char devPubtopic[] = "/wonderfulhome/pub"; // 发布
static char SETVOLUME[] = {"AA 13 01 1C D2"};	 // 设置音量

int main(void)
{

	unsigned char *dataPtr = NULL;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();		 // 延时函数初始化
	Usart1_Init(9600);	 // Debug串口
	Usart2_Init(115200); // esp8266通讯串口
	LED_Init();			 // 初始化与LED连接的硬件接口
	DHT11_Init();
	BH1750_Init();
	TIM2_Int_Init(4999, 7199);
	TIM3_Int_Init(2499, 7199);
	OLED_Init();
	OLED_Clear();
	KEY_Init();
	EXTIX_Init();
	Beep_Init();
	infrared_INIT1();			// 右前方障碍物检测模块初始化	OUT->PA6
	infrared_INIT2();			// 左前方障碍物检测模块初始化	OUT->PA5
	Wave_SRD_Init();			// 初始化超声波   Trig->PB7  Echo->PB6     Trig更改为PE7      Echo更改为PE8
	Timer_SRD_Init(9998, 7199); // 定时器初始化
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

		// 获取温湿度
		DHT11_Read_Data(&humiH, &humiL, &tempH, &tempL);
		// UsartPrintf(USART1,"当前温度:%d.%d  湿度:%d.%d\r\n",tempH,tempL,humiH,humiL);

		// 获取光强
		if (!i2c_CheckDevice(BH1750_Addr))
		{
			light = LIght_Intensity();
			// UsartPrintf(USART1,"当前光照强度:%.2f lx\r\n",light);
		}

		if (alarm_is_free >= 10) // 报警器是否空闲
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

		// 产生高于10Us的高电平与OLED上显示超声波所测距离
		Wave_SRD_Strat(t);
		Distance = TIM_GetCounter(TIM4) * 340 / 200.0;
		// 障碍物检测报警部分
		if (timeCount <= 180) // 若长时间没有障碍物（考虑实际演示，设置的是3min），提醒盲人前方一路畅通
		{

			if (Distance > 0)
			{

				delay_ms(10);
				if (Distance < Disnum) // 检测到障碍物距离小于30cm时报警
				{

					disWarn = 1; // 发送到手机端
					LED1 = 0;	 // 开警示灯
					playsong(2); // 语音提醒正前方有障碍物
					delay_ms(1000);
					delay_ms(1000);
					delay_ms(1000);
					LED1 = 1; // 关灯
					//					BEEP=0;
					Strightline = 0; // 有障碍物，计数清0
				}
				else
				{
					Strightline++; // 没有障碍物，计数
				}
			}
			if (Detect2() == 0) // 左前方障碍物检测   PA6
			{
				LED1 = 0;
				//				BEEP=0;

				disWarn = 2; // 发送到手机端
				delay_ms(10);
				playsong(3);
				delay_ms(1000);
				delay_ms(1000);
				delay_ms(1000);
				LED1 = 1;
				//				BEEP=1;
				Strightline = 0; // 有障碍物，计数清0
			}
			else if (Detect1() == 0) // 右前方障碍物检测   PA5
			{
				LED1 = 0;
				//				BEEP=0;
				disWarn = 3; // 发送到手机端
				playsong(4);
				delay_ms(1000);
				delay_ms(1000);
				delay_ms(1000);
				LED1 = 1;
				//				BEEP=0;
				Strightline = 0; // 有障碍物，计数清0
			}
			else
			{
				Strightline++; // 没有障碍物，计数
			}
			delay_ms(1000);
		}
		else
		{

			disWarn = 0; // 发送到手机端
			playsong(7); // 播放语音提醒前方一路畅通
			delay_ms(1000);
			delay_ms(1000);
			delay_ms(1000);
			Strightline = 0;
		}
	}
}
