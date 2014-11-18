#include <SPI23LCXXXX.h>

GPIO_InitTypeDef G;
SPI_InitTypeDef S;

uint8_t WriteMode = 0;

typedef union{
	uint32_t Int;
	float Flt;
} Type;

typedef union{
	uint32_t *IntP;
	float *FltP;
} TypeP;

static void SPI_WaitFlags(uint8_t TxRx){
	while(SPI_I2S_GetFlagStatus(S_SPI, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(S_SPI, SPI_I2S_FLAG_BSY) == SET);
	if(!TxRx){
		while(SPI_I2S_GetFlagStatus(S_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	}
	return;
}

static void SPI_SB(uint8_t Dat){
	uint8_t DatB;
	SPI_SendData8(S_SPI, Dat);
	SPI_WaitFlags(1);
	DatB = SPI_ReceiveData8(S_SPI);
}

static uint8_t SPI_RB(void){
	uint8_t Dat;
	SPI_SendData8(S_SPI, 0xFF);
	SPI_WaitFlags(0);
	Dat = SPI_ReceiveData8(S_SPI);
}

void S23LC_Init(void){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	G.GPIO_Pin = S_MISO | S_MOSI | S_CLK;
	G.GPIO_OType = GPIO_OType_PP;
	G.GPIO_Mode = GPIO_Mode_AF;
	G.GPIO_PuPd = GPIO_PuPd_UP;
	G.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(S_GPIO, &G);

	GPIO_PinAFConfig(S_GPIO, S_CLKPS, S_GPIOAF);
	GPIO_PinAFConfig(S_GPIO, S_MISOPS, S_GPIOAF);
	GPIO_PinAFConfig(S_GPIO, S_MOSIPS, S_GPIOAF);

	G.GPIO_Pin = S_NS;
	G.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(S_GPIO, &G);

	S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	S.SPI_CPHA = SPI_CPHA_1Edge;
	S.SPI_CPOL = SPI_CPOL_Low;
	S.SPI_DataSize = SPI_DataSize_8b;
	S.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	S.SPI_FirstBit = SPI_FirstBit_MSB;
	S.SPI_Mode = SPI_Mode_Master;
	S.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(S_SPI, &S);

	SPI_RxFIFOThresholdConfig(S_SPI, SPI_RxFIFOThreshold_QF);
	SPI_Cmd(S_SPI, ENABLE);

	S23_IOAccess(Single);
	S23_WriteModeReg(0x00);
}

void S23_IOAccess(S23_Access S){
	GPIO_ResetBits(S_GPIO, S_NS);

	switch(S){
	case Single:
		SPI_SB(S_RSTIO);
		break;
	case Dual:
		SPI_SB(S_EDIO);
		break;
	case Quad:
		SPI_SB(S_EQIO);
		break;
	}

	GPIO_SetBits(S_GPIO, S_NS);
}

uint8_t S23_RByte(uint32_t Addr){
	uint8_t Dat;

	GPIO_ResetBits(S_GPIO, S_NS);

	//Clear FIFO
	while(SPI_GetReceptionFIFOStatus(S_SPI) != SPI_ReceptionFIFOStatus_Empty){
		SPI_ReceiveData8(S_SPI);
	}

	SPI_SB(S_READ);

	SPI_SB(Addr>>16);
	SPI_SB((Addr>>8)&255);
	SPI_SB(Addr&255);

	Dat = SPI_RB();

	GPIO_SetBits(S_GPIO, S_NS);
	return Dat;
}

uint16_t S23_RWord(uint32_t Addr){
	uint16_t Dat;

	if(WriteMode != 0x40){
		S23_WriteModeReg(0x40);
	}

	GPIO_ResetBits(S_GPIO, S_NS);

	//Clear FIFO
	while(SPI_GetReceptionFIFOStatus(S_SPI) != SPI_ReceptionFIFOStatus_Empty){
		SPI_ReceiveData8(S_SPI);
	}

	SPI_SB(S_READ);

	SPI_SB(Addr>>16);
	SPI_SB((Addr>>8)&255);
	SPI_SB(Addr&255);

	Dat = SPI_RB()<<8;
	Dat |= SPI_RB();

	GPIO_SetBits(S_GPIO, S_NS);
	return Dat;
}

uint32_t S23_RDWord(uint32_t Addr){
	uint32_t Dat;

	if(WriteMode != 0x40){
		S23_WriteModeReg(0x40);
	}

	GPIO_ResetBits(S_GPIO, S_NS);

	//Clear FIFO
	while(SPI_GetReceptionFIFOStatus(S_SPI) != SPI_ReceptionFIFOStatus_Empty){
		SPI_ReceiveData8(S_SPI);
	}

	SPI_SB(S_READ);

	SPI_SB(Addr>>16);
	SPI_SB((Addr>>8)&255);
	SPI_SB(Addr&255);

	Dat = SPI_RB()<<24;
	Dat |= (SPI_RB()<<16);
	Dat |= (SPI_RB()<<8);
	Dat |= SPI_RB();

	GPIO_SetBits(S_GPIO, S_NS);
	return Dat;
}

float S23_RFloat(uint32_t Addr){
	Type N;
	N.Int = S23_RDWord(Addr);

	return N.Flt;
}

void S23_WByte(uint32_t Addr, uint8_t Dat){
	GPIO_ResetBits(S_GPIO, S_NS);

	SPI_SB(S_WRITE);

	SPI_SB(Addr>>16);
	SPI_SB((Addr>>8)&255);
	SPI_SB(Addr&255);

	SPI_SB(Dat);

	GPIO_SetBits(S_GPIO, S_NS);
}

void S23_WWord(uint32_t Addr, uint16_t Dat){
	if(WriteMode != 0x40){
		S23_WriteModeReg(0x40);
	}

	GPIO_ResetBits(S_GPIO, S_NS);

	SPI_SB(S_WRITE);

	SPI_SB(Addr>>16);
	SPI_SB((Addr>>8)&255);
	SPI_SB(Addr&255);

	SPI_SB(Dat>>8);
	SPI_SB(Dat);

	GPIO_SetBits(S_GPIO, S_NS);
}

void S23_WDWord(uint32_t Addr, uint32_t Dat){
	if(WriteMode != 0x40){
		S23_WriteModeReg(0x40);
	}

	GPIO_ResetBits(S_GPIO, S_NS);

	SPI_SB(S_WRITE);

	SPI_SB(Addr>>16);
	SPI_SB((Addr>>8)&255);
	SPI_SB(Addr&255);

	SPI_SB(Dat>>24);
	SPI_SB((Dat>>16)&255);
	SPI_SB((Dat>>8)&255);
	SPI_SB(Dat&255);

	GPIO_SetBits(S_GPIO, S_NS);
}

void S23_WFloat(uint32_t Addr, float Dat){
	Type N;

	N.Flt = Dat;

	S23_WDWord(Addr, N.Int);
}

void S23_WriteModeReg(uint8_t Dat){
	GPIO_ResetBits(S_GPIO, S_NS);

	WriteMode = Dat;

	SPI_SB(S_WRMR);
	SPI_SB(Dat);

	GPIO_SetBits(S_GPIO, S_NS);
}

uint8_t S23_ReadModeReg(void){
	uint8_t Dat;

	//Clear FIFO
	while(SPI_GetReceptionFIFOStatus(S_SPI) != SPI_ReceptionFIFOStatus_Empty){
		SPI_ReceiveData8(S_SPI);
	}

	GPIO_ResetBits(S_GPIO, S_NS);

	SPI_SB(S_RDMR);
	Dat = SPI_RB();

	WriteMode = Dat;

	GPIO_SetBits(S_GPIO, S_NS);
	return Dat;
}

void S23_SpeedTest(void){
	uint32_t Cnt;
	S23_WriteModeReg(0x40);

	GPIO_ResetBits(S_GPIO, S_NS);

	SPI_SB(S_WRITE);

	SPI_SB(0);
	SPI_SB(0);
	SPI_SB(0);

	for(Cnt = 0; Cnt<S_SIZE; Cnt++){
		SPI_SB(Cnt);
	}

	GPIO_SetBits(S_GPIO, S_NS);
}

