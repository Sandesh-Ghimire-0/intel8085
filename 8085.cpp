#include"8085.hpp"

int CheckParity(uint8_t result)
{
    int one_count = 0;
    int last_bit;
    while(result != 0){
        last_bit = result & 0x01;
        one_count += last_bit;
        result = result >> 1;
    }
    return (~(one_count % 2));  // return 1 if even parity else 0;
}

void updateFlagsDCR(State8085* state, uint8_t result) {
    state->cc.z = (result == 0);          // Is the result zero?
    state->cc.s = (result & 0x80) != 0;   // Is bit 7 set (negative)?
    state->cc.p = CheckParity(result);    // Is the parity even? yet to be done!!
    //Note: DCR and INR does NOT affect the Carry (CY) flag.
}

void updateFlagsINR(State8085* state, uint8_t result) {
    state->cc.z = (result == 0);          // Is the result zero?
    state->cc.s = (result & 0x80) != 0;   // Is bit 7 set (negative)?
    state->cc.p = CheckParity(result);    // Is the parity even? yet to be done!!
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
            std::cout << "parity prev is  " << (int)state->cc.p << std::endl;
            updateFlagsINR(state, state->b);
            std::cout << "parity is  " << (int)state->cc.p << std::endl;
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
        
        case 0x3F:{ // CMC
            state->cc.cy = ~state->cc.cy; break;
        }
        case 0x2f: { // CMA
            state->a = ~state->a; 
            break;
        }
        case 0x40:{ // MOV B, B
            // do nothing since it is just moving B to B
            break;
        }

        case 0x41:{ 
            state->b = state->c;
            break;
        }
        case 0x42:{  
            state->b = state->d;
            break;
        }
        case 0x43:{  
            state->b = state->e;
            break;
        }
        case 0x44:{
            state->b = state->h;
            break;
        }
        case 0x45:{
            state->b = state->l;
            break;
        }
        case 0x46:{
            state->b = state->memory[state->h << 8 | state-> l];
            break;
        }
        case 0x47:{
            state->b = state->a;
            break;
        }
        case 0x48:{  // mov c
            state->c = state->b;
            break;
        }
        case 0x49:{
            // do nothing since mov c to c
            break;
        }
        case 0x4A:{
            state->c = state->d;
            break;
        }
        case 0x4B:{
            state->c = state->e;
            break;
        }
        case 0x4C:{
            state->c = state->h;
            break;
        }
        case 0x4D:{
            state->c = state->l;
            break;
        }
        case 0x4E:{
            state->c = state->memory[state->h << 8 | state-> l];
            break;
        }
        case 0x4F:{  
            state->c = state->a;
            break;
        }
        case 0x50:{ // mov d
            state->d = state->b;
            break;
        }
        case 0x51:{
            state->d = state->c;
            break;
        }
        case 0x52:{

            break;
        }
        case 0x53:{
            state->d = state->e;
            break;
        }
        case 0x54:{
            state->d = state->h;
            break;
        }
        case 0x55:{
            state->d = state->l;
            break;
        }
        case 0x56:{
            state->d = state->memory[state->h << 8 | state-> l];
            break;
        }
        case 0x57:{
            state->d = state->a;
            break;
        }
        case 0x58:{  // mov e
            state->e = state->b;
            break;
        }
        case 0x59:{
            state->e = state->c;
            break;
        }
        case 0x5A:{
            state->e = state->d;
            break;
        }
        case 0x5B:{

            break;
        }
        case 0x5C:{
            state->e = state->h;
            break;
        }
        case 0x5D:{
            state->e = state->l;
            break;
        }
        case 0x5E:{
            state->e = state->memory[state->h << 8 | state-> l];
            break;
        }
        case 0x5F:{
            state->e = state->a;
            break;
        }
        case 0x60:{
            state->h = state->b;
            break;
        }
        case 0x61:{
            state->h = state->c;
            break;
        }
        case 0x62:{
            state->h = state->d;
            break;
        }
        case 0x63:{
            state->h = state->e;
            break;
        }
        case 0x64:{

            break;
        }
        case 0x65:{
            state->h = state->l;
            break;
        }
        case 0x66:{
            state->h = state->memory[state->h << 8 | state-> l];
            break;
        }
        case 0x67:{
            state->h = state->a;
            break;
        }
        case 0x68:{
            state->l = state->b;
            break;
        }
        case 0x69:{
            state->l = state->c;
            break;
        }
        case 0x6A:{
            state->l = state->d;
            break;
        }
        case 0x6B:{
            state->l = state->e;
            break;
        }
        case 0x6C:{
            state->l = state->h;
            break;
        }
        case 0x6D:{
            
            break;
        }
        case 0x6E:{
            state->l = state->memory[state->h << 8 | state-> l];
            break;
        }
        case 0x6F:{
            state->l = state->a;
            break;
        }
        case 0x70:{
            state->memory[state->h << 8 | state->l] = state->b;
            break;
        }
        case 0x71:{
            state->memory[state->h << 8 | state->l] = state->c;
            break;
        }
        case 0x72:{
            state->memory[state->h << 8 | state->l] = state->d;
            break;
        }
        case 0x73:{
            state->memory[state->h << 8 | state->l] = state->e;
            break;
        }
        case 0x74:{
            state->memory[state->h << 8 | state->l] = state->h;
            break;
        }
        case 0x75:{
            state->memory[state->h << 8 | state->l] = state->l;
            break;
        }
        case 0x76:{  // This is supposed to be a halt operation.
            
            break;
        }
        case 0x77:{
            state->memory[state->h << 8 | state->l] = state->a;
            break;
        }
        case 0x78:{
            state->a = state->b;
            break;
        }
        case 0x79:{
            state->a = state->c;
            break;
        }
        case 0x7A:{  
            state->a = state->d;
            break;
        }
        case 0x7B:{
            state->a = state->e;
            break;
        }
        case 0x7C:{
            state->a = state->h;
            break;
        }
        case 0x7D:{  
            state->a = state->l;
            break;
        }
        case 0x7E:{
            state->a = state->memory[state->h << 8 | state-> l];
            break;
        }
         case 0x7F:{

            break;
        }
        // 01 or MOV family done;
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
    state.memory[0] = 0x04;
    state.b = 0b10100010;

    while (true) {
        Emulate8085(&state);
    }

    return 0;
}