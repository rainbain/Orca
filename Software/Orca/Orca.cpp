#include <stdio.h>
#include <chrono>
#include <thread>
#include <unistd.h>

#include "Krita/System.h"
#include "Krita/Hardware/AXIGPIO.h"

#include "Flipper/DebugUtils.h"
#include "Flipper/Flipper.h"

using namespace ZynqUSP;
using namespace FlipperAPI;

#define CAST_POINTER(pt) ((uint32_t)(uint64_t)pt)

void DumpFIFO(CP *cp, GXFIFO fifo, size_t dumpSize = 4096){
    std::string fifoDump = DebugUtils::DumpCurrentFIFO(cp);
    printf("%s", fifoDump.c_str());
    fifoDump = DebugUtils::DumpFIFOData(cp, fifo, dumpSize/32);
    printf("%s", fifoDump.c_str());
}

int main(){
    System* sys = new System();

    Flipper* flipper = new Flipper(sys);
    CP *cp = flipper->GetCP();

    Memory *mem = sys->GetMemory();
    AXIGPIO *gpio = (AXIGPIO*)mem->Map(0xA0000000, sizeof(AXIGPIO));

    cp->SetCPIRQEnable(false);
    printf("Interrupts before init: %d\n", gpio->gpio[0].data);
    cp->SetCPIRQEnable(true);
    cp->SetFIFOOverflowIRQEnable(true);
    printf("Interrupts after init: %d\n", gpio->gpio[0].data);

    GXFIFO fifo = cp->CreateGXFIFO();

    cp->LoadFIFO(fifo);

    for(uint32_t i = 0; i < 4096; i++) cp->WriteU32(i);

    DumpFIFO(cp, fifo, 64);

    printf("Interrupts after fifo: %d\n", gpio->gpio[0].data);
    
    printf("Press any key to continue.");
    getchar();

    cp->DestroyFIFO(fifo);

    delete flipper;
    delete sys;
}