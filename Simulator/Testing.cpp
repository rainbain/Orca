#include <stdio.h>

#include "CommandInterface/CommandBuffer.hpp"

#include "TestBench/TbServer.h"

#define FIFO_SIZE 4096

int main(){
    // Create test bench
    TbServer *tbserver = new TbServer();
    tbserver->Open(10, "trace.vcd");

    // Setup Command Interface
    CommandBuffer *buffer = new CommandBuffer((CommandInterface*)tbserver->controlInterface, FIFO_SIZE);


    uint32_t xfData[] = {0x12345678, 0x9ABCDEF0, 0x87654321, 0x0FEDCBA9};
    buffer->PushXF(0x0010, 4, xfData);
    buffer->Flush();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    tbserver->Close();
    delete tbserver;
}