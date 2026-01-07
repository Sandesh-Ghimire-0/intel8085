#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <bitset>


struct ConditionCodes {
    uint8_t z:1;    // Zero  // :1 is the bit field declaration
    uint8_t s:1;    // Sign      i.e z will use 1 bit instead of all 8 bits.
    uint8_t p:1;    // Parity
    uint8_t cy:1;   // Carry
    uint8_t ac:1;   // Auxiliary Carry
    uint8_t pad:3;  // Padding to fill 8 bits
};

struct State8085 {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;    // Stack Pointer
    uint16_t pc;    // Program Counter
    uint8_t *memory;
    struct ConditionCodes cc;
};

void Emulate8085(State8085* state);
