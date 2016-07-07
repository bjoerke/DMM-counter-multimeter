#include "multimeter.h"

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
	(void) ADCW;// Reading ADCW (ADCL + ADCH) once so the result of the next conversion can be used
}

uint16_t ADC_get(uint8_t channel) {
	uint8_t cntr;
	uint16_t sum = 0;
	ADMUX &= ~0x1f;
	ADMUX |= channel;

	for(cntr = 0; cntr < 30; cntr++)				// loop for collecting 30 values of the adc-conversion
	{
		ADCSRA |= (1 << ADSC);						// ADSC = ADC Start Conversion

		while (ADCSRA & (1 << ADSC))				// Waiting for completion of the conversion.
		{
		}
		sum += ADCW;
	}
	sum /= cntr;									// calculate the mean of 30 times adc-conversion
	return sum;
}

void DMM_SetDefault(void){
	DMM_SetURange(DMM_RANGE_500V);
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
// ## TODO: - Units not yet set.								##
// ##		- put the correct range in front of the unit?       ##
// ##		  --> or just display the full result?				##
// ##		- set up AC-Mode?									##
// ##		- set safe mode at start-up.						##
//##		- optimize the Auto-R-Mode							##
// ###############################################################

uint8_t meter_TakeMeasurement(int32_t *res, char *unit, uint32_t *decPoint, uint8_t measurement,
		uint8_t range) {
	uint8_t outOfRange = 0;
	uint16_t adc;
	double result;
	double corr_result = 0;
	//double dummy = 0; 
	//uint16_t oldrange;

	if (measurement < DMM_MEASURE_R)// everything smaller than DMM_MEASURE_R means: Voltage and Current
	{
		adc = ADC_get(0);					// argument: 0 = Input channel PA0
		result = (((double) adc * 3.3) / 1024);	// calculate the adc-conversion result
	} else {
		// Resistance (or continuity) measurement
		adc = ADC_get(1);					// argument: 1 = Input channel PA1
		result = (((double) adc * 3.3) / 1024);	// calculate the adc-conversion result
	}
	// Can't ever reach this -> not needed
//	else if (range == DMM_RANGE_NONE) {
//		// don't take measurement
//		return 666;				// Number of the beast --> Something wrong...
//	}

	uint8_t actualRange;
	switch (measurement) {
	case DMM_MEASURE_U_DC:
	case DMM_MEASURE_U_AC:
		// measuring voltage
		// ############################
		// ## Check for Voltage Mode ##
		// ############################
		actualRange = range;
		if (range == DMM_RANGE_AUTO) {
			if (selectedAutoRange > 4 || selectedAutoRange < 1)
				selectedAutoRange = 4;

			time_Waitms(100);
			if ((adc < 90) && (selectedAutoRange > DMM_RANGE_2V)) {
				selectedAutoRange--;
			} else if ((adc > 1000) && (selectedAutoRange < DMM_RANGE_500V)) {
				selectedAutoRange++;
			}
			actualRange = selectedAutoRange;
		}
		DMM_SetURange(actualRange);
		if (adc > 1000)
			outOfRange = 1;
		switch (actualRange) {
		case DMM_RANGE_500V:
			corr_result = ((result / U_R_REF_500V) * U_R_SUM) / 15;	// calculate the correct input-voltage (divided by 15 to compensate the amplification)
			break;
		case DMM_RANGE_200V:
			corr_result = ((result / U_R_REF_200V) * U_R_SUM) / 15;	// calculate the correct input-voltage (divided by 15 to compensate the amplification)
			break;
		case DMM_RANGE_20V:
			corr_result = ((result / U_R_REF_20V) * U_R_SUM) / 15;// calculate the correct input-voltage (divided by 15 to compensate the amplification)
			break;
		case DMM_RANGE_2V:
			corr_result = ((result / U_R_REF_2V) * U_R_SUM) / 15;// calculate the correct input-voltage (divided by 15 to compensate the amplification)
			break;
		}
		corr_result *= 1000;	// Multiply by 1000 to return as well 3 positions after the decimal point
		*decPoint = 1;
		*unit++ = 'V';
		*unit = 0;
		break;
	case DMM_MEASURE_I_DC:
	case DMM_MEASURE_I_AC:
		// ############################
		// ## Check for Current Mode ##
		// ############################
		actualRange = range;
		if (range == DMM_RANGE_AUTO) {
			if (selectedAutoRange > 4 || selectedAutoRange < 1)
				selectedAutoRange = 4;

			time_Waitms(100);
			if ((adc < 90) && (selectedAutoRange > DMM_RANGE_200uA)) {
				selectedAutoRange--;
			} else if ((adc > 1000) && (selectedAutoRange < DMM_RANGE_200mA)) {
				selectedAutoRange++;
			}
			actualRange = selectedAutoRange;
		}
		DMM_SetIRange(actualRange);
		if (adc > 1000)
			outOfRange = 1;
		switch (actualRange) {
		case DMM_RANGE_10A:
			corr_result = (result / I_R_REF_10A) / 15;	// calculate the correct input-voltage (divided by 15 to compensate the amplification)
			*decPoint = 1000000;
			break;
		case DMM_RANGE_200mA:
			corr_result = (result / I_R_REF_200mA) / 15;// calculate the correct input-current (divided by 15 to compensate the amplification)
			*decPoint = 1000;
			*unit++ = 'm';
			break;
		case DMM_RANGE_20mA:
			corr_result = (result / I_R_REF_20mA) / 15;	// calculate the correct input-current (divided by 15 to compensate the amplification)
			*decPoint = 1000;
			*unit++ = 'm';
			break;
		case DMM_RANGE_2mA:
			corr_result = (result / I_R_REF_2mA) / 15;// calculate the correct input-current (divided by 15 to compensate the amplification)
			*decPoint = 1000;
			*unit++ = 'm';
			break;
		case DMM_RANGE_200uA:
			corr_result = (result / I_R_REF_200uA) / 15;// calculate the correct input-current (divided by 15 to compensate the amplification)
			*decPoint = 1;
			*unit++ = 'u';
			break;
		}
		corr_result *= 10000000; // LSB is 0.1uA
		*unit++ = 'A';
		*unit = 0;
		break;
	case DMM_MEASURE_R:
		// ###############################
		// ## Check for Resistance Mode ##
		// ###############################
		actualRange = range;
		if (range == DMM_RANGE_AUTO) {
			if (selectedAutoRange > 5 || selectedAutoRange < 1)
				selectedAutoRange = 5;

			time_Waitms(100);
			if ((adc < 230) && (selectedAutoRange > DMM_RANGE_2kOhm)) {
				selectedAutoRange--;
			} else if ((adc > 790) && (selectedAutoRange < DMM_RANGE_20MOhm)) {
				selectedAutoRange++;
			}
			actualRange = selectedAutoRange;
		}
		DMM_SetRRange(actualRange);
		if (adc > 950)
			outOfRange = 1;
		switch (actualRange) {
		case DMM_RANGE_20MOhm:
			corr_result = (result * R_R_REF_20MOhm) / (R_U_REF - result);
			*decPoint = 10000;
			*unit++ = 'k';
			break;
		case DMM_RANGE_2MOhm:
			corr_result = (result * R_R_REF_2MOhm) / (R_U_REF - result);
			*decPoint = 10000;
			*unit++ = 'k';
			break;
		case DMM_RANGE_200kOhm:
			corr_result = (result * R_R_REF_200kOhm) / (R_U_REF - result);
			*decPoint = 10000;
			*unit++ = 'k';
			break;
		case DMM_RANGE_20kOhm:
			corr_result = (result * R_R_REF_20kOhm) / (R_U_REF - result);
			*decPoint = 10;
			break;
		case DMM_RANGE_2kOhm:
			corr_result = (result * R_R_REF_2kOhm) / (R_U_REF - result);
			*decPoint = 10;
			break;
		}
		corr_result *= 100; // LSB is 10mOhm
		*unit++ = 'O';
		*unit++ = 'h';
		*unit++ = 'm';
		*unit = 0;
		break;
	case DMM_MEASURE_CONT:
		// TODO
		// Idee:	- Aehnlich wie Auto-R-Modus aufbauen.
		//			- Kleinsten Widerstandsmessbereich auswaehlen (2kOhm)
		//			- ADC-Wandlung durchfuehren, wenn adc < 5.12, liegt der Widerstand unter 10 Ohm, dann kann man das schon mal als Durchgang ansehen
		//			- Ergebnis ausgeben
		//			- Je nach Ergebnis Farbwechsel des Displays: Bei Durchgang gruen und andernfalls rot.

		//DMM_SetCRange(DMM_RANGE_CONTIN);
		DMM_SetRRange(DMM_RANGE_2kOhm);
		if (adc > 950)
			outOfRange = 1;
		corr_result = (result * R_R_REF_2kOhm) / (R_U_REF - result);
		if (corr_result < 20) {
			// change display-colour
			time.beep = 1;
			Backlight_LED(BL_RED_ON);
			// works! but it needs a switch-back-function --> main?
		} else {
			time.beep = 0;
			Backlight_LED(BL_BLUE_ON | BL_GREEN_ON | BL_RED_ON);
		}
		*decPoint = 1;
		*unit++ = 'O';
		*unit++ = 'h';
		*unit++ = 'm';
		*unit = 0;
		break;
	}
	if (measurement == DMM_MEASURE_I_DC || measurement == DMM_MEASURE_U_DC) {
		// invert for negative results
		if (!(PINB & (1 << PINB2))) {
			corr_result = -corr_result;
		}
	} else if (measurement == DMM_MEASURE_I_AC
			|| measurement == DMM_MEASURE_U_AC) {
		// include form factor for sinusoid signal
		corr_result *= 1.11;
	}

	*res = (int32_t) corr_result;
	return outOfRange;
}
