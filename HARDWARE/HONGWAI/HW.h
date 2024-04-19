#ifndef __HW_H
#define __HW_H
#include "sys.h"
#define Detect1() GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)   //右前方障碍物检测
#define Detect2() GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)	//左前方障碍物检测

void infrared_INIT1(void);
void infrared_INIT2(void);


#endif
