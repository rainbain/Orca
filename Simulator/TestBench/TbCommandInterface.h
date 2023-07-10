// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * TbCommandInterface
 * rainbain
 * 7/9/2023
 * Orca Emulator
 *
 * Standured command interface for the TestBech Server
*/
#pragma once

#include "../CommandInterface/CommandInterface.hpp"

#include "TestBench.h"
#include <mutex>

class TbCommandInterface : public CommandInterface {
        std::mutex* systemMutex;
        TestBench *tb;

        bool GetCpReady();

    public:
        TbCommandInterface(std::mutex* systemMutex, TestBench *tb);
        ~TbCommandInterface();

        /*
         * Command Interface
        */

        void Open();
	    void Close();

	    void WriteData(uint32_t* data, uint32_t size);
	    void ReadData(uint32_t* data, uint32_t size);
};