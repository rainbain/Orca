// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * DebugUtils
 * rainbain
 * 9/4/2023
 * Orca Emulator
 *
 * A few utilitys to help get debug information.
*/

#pragma once

#include <string>

#include "GX/CP.h"

namespace Flipper {
    class DebugUtils {
    public:
        static std::string FormattedTime();

        static std::string DumpCurrentFIFO(CP* cp);

        // fifoDumpSize is the number of 32 byte sections of the fifo to print.
        // -1 will print all to the fifo size, 0 will print non, and so on.
        static std::string DumpFIFOData(CP* cp, GXFIFO fifo, size_t fifoDumpSize = -1);
    };
}