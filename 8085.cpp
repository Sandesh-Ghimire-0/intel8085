#include"8085.hpp"

void updateFlagsDCR(State8085* state, uint8_t result) {
    state->cc.z = (result == 0);          // Is the result zero?
    state->cc.s = (result & 0x80) != 0;   // Is bit 7 set (negative)?
    //state->cc.p = CheckParity(result);    // Is the parity even? yet to be done!!
    // Note: DCR does NOT affect the Carry (CY) flag.
}

void updateFlagsINR(State8085* state, uint8_t result) {
    state->cc.z = (result == 0);          // Is the result zero?
    state->cc.s = (result & 0x80) != 0;   // Is bit 7 set (negative)?
    //state->cc.p = CheckParity(result);    // Is the parity even? yet to be done!!
    // Note: DCR does NOT affect the Carry (CY) flag.
}
void Emulate8085(State8085* state) {
    // 1. Fetch: Get the opcode at the current Program Counter
    unsigned char *opcode = &state->memory[state->pc];
    
    // 2. Decode & Execute
    switch (*opcode) {
        case 0x00: // NOP (No Operation)
            break;
        //LXI implementation started
        case 0X01:{
            state->b = state->memory[state->pc+2]; // Higher byte goes to B
            state->c = state->memory[state->pc+1]; // Lower byte goes to C
            state->pc += 2;
            break;
        }
        case 0x11:{
            state->d = state->memory[state->pc+2]; // Higher byte goes to D
            state->e = state->memory[state->pc+1]; // Lower byte goes to E
            state->pc += 2;
            break;
        }
        case 0X21:{
            state->h = state->memory[state->pc+2]; // Higher byte goes to H
            state->l = state->memory[state->pc+1]; // Lower byte goes to L
            state->pc += 2;
            break;
        }
        case 0x31:{  // LXI SP, data16
            state->sp = (state->memory[state->pc+2] << 8) | state->memory[state->pc+1];
            state->pc += 2;
            break;
        }
        // LXI completed
        //STAX started
        case 0x02:{
            state->memory[(state->b << 8) | state->c] = state->a; // no need to increment stack 
            break;  // no need to increment pc for single byte opcode
        }
        case 0x12:{
            state->memory[(state->d << 8) | state->e] = state->a;
            break;
        }
        //STAX finished
        //INX started 
        case 0x03:{  // INX B
            state->c++;
            if (state->c == 0) {  // C wrapped from 0xFF to 0x00
                state->b++;       // So increment B 
            }
            break;
        }
        case 0x13:{  // INX D
            state->e++;
            if (state->e == 0) {  // C wrapped from 0xFF to 0x00
                state->d++;       // So increment B 
            }
            break;
        }
        case 0x23: { // INX H
            state->l++;
            if (state->l == 0) {  // C wrapped from 0xFF to 0x00
                state->h++;       // So increment B 
            }
            break;
        }
        case 0x33:{  // INX SP
            state->sp++; // If you execute INX SP, you are effectively "shrinking" the stack by moving the pointer toward higher memory addresses (where the stack started).
            break;
        }
        //INX finished
        //INR started  (flag check is yet to be done)
        case 0x04: { // INR b
            state->b++;
            updateFlagsINR(state, state->b);
            break;
        }
        case 0x0c: { // INR c
            state->c++;
            updateFlagsINR(state, state->c);
            break;
        }
        case 0x14: { // INR d
            state->d++;
            updateFlagsINR(state, state->d);
            break;
        }
        case 0x1c: { // INR e
            state->e++;
            updateFlagsINR(state, state->e);
            break;
        }
        case 0x24: { // INR h
            state->h++;
            updateFlagsINR(state, state->h);
            break;
        }
        case 0x2c: { // INR l
            state->l++;
            updateFlagsINR(state, state->l);
            break;
        }
        case 0x34: { // INR memory's value at HL
            uint8_t result = --state->memory[state->h << 8 | state->l];
            updateFlagsINR(state, result);
            break;
        }
        case 0x3c: { // INR a
            state->a++;
            updateFlagsINR(state, state->a);
            break;
        }
        //INR finished
        //DCR started (flagis yet to be checked)
        case 0x05:{  // DCR b
            state->b--;
            updateFlagsDCR(state, state->b);
            break;
        }
        case 0x0d:{  // DCR c
            state->c--;
            updateFlagsDCR(state, state->c);
            break;
        }
        case 0x15: { // DCR d
            state->d--;
            updateFlagsDCR(state, state->d);
            break;
        }
        case 0x1d:{  // DCR e
            state->e--;
            updateFlagsDCR(state, state->e);
            break;
        }
        case 0x25:{  // DCR h
            state->h--;
            updateFlagsDCR(state, state->h);
            break;
        }
        case 0x2d: { // DCR l
            state->l--;
            updateFlagsDCR(state, state->l);
            break;
        }
        case 0x35: { // DCR memory's value at HL
           uint8_t result = --state->memory[state->h << 8 | state->l];
           updateFlagsDCR(state, result);
            break;
        }
        case 0x3d: { // DCR a
            state->a--;
            updateFlagsDCR(state, state->b);
            break;
        }
        //DCR   finished
        //MVI started, we will start using opcode[1], opcode[2] from here 
        case 0x06: {// MVI b, byte.
            state->b = opcode[1]; // Get the data from the next byte    
            state->pc++;
            break;
        }
        case 0x0e: {// MVI c, byte 
            state->c = opcode[1]; //           
            state->pc++;
            break;
        }
        case 0x16: {// MVI d, byte  
            state->d = opcode[1]; //           
            state->pc++;
            break;
        }
        case 0x1e: {// MVI e, byte  
            state->e = opcode[1]; //           
            state->pc++;
            break;
        }
        case 0x26: {// MVI h, byte  
            state->h = opcode[1]; //           
            state->pc++;
            break;
        }
        case 0x2e: {// MVI l, byte  
            state->l = opcode[1]; //           
            state->pc++;
            break;
        }
        case 0x36: {// MVI m, byte  
            state->memory[state->h << 8 | state->l] = opcode[1]; //  .    
            state->pc++;
            break;
        }
        case 0x3e: {// MVI a, byte  
            state->a = opcode[1]; //           
            state->pc++;
            break;
        }
        //MVI finished
        case 0x80: // ADD B (Add register B to A)
        {
            uint16_t res = (uint16_t)state->a + (uint16_t)state->b;
            state->cc.z = ((res & 0xff) == 0);
            state->cc.s = ((res & 0x80) != 0);
            state->cc.cy = (res > 0xff);
            state->a = res & 0xff;
        }
            break;

        case 0xc3:{// JMP address
            state->pc = (opcode[2] << 8) | opcode[1];
            return; // Return so we don't increment PC at the bottom
        }
    }

    // Move to next instruction
    state->pc += 1;  
}

int main() {
    State8085 state = {0};
    state.memory = new uint8_t[0x10000]; // 64KB RAM
    state.pc = 0; // Standard start point
    state.sp = 0xFFFF;
    state.memory[0] = 8;
    std::cout << state.memory[0] << "...." << std::endl;  // error
    std::cout << static_cast<int>(state.memory[0]) << "...." << std::endl; // the result i wanted.

    // while (true) {
    //     Emulate8085(&state);
    // }

    return 0;
}