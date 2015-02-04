#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_it.h"


uint16_t g_autoreload = 1001;
//����TIM8��TIM7������������ʱ����ʱ����Ԫ
void TIM_conf(void)
{
	TIM_TimeBaseInitTypeDef TIM_Struct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7 , ENABLE);
	
	TIM_Struct.TIM_ClockDivision = 0;									//ע�⣺��ͨ�ö�ʱ���У���һ��ʱ�ӷ�Ƶ����CKD��TIMx_CR1[9:8]������������ʱ��û�С�
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up;	//������ʱ��ֻ�����ϼ���ģʽ
	TIM_Struct.TIM_Prescaler = 7200 - 1;							//����PSCԤ��Ƶϵ��Ϊ7200
	TIM_Struct.TIM_RepetitionCounter = 2;
	/*
	�����Զ���װ�ؼĴ�������ֵ��
		����TIMxCLK = 72MHz��Ԥ��Ƶϵ��Ϊ7200��
		TIM_Period = TIMxARR = CK_CNT��Hz��x ��ʱʱ�䣨s������ʱʱ��Ҳ�������ж�֮���ʱ�䣩
		=TIMxCLK/(TIM_Prescaler+1)��Hz�� x ��ʱʱ�䣨s��
	�����ö�ʱʱ����Ϊ1s��	TIM_Period = TIMxARR = 10000��
	*/
	TIM_Struct.TIM_Period = g_autoreload;
	TIM_Struct.TIM_RepetitionCounter = 0;							//������ظ������Ĵ���(TIMx_RCR)��ֻ�и߼���ʱ�����У�ÿ���һ�Σ���ֵ��һ���ظ������ﵽ0��ʱ���������������/����ʱ�����¼�(UEV)
	
	TIM_ARRPreloadConfig(TIM8,ENABLE);
	TIM_TimeBaseInit(TIM6,&TIM_Struct);
	
	//tim7�Ķ�ʱʱ��Ϊ2s
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
