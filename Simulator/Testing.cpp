#include <stdio.h>

#include <stdint.h>

#include "TestBench/TestBench.h"

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

class TestHandler {
public:
    uint8_t TestAxiRead(uint32_t addr){
        return addr * 4;
    }
};

int main(){
    TestBench* tb = new TestBench(1000);

    TestHandler me;

    Verilated::traceEverOn(true);
    
    tb->TraceOpen("trace.vcd");
    tb->SetupAXICallbacks(
        std::bind(&TestHandler::TestAxiRead, &me, std::placeholders::_1)
    );

    AXILiteIF* cpuInterface = tb->GetCPUInterface();

    tb->Open();

    cpuInterface->WriteU32(0x20, 0xFFFFFFFF);
    printf("%d!\n", cpuInterface->ReadU32(0x20));

    tb->Close();

    delete tb;
}