# stm32实现智能导盲杖系统
  考虑到生活中日趋升高的盲人道路事故率以及传统导盲杖的局限性，我们提出了智能导盲杖的概念，以满足使用者对导盲杖功能性、安全性的要求。该款智能导盲杖主体以市场上现有的普通导盲杖为基础，主控模块使用基ARM CortexTM-M3的932位微控制器STM32F103ZET6单片机，主要由GPS定位模块、GPRS无线通信模块、摔倒检测模块、毫米波检测模块、近距离自声光报警部分、语音播报提醒模块组成。
当智能导盲杖上的毫米波检测模块检测到前方障碍物距离过近，容易出现碰撞危险时，会自动发出语音播报提醒盲人调整移动方向。当置于智能导盲杖上的加速度检测模块或倾斜角度检测模块检测到数据异常时，系统进行摔倒检测并返回一系列的模拟电压值，当超过设定值后，由LED灯带和蜂鸣器组成的声光报警装置自动发出SOS声光警报提醒周围人，GPS模块自动定位采集盲人位置信息。同时GPRS也会通过无线通讯拨打电话和发送短消息提醒给监护人员，并实现远程报警求救。监护人员在接收到提醒后可在设备云上实时了解盲人的位置，节约救援时间，提高救援效率。
