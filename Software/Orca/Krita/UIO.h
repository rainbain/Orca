// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * UIO
 * rainbain
 * 9/28/2023
 * Orca Emulator
 *
 * Manages memory between the hardware and user space software.
*/

#pragma once

#include <stdint.h>
#include <map>
#include <string>
#include <mutex>
#include <thread>
#include <functional>

namespace ZynqUSP {
    class UIO {
        public:
            typedef std::function<void(uint32_t)> IRQHandler;
        private:    
            /*
             * Interrupt Thread
            */
            std::mutex threadLock;
            std::thread irqThread;
            bool threadOpen;
            IRQHandler handler;
            int threadFd;
            int pipe_fds[2];

            void ThreadFunction();

            bool GetThreadOpen();
            void SetThreadOpen(bool val);


            int fd;
            bool isOpen;
            size_t addressSpace;
            void* mapping;
        public:
            UIO();
            ~UIO();

            void* GetMemory(size_t offset);

            bool Open(std::string path, size_t addressSpace);

            void SetupInterrupts(IRQHandler onInterrupt);
    };

    class UIOIndexing {
            std::string uioEnumerationPath;

            std::map<std::string, int> entrys;

            void Index();
            std::string GetIndexIndintifyer(uint64_t addr, std::string deviceName);
            std::string GetUIOPath(int id);
        public:
            UIOIndexing();

            bool IndexUIO(UIO *uio, uint64_t addr, size_t addressSpace, std::string deviceName);
            std::string DumpUIOTable();
    };
}