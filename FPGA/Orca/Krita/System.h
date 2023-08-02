// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * System
 * rainbain
 * 7/28/2023
 * Orca Emulator
 *
 * Manages the KR260 and its components.
*/

#pragma once

#include <stdint.h>
#include <thread>
#include <mutex>

#include "Memory.h"
#include "TemperatureSensors.h"

namespace ZynqUSP {
    class System {
        //
        // Server Side
        //
        TemperatureSensors sensors;
        Memory mem;
        std::thread serverThread;

        void ServerFunction();

        //
        // Client Side
        //

        void LoadConfig();

        //
        // Shared
        //

        std::mutex mtx;
        bool running;
    public:
        System();
        ~System();

        // Done automaticly
        void Start();
        void Stop();

        void SetRunning(bool running);
        bool GetRunning();
    };
};