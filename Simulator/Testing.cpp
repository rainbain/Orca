#include <stdio.h>

#include "TestBench/TestBench.h"

int main(){
    TestBench *tb = new TestBench;
    tb->Reset();
    tb->Tick(200);
}