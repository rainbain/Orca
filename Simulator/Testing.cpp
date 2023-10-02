#include <stdio.h>

#include <stdint.h>

#include "TestBench/TestBench.h"
#include "Flipper/Flipper.h"
#include "Flipper/DebugUtils.h"

using namespace FlipperAPI;

void DumpFIFO(CP *cp, GXFIFO fifo, size_t dumpSize = 4096){
    std::string fifoDump = DebugUtils::DumpCurrentFIFO(cp);
    printf("%s", fifoDump.c_str());
    fifoDump = DebugUtils::DumpFIFOData(cp, fifo, dumpSize/32);
    printf("%s", fifoDump.c_str());
}

uint32_t memoryHighWord;

void AXIWriteHandler(uint32_t addr, uint32_t data, uint8_t strobe){
    if(strobe == 0)return;
    uint8_t* vp = (uint8_t*)(((uint64_t)memoryHighWord) << 32 | addr);
    uint8_t b0 = data & 0xFF;
    uint8_t b1 = (data >> 8) & 0xFF;
    uint8_t b2 = (data >> 16) & 0xFF;
    uint8_t b3 = data >> 24;

    if(strobe & 0b0001)vp[0] = b0;
    if(strobe & 0b0010)vp[1] = b1;
    if(strobe & 0b0100)vp[2] = b2;
    if(strobe & 0b1000)vp[3] = b3;
}

using namespace FlipperAPI;

int main(){
    TestBench* tb = new TestBench(10000);

    Verilated::traceEverOn(true);
    
    tb->TraceOpen("trace.vcd");
    tb->SetupAXICallbacks(
        AXIWriteHandler
    );

    tb->Open();

    Flipper *flipper = new Flipper(tb->GetCPUInterface(), tb->GetCPUInterrupt());

    CP* cp = flipper->GetCP();

    cp->SetCPIRQEnable(true);
    cp->SetFIFOOverflowIRQEnable(true);

    GXFIFO fifo = cp->CreateGXFIFO();

    cp->LoadFIFO(fifo);

    for(uint32_t i = 0; i < 4096; i++) cp->WriteU32(i);

    DumpFIFO(cp, fifo, 64);

    printf("Press any key to continue.");
    getchar();

    cp->DestroyFIFO(fifo);

    tb->Close();

    delete flipper;
    delete tb;
}
