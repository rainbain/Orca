// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * CFox
 * rainbain
 * 8/14/2023
 * Orca Emulator
 *
 * A "offbrand" libc like implmentation. This code will be added to all my embedded platforms.
*/

// I expect these to be present
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "Memory.h"
#include "IO.h"
#include "String.h"
#include "File.h"