#include "multimeter.h"

void DMM_SetURange(uint8_t uRange){
	// Select range
	switch(uRange){
	case DMM_RANGE_2V:
		shift_Set(DMM_RANGESEL_LSB);
		shift_Set(DMM_RANGESEL_MSB);
		break;
	case DMM_RANGE_20V:
		shift_Clear(DMM_RANGESEL_LSB);
		shift_Set(DMM_RANGESEL_MSB);
		break;
	case DMM_RANGE_200V:
		shift_Set(DMM_RANGESEL_LSB);
		shift_Clear(DMM_RANGESEL_MSB);
		break;
	case DMM_RANGE_500V:
		shift_Clear(DMM_RANGESEL_LSB);
		shift_Clear(DMM_RANGESEL_MSB);
		break;
	}
	// Select voltage measurement
	shift_Clear(DMM_R_SELECTOR);
	shift_Clear(DMM_U_I_SELECTOR);
	// update shift register content
	shift_Update();
}

void DMM_SetIRange(uint8_t iRange){
	// Select range
	switch(iRange){
	case DMM_RANGE_200uA:
		shift_Set(DMM_RANGESEL_LSB);
		shift_Set(DMM_RANGESEL_MSB);
		break;
	case DMM_RANGE_2mA:
		shift_Clear(DMM_RANGESEL_LSB);
		shift_Set(DMM_RANGESEL_MSB);
		break;
	case DMM_RANGE_20mA:
		shift_Set(DMM_RANGESEL_LSB);
		shift_Clear(DMM_RANGESEL_MSB);
		break;
	case DMM_RANGE_200mA:
		shift_Clear(DMM_RANGESEL_LSB);
		shift_Clear(DMM_RANGESEL_MSB);
		break;
	case DMM_RANGE_10A:
		shift_Clear(DMM_RANGESEL_LSB);
		shift_Clear(DMM_RANGESEL_MSB);
		break;
	}
	// Select voltage measurement
	shift_Clear(DMM_R_SELECTOR);
	shift_Set(DMM_U_I_SELECTOR);
	// update shift register content
	shift_Update();
}

void DMM_SetRRange(uint8_t rRange){
	// Select range
	shift_Set(DMM_R_RANGE1);
	shift_Set(DMM_R_RANGE2);
	shift_Set(DMM_R_RANGE3);
	shift_Set(DMM_R_RANGE4);
	shift_Set(DMM_R_RANGE5);
	switch(rRange){
	case DMM_RANGE_2kOhm:
		shift_Clear(DMM_R_RANGE1);
		break;
	case DMM_RANGE_20kOhm:
		shift_Clear(DMM_R_RANGE2);
		break;
	case DMM_RANGE_200kOhm:
		shift_Clear(DMM_R_RANGE3);
		break;
	case DMM_RANGE_2MOhm:
		shift_Clear(DMM_R_RANGE4);
		break;
	case DMM_RANGE_20MOhm:
		shift_Clear(DMM_R_RANGE5);
		break;
	}
	// Select voltage measurement
	shift_Set(DMM_R_SELECTOR);
	shift_Clear(DMM_U_I_SELECTOR);
	// update shift register content
	shift_Update();
}
