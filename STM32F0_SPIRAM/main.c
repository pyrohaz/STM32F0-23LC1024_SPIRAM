#include <SPI23LCXXXX.h>

GPIO_InitTypeDef G;

int main(void)
{
	S23LC_Init();

	G.GPIO_Pin = GPIO_Pin_0;
	G.GPIO_OType = GPIO_OType_PP;
	G.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOA, &G);

	S23_WByte(0, 'A');
	S23_WWord(1, 1234);
	S23_WDWord(3, 12345678);
	S23_WFloat(7, 1.2345);

	uint8_t B;
	uint16_t W;
	uint32_t D, Cnt;
	float F;

	while(1)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
		S23_SpeedTest();

		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
	}
}
