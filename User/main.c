#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_it.h"


uint16_t g_autoreload = 1001;
//配置TIM8和TIM7这两个基本定时器的时基单元
void TIM_conf(void)
{
	TIM_TimeBaseInitTypeDef TIM_Struct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7 , ENABLE);
	
	TIM_Struct.TIM_ClockDivision = 0;									//注意：在通用定时器中，有一个时钟分频因子CKD（TIMx_CR1[9:8]），而基本定时器没有。
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up;	//基本定时器只有向上计数模式
	TIM_Struct.TIM_Prescaler = 7200 - 1;							//设置PSC预分频系数为7200
	TIM_Struct.TIM_RepetitionCounter = 2;
	/*
	计算自动重装载寄存器的数值：
		由于TIMxCLK = 72MHz，预分频系数为7200，
		TIM_Period = TIMxARR = CK_CNT（Hz）x 定时时间（s）（定时时间也是两次中断之间的时间）
		=TIMxCLK/(TIM_Prescaler+1)（Hz） x 定时时间（s）
	我们让定时时间设为1s，	TIM_Period = TIMxARR = 10000；
	*/
	TIM_Struct.TIM_Period = g_autoreload;
	TIM_Struct.TIM_RepetitionCounter = 0;							//这个是重复计数寄存器(TIMx_RCR)，只有高级定时器才有，每溢出一次，该值减一，重复计数达到0的时候产生计数器上溢/下溢时更新事件(UEV)
	
	TIM_ARRPreloadConfig(TIM8,ENABLE);
	TIM_TimeBaseInit(TIM6,&TIM_Struct);
	
	//tim7的定时时间为2s
	TIM_Struct.TIM_Period = 20000;
	TIM_TimeBaseInit(TIM7,&TIM_Struct);
	
	
	TIM_ClearFlag(TIM8, TIM_IT_Update);
	TIM_ITConfig(TIM8, TIM_IT_Update,ENABLE);
	
	
	TIM_ClearFlag(TIM7, TIM_IT_Update);
	TIM_ITConfig(TIM7, TIM_IT_Update,ENABLE);
	
	TIM_Cmd(TIM8,ENABLE);
	TIM_Cmd(TIM7,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 , DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7 , DISABLE);
}

void NVIC_conf(void)
{
	NVIC_InitTypeDef NVIC_Struct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_Struct.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Struct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Struct);
	
	NVIC_Struct.NVIC_IRQChannel = TIM8_UP_IRQn;
	NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Struct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Struct);
}

void GPIO_conf(void)
{
	GPIO_InitTypeDef GPIO_Struct;
	
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Struct);
	
	GPIO_Struct.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOC,&GPIO_Struct);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);

}

void RCC_conf(void)
{
	SystemInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
}
/*
void TIM6_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update) == SET )
	{
		GPIO_WriteBit(GPIOB,GPIO_Pin_0,Bit_SET - GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_0));
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
	}
}

void TIM7_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM7,TIM_IT_Update) == SET )
	{
		GPIO_WriteBit(GPIOC,GPIO_Pin_4,Bit_SET - GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_4));
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
	}
}
*/
//Program Size: Code=660 RO-data=320 RW-data=0 ZI-data=1024  
int main()
{
	RCC_conf();
	GPIO_conf();
	TIM_conf();
	NVIC_conf();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
	while(1)
		;
}
