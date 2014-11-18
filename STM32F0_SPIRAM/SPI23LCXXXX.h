#ifndef SPI23LCXXXX_H
#define SPI23LCXXXX_H

#include <stm32f0xx_spi.h>
#include <stm32f0xx_rcc.h>
#include <stm32f0xx_gpio.h>

#define S_NS		GPIO_Pin_4
#define S_CLK		GPIO_Pin_5
#define S_MISO		GPIO_Pin_6
#define S_MOSI		GPIO_Pin_7

#define S_CLKPS		GPIO_PinSource5
#define S_MISOPS	GPIO_PinSource6
#define S_MOSIPS	GPIO_PinSource7

#define S_GPIO		GPIOA
#define S_GPIOAF	GPIO_AF_0
#define S_SPI		SPI1

#define S_READ		0x03
#define S_WRITE		0x02
#define S_EDIO		0x3B
#define S_EQIO		0x38
#define S_RSTIO		0xFF
#define S_RDMR		0x05
#define S_WRMR		0x01

#define S_SIZE		0x1FFFF

typedef enum{
	Single,
	Dual,
	Quad
} S23_Access;

void S23LC_Init(void);

void S23_WByte(uint32_t, uint8_t);
void S23_WWord(uint32_t, uint16_t);
void S23_WDWord(uint32_t, uint32_t);
void S23_WFloat(uint32_t, float);

void S23_SpeedTest(void);

uint8_t S23_RByte(uint32_t);
uint16_t S23_RWord(uint32_t);
uint32_t S23_RDWord(uint32_t);
float S23_RFloat(uint32_t);

void S23_IOAccess(S23_Access);
void S23_WriteModeReg(uint8_t);
uint8_t S23_ReadModeReg(void);

#endif
