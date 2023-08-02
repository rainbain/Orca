#include <stdio.h>
#include <chrono>
#include <thread>

#include "Krita/System.h"

using namespace ZynqUSP;

int main(){
    System* sys = new System();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    delete sys;
}