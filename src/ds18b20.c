#include "ds18b20.h"

static const uint8_t s_numb[]={0x28, 0x0F, 0xF2, 0x6A, 0x00, 0x00, 0x00, 0x94};
uint8_t dt[9];

uint8_t ds18b20_Reset(void)
{
	uint8_t status;
	DS18b20_port->RXTX &= ~DS18b20_pin27;
	delayMicroseconds(480);
	DS18b20_port->RXTX |= DS18b20_pin27;
	delayMicroseconds(65);
	status = DS18b20_port->RXTX & DS18b20_pin27;
	delayMicroseconds(480-65);
	return (status ? 1 : 0);
}

uint8_t ds18b20_Reset_(void)
{
 static uint8_t status = 1;
  static uint32_t time = 0, time_out = 0;
  static uint8_t state;
  uint32_t current_time = micros();

  if((int32_t)(current_time-time_out) >= 0) {
	  state = RESET_L;
	  DS18b20_port->RXTX |= DS18b20_pin27;
  }

  switch (state) {
  	  case RESET_L:
  		state = RESET_H;
  		time_out = current_time + TIMEOUT_RESET;
  		time = current_time + 485;  		  				//задержка как минимум на 480 микросекунд
		DS18b20_port->RXTX &= ~DS18b20_pin27;				//низкий уровень
  		  break;

  	  case RESET_H:
  		  if((int32_t)(current_time-time) >= 0) {
  			  state = RESET_PRESENCE;
  			  time = current_time + 65;						//задержка как минимум на 60 микросекунд
  			  DS18b20_port->RXTX |= DS18b20_pin27;			//высокий уровень
  		  }
  		  break;

  	  case RESET_PRESENCE:
  		  if((int32_t)(current_time-time) >= 0) {
  			state = RESULT;
  			time = current_time + 500;						//задержка как минимум на 480 микросекунд
  			status = DS18b20_port->RXTX & DS18b20_pin27;	//проверяем уровень
  		  }
  		  break;

  	  case RESULT:
  		  if((int32_t)(current_time - time) >= 0) {
  			state = RESET_L;
  			return (status ? 1 : 0);						//вернём результат
  		  }
  		  break;
  }
  return 1;
}

uint16_t ds18b20_Tread (void)
{
	uint16_t temp = 0;
	static uint8_t state = MEASURE_TEMPER;
//	MDR_PORTB->RXTX |= PORT_Pin_6;

	switch (state) {
		case MEASURE_TEMPER:
			if(!ds18b20_MeasureTemperCmd(MODE_MATCH_ROM, 0)) {
				state = READ_STRATCPAD;
			}

			break;

		case READ_STRATCPAD:
			if (!ds18b20_ReadStratcpad_(MODE_MATCH_ROM, dt, 0)) {
				//temp =  (dt[1] << 8) | dt[0];
				//temp = ds18b20_Convert((uint16_t*) dt);
				//state = MEASURE_TEMPER;
				state = CRC;
			}
			break;

		case CRC:
//			for(i=0; i<9; i++) {
//				data = dt[i];
//				for(j=0; j<8; j++) {
//					if ((crc ^ data)&1) {
//						crc = (crc >> 1) ^ 0x8c;		// полином 0х31, но т.к. данные передаются младшим битом вперд тогда полином будет 0х8с
//					}
//					else {
//						crc = (crc >> 1);
//					}
//					data = data >> 1;
//				}
//
//			}

			if (calc_CRC(dt, 9) == 0) {
//				*((uint16_t*) dt) = 0xFF5E;
				temp = ds18b20_Convert((uint16_t*) dt);
			}

			state = MEASURE_TEMPER;
			break;
	}
//	MDR_PORTB->RXTX &=~PORT_Pin_6;
	return temp;

//	if (ds18b20_Reset()) {
//		MDR_PORTB->RXTX &=~PORT_Pin_6;
//	} else {
//		MDR_PORTB->RXTX |= PORT_Pin_6;
//	}
//
}

uint8_t ds18b20_ReadBit(void)
{
	uint8_t bit = 0;
	DS18b20_port->RXTX &= ~DS18b20_pin27;						//низкий уровень
	delayMicroseconds(2);
	DS18b20_port->RXTX |= DS18b20_pin27;						//высокий уровень
	delayMicroseconds(13);
	bit = ((DS18b20_port->RXTX & DS18b20_pin27) ? 1 : 0);		//проверяем уровень
	delayMicroseconds(45);
	return bit;
}

uint8_t ds18b20_ReadByte(void)
{
	uint8_t data = 0;
	for (uint8_t i = 0; i <= 7; i++) {
		data += ds18b20_ReadBit() << i;
	}
	return data;
}

void ds18b20_WriteBit(uint8_t bit)
{
	DS18b20_port->RXTX &= ~DS18b20_pin27;
	delayMicroseconds(bit ? 3 : 65);
	DS18b20_port->RXTX |= DS18b20_pin27;
	delayMicroseconds(bit ? 65 : 3);
}

void ds18b20_WriteByte(uint8_t dt)
{
  for (uint8_t i = 0; i < 8; i++) {
    ds18b20_WriteBit((dt >> i) & 1);
    delayMicroseconds(5); 									//Delay Protection
  }
}

uint8_t ds18b20_init(uint8_t mode)
{
	if(ds18b20_Reset()) {
		return 1;
	}

	if(mode == MODE_SKIP_ROM) {
		ds18b20_WriteByte(SKIP_ROM);
		ds18b20_WriteByte(WRITE_SCRATCHPAD);
		ds18b20_WriteByte(100);
		ds18b20_WriteByte(30 | (1<<7));
		ds18b20_WriteByte(RESOLUTION_12BIT);
	}
  return 0;
}

uint8_t ds18b20_MeasureTemperCmd(uint8_t mode, uint8_t DevNum)
{
	uint8_t i;
	if(ds18b20_Reset_()) {
		return 1;
	}
	MDR_PORTB->RXTX |= PORT_Pin_6;
	if(mode == MODE_SKIP_ROM) {
		ds18b20_WriteByte(SKIP_ROM);								//SKIP ROM
	} else if (mode == MODE_MATCH_ROM) {
		ds18b20_WriteByte(MATCH_ROM);

		for(i=0; i<8; i++) {
			ds18b20_WriteByte(*(s_numb+i));
		}
	} else {
		return 1;
	}

	MDR_PORTB->RXTX &=~PORT_Pin_6;
	ds18b20_WriteByte(CONVERT_T);								//CONVERT T
	return 0;
}

uint8_t ds18b20_ReadStratcpad(uint8_t mode, uint8_t *Data, uint8_t DevNum)
{
	uint8_t i;
	if(ds18b20_Reset_()) {
		return 1;
	}

	if(mode == MODE_SKIP_ROM) {
		ds18b20_WriteByte(SKIP_ROM);							//SKIP ROM
	} else if (mode == MODE_MATCH_ROM) {
		ds18b20_WriteByte(MATCH_ROM);
	} else {
		return 1;
	}

	ds18b20_WriteByte(READ_SCRATCHPAD);								//READ SCRATCHPAD
	for(i=0;i<9;i++) {
		Data[i] = ds18b20_ReadByte();
	}
	return  0;
}

uint8_t ds18b20_ReadStratcpad_(uint8_t mode, uint8_t *Data, uint8_t DevNum)
{
	static uint8_t i = 0;
	if(ds18b20_Reset_() && (!i)) {
		return 1;
	}

	if((mode == MODE_SKIP_ROM) && (!i)) {
		ds18b20_WriteByte(SKIP_ROM);							//SKIP ROM
	} else if((mode == MODE_MATCH_ROM) && (!i)) {
		ds18b20_WriteByte(MATCH_ROM);

		for (i=0; i<8; i++) {
			ds18b20_WriteByte(*(s_numb+i));
		}
		i=0;
	} else if (!i) {
		return 1;
	}

	if (!i) {
		ds18b20_WriteByte(READ_SCRATCHPAD);						//READ SCRATCHPAD
	}

	if(i<9) {
		Data[i++] = ds18b20_ReadByte();
	} else {
		i = 0;
		return 0;
	}
	return  1;
}

uint16_t ds18b20_Convert(uint16_t *dt)
{
	uint16_t temp;
	if(*dt & 0x0800) {
		temp = (~*dt) + 1 + 0x0800;
	} else {
		temp = *dt;
	}
	temp = ((temp<<4) & 0xFF00) |(((temp & 0x000F) * 100) >> 4);
//		temp = (*dt<<4) & 0xFF00;
//		temp |= ;


	return temp;
}

uint8_t calc_CRC (uint8_t * dt, uint8_t lenght)
{
	uint8_t i, j, data, crc = 0;
	for(i=0; i<lenght; i++) {
		data = dt[i];
		for(j=0; j<8; j++) {
			if ((crc ^ data)&1) {
				crc = (crc >> 1) ^ 0x8c;		// полином 0х31, но т.к. данные передаются младшим битом вперд тогда полином будет 0х8с
			}
			else {
				crc = (crc >> 1);
			}
			data = data >> 1;
		}
	}
	return crc;
}

uint8_t ds18b20_ReadRom_(uint8_t *Data)
{
	static uint8_t i = 0;
	if(ds18b20_Reset_() && (!i)) {
		return 1;
	}

	if (!i) {
		ds18b20_WriteByte(READ_ROM);						//READ LASERED ROM
	}

	if(i<8) {
		Data[i++] = ds18b20_ReadByte();
	} else {
		i = 0;
		return calc_CRC(Data,8);
	}
	return 1;
}
