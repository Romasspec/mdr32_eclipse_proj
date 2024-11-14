.syntax unified
.cpu cortex-m3
.thumb

  .section .text.Reset_Handler
  .weak Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
/*  ldr   r0, =_estack*/
/* mov   sp, r0          /* set stack pointer */

  	ldr r0, =_sdata
	ldr r1, =_edata
	ldr r2, =_sidata
	movs r3, #0
	bl LoopCopyDataInit
	bl startLoopFillZerobss
	bl    main

	b blink_3

CopyDataInit:
	ldr r4, [r2, r3]
	str r4, [r0, r3]
	adds r3, r3, #4

LoopCopyDataInit:
	adds r4, r0, r3
	cmp r4, r1
	bcc CopyDataInit
	bx lr

	b blink_4

startLoopFillZerobss:

	ldr r2, =_sbss
	ldr r4, =_ebss
	movs r3, #0
	b LoopFillZerobss

FillZerobss:
	str r3, [r2]
	adds r2, r2, #4

LoopFillZerobss:
	cmp r2, r4
	bcc FillZerobss
	bx lr

  b blink_5

    .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler

	.section .vectors
	.word _estack
	.word Reset_Handler
	.word NMI_Handler
	.word HardFault_Handler
	.word	MemManage_Handler
	.word	BusFault_Handler
	.word	UsageFault_Handler
	.word	0
	.word	0
	.word	0
	.word	0
	.word	SVC_Handler
	.word	DebugMon_Handler
	.word	0
	.word	PendSV_Handler
	.word	SysTick_Handler
	.word	CAN1_IRQHandler           			/* Window Watchdog interrupt                        */
	.word	CAN2_IRQHandler            			/* PVD through EXTI line detection interrupt        */
	.word	USB_IRQHandler         				/* Tamper interrupt                                 */
	.word	0            						/* RTC global interrupt                             */
	.word	0     	     						/* Flash global interrupt                           */
	.word	DMA_IRQHandler            			/* RCC global interrupt                             */
	.word	UART1_IRQHandler          			/* EXTI Line0 interrupt                             */
	.word	UART2_IRQHandler          			/* EXTI Line1 interrupt                             */
	.word	SSP1_IRQHandler          			/* EXTI Line2 interrupt                             */
	.word	0          							/* EXTI Line3 interrupt                             */
	.word	I2C_IRQHandler          			/* EXTI Line4 interrupt                             */
	.word	POWER_IRQHandler  					/* DMA1 Channel1 global interrupt                   */
	.word	WWDG_IRQHandler  					/* DMA1 Channel2 global interrupt                   */
	.word	0						  			/* DMA1 Channel3 global interrupt                   */
	.word	Timer1_IRQHandler  					/* DMA1 Channel4 global interrupt                   */
	.word	Timer2_IRQHandler  					/* DMA1 Channel5 global interrupt                   */
	.word	Timer3_IRQHandler  					/* DMA1 Channel6 global interrupt                   */
	.word	ADC_IRQHandler  					/* DMA1 Channel7 global interrupt                   */
	.word	0         							/* ADC1 and ADC2 global interrupt                   */
	.word	COMPARATOR_IRQHandler	  			/* USB High Priority or CAN TX interrupts           */
	.word	SSP2_IRQHandler 					/* USB Low Priority or CAN RX0 interrupts           */
	.word	0        							/* CAN RX1 interrupt                                */
	.word	0        							/* CAN SCE interrupt                                */
	.word	0        							/* EXTI Line[9:5] interrupts                        */
	.word	0       							/* TIM1 Break interrupt                             */
	.word	0        							/* TIM1 Update interrupt                            */
	.word	0   								/* TIM1 Trigger and Commutation interrupts          */
	.word	BACKUP_IRQHandler        			/* TIM1 Capture Compare interrupt                   */
	.word	EXT_INT1_IRQHandler           		/* TIM2 global interrupt                            */
	.word	EXT_INT2_IRQHandler          		/* TIM3 global interrupt                            */
	.word	EXT_INT3_IRQHandler           		/* TIM4 global interrupt                            */
	.word	EXT_INT4_IRQHandler        			/* I2C1 event interrupt                             */
	
/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

  	.weak	NMI_Handler
	.thumb_set NMI_Handler,Default_Handler

  	.weak	HardFault_Handler
	.thumb_set HardFault_Handler,Default_Handler

  	.weak	MemManage_Handler
	.thumb_set MemManage_Handler,Default_Handler

  	.weak	BusFault_Handler
	.thumb_set BusFault_Handler,Default_Handler

	.weak	UsageFault_Handler
	.thumb_set UsageFault_Handler,Default_Handler
	
	.weak	SVC_Handler
	.thumb_set SVC_Handler,Default_Handler

	.weak	DebugMon_Handler
	.thumb_set DebugMon_Handler,Default_Handler
	
	.weak	PendSV_Handler
	.thumb_set PendSV_Handler,Default_Handler

	.weak	SysTick_Handler
	.thumb_set SysTick_Handler,Default_Handler

	.weak	CAN1_IRQHandler
	.thumb_set CAN1_IRQHandler,Default_Handler
	
	.weak	CAN2_IRQHandler
	.thumb_set CAN2_IRQHandler,Default_Handler
	
	.weak	USB_IRQHandler
	.thumb_set USB_IRQHandler,Default_Handler
	
	.weak	DMA_IRQHandler
	.thumb_set DMA_IRQHandler,Default_Handler
	
	.weak	UART1_IRQHandler
	.thumb_set UART1_IRQHandler,Default_Handler
	
	.weak	UART2_IRQHandler
	.thumb_set UART2_IRQHandler,Default_Handler
	
	.weak	SSP1_IRQHandler
	.thumb_set SSP1_IRQHandler,Default_Handler
	
	.weak	I2C_IRQHandler
	.thumb_set I2C_IRQHandler,Default_Handler
	
	.weak	POWER_IRQHandler
	.thumb_set POWER_IRQHandler,Default_Handler
	
	.weak	WWDG_IRQHandler
	.thumb_set WWDG_IRQHandler,Default_Handler
	
	.weak	Timer1_IRQHandler
	.thumb_set Timer1_IRQHandler,Default_Handler
	
	.weak	Timer2_IRQHandler
	.thumb_set Timer2_IRQHandler,Default_Handler
	
	.weak	Timer3_IRQHandler
	.thumb_set Timer3_IRQHandler,Default_Handler
	
	.weak	ADC_IRQHandler
	.thumb_set ADC_IRQHandler,Default_Handler
	
	.weak	COMPARATOR_IRQHandler
	.thumb_set COMPARATOR_IRQHandler,Default_Handler
	
	.weak	SSP2_IRQHandler
	.thumb_set SSP2_IRQHandler,Default_Handler
	
	.weak	BACKUP_IRQHandler
	.thumb_set BACKUP_IRQHandler,Default_Handler
	
	.weak	EXT_INT1_IRQHandler
	.thumb_set EXT_INT1_IRQHandler,Default_Handler
	
	.weak	EXT_INT2_IRQHandler
	.thumb_set EXT_INT2_IRQHandler,Default_Handler
	
	.weak	EXT_INT3_IRQHandler
	.thumb_set EXT_INT3_IRQHandler,Default_Handler
	
	.weak	EXT_INT4_IRQHandler
	.thumb_set EXT_INT4_IRQHandler,Default_Handler
	
	.weak	SystemInit
