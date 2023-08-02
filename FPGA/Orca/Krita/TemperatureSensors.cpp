// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * TemperatureSensors
 * rainbain
 * 8/1/2023
 * Orca Emulator
 *
 * Reads temperature data from linux.
*/

#include "TemperatureSensors.h"

#include <math.h>

using namespace ZynqUSP;

#define MAX_TEMP_STR_SIZE 100

float TemperatureSensors::ReadZone(std::string thermalZone){
    FILE *file = fopen(thermalZone.c_str(), "r");
    if(file == NULL){
        printf("Failed to open temperature zone: \"%s\"", thermalZone.c_str());
        return 0.0f;
    }

    char tempString[MAX_TEMP_STR_SIZE];
    if(fgets(tempString, sizeof(tempString), file) == NULL){
        printf("Temperature region read write error on \"%s\"!\n", thermalZone.c_str());
        fclose(file);
        return 0.0f;
    }

    float temperature = atof(tempString);
    if(temperature == 0.0f){
        printf("Temperature formatting error: \"%s\" on \"%s\"\n", tempString, thermalZone.c_str());
        fclose(file);
        return 0.0f;
    }

    fclose(file);

    return temperature / 1000.0f;
}



TemperatureSensors::TemperatureSensors(std::string temeratureZonesPath){
    this->thermalZone0 = temeratureZonesPath + "temp1_input";
    this->thermalZone1 = temeratureZonesPath + "temp2_input";
}

TemperatureSensors::~TemperatureSensors(){
}


float TemperatureSensors::GetMaximum(){
    return fmaxf(ReadZone(thermalZone0), ReadZone(thermalZone1));
}