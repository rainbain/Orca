#include <stdio.h>
#include <chrono>
#include <thread>

#include "Krita/System.h"

using namespace ZynqUSP;

int main(){
    System* sys = new System();
    
    printf("Press any key to continue.");
    getchar();

    delete sys;
}