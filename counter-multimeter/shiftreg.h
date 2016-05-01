#ifndef SHIFTREG_H_
#define SHIFTREG_H_

#include <avr/io.h>

/*
 * Shift register port definitions
 */
#define SHIFT_IN_DDR        DDRA
#define SHIFT_IN_PORT       PORTA
#define SHIFT_IN_PIN        2

#define SHIFT_CLK_DDR       DDRD
#define SHIFT_CLK_PORT      PORTD
#define SHIFT_CLK_PIN       7

#define SHIFT_LATCH_DDR     DDRD
#define SHIFT_LATCH_PORT    PORTD
#define SHIFT_LATCH_PIN     5

struct {
    uint16_t content;
} shiftReg;

/**
 * \brief Initializes the external shift register
 *
 * Used uC outputs are configure. Also, all shift register
 * outputs are set to 0
 */
void shift_Init(void);

/**
 * \brief Set a shift register output high
 *
 * Only writes into the buffer, the output of the
 * register changes after calling shift_Update()
 */
void shift_Set(uint8_t bit);

/**
 * \brief Set a shift register output high
 *
 * Writes into the buffer and calls shift_Update()
 * immediately after that. Thus, the register output
 * assumes the new state after this function returns
 */
void shift_SetAndUpdate(uint8_t bit);

/**
 * \brief Set a shift register output low
 *
 * Only writes into the buffer, the output of the
 * register changes after calling shift_Update()
 */
void shift_Clear(uint8_t bit);

/**
 * \brief Set a shift register output low
 *
 * Writes into the buffer and calls shift_Update()
 * immediately after that. Thus, the register output
 * assumes the new state after this function returns
 */
void shift_ClearAndUpdate(uint8_t bit);

/**
 * \brief Updates the shift register outputs
 *
 * Transfers the content of the internal buffer
 * to the external shift registers
 */
void shift_Update(void);



#endif /* SHIFTREG_H_ */
