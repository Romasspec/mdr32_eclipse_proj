.syntax unified
.cpu cortex-m3
.thumb

  .section .text.Reset_Handler
  .weak Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
@  ldr   r0, =_estack
@  mov   sp, r0          /* set stack pointer */
  b    init
  b    main

    .section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler

	.section .start
	.word _estack
	.word Reset_Handler
	.word NMI_Handler
	.word HardFault_Handler

  	.weak	NMI_Handler
	.thumb_set NMI_Handler,Default_Handler

  	.weak	HardFault_Handler
	.thumb_set HardFault_Handler,Default_Handler
