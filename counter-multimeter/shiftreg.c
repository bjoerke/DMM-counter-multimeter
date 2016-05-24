#include "shiftreg.h"

void shift_Init(void) {
    // configure pins as outputs
    SHIFT_IN_DDR |= (1 << SHIFT_IN_PIN);
    SHIFT_CLK_DDR |= (1 << SHIFT_CLK_PIN);
    SHIFT_LATCH_DDR |= (1 << SHIFT_LATCH_PIN);
    // initialize shift register content
    shiftReg.content = 0;
    shift_Update();
}

void shift_Set(uint8_t bit) {
    shiftReg.content |= (1 << bit);
}

void shift_SetAndUpdate(uint8_t bit) {
    shift_Set(bit);
    shift_Update();
}

void shift_Clear(uint8_t bit) {
    shiftReg.content &= ~(1 << bit);
}

void shift_ClearAndUpdate(uint8_t bit) {
    shift_Clear(bit);
    shift_Update();
}

void shift_Update(void) {
    uint16_t buf = shiftReg.content;
    uint8_t i;
    // iterate trough all 16 bit
    for (i = 0; i < 16; i++) {
        if (buf & 0x8000) {
            SHIFT_IN_PORT |= (1 << SHIFT_IN_PIN);
        } else {
            SHIFT_IN_PORT &= ~(1 << SHIFT_IN_PIN);
        }
        SHIFT_CLK_PORT |= (1<<SHIFT_CLK_PIN);
        asm volatile("NOP");
        asm volatile("NOP");
        asm volatile("NOP");
        asm volatile("NOP");
        SHIFT_CLK_PORT &= ~(1<<SHIFT_CLK_PIN);
        asm volatile("NOP");
        asm volatile("NOP");
        asm volatile("NOP");
        asm volatile("NOP");
        buf <<= 1;
    }
    // update shift register outputs
    SHIFT_LATCH_PORT |= (1<<SHIFT_LATCH_PIN);
    asm volatile("NOP");
    asm volatile("NOP");
    asm volatile("NOP");
    asm volatile("NOP");
    SHIFT_LATCH_PORT &= ~(1<<SHIFT_LATCH_PIN);
}
