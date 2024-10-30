#ifndef DS18B20_H_
#define DS18B20_H_

//--------------------------------------------------
//#include <string.h>
//#include <stdlib.h>
//#include <stdint.h>
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "time.h"

#define DS18b20_pin27		PORT_Pin_2
#define DS18b20_port		MDR_PORTA

#define TIMEOUT_RESET		1500
#define CRC_POLINOM			((1<<5)|(1<<4)|1)
//	ROM FUNCTION COMMANDS
#define READ_ROM			0x33
#define MATCH_ROM			0x55
#define SKIP_ROM			0xCC
#define SEARCH_ROM			0xF0
#define ALARM_SEARCH		0xEC

// 	MEMORY COMMAND FUNCTIONS
#define WRITE_SCRATCHPAD	0x4E
#define READ_SCRATCHPAD		0xBE
#define COPY_SCRATCHPAD		0x48
#define CONVERT_T			0x44
#define RECALL_E2			0xB8
#define READ_POWER_SUPPLY	0xB4


enum {
	RESET_L = 0,
	RESET_H,
	RESET_PRESENCE,
	RESULT
};

enum {
	MODE_SKIP_ROM  = 0,
	MODE_MATCH_ROM
};

enum {
	RESOLUTION_9BIT		= 0x1F,
	RESOLUTION_10BIT	=  0x3F,
	RESOLUTION_11BIT	=  0x5F,
	RESOLUTION_12BIT	=  0x7F
};

enum {
	MEASURE_TEMPER = 0,
	READ_STRATCPAD,
	CRC
};

uint8_t ds18b20_Reset_delay(void);

uint8_t ds18b20_Reset(void);
uint16_t ds18b20_Tread (void);
uint8_t ds18b20_ReadBit(void);
uint8_t ds18b20_ReadByte(void);
void ds18b20_WriteBit(uint8_t bit);
void ds18b20_WriteByte(uint8_t dt);
uint8_t ds18b20_init(uint8_t mode);
uint8_t ds18b20_MeasureTemperCmd(uint8_t mode, uint8_t DevNum);
uint8_t ds18b20_ReadStratcpad(uint8_t mode, uint8_t *Data, uint8_t DevNum);
uint8_t ds18b20_ReadStratcpad_(uint8_t mode, uint8_t *Data, uint8_t DevNum);
uint16_t ds18b20_Convert(uint16_t *dt);
uint8_t calc_CRC (uint8_t * dt, uint8_t lenght);
uint8_t ds18b20_ReadRom_(uint8_t *Data);
//--------------------------------------------------

#endif /* DS18B20_H_ */
