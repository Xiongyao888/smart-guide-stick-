#include "jq8400.h"   
#include "usart.h"
 /*
  * @brief  初始化控制JQ8400的串口2
  */
//void USART3_Config(void)



void USART3_Init(u32 bound)
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART3_InitStructure;

	RCC_APB2PeriphClockCmd( USART3_RX_CLK | USART3_TX_CLK , ENABLE);//串口GPIO时钟
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3 ,ENABLE);//串口外设时钟

	GPIO_InitStructure.GPIO_Pin = USART3_TX_PIN;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(USART3_TX_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USART3_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(USART3_RX_PORT, &GPIO_InitStructure);	


	USART3_InitStructure.USART_BaudRate = bound;
	USART3_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx ;
	USART3_InitStructure.USART_Parity = USART_Parity_No;
	USART3_InitStructure.USART_StopBits = USART_StopBits_1 ;
	USART3_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART3_InitStructure.USART_WordLength = USART_WordLength_8b;

	USART_Init( USART3,&USART3_InitStructure);
		
	USART_Cmd( USART3,ENABLE);
		
}

/***************** 发送一个 32 位数 **********************/
void Usart3_Send32bit( USART_TypeDef * pUSARTx, uint32_t ch)
{
   uint8_t temp_1, temp_2, temp_3, temp_4; //4个8位

   /* 取出第一八位 */
   temp_1 = (ch&0XFF000000)>>24;
   /* 取出第二八位 */
   temp_2 = (ch&0X00FF0000)>>16;
	 /* 取出第三八位 */
   temp_3 = (ch&0X0000FF00)>>8;
   /* 取出第四八位 */
   temp_4 = (ch&0X000000FF);

   /* 发送第一八位 */
   USART_SendData(pUSARTx,temp_1);
   while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
   /* 发送第二八位 */
   USART_SendData(pUSARTx,temp_2);
   while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	 /* 发送第三八位 */
   USART_SendData(pUSARTx,temp_3);
   while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
   /* 发送第四八位 */
   USART_SendData(pUSARTx,temp_4);
   while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
 }

/***************** 发送一个 8 位数 **********************/ 
void Usart3_Send8bit( USART_TypeDef * pUSARTx, uint8_t eight)
{
   /* 发送一个字节数据到 USART */
   USART_SendData(pUSARTx,eight);

   /* 等待发送数据寄存器为空 */
   while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}
 
/***************** 循环模式：单曲停止 **********************/ 
void asong_stop(void)
{
	 Usart3_Send32bit( USART3, 0xAA180102);
	 Usart3_Send8bit( USART3, 0xC5);
}

/***************** 播放模式：指定歌曲（playsong(1)：播放00001曲目） **********************/ 
//曲目的文件名需要按指定格式来保存的，使用说明书上有写
/*
指定曲目(07)
指令：AA 07 02 曲目高 曲目低 SM
返回：无
和检验（SM）：为之前所有字节之和的低8位,即前面的数据相加后取低8位
例如:  AA 07 02 00 08 BB 指定播放当前盘符第8首（AA+07+02+00+08=BB取低8位，即为BB）
*/
void playsong(int i)
{
	switch(i)
	{
		case 1:	 
	    Usart3_Send32bit( USART1,0xAA070200);
	    Usart3_Send8bit( USART1, 0x01);
	    Usart3_Send8bit( USART1, 0xB4);	//控制指令要48位，故拆分开，32位+8位+8位
		  break;
		case 2:	 
	    Usart3_Send32bit( USART1,0xAA070200);
	    Usart3_Send8bit( USART1, 0x02);
	    Usart3_Send8bit( USART1, 0xB5);
		  break;
		case 3:	 
	    Usart3_Send32bit( USART1,0xAA070200);
	    Usart3_Send8bit( USART1, 0x03);
	    Usart3_Send8bit( USART1, 0xB6);
		  break;
		case 4:	 
	    Usart3_Send32bit( USART1,0xAA070200);
	    Usart3_Send8bit( USART1, 0x04);
	    Usart3_Send8bit( USART1, 0xB7);
		  break;
		case 5:	 
	    Usart3_Send32bit( USART1,0xAA070200);
	    Usart3_Send8bit( USART1, 0x05);
	    Usart3_Send8bit( USART1, 0xB8);
		  break;
		case 6:	 
	    Usart3_Send32bit( USART1,0xAA070200);
	    Usart3_Send8bit( USART1, 0x06);
	    Usart3_Send8bit( USART1, 0xB9);
		  break;
		case 7:	 
	    Usart3_Send32bit( USART1,0xAA070200);
	    Usart3_Send8bit( USART1, 0x07);
	    Usart3_Send8bit( USART1, 0xBA);
		  break;
		case 8:	 
	    Usart3_Send32bit( USART1,0xAA070200);
	    Usart3_Send8bit( USART1, 0x08);
	    Usart3_Send8bit( USART1, 0xBB);
		  break;
		case 9:	 
	    Usart3_Send32bit( USART1,0xAA070200);
	    Usart3_Send8bit( USART1, 0x09);
	    Usart3_Send8bit( USART1, 0xBC);
		  break;
		/*
		case 10:	 
	    Usart2_Send32bit( USART2,0xAA070200);
	    Usart2_Send8bit( USART2, 0x10);
	    Usart2_Send8bit( USART2, 0xBD);
		  break;
		
		
		default:
			Usart3_Send32bit( USART2,0xAA070200);
	    Usart2_Send8bit( USART2, 0x05);
	    Usart2_Send8bit( USART2, 0xB8);
		*/
		
	}

}

