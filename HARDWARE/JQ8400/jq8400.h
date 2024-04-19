#ifndef __JQ8400_H
#define	__JQ8400_H


#include "stm32f10x.h"

/*串口2总线*/

/*串口2GPIO*/
// RX
#define USART3_TX_PORT    	    GPIOB			              	/* GPIO端口 */
#define USART3_TX_CLK 	        RCC_APB2Periph_GPIOB			/* GPIO端口时钟 */
#define USART3_TX_PIN		        GPIO_Pin_11			        /* 连接到SCL时钟线的GPIO */

// TX
#define USART3_RX_PORT    	    GPIOB			              
#define USART3_RX_CLK 	        RCC_APB2Periph_GPIOB		
#define USART3_RX_PIN		        GPIO_Pin_10			        

// BUSY
//#define JQ8400_BUSY    	    GPIOC			                /* GPIO端口 */
//#define JQ8400_BUSY_CLK 	  RCC_APB1Periph_GPIOA				/* GPIO端口时钟 */
//#define JQ8400_BUSY_PIN		  GPIO_Pin_3			        /* 连接到SCL时钟线的GPIO */


void Usart3_Send8bit( USART_TypeDef * pUSARTx, uint8_t eight);
void USART3_Init(u32 bound);
void Usart3_Send32bit( USART_TypeDef * pUSARTx, uint32_t ch);
void asong_stop(void);
void playsong(int i);
void JQ8400_SetVolume(int volume);
#endif /* __JQ8400_H */

