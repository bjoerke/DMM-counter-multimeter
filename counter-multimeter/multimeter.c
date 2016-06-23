#include "multimeter.h"

static uint8_t u_range_marker = DMM_RANGE_500V;
static uint8_t i_range_marker = DMM_RANGE_10A;
static uint8_t r_range_marker = DMM_RANGE_20MOhm;

void ADC_Init(void) {
	ADCSRA = 0x00;							// ADC Control and Status Register A
//	ADMUX = (1 << REFS0);// | (1 << ADLAR);									// REFS0 (Bit6 in ADMUX) = ADC Voltage Reference Selection => REFS[1:0] = 01 => AVcc with external capacitor at AREF pin
																			// ADLAR (Bit5 in ADMUX) = ADC Left Adjust Result (in this case TRUE)
																			// [REMOVED] MUX0 (Bit0 in ADMUX) = Analog Channel and Gain Selection Bit0 => MUX[4:0] = 00000 => Single Ended Input = ADC0

//	ACSR  =  0x80;															// Analog Comparator Control and Status Register = 0000 1000 => ACIE = 1 Analog Comparator Interrupt Enable

//	ADCSRA |= (1 << ADATE);													// ADATE = ADC Auto Trigger Enable
//	ADCSRB |= (1 << ADTS2);													// ADTS2 = ADC Trigger Select Bit2 (to be enabled in ADCSRB) => ADTS[2:0] = 010 => External Interrupt Request 0
	ADCSRA |= (1 << ADEN) /*| ( 1 << ADIE)*/| (1 << ADPS2) | (1 << ADPS1)
			| (1 << ADPS0);		// ADEN = ADC Enable
	// ADIE = ADC Interrupt Enable
	// ADPS1 (Bit1 in ADCSRA) = ADC Prescaler Select Bit1
	// ADPS0 (Bit0 in ADCSRA) = ADC Prescaler Select Bit0 => ADPS[2:0] = 011
	// => Division Factor = 128 (between system clock frequency and the input clock to the ADC)
	ADCSRA |= (1 << ADSC);						// ADSC = ADC Start Conversion

	while (ADCSRA & (1 << ADSC))	// Waiting for completion of the conversion.
	{
	}
	(void) ADCW;					// Reading ADCW (ADCL + ADCH) once so the result of the next conversion can be used
}

uint16_t ADC_get(uint8_t channel) {
	ADMUX &= ~0x1f;
	ADMUX |= channel;

	ADCSRA |= (1 << ADSC);						// ADSC = ADC Start Conversion

	while (ADCSRA & (1 << ADSC))				// Waiting for completion of the conversion.
	{
	}
	return ADCW ;
}
void DMM_SetURange(uint8_t uRange) {
	// Select range
	switch (uRange) {
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

void DMM_SetIRange(uint8_t iRange) {
	// Select range
	switch (iRange) {
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
	// Select Current measurement
	shift_Clear(DMM_R_SELECTOR);
	shift_Set(DMM_U_I_SELECTOR);
	// update shift register content
	shift_Update();
}

void DMM_SetRRange(uint8_t rRange) {
	// Select range
	shift_Set(DMM_R_RANGE1);
	shift_Set(DMM_R_RANGE2);
	shift_Set(DMM_R_RANGE3);
	shift_Set(DMM_R_RANGE4);
	shift_Set(DMM_R_RANGE5);
	switch (rRange) {
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
	// Select Resistance measurement
	shift_Set(DMM_R_SELECTOR);
	shift_Clear(DMM_U_I_SELECTOR);
	// update shift register content
	shift_Update();
}

void DMM_SetCRange(uint8_t rRange) {
	// Set mode
	// TODO: Set correct Range in Resistance Mode
	shift_Set(DMM_RANGESEL_LSB);
	shift_Set(DMM_RANGESEL_MSB);

	// Select Resistance measurement
	shift_Set(DMM_R_SELECTOR);
	shift_Clear(DMM_U_I_SELECTOR);
	// update shift register content
	shift_Update();
}

// This function evaluates which parameter and which range is requested by the user
// ###############################################################
// ## TODO: - Evaluate the sign!							    ##
// ##		- Units not yet set.								##
// ##		- put the correct range in front of the unit?       ##
// ##		  --> or just display the full result?				##
// ##		- set up AC-Mode?									##
// ##		- set safe mode at start-up.						##
//##		- optimize the Auto-R-Mode							##
// ###############################################################

int32_t meter_TakeMeasurement(uint8_t range) 
{
	uint16_t adc;
	double result;
	double corr_result = 0;
	//double dummy = 0; 
	//uint16_t oldrange;
	
	if(range < DMM_RANGE_2kOhm)						// everything smaller than DMM_RANGE_2kOhm means: Voltage and Current
	{
		adc = ADC_get(0);							// argument: 0 = Input channel PA0
		result = (((double)adc * 3.3) / 1024);		// calculate the adc-conversion result
	}
	else if(range >= DMM_RANGE_2kOhm)
	{
		adc = ADC_get(1);							// argument: 1 = Input channel PA1
		result = (((double)adc * 3.3) / 1024);		// calculate the adc-conversion result
	}
	else if (range == DMM_RANGE_NONE)
	{
		// don't take measurement
		return 666;									// Number of the beast --> Something wrong...
	}

	switch (range) 
	{
		// Check for Voltage Mode
		case DMM_RANGE_AUTO_U:
			switch(u_range_marker)
			{
				case DMM_RANGE_500V:
					DMM_SetURange(DMM_RANGE_500V);
					break;
				case DMM_RANGE_200V:
					DMM_SetURange(DMM_RANGE_200V);
					break;
				case DMM_RANGE_20V:
					DMM_SetURange(DMM_RANGE_20V);
					break;
				case DMM_RANGE_2V:
					DMM_SetURange(DMM_RANGE_2V);
					break;
			}
			time_Waitms(100);
			adc = ADC_get(0);
			if((adc < 100) && (u_range_marker > DMM_RANGE_2V))
			{
				u_range_marker --;
			}
			else if((adc > 900) && (u_range_marker < DMM_RANGE_500V))
			{
				u_range_marker ++;
			}
			else
			{
				result = (((double)adc * 3.3) / 1024);		// calculate the adc-conversion result
				
				switch(u_range_marker)
				{
					case DMM_RANGE_500V:
						corr_result = ((result / U_R_REF_500V) * U_R_SUM) / 15;				// calculate the correct input-voltage (divided by 15 to compensate the amplification)
						break;
					case DMM_RANGE_200V:
						corr_result = ((result / U_R_REF_200V) * U_R_SUM) / 15;				// calculate the correct input-voltage (divided by 15 to compensate the amplification)
						break;
					case DMM_RANGE_20V:
						corr_result = ((result / U_R_REF_20V) * U_R_SUM) / 15;				// calculate the correct input-voltage (divided by 15 to compensate the amplification)
						break;
					case DMM_RANGE_2V:
						corr_result = ((result / U_R_REF_2V) * U_R_SUM) / 15;				// calculate the correct input-voltage (divided by 15 to compensate the amplification)
						break;
				}
			}
			corr_result *= 100;
			break;
		case DMM_RANGE_500V:
			DMM_SetURange(DMM_RANGE_500V);
			corr_result = ((result / U_R_REF_500V) * U_R_SUM) / 15;				// calculate the correct input-voltage (divided by 15 to compensate the amplification)
			corr_result *= 100;													// Multiply by 100 to return as well 2 positions after the decimal point
			u_range_marker = DMM_RANGE_500V;
			break;
		case DMM_RANGE_200V:
			DMM_SetURange(DMM_RANGE_200V);
			corr_result = ((result / U_R_REF_200V) * U_R_SUM) / 15;				// calculate the correct input-voltage (divided by 15 to compensate the amplification)
			corr_result *= 100;													// Multiply by 100 to return as well 2 positions after the decimal point
			u_range_marker = DMM_RANGE_200V;
			break;
		case DMM_RANGE_20V:
			DMM_SetURange(DMM_RANGE_20V);
			corr_result = ((result / U_R_REF_20V) * U_R_SUM) / 15;				// calculate the correct input-voltage (divided by 15 to compensate the amplification)
			corr_result *= 100;													// Multiply by 100 to return as well 2 positions after the decimal point
			u_range_marker = DMM_RANGE_20V;
			break;
		case DMM_RANGE_2V:
			DMM_SetURange(DMM_RANGE_2V);
			corr_result = ((result / U_R_REF_2V) * U_R_SUM) / 15;				// calculate the correct input-voltage (divided by 15 to compensate the amplification)
			corr_result *= 100;													// Multiply by 100 to return as well 2 positions after the decimal point
			u_range_marker = DMM_RANGE_2V;
			break;

		// Check for Current Mode
		case DMM_RANGE_AUTO_I:
			switch(i_range_marker)
			{
				case DMM_RANGE_10A:
					DMM_SetIRange(DMM_RANGE_10A);
					break;
				case DMM_RANGE_200mA:
					DMM_SetIRange(DMM_RANGE_200mA);
					break;
				case DMM_RANGE_20mA:
					DMM_SetIRange(DMM_RANGE_20mA);
					break;
				case DMM_RANGE_2mA:
					DMM_SetIRange(DMM_RANGE_2mA);
					break;
				case DMM_RANGE_200uA:
					DMM_SetIRange(DMM_RANGE_200uA);
					break;
			}
			time_Waitms(100);
			adc = ADC_get(0);
			if((adc < 100) && (i_range_marker > DMM_RANGE_200uA))
			{
				i_range_marker --;
			}
			else if((adc > 900) && (i_range_marker < DMM_RANGE_10A))
			{
				i_range_marker ++;
			}
			else
			{
				result = (((double)adc * 3.3) / 1024);		// calculate the adc-conversion result
				
				switch(i_range_marker)
				{
					case DMM_RANGE_10A:
						corr_result = ((result / U_R_REF_500V) * U_R_SUM) / 15;				// calculate the correct input-voltage (divided by 15 to compensate the amplification)
						corr_result *= 10;													// return the result + 1 position after the decimal point
						break;
					case DMM_RANGE_200mA:
						corr_result = (result / I_R_REF_200mA) / 15;						// calculate the correct input-current (divided by 15 to compensate the amplification)
						corr_result *= 10000;												// return the result in mA + 1 position after the decimal point
						break;
					case DMM_RANGE_20mA:
						corr_result = (result / I_R_REF_20mA) / 15;							// calculate the correct input-current (divided by 15 to compensate the amplification)
						corr_result *= 10000;												// return the result in mA + 1 position after the decimal point
						break;
					case DMM_RANGE_2mA:
						corr_result = (result / I_R_REF_2mA) / 15;							// calculate the correct input-current (divided by 15 to compensate the amplification)
						corr_result *= 10000;												// return the result in mA + 1 position after the decimal point
						break;
					case DMM_RANGE_200uA:
						corr_result = (result / I_R_REF_200uA) / 15;						// calculate the correct input-current (divided by 15 to compensate the amplification)
						corr_result *= 10000000;											// return the result in uA + 1 position after the decimal point
				}
			}
			break;
		case DMM_RANGE_10A:
			DMM_SetIRange(DMM_RANGE_10A);
			corr_result = (result / I_R_REF_10A) / 15;					// calculate the correct input-current (divided by 15 to compensate the amplification)
			corr_result *= 10;											// return the result + 1 position after the decimal point
			i_range_marker = DMM_RANGE_10A;
			break;
		case DMM_RANGE_200mA:
			DMM_SetIRange(DMM_RANGE_200mA);
			corr_result = (result / I_R_REF_200mA) / 15;				// calculate the correct input-current (divided by 15 to compensate the amplification)
			corr_result *= 10000;										// return the result in mA + 1 position after the decimal point
			i_range_marker = DMM_RANGE_200mA;
			break;
		case DMM_RANGE_20mA:
			DMM_SetIRange(DMM_RANGE_20mA);
			corr_result = (result / I_R_REF_20mA) / 15;					// calculate the correct input-current (divided by 15 to compensate the amplification)
			corr_result *= 10000;										// return the result in mA + 1 position after the decimal point
			i_range_marker = DMM_RANGE_20mA;
			//// Trying to display an "m"
			//LCD_GotoXY(14, 3);
			//LCD_PutChar(0x6D);
			////LCD_Update();
			break;
		case DMM_RANGE_2mA:
			DMM_SetIRange(DMM_RANGE_2mA);
			corr_result = (result / I_R_REF_2mA) / 15;					// calculate the correct input-current (divided by 15 to compensate the amplification)
			corr_result *= 10000;										// return the result in mA + 1 position after the decimal point
			i_range_marker = DMM_RANGE_2mA;
			break;
		case DMM_RANGE_200uA:
			DMM_SetIRange(DMM_RANGE_200uA);
			corr_result = (result / I_R_REF_200uA) / 15;				// calculate the correct input-current (divided by 15 to compensate the amplification)
			corr_result *= 10000000;									// return the result in uA + 1 position after the decimal point
			i_range_marker = DMM_RANGE_200uA;
			break;

		// Check for Resistance Mode
		case DMM_RANGE_AUTO_R:											// To be optimized
			switch(r_range_marker)
			{
				case DMM_RANGE_20MOhm:
					DMM_SetRRange(DMM_RANGE_20MOhm);
					break;
				case DMM_RANGE_2MOhm:
					DMM_SetRRange(DMM_RANGE_2MOhm);
					break;
				case DMM_RANGE_200kOhm:
					DMM_SetRRange(DMM_RANGE_200kOhm);
					break;
				case DMM_RANGE_20kOhm:
					DMM_SetRRange(DMM_RANGE_20kOhm);
					break;
				case DMM_RANGE_2kOhm:
					DMM_SetRRange(DMM_RANGE_2kOhm);
					break;
			}
			time_Waitms(100);
			adc = ADC_get(1);
			if((adc < 100) && (r_range_marker > DMM_RANGE_2kOhm))
			{
				r_range_marker --;
			}
			else if((adc > 900) && (u_range_marker < DMM_RANGE_20MOhm))
			{
				r_range_marker ++;
			}
			else
			{
				result = (((double)adc * 3.3) / 1024);		// calculate the adc-conversion result
				
				switch(r_range_marker)
				{
					case DMM_RANGE_20MOhm:
						corr_result = (result * R_R_REF_20MOhm) / (R_U_REF - result);
						break;
					case DMM_RANGE_2MOhm:
						corr_result = (result * R_R_REF_2MOhm) / (R_U_REF - result);
						break;
					case DMM_RANGE_200kOhm:
						corr_result = (result * R_R_REF_200kOhm) / (R_U_REF - result);
						break;
					case DMM_RANGE_20kOhm:
						corr_result = (result * R_R_REF_20kOhm) / (R_U_REF - result);
						break;
					case DMM_RANGE_2kOhm:
						corr_result = (result * R_R_REF_2kOhm) / (R_U_REF - result);
						break;
				}
			}
			break;
		case DMM_RANGE_20MOhm:													// needs to display an 'k' for kilo-Ohm
			DMM_SetRRange(DMM_RANGE_20MOhm);
			corr_result = (result * R_R_REF_20MOhm) / (R_U_REF - result);
			r_range_marker = DMM_RANGE_20MOhm;
			break;
		case DMM_RANGE_2MOhm:													// needs to display an 'k' for kilo-Ohm
			DMM_SetRRange(DMM_RANGE_2MOhm);
			corr_result = (result * R_R_REF_2MOhm) / (R_U_REF - result);
			r_range_marker = DMM_RANGE_2MOhm;
			break;
		case DMM_RANGE_200kOhm:													// needs to display an 'k' for kilo-Ohm
			DMM_SetRRange(DMM_RANGE_200kOhm);
			corr_result = (result * R_R_REF_200kOhm) / (R_U_REF - result);
			r_range_marker = DMM_RANGE_200kOhm;
			break;
		case DMM_RANGE_20kOhm:
			DMM_SetRRange(DMM_RANGE_20kOhm);
			corr_result = (result * R_R_REF_20kOhm) / (R_U_REF - result);
			r_range_marker = DMM_RANGE_20kOhm;
			break;
		case DMM_RANGE_2kOhm:
			DMM_SetRRange(DMM_RANGE_2kOhm);
			corr_result = (result * R_R_REF_2kOhm) / (R_U_REF - result);
			r_range_marker = DMM_RANGE_2kOhm;
			break;

		// Check for Continuity Mode
		case DMM_RANGE_CONTIN:											// TODO
			// Idee:	- Aehnlich wie Auto-R-Modus aufbauen.
			//			- Kleinsten Widerstandsmessbereich auswaehlen (2kOhm)
			//			- ADC-Wandlung durchfuehren, wenn adc < 5.12, liegt der Widerstand unter 10 Ohm, dann kann man das schon mal als Durchgang ansehen
			//			- Ergebnis ausgeben
			//			- Je nach Ergebnis Farbwechsel des Displays: Bei Durchgang gruen und andernfalls rot.

			//DMM_SetCRange(DMM_RANGE_CONTIN);
			break;
	}

	return corr_result;
}
