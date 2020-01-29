#include "stm32f10x.h"

#define DS_595_HIGH()     GPIO_SetBits(GPIOB,GPIO_Pin_0);
#define DS_595_LOW()      GPIO_ResetBits(GPIOB,GPIO_Pin_0);

#define SCK_595_HIGH()    GPIO_SetBits(GPIOB,GPIO_Pin_1);	  //Ê±ÖÓ
#define SCK_595_LOW()     GPIO_ResetBits(GPIOB,GPIO_Pin_1);

#define RCK_595_HIGH()    GPIO_SetBits(GPIOC,GPIO_Pin_5);
#define RCK_595_LOW()     GPIO_ResetBits(GPIOC,GPIO_Pin_5);

void Led_Control_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
						   
   	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC, ENABLE );
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

//	GPIO_ResetBits(GPIOD, GPIO_Pin_13);//Ô¤ÖÃÎªµÍ

}
void delaytim(u8 data)
{
 int i,j;
   for(i=0;i<data;i++)
   {
	   for(j=0;j<5;j++);
   }
}
u8 led_str[5]={0,0,0,0,0};
//u8 led_str[5]={0xff,0xff,0xff,0xff,0xff};
void Out595(u8 *str)
{
u8 Reg,i,j;
u8 *p;
    p = str;
	RCK_595_LOW()
	SCK_595_LOW()
	delaytim(1);
	for(j=0;j<5;j++)
	{	
		Reg = *p;
		for(i=0;i<8;i++)
		{
			delaytim(1);
		    if(Reg&0x1)DS_595_HIGH()
			else DS_595_LOW() 
			Reg>>=1;	
			delaytim(1);
		    SCK_595_HIGH()
			delaytim(1);
			SCK_595_LOW()	
		} 
		 p++;
	}
	 RCK_595_HIGH()
	 delaytim(1);
	 RCK_595_LOW()
}
void Led_74ls595(u8 light, u8 channel)	  //light =0 µÆÃð £½1 µÆÁÁ   channel Ñ¡ÔñÄÄÒ»Â·	 1-40
{
u8 chan =(channel-1)/8;
u8 led =!light;
u8 residue = (channel-1)%8;

	  led_str[chan] = led_str[chan]&(~(1<<residue)) | (led<<residue);
	  Out595(led_str);

}



