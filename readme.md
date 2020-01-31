/*
��������Ϊһ24C02
*/
//#include "includes.h"
#include "stm32f10x.h"
#include "i2c_ee.h"
#include "string.h"
#include "my.h"

#define AT24C01A		//24C01A,I2Cʱ��������24C02һ��
//#define AT24C01		//24C01,I2C��ʱ�����ͨ���е㲻ͬ

#define EEPROM_ADDR		0xA0
#define I2C_PAGESIZE	64		//24C01/01Aҳ������4��

//void delay1ms(int m)
//{
//u16 j,jj;
//	for(jj=0;jj<m;jj++)
//	{
//		for(j=0;j<1330;j++);
//	}
//}

void I2C_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);

	/* PB6,7 SCL and SDA */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    I2C_DeInit(I2C1);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x30;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;//50K�ٶ�	   100000
    
	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
	/*����1�ֽ�1Ӧ��ģʽ*/
	I2C_AcknowledgeConfig(I2C1, ENABLE);

}

/***************************************************
**������:I2C_ReadS
**����:��ȡ24C02����ֽ�
**ע������:24C02��256�ֽ�,8λ��ַ,A0-A2�̶�Ϊ0,��������ַΪEEPROM_ADDR
***************************************************/
void I2C_ReadS_24C(u16 addr ,u8* pBuffer,u16 no)
{
    if(no==0)
		return;
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
		
	/*����1�ֽ�1Ӧ��ģʽ*/
	I2C_AcknowledgeConfig(I2C1, ENABLE);


	/* ������ʼλ */
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,��ģʽ*/

#ifdef AT24C01A	
    /*����������ַ(д)*/
//  	I2C_Send7bitAddress(I2C1, EEPROM_ADDR+ (u8)((addr>>7)&0x0E), I2C_Direction_Transmitter);
//  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
   I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  	/*���͵�ַ*/
	I2C_SendData(I2C1, (u8)(addr>>8));
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	/*���͵�ַ*/
	I2C_SendData(I2C1, (u8)(addr&0xff));
   while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));/*�����ѷ���*/

		
	/*��ʼλ*/
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	/*������*/
	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	
#else	
	/*����������ַ(��)24C01*/
	I2C_Send7bitAddress(I2C1, addr<<1, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
#endif
	
    while (no)
    {
		if(no==1)
		{
     		I2C_AcknowledgeConfig(I2C1, DISABLE);	//���һλ��Ҫ�ر�Ӧ���
    		I2C_GenerateSTOP(I2C1, ENABLE);			//����ֹͣλ
		}
	    
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)); /* EV7 */
	    *pBuffer = I2C_ReceiveData(I2C1);
	    pBuffer++;
	    /* Decrement the read bytes counter */
	    no--;
    }
	//�ٴ�����Ӧ��ģʽ
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}

/****************************************************
**������:I2C_Standby_24C
**����:24C�Ƿ�׼������д����ж�
**ע������:��������������Ϊ:��æ
****************************************************/
void I2C_Standby_24C(void)      
{
  vu16 SR1_Tmp;
  do
  {
    /*��ʼλ*/
    I2C_GenerateSTART(I2C1, ENABLE);
    /*��SR1*/
    SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
    /*������ַ(д)*/
    #ifdef AT24C01A
	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
	#else
	I2C_Send7bitAddress(I2C1, 0, I2C_Direction_Transmitter);
	#endif
  }while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));
  
  /**/
  I2C_ClearFlag(I2C1, I2C_FLAG_AF);
  /*ֹͣλ*/    
  I2C_GenerateSTOP(I2C1, ENABLE);
}

/*************************************************
**������:I2C_ByteWrite_24C
**����:дһ���ֽ�
**ע������:��д��ͬ����Ҫ����æ�ж�
*************************************************/
void I2C_ByteWrite_24C(u16 addr,u8 dat)
{
  /* ��ʼλ */
  	I2C_GenerateSTART(I2C1, ENABLE);
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));  

#ifdef AT24C01A
  	/* ����������ַ(д)*/
//  	I2C_Send7bitAddress(I2C1, EEPROM_ADDR+ (u8)((addr>>7)&0x0E), I2C_Direction_Transmitter);
//  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
   I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  	/*���͵�ַ*/
	I2C_SendData(I2C1, (u8)(addr>>8));
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(I2C1, (u8)(addr&0xff));
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
#else	
	I2C_Send7bitAddress(I2C1, addr<<1, I2C_Direction_Transmitter);
 	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
#endif

	/* дһ���ֽ�*/
  	I2C_SendData(I2C1, dat); 
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
  	/* ֹͣλ*/
  	I2C_GenerateSTOP(I2C1, ENABLE);
  
  	I2C_Standby_24C();
}

/*************************************************
**������:I2C_PageWrite_24C
**����:д��һҳ(����)
**ע������:�˺�����Ⱥд�����
*************************************************/
extern void delayms(u16 t);
void I2C_PageWrite_24C(u16 addr,u8* pBuffer, u8 no)
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

	/*��ʼλ*/
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); 

#ifdef AT24C01A
	/*������ַ(д)*/
//  	I2C_Send7bitAddress(I2C1, EEPROM_ADDR+ (u8)((addr>>7)&0x0E), I2C_Direction_Transmitter);
//  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
   I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  	/*���͵�ַ*/
	I2C_SendData(I2C1, (u8)(addr>>8));
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));


	I2C_SendData(I2C1, (u8)(addr&0xff));
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
 
#else
	I2C_Send7bitAddress(I2C1, addr<<1, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); 
#endif
	while(no--)  
	{
	  I2C_SendData(I2C1, *pBuffer); 
	  pBuffer++; 
	  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	
	/*ֹͣλ*/
	I2C_GenerateSTOP(I2C1, ENABLE);
		delayms(30);
}



/*************************************************
**������:I2C_WriteS_24C
**����:ҳд��24C
**ע������:24C02�������һ��д��8���ֽ�
*************************************************/
void I2C_WriteS_24C(u16 addr, u8 *src, u16 n)
{

	 u16 leng;
	  u8 page_len;
	  u16 i;
	//1.�Ȱ�ҳ������Ĳ���д��
	leng=addr%I2C_PAGESIZE + n;
 if(leng<=I2C_PAGESIZE)
  {
    I2C_PageWrite_24C(addr, src,  n);
  }
  else
  {//page> I2C_PAGESIZE
    page_len=I2C_PAGESIZE-addr%I2C_PAGESIZE;
    I2C_PageWrite_24C(addr, src, page_len); //frist page
    leng=n-page_len;
    
    i=0;
    while(1)
    {
      if(leng<I2C_PAGESIZE) break;
      I2C_PageWrite_24C(addr+page_len+(u16)i*I2C_PAGESIZE, src+page_len+(u16)i*I2C_PAGESIZE,  I2C_PAGESIZE); 
      i++;
      leng=leng-I2C_PAGESIZE;
          
    }
      I2C_PageWrite_24C(addr+page_len+(u16)i*I2C_PAGESIZE, src+page_len+(u16)i*I2C_PAGESIZE, leng);      
  }
}


//������
void I2C_Test(u16 adr)
{
	u8 i;
	char I2c_Buf[128];
	u16 add=adr;
	I2C_Standby_24C();
	strcpy(I2c_Buf,"abcdefghijklmnopqrstuvwxyz\n");
	i= strlen(I2c_Buf);
	I2C_WriteS_24C(add,(u8*)I2c_Buf,strlen(I2c_Buf));
	//��仺��
	for(i=0;i<128;i++)
		I2c_Buf[i]=0;
//	bzero(I2c_Buf,128);
	I2C_ReadS_24C(add,(u8*)I2c_Buf,27);
	USART1Write((u8*)I2c_Buf,27);

	//��仺��
	for(i=0;i<128;i++)
		I2c_Buf[i]=i;
		
	//д
	I2C_WriteS_24C(adr,(u8*)I2c_Buf,128);
	
	//�建��
	for(i=0;i<128;i++)
		I2c_Buf[i]=0;
		
	//��
	I2C_ReadS_24C(adr,(u8*)I2c_Buf,128);
	
	for(i=1;i<120;i++)
	{	
		if(I2c_Buf[i]!=i)
		{
			USART1Write("Write I2C is wrong\n",sizeof("Write I2C is wrong\n"));
		//	while(1);
		}
	}
	 USART1Write("Write I2C is OK\n",sizeof("Write I2C is OK\n"));

}
