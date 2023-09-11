#include <stdio.h>

#include <stdint.h>

#include "TestBench/TestBench.h"

#include "LowlevelAPI/CP.h"

#define FIFO_SIZE 4096

#define GET_BIT(v, b) ((v >> b) & 1)

float u16ToF(int16_t vs, uint8_t shift){
    // XOR with sign bit to get absolute value
    uint32_t vSigned = GET_BIT(vs, 15);
    uint32_t v = (vs ^ (vSigned ? 0xFFFF : 0x0000)) + vSigned;

    // Total the number of 1s
    uint8_t total;
    uint8_t power;
    for(uint8_t i = 0; i < 16; i++){
        if(GET_BIT(v, i)){
            total++;
            power = i;
        }
    }

    // The exponent with be 127 + total
    uint8_t exponent = power + (1<<7) - 1 - shift;

    // Shift 25, as there is a emplyed 1 with floats. Then mask it
    uint32_t mantisa = ((v << (23 - power)) & ((1<<23)-1));

    uint32_t floatValue = (vSigned << 31) | (exponent << 23) | mantisa;
    printf("%d\n", floatValue);
    return *((float*)((uint32_t*)&floatValue));
}

uint32_t memoryHighWord;

void AXIWriteHandler(uint32_t addr, uint32_t data, uint8_t strobe){
    if(strobe == 0)return;
    uint8_t* vp = (uint8_t*)(((uint64_t)memoryHighWord) << 32 | addr);
    uint8_t b0 = data & 0xFF;
    uint8_t b1 = (data >> 8) & 0xFF;
    uint8_t b2 = (data >> 16) & 0xFF;
    uint8_t b3 = data >> 24;

    //printf("WE: %d %d %d %d\n", b0, b1, b2, b3);

    if(strobe & 0b0001)vp[0] = b0;
    if(strobe & 0b0010)vp[1] = b1;
    if(strobe & 0b0100)vp[2] = b2;
    if(strobe & 0b1000)vp[3] = b3;
}

int main(){
    TestBench* tb = new TestBench(10000);

    Verilated::traceEverOn(true);
    
    tb->TraceOpen("trace.vcd");
    tb->SetupAXICallbacks(
        AXIWriteHandler
    );

    AXILiteIF* cpuInterface = tb->GetCPUInterface();

    tb->Open();

    CP* cp = new CP(cpuInterface);

    void *commandFIFO = aligned_alloc(1024*4, 1024);


    memoryHighWord = ((uint64_t)commandFIFO) >> 32;
    cp->SetFIFOBase((uint64_t)commandFIFO);
    cp->SetFIFOEnd((uint64_t)commandFIFO + 1024);

    for(uint32_t i = 0; i < 1024; i++){
        cp->WriteU8(i);
    }

    for(uint32_t i = 0; i < 128; i++){
        uint8_t v = ((uint8_t*)commandFIFO)[i];
        printf("%d -- %d!\n", i, v);
    }


    tb->Close();

    free(commandFIFO);
    delete tb;
}
