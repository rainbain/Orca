// Copyright 2023 Orca Hardware Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * OrcaBoot
 * rainbain
 * 8/16/2023
 * Orca Emulator
 *
 * Serves as a system sanity check, will lator also be added to display
 * a logo to the graphics system.
*/

#include "SplashScreen.h"

#include "CFox/CFox.h"

const char* SplashScreen_TextLogo =
" $$$$$$\\  $$$$$$$\\   $$$$$$\\   $$$$$$\\        $$$$$$$$\\ $$\\      $$\\ $$\\   $$\\ \n"
"$$  __$$\\ $$  __$$\\ $$  __$$\\ $$  __$$\\       $$  _____|$$$\\    $$$ |$$ |  $$ |\n"
"$$ /  $$ |$$ |  $$ |$$ /  \\__|$$ /  $$ |      $$ |      $$$$\\  $$$$ |$$ |  $$ |\n"
"$$ |  $$ |$$$$$$$  |$$ |      $$$$$$$$ |      $$$$$\\    $$\\$$\\$$ $$ |$$ |  $$ |\n"
"$$ |  $$ |$$  __$$< $$ |      $$  __$$ |      $$  __|   $$ \\$$$  $$ |$$ |  $$ |\n"
"$$ |  $$ |$$ |  $$ |$$ |  $$\\ $$ |  $$ |      $$ |      $$ |\\$  /$$ |$$ |  $$ |\n"
" $$$$$$  |$$ |  $$ |\\$$$$$$  |$$ |  $$ |      $$$$$$$$\\ $$ | \\_/ $$ |\\$$$$$$  |\n"
" \\______/ \\__|  \\__| \\______/ \\__|  \\__|      \\________|\\__|     \\__| \\______/ \n"
"\n"
"-------------------------------------------------------------------------------\n"
"Orca Boot Version: 1.0\n";

void SplashScreen_Display(){
    putstr(SplashScreen_TextLogo);
}