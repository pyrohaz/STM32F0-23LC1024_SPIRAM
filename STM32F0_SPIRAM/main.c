#include <SPI23LCXXXX.h>

/*
 * Microchip 23LC1024 SPI RAM example program using
 * the STM32F0 Discovery board available from STMicroelectronics
 *
 * Author: Harris Shallcross
 * Year: 18/11/2014
 *
 *A simple example of using the Microchip 23LC1024 SPI RAM chip. A few functions are
 *included in the SPI23LCXXXX library and the library should hopefully work for all
 *23LCXXXX series of chips. 1MBit of data is available using the 23LC1024 IC as
 *defined in the SPI23LCXXXX.h file.
 *
 *Code and example descriptions can be found on my blog at:
 *www.hsel.co.uk
 *
 *The MIT License (MIT)
 *Copyright (c) 2014 Harris Shallcross
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all
 *copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *SOFTWARE.
 */

#define TestPin GPIO_Pin_0

GPIO_InitTypeDef G;

int main(void)
{
	//Initialize the SPI RAM!
	S23LC_Init();

	//Set up a test pin which is high for the speed test cycle
	G.GPIO_Pin = TestPin;
	G.GPIO_OType = GPIO_OType_PP;
	G.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOA, &G);

	//Write initial data to the SPI RAM or each size
	S23_WByte(0, 'A');
	S23_WWord(1, 1234);
	S23_WDWord(3, 12345678);
	S23_WFloat(7, 1.2345);

	//Variables to hold the readback data
	uint8_t B = 0;
	uint16_t W = 0;
	uint32_t D = 0, Cnt = 0;
	float F = 0.0f;

	while(1)
	{
		//Set test pin high!
		GPIO_SetBits(GPIOA, TestPin);

		//Execute speed test
		S23_SpeedTest();

		//Reset test pin
		GPIO_ResetBits(GPIOA, TestPin);

		//Using a logic analyzer, one can look at the duration that this pin is
		//high and use this to calculate the bit rate, using S_SIZE as the amount
		//of data transferred

		//Read back all the initially stored data
		B = S23_RByte(0);
		W = S23_RWord(1);
		D = S23_RDWord(3);
		F = S23_RFloat(7);

	}
}
