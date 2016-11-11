#ifndef MAX7219_H_
#define MAX7219_H_

#include "stm32f1xx_hal.h"
#include "main.h"


#define CS_ON()		 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);												
#define CS_OFF()     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);

#define MAX7219_NoOperation						0x00
#define MAX7219_DecodeMode						0x09
#define MAX7219_Intensity					    0x0A
#define MAX7219_ScanLimit					    0x0B
#define MAX7219_ShutDown					    0x0C
#define MAX7219_DisplayTest						0x0F




void MAX7219_Init (SPI_HandleTypeDef *hspi);
void MAX7219_SetIntNumber (SPI_HandleTypeDef *hspi,signed long value);
void MAX7219_SetFloatNumber (SPI_HandleTypeDef *hspi,float value,char Accuracy);
void MAX7219_SetDisplayTest (SPI_HandleTypeDef *hspi,unsigned char Enable);
void MAX7219_SetIntensity (SPI_HandleTypeDef *hspi,unsigned char Intensity);
void MAX7219_SetPowerState (SPI_HandleTypeDef *hspi,unsigned char PowerState);
void MAX7219_SetScanLimit (SPI_HandleTypeDef *hspi,unsigned char Limit);
void MAX7219_NoOp (SPI_HandleTypeDef *hspi);
void MAX7219_Clear (SPI_HandleTypeDef *hspi);
void MAX7219_SetDecodeMode (SPI_HandleTypeDef *hspi,unsigned char Decode);

#endif /* MAX7219_H_ */
