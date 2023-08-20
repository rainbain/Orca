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

using json = nlohmann::json;

using namespace ZynqUSP;

#define MAX_TEMP_STR_SIZE 100


std::string ThermalGraph::getPointName(size_t number){
    return "pt" + std::to_string(number);
}



ThermalGraph::ThermalGraph(){

}


uint8_t ThermalGraph::GetPoint(float p){
    size_t len = thermalPoints.size();
    for(size_t i = 0; i < len; i++){
        if(p > thermalPoints[i].temp){
            continue;
        }else {
            return thermalPoints[i].fan;
        }
    }
    printf("Warning, SoC temperature is %fC. This is outside the bounds of the thermal graph!\n", p);
    return 100; // Outside of thermal graph. Oof
}

void ThermalGraph::LoadConfig(nlohmann::json j){
    json graph = j["graph"];

    // Calculate number of points
    size_t points = 0;
    while(graph.contains(getPointName(points)))points++;

    // Phrase in each point
    thermalPoints.resize(points);
    for(size_t i = 0; i < points; i++){
        json point = graph[getPointName(i)];
        thermalPoints[i].temp = point["temp"];
        thermalPoints[i].fan = point["fan"];
    }

    printf("Phrased %d points into thermal graph.\n", (int)points);
}




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


void TemperatureSensors::LoadConfig(nlohmann::json j){
    json thermal = j["thermal"];

    graph.LoadConfig(thermal);
}

float TemperatureSensors::GetMaximum(){
    return fmaxf(ReadZone(thermalZone0), ReadZone(thermalZone1));
}

uint8_t TemperatureSensors::GetFanSpeed(){
    return graph.GetPoint(GetMaximum());
}