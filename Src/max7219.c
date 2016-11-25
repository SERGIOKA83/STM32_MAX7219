#include "MAX7219.h"

#define MaxDigits			    0x08
#define MaxArraySize			2
#define NumSpi			        SPI2

/* You can manualy assign the value of chips in serialy connected here*/
unsigned char ChipsInSerial=0;	//otherwise search procedure will be needed and loop must be closed
volatile uint8_t complete=0;
unsigned char Digits=8;
unsigned char DecModFlag=0;

//uint8_t complete=1;
unsigned char StringLen (char * String);
void uLongToStr (unsigned long Value, char * String);
void sLongToStr (signed long Value, char * String);
void FloatToString (float Value, char * String, char Accuracy);
void MAX7219_WriteReg (SPI_HandleTypeDef *hspi,uint8_t Reg, uint8_t Data);
void MAX7219_WriteReg (SPI_HandleTypeDef *hspi,uint8_t Reg, uint8_t Data)
{
    while(!complete);
	complete = 0;
	uint8_t RegData[MaxArraySize]; // 0 - register, 1 - Data
    RegData[0]=Reg;
    RegData[1]=Data;
    
    CS_OFF();
  
   // HAL_SPI_Transmit(hspi ,&RegData[0],1,5);//РАБОТАЕТ
   // HAL_SPI_Transmit(hspi ,&RegData[1],1,5);//РАБОТАЕТ
   
    HAL_SPI_Transmit_DMA(hspi ,RegData,MaxArraySize);
 
		

  
	CS_ON();
	
}

void MAX7219_Init (SPI_HandleTypeDef *hspi)
{   
    HAL_Delay(50);
	MAX7219_SetDisplayTest(hspi,0);
	MAX7219_SetPowerState (hspi,0x01);
    MAX7219_SetScanLimit (hspi,0x07);
    MAX7219_SetIntensity (hspi,0x07);
    MAX7219_SetDecodeMode (hspi,0x00);
    MAX7219_Clear (hspi);
}

void MAX7219_SetIntNumber (SPI_HandleTypeDef *hspi,signed long value)
{    char StrFl[MaxDigits]={0};
     
     unsigned char Di=MaxDigits;
     unsigned char CNT;

     sLongToStr (value, StrFl);
    CNT = StringLen (StrFl);
    MAX7219_Clear (hspi);
    for (unsigned char DigitCNT=0x01;  DigitCNT<=MaxDigits; DigitCNT++)
	{    Di-=1;
       if (Di<CNT){    
	      MAX7219_WriteReg(hspi,DigitCNT, StrFl[Di]);
       }
       else{
          MAX7219_WriteReg(hspi,DigitCNT,0x0F);               
       }
    
	}
}    

void MAX7219_SetFloatNumber (SPI_HandleTypeDef *hspi, float value,char Accuracy)
{
	char StrFl[MaxDigits]={0};
    unsigned char Di=MaxDigits;
   volatile unsigned char CNT;
    
    FloatToString (value, StrFl, Accuracy);
    CNT = StringLen (StrFl);
    MAX7219_Clear (hspi);
	for (unsigned char DigitCNT=0x01;  DigitCNT<=MaxDigits; DigitCNT++)
	{    Di-=1;
       if (Di<CNT){
	   MAX7219_WriteReg(hspi,DigitCNT, StrFl[Di]);
            }
       else{
          MAX7219_WriteReg(hspi,DigitCNT,0x0F);               
       }
     
	}
}

void MAX7219_SetDisplayTest (SPI_HandleTypeDef *hspi,unsigned char Enable)
{
	MAX7219_WriteReg(hspi,MAX7219_DisplayTest, Enable);
}

void MAX7219_SetIntensity (SPI_HandleTypeDef *hspi,unsigned char Intensity)
{
	MAX7219_WriteReg(hspi,MAX7219_Intensity, Intensity);
}

void MAX7219_SetPowerState (SPI_HandleTypeDef *hspi,unsigned char PowerState)
{
	MAX7219_WriteReg(hspi,MAX7219_ShutDown, PowerState);
}

void MAX7219_SetScanLimit (SPI_HandleTypeDef *hspi,unsigned char Limit)
{
	MAX7219_WriteReg(hspi,MAX7219_ScanLimit, Limit);
}

void MAX7219_SetDecodeMode (SPI_HandleTypeDef *hspi,unsigned char Decode)
{
	MAX7219_WriteReg(hspi,MAX7219_DecodeMode, Decode);
    DecModFlag=Decode;
}

void MAX7219_NoOp (SPI_HandleTypeDef *hspi)
{
    
	MAX7219_WriteReg(hspi,MAX7219_NoOperation , 0x00);
   
}

void MAX7219_Clear (SPI_HandleTypeDef *hspi)
{
    if (DecModFlag==0)
     {
        for(unsigned char i=0x00; i<=Digits; i++)
        {
	      MAX7219_WriteReg(hspi,i , 0x00);
        }
     }
    else
     {
         for(unsigned char i=0x01; i<=Digits; i++)
        {
	      MAX7219_WriteReg(hspi,i , 0x0F);
        }
         
     }
}

void FloatToString (float Value, char * String, char Accuracy)
{
	signed long k = 1;
	signed long tmp; 
	char Len;
    char MD=MaxDigits;
    sLongToStr((signed long)Value, String);
	Len = StringLen(String);
    
    if (Accuracy>MD-(Len+1)) 
        Accuracy=MD-Len;
    
	while (Accuracy--) k = k * 10;
	if (Value<0)	tmp = ((signed long)Value)*k - (float)(Value*k); 
		else tmp = (float)(Value*k) - ((signed long)Value)*k;
	
	//String[Len++]='.'; 
	uLongToStr(tmp, String+Len);
    String[--Len]|=0x80;
  
   
}

void uLongToStr (unsigned long Value, char * String)
{
  char zero=0;
  char tmp=0;
  unsigned long cnt=1000000000;
  while (cnt!=1)
  {
    while (Value>=cnt)
    {
      Value-=cnt;
      tmp++;
    }
    if (tmp) zero=1;
    if (zero) * String++ = tmp+48;
    tmp=0;
    cnt/=10;
  }
  * String = Value+48;
}

void sLongToStr (signed long Value, char * String)
{
	unsigned long tmp;
	if (Value<0) 
	{
		Value=-Value;
		tmp=Value;
		//String[0]='-';
        String[0]=0x0A;
		uLongToStr(tmp, String+1);
	}
	else
	{
		tmp=Value;
		uLongToStr(tmp, String);
	}
}

unsigned char StringLen (char * String)
{
        char cnt=0;
        while (* String++) cnt++;
        return cnt;
}


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi2)
 {
     if (hspi2->Instance == NumSpi)
     {   
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
      complete = 1;
     }   
 }
