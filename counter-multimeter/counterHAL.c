#include "counterHAL.h"

void counter_Init(void) {
	// reference gate enable pin (OC1B)
	DDRD |= (1 << PD4);
	// input capture pin (ICP1)
	DDRD &= ~(1 << PD6);
	PORTD |= (1 << PD6);
	// signal and reference counter pins
	DDRB &= ~((1 << PB0) | (1 << PB1));
	// enable pull-ups
	PORTB |= (1 << PB0) | (1 << PB1);

	shift_Clear(CNT_RESET_SHIFT);
//    shift_Set(CNT_HFSEL1_SHIFT);
//    shift_Set(CNT_HFSEL2_SHIFT);
	shift_Clear(CNT_INPUTSEL_SHIFT);
	counter_SelectMux(CNT_MUX_TTL);
	counter.prescaler = 1;

	counter_RefInternal();
	// setup timer 0 with T0 as clock source
	TCCR0A = 0;
	TCCR0B = (1 << CS02) | (1 << CS01) | (1 << CS00);
	TIMSK0 |= (1 << TOIE0);
	counter.refGateStatus = CNT_GATE_CLOSED;
	counter.sigGateStatus = CNT_GATE_CLOSED;
}

void counter_RefInternal(void) {
	if (!counter.refInternal) {
		counter.refInternal = 1;
		counter.refExternal = 0;
		// reset any previous timer configuration
		TCCR1A = 0;
		TCCR1B = 0;
		TIMSK1 = 0;
		// Use timer in CTC mode, resetting at 2000
		TCCR1B |= (1 << WGM12);
		OCR1A = 1999;
		TIMSK1 |= (1 << OCIE1A);
		// configure Timer 3 clocked by local crystal.
		// Using prescaler 8 results in 2MHz timer clock,
		// this gives us 1000 overflows per second
		TCCR1B |= (1 << CS11);
	}
}

uint8_t counter_RefExternal(void) {
	uint8_t res = 0;
	if (!counter.refExternal) {
		counter.refExternal = 1;
		counter.refInternal = 0;
		// reset any previous timer configuration
		TCCR1A = 0;
		TCCR1B = 0;
		TIMSK1 = 0;
		// Use timer in CTC mode, resetting at 2000
		TCCR1B |= (1 << WGM12);
		OCR1A = 1999;
		TIMSK1 |= (1 << OCIE1A);
		// configure Timer 3 clocked by external clock.
		// Using external clock at T0, where 2MHz should be applied
		// this gives us 10k overflows per second
		TCCR1B |= (1 << CS12) | (1 << CS11) | (1 << CS10);

		// check whether timer runs properly
		counter.refOverflows = 0;
		time_Waitms(100);
		uint16_t buf = counter_GetRefOvfs();
		if (buf < 90 || buf > 110) {
			// external clock deviates too much -> probably
			// no external signal or wrong frequency
			// -> switch back to internal reference
			counter_RefInternal();
			res = 1;
		}
	}
	return res;
}

uint32_t counter_MeasureRefGate(uint32_t ticks) {
	if (ticks < 3000) {
		// gate time is too short
		// minimum gate time is 3000 ticks or 1500us
		return 0;
	} else if (ticks > 131070000UL) {
		// gate time is too long
		// maximum gate time is 131070000 ticks or 65.535 seconds
		return 0;
	}
#ifdef CNT_SIMULATION
	// calculate input pin frequency
	uint64_t pinFreq = counter.inputFrequency / counter.prescaler;
	// account for aliasing
	pinFreq = pinFreq % F_CPU;
	if (pinFreq > F_CPU / 2) {
		pinFreq = F_CPU - pinFreq;
	}
	// calculate pin changes during measurement window
	pinFreq *= ticks;
	pinFreq /= 2000000UL;
	// simulate measurement time
	time_Waitms(ticks / 2000);
	return pinFreq;
#else
	// reset impulse counter
	shift_ClearAndUpdate(CNT_RESET_SHIFT);
	// this is interrupt-save because hardware counter is stopped at the moment
	TCNT0 = 0;
	counter.signalOverflows = 0;
	// enable impulse counter
	shift_SetAndUpdate(CNT_RESET_SHIFT);
	/*****************************************************************
	 * Step 1: 	Calculate exact timer compare values and overflows
	 *          too achieve precise gate time
	 ****************************************************************/
	uint16_t overflows = ticks / 2000;
	uint16_t remainingTicks = ticks - (uint32_t) overflows * 2000;
	uint16_t startOCR, stopOCR;
	// move start and stop compare matches into middle timer values.
	// This is to avoid collision of overflow and compare match interrupts
	if (remainingTicks < 1000) {
		startOCR = 500;
		stopOCR = (500 + remainingTicks);
	} else {
		startOCR = 1500;
		stopOCR = 1500 + remainingTicks - 2000;
		overflows++;
	}
	// overflows is at least 2 at this point
	/*****************************************************************
	 * Step 2:  Handle interrupts and OC0B pin
	 ****************************************************************/
	// prepare gate open compare match
	OCR1B = startOCR;
	// wait for the double buffered register to update
	uint16_t buf = counter_GetRefOvfs();
	while (buf == counter_GetRefOvfs())
		;
//    // reset timer
//    TCNT1 = 0;
	// reset reference overflow counter
	// safe because timer has just been reseted
	counter.refOverflows = 0;
	// enable compare match pin functionality
	// start hardware signal counter as soon as gate opens
	TCCR1A |= (1 << COM1B1) | (1 << COM1B0);
	// set gate status and enable 'gate opened interrupt'
	counter.refGateStatus = CNT_GATE_OPENING;
	TIFR1 |= (1 << OCF1B);
	TIMSK1 |= (1 << OCIE1B);
	// wait for gate to open
	while (counter.refGateStatus != CNT_GATE_OPEN)
		;
	TIMSK1 &= ~(1 << OCIE1B);
	// prepare gate close compare match
	OCR1B = stopOCR;
	// wait for timer to finish overflows
	while (counter_GetRefOvfs() != overflows)
		;
	// enable compare match pin functionality
	// start hardware signal counter as soon as gate opens
	TCCR1A &= ~(1 << COM1B0);
	// set gate status and enable 'gate opened interrupt'
	counter.refGateStatus = CNT_GATE_CLOSING;
	TIFR1 |= (1 << OCF1B);
	TIMSK1 |= (1 << OCIE1B);
	// wait for gate to close
	while (counter.refGateStatus != CNT_GATE_CLOSED)
		;
	// keep pin low even after disabling compare match action
	TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));
	TIMSK1 &= ~(1 << OCIE1B);

	// at this point OC0B (PB4) has been high for exactly ticks*(1/2MHz) seconds
	/*****************************************************************
	 * Step 3:  Calculate counted signal impulses
	 ****************************************************************/
	// once again, timer 1 is stopped so this is interrupt save
	uint32_t signalPulses = counter.signalOverflows * 256;
	signalPulses += TCNT0;
	return signalPulses;
#endif
}

uint32_t counter_SignalPulsesTime(uint32_t edges, uint16_t timeout) {
#ifdef CNT_SIMULATION
	// calculate input pin frequency
	uint32_t pinFreq = counter.inputFrequency / counter.prescaler;
	// account for aliasing
	pinFreq = pinFreq % F_CPU;
	if (pinFreq > F_CPU / 2) {
		pinFreq = F_CPU - pinFreq;
	}
	// calculate time between pinchanges (in ps)
	uint64_t timeDiffEdgeps = (uint64_t) 500000000000UL / pinFreq;
	uint32_t timeDiffms = timeDiffEdgeps / 1000000;
	timeDiffms *= edges;
	timeDiffms /= 1000;
	if (timeDiffms >= timeout) {
		// timeout occured
		time_Waitms(timeout);
		return 0;
	} else {
		time_Waitms(timeDiffms);
		// calculate reference timer ticks
		timeDiffEdgeps *= edges;
		// reference timer does one step each 500ns
		timeDiffEdgeps /= 500000;
		return timeDiffEdgeps;
	}
#else
	// setup input capture functionality at ICP3
	// start with rising edge
	TCCR1B |= (1 << ICES1);
	uint16_t buf = counter_GetRefOvfs();
	while (buf == counter_GetRefOvfs())
		;
	// reset reference overflow counter
	// safe because timer has just been reseted
	counter.refOverflows = 0;
	// initialize edge counter
	counter.sigGateEdges = edges;
	counter.sigGateStatus = CNT_GATE_OPENING;
	// enable impulse detection interrupt
	if (TIFR1 & (1 << ICF1))
		TIFR1 |= (1 << ICF1);
	TIMSK1 |= (1 << ICIE1);
	uint32_t starttime = time_Getms();
	// wait for gate to close (this is happening in Timer3 input capture interrupt)
	while ((counter.sigGateStatus != CNT_GATE_CLOSED)
			&& (starttime + timeout > time_Getms()))
		;
	TIMSK1 &= ~(1 << ICIE1);
	if (counter.sigGateStatus != CNT_GATE_CLOSED) {
		// timeout has occured
		return 0;
	}
	if (counter.sigGateOpenCnt > counter.sigGateCloseCnt) {
		// overflow counter overflowed -> time period has been too long
		// Measurement won't be accurate -> return 0, indicating an error
		return 0;
	} else {
		// calculate passed time
		return counter.sigGateCloseCnt - counter.sigGateOpenCnt;
	}
#endif
}

uint16_t counter_MeasureDuty(uint16_t timeout) {
#ifdef CNT_SIMULATION
	// dummy duty cycle of 50%
	return 500;
#else
	// setup input capture functionality at ICP3
	// start with rising edge
	TCCR1B |= (1 << ICES1);
	uint16_t buf = counter_GetRefOvfs();
	while (buf == counter_GetRefOvfs())
		;
	// reset reference overflow counter
	// safe because timer has just been reseted
	counter.refOverflows = 0;
	// enable duty cycle measurement
	counter.sigGateStatus = CNT_GATE_DUTY_HIGH1;
	// enable impulse detection interrupt
	TIFR1 |= (1 << ICF1);
	TIMSK1 |= (1 << ICIE1);
	uint32_t starttime = time_Getms();
	// wait for gate to close (this is happening in Timer3 input capture interrupt)
	while ((counter.sigGateStatus != CNT_GATE_CLOSED)
			&& (starttime + timeout > time_Getms()))
		;
	TIMSK1 &= ~(1 << ICIE1);
	if (counter.sigGateStatus != CNT_GATE_CLOSED) {
		// timeout has occured
		return 0;
	}
	if (counter.sigGateOpenCnt > counter.sigGateCloseCnt) {
		// overflow counter overflowed -> time period has been too long
		// Measurement won't be accurate -> return 0, indicating an error
		return 0;
	} else {
		// calculate duty cycle
		uint32_t period = counter.sigGateCloseCnt - counter.sigGateOpenCnt;
		uint32_t high = counter.sigDutyMidCnt - counter.sigGateOpenCnt;
		return ((uint64_t) (high)) * 1000 / period;
	}
#endif
}

void counter_SelectInput(uint8_t in, uint8_t prescaler) {
	switch (in) {
	case CNT_IN_TTL:
		counter.input = CNT_IN_TTL;
		counter_SelectMux(CNT_MUX_TTL);
		counter.prescaler = 1;
		break;
	case CNT_IN_LF:
		counter.input = CNT_IN_LF;
		// select LF input signal
		shift_Clear(CNT_INPUTSEL_SHIFT);
		// select prescaler on counter ic
		// LF has a fixed prescaler of 4 (except in LF direct).
		// Too achieve a prescaler of 32 for example, the prescaler
		// of the counter ic must be set to 8
		switch (prescaler) {
		case CNT_LF_PRE_1:
			// no prescaler used, switch to direct LF
			counter_SelectMux(CNT_MUX_LF_DIRECT);
			counter.prescaler = 1;
			break;
		case CNT_LF_PRE_4:
			counter_SelectMux(CNT_MUX_DIVIDER1);
			counter.prescaler = 4;
			break;
		case CNT_LF_PRE_8:
			counter_SelectMux(CNT_MUX_DIVIDER2);
			counter.prescaler = 8;
			break;
		case CNT_LF_PRE_16:
			counter_SelectMux(CNT_MUX_DIVIDER4);
			counter.prescaler = 16;
			break;
		case CNT_LF_PRE_32:
			counter_SelectMux(CNT_MUX_DIVIDER8);
			counter.prescaler = 32;
			break;
		case CNT_LF_PRE_64:
			counter_SelectMux(CNT_MUX_DIVIDER16);
			counter.prescaler = 64;
			break;
		}
		break;
//    case CNT_IN_HF:
//    	counter.input = CNT_IN_HF;
//       // select HF input signal
//        shift_Set(CNT_INPUTSEL_SHIFT);
//        // select prescaler on counter ic and HF frontend
//        switch (prescaler) {
//        case CNT_HF_PRE_20:
//            shift_Set(CNT_HFSEL1_SHIFT);
//            shift_Set(CNT_HFSEL2_SHIFT);
//            counter_SelectMux(CNT_MUX_DIVIDER1);
//            counter.prescaler = 20;
//            break;
//        case CNT_HF_PRE_40:
//            shift_Set(CNT_HFSEL1_SHIFT);
//            shift_Set(CNT_HFSEL2_SHIFT);
//            counter_SelectMux(CNT_MUX_DIVIDER2);
//            counter.prescaler = 40;
//            break;
//        case CNT_HF_PRE_80:
//            shift_Set(CNT_HFSEL1_SHIFT);
//            shift_Set(CNT_HFSEL2_SHIFT);
//            counter_SelectMux(CNT_MUX_DIVIDER4);
//            counter.prescaler = 80;
//            break;
//        case CNT_HF_PRE_160:
//            shift_Set(CNT_HFSEL1_SHIFT);
//            shift_Clear(CNT_HFSEL2_SHIFT);
//            counter_SelectMux(CNT_MUX_DIVIDER4);
//            counter.prescaler = 160;
//            break;
//        }
//        break;
	default:
		// should never reach this point
		// switch to unused MUX input
		// TODO throw error
		counter_SelectMux(7);
		break;
	}
}

void counter_SelectMux(uint8_t mux) {
	if (mux & 0x01) {
		shift_Set(CNT_DIVSEL1_SHIFT);
	} else {
		shift_Clear(CNT_DIVSEL1_SHIFT);
	}
	if (mux & 0x02) {
		shift_Set(CNT_DIVSEL2_SHIFT);
	} else {
		shift_Clear(CNT_DIVSEL2_SHIFT);
	}
	if (mux & 0x04) {
		shift_Set(CNT_DIVSEL3_SHIFT);
	} else {
		shift_Clear(CNT_DIVSEL3_SHIFT);
	}
	shift_Update();
}

#ifdef CNT_SIMULATION
void counter_SimUARTInput(uint8_t c) {
	if (c != '\n') {
		counter.uartInBuf[counter.uartInBufPos++] = c;
	} else {
		// newline received -> interpret input as simulation frequency
		counter.inputFrequency = strtoul(counter.uartInBuf, 0, 10);
		// return received freq on uart to detect transmission errors
		ltoa(counter.inputFrequency, counter.uartInBuf, 10);
		UART_PutString(counter.uartInBuf);
		counter.uartInBufPos = 0;
		uint8_t i = 0;
		for (i = 0; i < 20; i++) {
			counter.uartInBuf[i] = 0;
		}
	}
}
#endif

/*
 * ISR occurs whenever the reference counter reaches its limit.
 * This happens every millisecond
 */ISR(TIMER1_COMPA_vect) {
	counter.refOverflows++;
#ifdef CNT_SIMULATION
	int16_t c = UART_GetChar();
	if (c >= 0) {
		counter_SimUARTInput(c);
	}
#endif
}

/*
 * ISR indicates that the gate has opened or closed during
 * the direct frequency measurement (deactivated otherwise)
 */ISR(TIMER1_COMPB_vect) {
	if (counter.refGateStatus == CNT_GATE_OPENING) {
		counter.refGateStatus = CNT_GATE_OPEN;
	} else if (counter.refGateStatus == CNT_GATE_CLOSING) {
		counter.refGateStatus = CNT_GATE_CLOSED;
	}
}

/*
 * ISR indicated that the signal counter has overflowed.
 * This happens after 256 impulses
 */ISR(TIMER0_OVF_vect) {
	counter.signalOverflows++;
}

/*
 * ISR is active during indirect frequency measurement and saves
 * the timestamps of the first and last signal edge
 */ISR(TIMER1_CAPT_vect) {
	// save timestamp
	uint16_t captureTime = ICR1;
	// change edge detection
	TCCR1B ^= (1 << ICES1);
	switch (counter.sigGateStatus) {
	case CNT_GATE_OPENING:
		counter.sigGateStatus = CNT_GATE_OPEN;
		// save timestamp of the first impulse
		counter.sigGateOpenCnt = (uint32_t) counter.refOverflows * 2000 + captureTime;
		break;
	case CNT_GATE_OPEN:
		counter.sigGateEdges--;
		if (counter.sigGateEdges == 0) {
			// reached correct number of edges
			counter.sigGateStatus = CNT_GATE_CLOSED;
			counter.sigGateCloseCnt = (uint32_t) counter.refOverflows * 2000 + captureTime;
		}
		break;
	case CNT_GATE_DUTY_HIGH1:
		counter.sigGateStatus = CNT_GATE_DUTY_LOW;
		// save timestamp of the first rising edge
		counter.sigGateOpenCnt = (uint32_t) counter.refOverflows * 2000 + captureTime;
		break;
	case CNT_GATE_DUTY_LOW:
		counter.sigGateStatus = CNT_GATE_DUTY_HIGH2;
		// save timestamp of the falling edge
		counter.sigDutyMidCnt = (uint32_t) counter.refOverflows * 2000 + captureTime;
		break;
	case CNT_GATE_DUTY_HIGH2:
		counter.sigGateStatus = CNT_GATE_CLOSED;
		// save timestamp of the second rising edge
		counter.sigGateCloseCnt = (uint32_t) counter.refOverflows * 2000 + captureTime;
		break;
	default:
		// This interrupt shouldn't be active if the gate isn't open/about to open!
		// -> disable interrupt
		counter.sigGateStatus = CNT_GATE_CLOSED;
		TIMSK1 &= ~(1 << ICIE1);
		break;
	}
}
