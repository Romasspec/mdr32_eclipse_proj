#include "define.h"

void rcc_init (void);
void gpio_init (void);
void can2_init(void);
void systic_init(void);
void blink(uint8_t n);

CAN_RxMsgTypeDef RxMsg;
CAN_TxMsgTypeDef TxMsg;
j1939msg_t CANmsg;
uint16_t temper = 0;
uint8_t temper_start = 0;


void (*task_ptr)(void);
void (*task_1ms_ptr)(void);
void (*task_5ms_ptr)(void);
void task_1ms(void);
void task_5ms(void);
void task_1ms_1(void);
void task_1ms_2(void);
void task_5ms_1(void);
void task_5ms_2(void);

// defined in linker script
extern unsigned int _sidata;
// Begin address for the .data section; defined in linker script
extern unsigned int _sdata;
// End address for the .data section; defined in linker script
extern unsigned int _edata;

// Begin address for the .bss section; defined in linker script
extern unsigned int __bss_start__;
// End address for the .bss section; defined in linker script
extern unsigned int __bss_end__;

inline void
__attribute__((always_inline))
__initialize_data (unsigned int* from, unsigned int* region_begin,
		   unsigned int* region_end)
{
  // Iterate and copy word by word.
  // It is assumed that the pointers are word aligned.
  unsigned int *p = region_begin;
  while (p < region_end)
    *p++ = *from++;
}

inline void
__attribute__((always_inline))
__initialize_bss (unsigned int* region_begin, unsigned int* region_end)
{
  // Iterate and clear word by word.
  // It is assumed that the pointers are word aligned.
  unsigned int *p = region_begin;
  while (p < region_end)
	*p++ = 0;
}

int main (void)
{
	uint8_t serial_number[8];
	__initialize_data(&_sidata, &_sdata, &_edata);
	__initialize_bss(&__bss_start__, &__bss_end__);

	rcc_init ();
	gpio_init ();
	systic_init();
	can2_init();

	task_ptr = &task_1ms;
	task_1ms_ptr = &task_1ms_1;
	task_5ms_ptr = &task_5ms_1;

	//while (ds18b20_init(MODE_SKIP_ROM));
	while (ds18b20_ReadRom_(serial_number));
	send_CAN(serial_number,8);

	delay_ms(100);

	ds18b20_init(MODE_SKIP_ROM);

	while (1)
	{
		(*task_ptr)();

		if (temper_start || !temper) {
			temper_start = 0;
			temper = ds18b20_Tread();
		}
	}
}

//=================================================================================
//  STATE-MACHINE SEQUENCING AND SYNCRONIZATION
//=================================================================================

//--------------------------------- FRAMEWORK -------------------------------------
void task_1ms(void)
{
	static uint32_t time_task = 0;
	uint32_t current_time = micros();
	if ((int32_t)(current_time-time_task) >= 0)
	{
		time_task = current_time + 1000;
		(*task_1ms_ptr)();
	}

	task_ptr = &task_5ms;
}

void task_5ms(void)
{
	static uint32_t time_task = 0;
	uint32_t current_time = micros();
	if ((int32_t)(current_time-time_task) >= 0)
	{
		time_task = current_time +  5000;
		(*task_5ms_ptr)();
	}
	task_ptr = &task_1ms;
}

//=================================================================================
//  A - TASKS - 1мс
//=================================================================================
void task_1ms_1(void)
{
	if ((MDR_CAN2->BUF_CON[1] & CAN_STATUS_RX_FULL) ==  CAN_STATUS_RX_FULL)
	{
		MDR_CAN2->BUF_CON[1] &=~CAN_STATUS_RX_FULL;

		CAN_GetRawReceivedData (MDR_CAN2, 1, &RxMsg);
		CANmsg.idt = RxMsg.Rx_Header.ID;
		CANmsg.data_u32[1] = RxMsg.Data[1];
		CANmsg.data_u32[0] = RxMsg.Data[0];

		if (CANmsg.pf == PGN_DEV_RESET)
		{
			RST_CLK_PCLKcmd (RST_CLK_PCLK_WWDG, ENABLE);
			MDR_WWDG->CFR	= 0x00000180;
			MDR_WWDG->CR	 = 0x7F;
			MDR_WWDG->CR	|= 0x80;
			//NVIC_SystemReset();
		}
	}

	task_1ms_ptr = &task_1ms_2;
}
//extern uint8_t dt[8];
void task_1ms_2(void)
{
	static uint32_t time_task = 0;
	uint32_t current_time = micros();
	if ((int32_t)(current_time-time_task) >= 0)
	{
		time_task = current_time + 1000000;

		CANmsg.p	= 7;
		CANmsg.r	= 0;
		CANmsg.dp	= 0;
		CANmsg.pf 	= 0x01;
		CANmsg.ps	= 0x00;
		CANmsg.sa 	= 0x07;
		CANmsg.len	= 2;
		CANmsg.data[0]	= ((uint8_t) temper) & 0xFF;
		CANmsg.data[1]	= ((uint8_t) (temper >> 8)) & 0xFF;
//		CANmsg.data[0] = dt[0];
//		CANmsg.data[1] = dt[2];
//		CANmsg.data[2] = dt[3];
//		CANmsg.data[3] = dt[4];
//		CANmsg.data[4] = dt[5];
//		CANmsg.data[5] = dt[6];
//		CANmsg.data[6] = dt[7];
//		CANmsg.data[7] = dt[8];

		TxMsg.ID 		= CANmsg.idt;
		TxMsg.DLC		= CANmsg.len;
		TxMsg.IDE		= CAN_ID_EXT;
		TxMsg.Data[0]	= CANmsg.data_u32[0];
		TxMsg.Data[1]	= CANmsg.data_u32[1];
		CAN_Transmit (MDR_CAN2, 0, &TxMsg);
//		(ds18b20_Reset() != 0) ? (MDR_PORTB->RXTX |= PORT_Pin_6) : (MDR_PORTB->RXTX &=~PORT_Pin_6);
//		temper = ds18b20_Tread();
		temper_start = 1;
	}

	task_1ms_ptr = &task_1ms_1;
}

//=================================================================================
//  B - TASKS - 5 мс
//=================================================================================
void task_5ms_1(void)
{
	static uint32_t time_task = 0;
	uint32_t current_time = micros();
	if ((int32_t)(current_time-time_task) >= 0)
	{
		time_task = current_time + 1000000;
		MDR_PORTB->RXTX ^= PORT_Pin_5;
	}

	task_5ms_ptr = &task_5ms_2;
}

void task_5ms_2(void)
{
	static uint32_t time_task = 0;
	uint32_t current_time = micros();
	if ((int32_t)(current_time-time_task) >= 0)
	{
		time_task = current_time + 500000;
//		if(DS18b20_port->RXTX & DS18b20_pin27)
//		{
//			MDR_PORTB->RXTX ^= PORT_Pin_6;
//		}


	}




	task_5ms_ptr = &task_5ms_1;
}
//------------------------------END FRAMEWORK -------------------------------------

void rcc_init(void)
{
	RST_CLK_DeInit();
	RST_CLK_HSEconfig (RST_CLK_HS_CONTROL_HSE_ON);									// eternal oscillator on
	while (!(RST_CLK_GetFlagStatus(RST_CLK_FLAG_HSERDY)));							// waiting
	RST_CLK_CPU_PLLcmd (ENABLE);													// PLL on
	// configures the CPU_PLL clock source and multiplication factor
	RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul5);			//RST_CLK_CPU_PLLsrcHSEdiv1
	while (!(RST_CLK_GetFlagStatus(RST_CLK_FLAG_PLLCPURDY)));
	// select the CPU_PLL output as input for CPU_C2_SEL
	RST_CLK_CPU_PLLuse (ENABLE);
	// configures the CPU_C3_SEL division factor
	RST_CLK_CPUclkPrescaler (RST_CLK_CPUclkDIV1);
	// select the HCLK clock source
	RST_CLK_CPUclkSelection (RST_CLK_CPUclkCPU_C3);
	// Enable peripheral clocks
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_RST_CLK, ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTA, 	ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTB, 	ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTC, 	ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTD, 	ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTE, 	ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTF, 	ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_EEPROM,	ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_TIMER1,	ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_TIMER2,	ENABLE);
	RST_CLK_PCLKcmd ( RST_CLK_PCLK_TIMER3,	ENABLE);
	RST_CLK_PCLKcmd	( RST_CLK_PCLK_UART2, 	ENABLE);
	RST_CLK_PCLKcmd	( RST_CLK_PCLK_ADC,	  	ENABLE);
	RST_CLK_PCLKcmd (RST_CLK_PCLK_WWDG, 	ENABLE);
	}

void gpio_init(void)
{
	PORT_InitTypeDef Port_Initstructure;

	PORT_DeInit (MDR_PORTA);
	PORT_DeInit (MDR_PORTB);
	PORT_DeInit (MDR_PORTC);
	PORT_DeInit (MDR_PORTD);
	PORT_DeInit (MDR_PORTE);
	PORT_DeInit (MDR_PORTF);


	PORT_StructInit (&Port_Initstructure);
	// Configure PORTB pins  for output
	Port_Initstructure.PORT_Pin		= PORT_Pin_3|PORT_Pin_5|PORT_Pin_6;
	Port_Initstructure.PORT_OE		= PORT_OE_OUT;
	Port_Initstructure.PORT_FUNC	= PORT_FUNC_PORT;
	Port_Initstructure.PORT_MODE	= PORT_MODE_DIGITAL;
	Port_Initstructure.PORT_SPEED	= PORT_SPEED_MAXFAST;
	PORT_Init (MDR_PORTB, &Port_Initstructure);

	// Configure PORTE pins  for input (CAN2RX)
	PORT_StructInit (&Port_Initstructure);
	Port_Initstructure.PORT_Pin		= PORT_Pin_6;
	Port_Initstructure.PORT_FUNC	= PORT_FUNC_ALTER;
	Port_Initstructure.PORT_MODE	= PORT_MODE_DIGITAL;
	PORT_Init (MDR_PORTE, &Port_Initstructure);

	// Configure PORTE pins  for output (CAN2TX)
	Port_Initstructure.PORT_SPEED	= PORT_SPEED_MAXFAST;
	Port_Initstructure.PORT_Pin		= PORT_Pin_7;
	PORT_Init (MDR_PORTE, &Port_Initstructure);

	PORT_StructInit (&Port_Initstructure);
	// Configure PORTA pins  for output OD
	Port_Initstructure.PORT_Pin		= DS18b20_pin27;
	Port_Initstructure.PORT_OE		= PORT_OE_OUT;
	Port_Initstructure.PORT_FUNC	= PORT_FUNC_PORT;
	Port_Initstructure.PORT_MODE	= PORT_MODE_DIGITAL;
	Port_Initstructure.PORT_SPEED	= PORT_SPEED_MAXFAST;
	Port_Initstructure.PORT_PD 		= PORT_PD_OPEN;
	PORT_Init (DS18b20_port, &Port_Initstructure);
	DS18b20_port->RXTX |= DS18b20_pin27;
}

void can2_init(void)
{
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	RST_CLK_PCLKcmd (RST_CLK_PCLK_CAN2, ENABLE);
	CAN_BRGInit (MDR_CAN2, CAN_HCLKdiv1);
	CAN_DeInit (MDR_CAN2);

	CAN_StructInit (&CAN_InitStructure);

	CAN_InitStructure.CAN_ROP				= DISABLE;
	CAN_InitStructure.CAN_SAP				= DISABLE;
	CAN_InitStructure.CAN_STM				= DISABLE;
	CAN_InitStructure.CAN_ROM				= DISABLE;
	CAN_InitStructure.CAN_OVER_ERROR_MAX 	= 255;
	CAN_InitStructure.CAN_SB				= CAN_SB_1_SAMPLE;
	CAN_InitStructure.CAN_PSEG				= CAN_PSEG_Mul_7TQ;
	CAN_InitStructure.CAN_SEG1				= CAN_SEG1_Mul_5TQ;
	CAN_InitStructure.CAN_SEG2				= CAN_SEG2_Mul_3TQ;
	CAN_InitStructure.CAN_SJW				= CAN_SJW_Mul_1TQ;
	CAN_InitStructure.CAN_BRP				= 19;
	CAN_Init (MDR_CAN2, &CAN_InitStructure);

	CAN_Cmd (MDR_CAN2, ENABLE);

	CAN_RxITConfig (MDR_CAN2, 1, ENABLE);
	CAN_TxITConfig (MDR_CAN2, 0, ENABLE);

	CAN_FilterInitStructure.Mask_ID 		= 0x1F00FF00;
	CAN_FilterInitStructure.Filter_ID 		= 0x0C000700;
	CAN_FilterInit (MDR_CAN2, 1, &CAN_FilterInitStructure);
	CAN_Receive (MDR_CAN2, 1, ENABLE);
}

void send_CAN(uint8_t *data, uint8_t lenght)
{
	uint8_t i;
	CANmsg.p	= 7;
	CANmsg.r	= 0;
	CANmsg.dp	= 0;
	CANmsg.pf 	= 0x01;
	CANmsg.ps	= 0x00;
	CANmsg.sa 	= 0x07;
	CANmsg.len	= lenght;

	for (i=0; i<lenght; i++) {
		CANmsg.data[i] = data[i];
	}

	TxMsg.ID 		= CANmsg.idt;
	TxMsg.DLC		= CANmsg.len;
	TxMsg.IDE		= CAN_ID_EXT;
	TxMsg.Data[0]	= CANmsg.data_u32[0];
	TxMsg.Data[1]	= CANmsg.data_u32[1];
	CAN_Transmit (MDR_CAN2, 0, &TxMsg);
}


void HardFault_Handler() {
	while(1) blink(2);
}

void delay(void)
{
	uint32_t j;
	for(j=0;j<1000000;j++)
	{

	}
}

void blink(uint8_t n)
{
	uint8_t ii;
	for(ii=0; ii<n; ii++)
	{
		MDR_PORTB->RXTX 	|= (1<<6);
		delay();
		MDR_PORTB->RXTX 	&=~(1<<6);
		delay();
	}
	delay();
	delay();
	delay();
}
