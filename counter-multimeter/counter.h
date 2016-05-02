#ifndef COUNTER_H_
#define COUNTER_H_
/*
 * uncomment to simulate counter functionality:
 * Instead of applying an actual signal, the signal
 * frequency is given via uart (signal frequency in ASCII
 * followed by '\n'). From the outside, all functions still
 * have the same functionality. Take care not to read from
 * the uart in other program parts.
 */
// #define CNT_SIMULATION

#include <avr/io.h>
#include <avr/interrupt.h>
#include "systime.h"
#include "shiftreg.h"
#ifdef CNT_SIMULATION
#include "includes/uart.h"
#include <stdlib.h>
#endif

#define CNT_GATE_CLOSED         0
#define CNT_GATE_OPENING        1
#define CNT_GATE_OPEN           2
#define CNT_GATE_CLOSING        3

#define CNT_GATE_MS(a)          (2000UL*a)

/*
 * different counter inputs
 */
#define CNT_IN_TTL			    0
#define CNT_IN_LF			    1
#define CNT_IN_HF			    2

/*
 * available prescaler at different inputs
 */
#define CNT_TTL_PRE_1           1

#define CNT_LF_PRE_1            1
#define CNT_LF_PRE_4            4
#define CNT_LF_PRE_8            8
#define CNT_LF_PRE_16           16
#define CNT_LF_PRE_32           32
#define CNT_LF_PRE_64           64

#define CNT_HF_PRE_20           20
#define CNT_HF_PRE_40           40
#define CNT_HF_PRE_80           80
#define CNT_HF_PRE_160          160

/*
 * positions of the counter pins on the shift registers
 */
#define CNT_DIVSEL1_SHIFT       2
#define CNT_DIVSEL2_SHIFT       3
#define CNT_DIVSEL3_SHIFT       4
#define CNT_RESET_SHIFT         0
#define CNT_HFSEL1_SHIFT        6
#define CNT_HFSEL2_SHIFT        5
#define CNT_INPUTSEL_SHIFT      1

/*
 * positions of counter signals on multiplexer
 */
#define CNT_MUX_TTL             6
#define CNT_MUX_LF_DIRECT       5
#define CNT_MUX_DIVIDER1        4
#define CNT_MUX_DIVIDER2        3
#define CNT_MUX_DIVIDER4        2
#define CNT_MUX_DIVIDER8        1
#define CNT_MUX_DIVIDER16       0

struct {
    volatile uint16_t refOverflows;
    volatile uint32_t signalOverflows;
    uint8_t refExternal :1;
    uint8_t refInternal :1;
    volatile uint8_t refGateStatus;
    volatile uint8_t sigGateStatus;
    uint16_t sigGateEdges;
    uint32_t sigGateOpenCnt;
    uint32_t sigGateCloseCnt;
    uint8_t prescaler;
#ifdef CNT_SIMULATION
    uint32_t inputFrequency;
    char uartInBuf[20];
    uint8_t uartInBufPos;
#endif
} counter;

/*
 * interrupt-save reading of counter.refOverflows
 */
inline uint16_t counter_GetRefOvfs(void) {
    TIMSK1 &= ~(1 << OCIE1A);
    uint16_t buf = counter.refOverflows;
    TIMSK1 |= (1 << OCIE1A);
    return buf;
}

void counter_Init(void);
/**
 * \brief Sets F_CPU as reference counter input
 *
 * Configures the reference counter to use 2MHz derived from
 * the external crystal. As all measurements depend on this counter
 * results will only be as accurate as the crystal
 */
void counter_RefInternal(void);

/**
 * \brief Sets external signal as reference counter input
 *
 * Attempts to use an external 2MHz signal to clock the reference counter.
 * The timer source is changed and subsequently checked for plausibility.
 * If no signal is available or the signal deviates from 2MHz the reference
 * is switched back to the crystal (using counter_RefInternal())
 *
 * \return 0 on success, 1 on failure
 */
uint8_t counter_RefExternal(void);

/**
 * \brief Selects an input source and signal prescaler
 *
 * Switches between three inputs and the corresponding prescalers.
 * Inputs can be:
 * CNT_IN_TTL with prescalers:
 *      CNT_TTL_PRE_1
 * CNT_IN_LF with prescalers:
 *      CNT_LF_PRE_1
 *      CNT_LF_PRE_4
 *      CNT_LF_PRE_8
 *      CNT_LF_PRE_16
 *      CNT_LF_PRE_32
 *      CNT_LF_PRE_64
 * CNT_IN_HF with prescalers:
 *      CNT_HF_PRE_20
 *      CNT_HF_PRE_40
 *      CNT_HF_PRE_80
 *      CNT_HF_PRE_160
 * \param in Selected input, either CNT_IN_TTL, CNT_IN_LF or CNT_IN_HF
 * \param prescaler Prescaler for the input (see description)
 */
void counter_SelectInput(uint8_t in, uint8_t prescaler);

/**
 * \brief Conducts a direct measurement of the signal frequency
 *
 * Enables the counter gate for a precise specified time and counts
 * the number of low-to-high transitions on the input signal during that
 * time.
 *
 * IMPORTANT:
 * No interrupts taking more than 250us are allowed to be enabled before
 * calling this function!
 *
 * \param ticks Gate-open-time in terms of reference timer ticks (1tick=500ns)
 *              Range is 1.5ms to approx. 65s
 * \return 0, if specified ticks is out of range. Otherwise the number of impulses
 *         counted during the gate-open-time
 */
uint32_t counter_MeasureRefGate(uint32_t ticks);

/**
 * \brief Conducts an indirect measurement of the signal frequency
 *
 * Measures the time (in terms of reference timer ticks) between a specific
 * number of edges in the input signal, an edge being a low-to-high OR a
 * high-to-low transition. Therefore, n periods equals 2n edges.
 *
 * IMPORTANT:
 * No interrupts taking more than the minimum time between two edges are
 * allowed to be enabled before calling this function!
 *
 * \param edges Number of edges to wait for
 * \param timeout Maximum wait time in ms
 * \return 0, if timeout occured. Otherwise the time between the first and last
 *         edge in terms of reference counter ticks
 */
uint32_t counter_SignalPulsesTime(uint16_t edges, uint16_t timeout);

/**
 * \brief Selects one of the input channels at the multiplier
 *
 * Internally used function, don't call from out of module unless you
 * know what you are doing.
 */
void counter_SelectMux(uint8_t mux);

#ifdef CNT_SIMULATION
void counter_SimUARTInput(uint8_t c);
#endif

#endif /* COUNTER_H_ */
