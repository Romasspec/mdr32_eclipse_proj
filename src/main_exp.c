#include "MDR32F9Qx_config.h"
//#include "MDR32F9Qx_rst_clk.h"
//#include "MDR32F9Qx_port.h"

//void RCCInit (void);
//void Gpio_init (void);
int main (void);
void init (void);
void fault_irq(void);
void blink(uint8_t n);
void delay(void);

volatile uint32_t j;

asm (".section .start\n\t"
	".word 0x20005000\n\t"
	".word start0+1\n\t"
	".word NMI_Handler+1\n\t"
	".word HardFault_Handler+1\n\t"
//	".word MemManage_Handler+1\n\t"
//	".word BusFault_Handler+1\n\t"
//	".word UsageFault_Handler+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
//	".word fault_irq+1\n\t"
	"start0: b start1\n\t"
	"b main+1\n\t"
	"start1: b start2\n\t"
	"b main+1\n\t"
	"b main+1\n\t"
	"b main+1\n\t"
	"b main+1\n\t"
	"b main+1\n\t"
	"start2: b init\n\t"
	);
//
//void NMI_Handler() {
//	while(1) blink(1);
//}

void HardFault_Handler() {
	while(1) blink(2);
}
//
//void MemManage_Handler() {
//	while(1) blink(3);
//}
//
//void BusFault_Handler() {
//	while(1) blink(4);
//}
//
//void UsageFault_Handler() {
//	while(1) blink(5);
//}
//
//void fault_irq() {
//	while(1) blink(6);
//}
//
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
//__attribute__ ((section(".text")))



void init (void)
{
	MDR_RST_CLK->PER_CLOCK |= (1<<22)|(1<<4);
	MDR_PORTB->OE 	= (1<<5)|(1<<6);
	MDR_PORTB->PWR 	= (3<<10)|(3<<12);
	MDR_PORTB->ANALOG = (1<<5)|(1<<6);
	MDR_PORTB->RXTX 	|= (1<<5);
//	while (1)
//	main();
}

void delay(void)
{
	for(j=0;j<100000;j++)
	{

	}
}
//__attribute__ ((section(".text")))
int main (void)
	{
//*((volatile uint32_t *) 0x8001000) = 0x20005000;
//	RCCInit ();
//	Gpio_init ();


	uint32_t i;

//	if (i>5) {
//		while(1)
//		{
//			//PORT_SetBits(MDR_PORTB, PORT_Pin_5);
//		}
//
//	}
//	init();
	delay();

//	while (1)
//	{
		//PORT_SetBits(MDR_PORTB, PORT_Pin_5);
		MDR_PORTB->RXTX 	|= (1<<5);
		for(i=0;i<1000000;i++)
		{

		}
		//PORT_ResetBits(MDR_PORTB, PORT_Pin_5);
		MDR_PORTB->RXTX 	&=~(1<<5);
		for(i=0;i<1000000;i++)
		{

		}
//	}

//	return 0;
}



//__attribute__ ((section(".start"),used))
//
//const uint32_t* st[] = {
//		(const uint32_t*) 0x20005000,
//		(const uint32_t*) init,
//		(const uint32_t*) 0,
//		(const uint32_t*) HardFault_Handler
//};


//void RCCInit(void)
//{
//	RST_CLK_DeInit();
//	RST_CLK_HSEconfig (RST_CLK_HS_CONTROL_HSE_ON);									// eternal oscillator on
//	while (!(RST_CLK_GetFlagStatus(RST_CLK_FLAG_HSERDY)));							// waiting
//	RST_CLK_CPU_PLLcmd (ENABLE);													// PLL on
//	// configures the CPU_PLL clock source and multiplication factor
//	RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul10);		//RST_CLK_CPU_PLLsrcHSEdiv1
//	while (!(RST_CLK_GetFlagStatus(RST_CLK_FLAG_PLLCPURDY)));
//	// select the CPU_PLL output as input for CPU_C2_SEL
//	RST_CLK_CPU_PLLuse (ENABLE);
//	// configures the CPU_C3_SEL division factor
//	RST_CLK_CPUclkPrescaler (RST_CLK_CPUclkDIV1);
//	// select the HCLK clock source
//	RST_CLK_CPUclkSelection (RST_CLK_CPUclkCPU_C3);
//	// Enable peripheral clocks
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_RST_CLK, ENABLE);
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTA, 	ENABLE);
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTB, 	ENABLE);
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTC, 	ENABLE);
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTD, 	ENABLE);
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTE, 	ENABLE);
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_PORTF, 	ENABLE);
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_EEPROM,	ENABLE);
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_TIMER1,	ENABLE);
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_TIMER2,	ENABLE);
//	RST_CLK_PCLKcmd ( RST_CLK_PCLK_TIMER3,	ENABLE);
//	RST_CLK_PCLKcmd	( RST_CLK_PCLK_UART2, 	ENABLE);
//	RST_CLK_PCLKcmd	( RST_CLK_PCLK_ADC,	  	ENABLE);
//	RST_CLK_PCLKcmd (RST_CLK_PCLK_WWDG, 	ENABLE);
//	}

//void Gpio_init(void)
//{
//	PORT_InitTypeDef Port_Initstructure;
//
//	PORT_DeInit (MDR_PORTA);
//	PORT_DeInit (MDR_PORTB);
//	PORT_DeInit (MDR_PORTC);
//	PORT_DeInit (MDR_PORTD);
//	PORT_DeInit (MDR_PORTE);
//	PORT_DeInit (MDR_PORTF);
//
//
//	PORT_StructInit (&Port_Initstructure);
//	// Configure PORTB pins  for output
//	Port_Initstructure.PORT_Pin		= PORT_Pin_5;
//	Port_Initstructure.PORT_OE		= PORT_OE_OUT;
//	Port_Initstructure.PORT_FUNC	= PORT_FUNC_PORT;
//	Port_Initstructure.PORT_MODE	= PORT_MODE_DIGITAL;
//	Port_Initstructure.PORT_SPEED	= PORT_SPEED_MAXFAST;
//	PORT_Init (MDR_PORTB, &Port_Initstructure);
//}
