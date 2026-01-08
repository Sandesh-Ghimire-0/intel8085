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
    uint8_t mask;
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
        case 0x0C: { // INR c
            state->c++;
            updateFlagsINR(state, state->c);
            break;
        }
        case 0x14: { // INR d
            state->d++;
            updateFlagsINR(state, state->d);
            break;
        }
        case 0x1C: { // INR e
            state->e++;
            updateFlagsINR(state, state->e);
            break;
        }
        case 0x24: { // INR h
            state->h++;
            updateFlagsINR(state, state->h);
            break;
        }
        case 0x2C: { // INR l
            state->l++;
            updateFlagsINR(state, state->l);
            break;
        }
        case 0x34: { // INR memory's value at HL
            uint8_t result = --state->memory[state->h << 8 | state->l];
            updateFlagsINR(state, result);
            break;
        }
        case 0x3C: { // INR a
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
        case 0x0D:{  // DCR c
            state->c--;
            updateFlagsDCR(state, state->c);
            break;
        }
        case 0x15: { // DCR d
            state->d--;
            updateFlagsDCR(state, state->d);
            break;
        }
        case 0x1D:{  // DCR e
            state->e--;
            updateFlagsDCR(state, state->e);
            break;
        }
        case 0x25:{  // DCR h
            state->h--;
            updateFlagsDCR(state, state->h);
            break;
        }
        case 0x2D: { // DCR l
            state->l--;
            updateFlagsDCR(state, state->l);
            break;
        }
        case 0x35: { // DCR memory's value at HL
           uint8_t result = --state->memory[state->h << 8 | state->l];
           updateFlagsDCR(state, result);
            break;
        }
        case 0x3D: { // DCR a
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
        case 0x0E: {// MVI c, byte 
            state->c = opcode[1]; //           
            state->pc++;
            break;
        }
        case 0x16: {// MVI d, byte  
            state->d = opcode[1]; //           
            state->pc++;
            break;
        }
        case 0x1E: {// MVI e, byte  
            state->e = opcode[1]; //           
            state->pc++;
            break;
        }
        case 0x26: {// MVI h, byte  
            state->h = opcode[1]; //           
            state->pc++;
            break;
        }
        case 0x2E: {// MVI l, byte  
            state->l = opcode[1]; //           
            state->pc++;
            break;
        }
        case 0x36: {// MVI m, byte  
            state->memory[state->h << 8 | state->l] = opcode[1]; //  .    
            state->pc++;
            break;
        }
        case 0x3E: {// MVI a, byte  
            state->a = opcode[1]; //           
            state->pc++;
            break;
        }
        //MVI finished
        //Rotate started
        case 0x07: { // RLC (Rotate Left Circular)
            uint8_t bit7 = (state->a & 0x80) >> 7;
            state->a = (state->a << 1) | bit7;
            state->cc.cy = bit7;
            break; // <--- DON'T FORGET
        }

        case 0x0F: { // RRC (Rotate Right Circular)
            uint8_t bit0 = (state->a & 0x01);
            state->a = (state->a >> 1) | (bit0 << 7);
            state->cc.cy = bit0;
            break;
        }

        case 0x17: { // RAL (Rotate Left through Carry)
            uint8_t old_cy = state->cc.cy;
            state->cc.cy = (state->a & 0x80) >> 7;
            state->a = (state->a << 1) | (old_cy & 0x01); // Use old carry
            break;
        }

        case 0x1F: { // RAR (Rotate Right through Carry)
            uint8_t old_cy = state->cc.cy;
            state->cc.cy = (state->a & 0x01);
            state->a = (state->a >> 1) | ((old_cy & 0x01) << 7); // Use old carry
            break;
        }
        //Rotate finished
        //DAD started
        // case 0x09:{
        //     uint8_t higher_nibble = state->h + state->b;
        //     uint8_t lower_nibble = state->l + state->c;
        //     if (lower_nibble < state->l || lower_nibble < state->c){
        //         ++higher_nibble;
        //         if (lower_nibble < state->h || lower_nibble < state->b){
        //             state->cc.cy = 1;
        //         }
        //     }

        // }
        case 0x09: { // DAD B (HL = HL + BC)
            // 1. Combine pairs into 16-bit values
            uint32_t hl = (state->h << 8) | state->l;
            uint32_t bc = (state->b << 8) | state->c;

            // 2. Perform the 32-bit addition to catch the 16-bit carry
            uint32_t res = hl + bc;

            // 3. Store the result back into H and L
            state->h = (res & 0xFF00) >> 8;
            state->l = (res & 0x00FF);

            state->cc.cy = (res > 0xFFFF);
             break;
        }
        case 0x19: { // DAD D (HL = HL + DE)
            // 1. Combine pairs into 16-bit values
            uint32_t hl = (state->h << 8) | state->l;
            uint32_t de = (state->d << 8) | state->e;

            // 2. Perform the 32-bit addition to catch the 16-bit carry
            uint32_t res = hl + de;

            // 3. Store the result back into H and L
            state->h = (res & 0xFF00) >> 8;
            state->l = (res & 0x00FF);

            state->cc.cy = (res > 0xFFFF);
             break;
        }
        case 0x29: { // DAD H (HL = HL + HL)
            // 1. Combine pairs into 16-bit values
            uint32_t hl = (state->h << 8) | state->l;

            // 2. Perform the 32-bit addition to catch the 16-bit carry
            uint32_t res = 2 * hl;

            // 3. Store the result back into H and L
            state->h = (res & 0xFF00) >> 8;
            state->l = (res & 0x00FF);

            state->cc.cy = (res > 0xFFFF);
             break;
        }
        case 0x39: { // DAD SP (HL = HL + SP)
            // 1. Combine pairs into 16-bit values
            uint32_t hl = (state->h << 8) | state->l;
            // 2. Perform the 32-bit addition to catch the 16-bit carry
            uint32_t res = hl + state->sp;
            // 3. Store the result back into H and L
            state->h = (res & 0xFF00) >> 8;
            state->l = (res & 0x00FF);

            state->cc.cy = (res > 0xFFFF);
             break;
        }

        //DAD finished
        //LDAX started
        case 0x0A:{ //load accumulator from bc pair
            uint16_t addr = state->memory[state->b << 8 | state->c];
            state->a = state->memory[addr];
            break;
        }

        case 0x1A:{  // for de pair
            uint16_t addr = state->memory[state->d << 8 | state->e];
            state->a = state->memory[addr];
            break;
        }
        //LDAX finished
        //DCX started
        case 0x0B:{ // BC
            uint16_t bc = state->b << 8 | state->c;
            bc-=1;
            state->b = bc >> 8 & 0xFF;
            state->c = bc & 0xFF;
            break;
        }
        case 0x1B:{ // DE
            uint16_t de = state->d << 8 | state->e;
            de-=1;
            state->d = de >> 8 & 0xFF;
            state->e = de & 0xFF;
            break;
        }
        case 0x2B:{ // HL
            uint16_t hl = state->h << 8 | state->l;
            hl-=1;
            state->h = hl >> 8 & 0xFF;
            state->l = hl & 0xFF;
            break;
        }
        case 0x3B:{ // SP
            state->sp--;
        }
        //STA started
        case 0X32:{
            uint16_t memory_addr = opcode[2] << 8 | opcode[1];
            state->memory[memory_addr] = state->a; 
            state->pc+=2;
            break;
        }
        //STA finished
        //LDA started
        case 0x3A:{
            state->a = state->memory[opcode[2] << 8 | opcode[1]];
            state->pc+=2;
            break;
        }
        //LDA finished
        //SHLD started
        case 0x22:{
            uint16_t memory_addr = opcode[2] << 8 | opcode[1];
            state->memory[memory_addr] = state->l;
            state->memory[memory_addr+1] = state->h;
            state->pc += 2;
            break;
        }
        //SHLD finished
        case 0x2A:{ // LHLD
            uint16_t memory_addr = opcode[2] << 8 | opcode[1];
            state->h = state->memory[memory_addr+1];
            state->l = state->memory[memory_addr];
            state->pc += 2;
            break;
        }
        //SHLD finished
        //STC started
        case 0x37:{
            state->cc.cy = 1;
            break;
        }
        //STC finished
        //CMC started
        case 0x3F:{
            state->cc.cy = ~state->cc.cy; break;
        }
        case 0x2f: { // CMA
            state->a = ~state->a; 
            break;
}
        //CMC finished
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
    std::cout << std::endl;
    std::cout << "PC prev -> " << state->pc <<std::endl;
    state->pc += 1;  
    std::cout << "PC INCREMENTED -> " << state->pc <<std::endl;
    exit(1);
}

int main() {
    State8085 state = {0};
    state.memory = new uint8_t[0x10000]; // 64KB RAM
    state.pc = 0; // Standard start point
    state.sp = 0xFFFF;
    state.memory[0] = 0x0f;
    state.a = 0b10100011;

    while (true) {
        Emulate8085(&state);
     }

    return 0;
}