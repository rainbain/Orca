#include <stdio.h>
#include <chrono>
#include <thread>
#include <unistd.h>

#include "Krita/System.h"

#include "Flipper/DebugUtils.h"
#include "Flipper/GX/CP.h"

using namespace ZynqUSP;

#define CAST_POINTER(pt) ((uint32_t)(uint64_t)pt)

void DumpFIFO(Flipper::CP *cp, Flipper::GXFIFO fifo){
    std::string fifoDump = Flipper::DebugUtils::DumpCurrentFIFO(cp);
    printf("%s", fifoDump.c_str());
    fifoDump = Flipper::DebugUtils::DumpFIFOData(cp, fifo, 4096/32);
    printf("%s", fifoDump.c_str());
}

int main(){

    System* sys = new System();

    Memory *mem = sys->GetMemory();

    Flipper::CP *cp = new Flipper::CP(mem);

    Flipper::GXFIFO fifo = cp->CreateGXFIFO();

    cp->LoadFIFO(fifo);

    memset(fifo.buffer, 0, fifo.size);
    cp->PrepareFIFORead(fifo);
    srand(0);
    
    for(uint16_t i = 0; i < 4096/2; i++){
        cp->WriteU32(rand());
    }


    DumpFIFO(cp, fifo);

    srand(0);
    uint32_t *bt = (uint32_t*)fifo.buffer;
    for(uint16_t i = 0; i < 4096/4; i++){
        rand();
    }
    for(uint16_t i = 0; i < 4096/4; i++){
        if(bt[i] != rand()){
            printf("%d!\n", i);
        }
    }
    
    printf("Press any key to continue.");
    getchar();

    cp->DestroyFIFO(fifo);

    delete cp;
    delete sys;
}