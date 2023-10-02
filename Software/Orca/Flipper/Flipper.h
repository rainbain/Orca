// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * Flipper
 * rainbain
 * 9/3/2023
 * Orca Emulator
 *
 * Flipper Base Class
*/

#pragma once

#include "../Krita/System.h"

#include "GX/CP.h"

namespace FlipperAPI {
    class Flipper {
            ZynqUSP::System *sys;
            ZynqUSP::UIO flipperUIO;

            CP cp;

            void FlipperIRQHandler(uint32_t interruptCount);

        public:
            Flipper(ZynqUSP::System *sys);
            ~Flipper();

            CP* GetCP();
    };
}