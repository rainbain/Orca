// Copyright 2023 Orca Hardware Emulator
// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * PSSYSMON
 * rainbain
 * 8/1/2023
 * Orca Emulator
 *
 * Power System Monitor Module Registers
 * This is not used right now
*/
#pragma once

#include <stdint.h>

namespace ZynqUSP {
    struct PSSYSMON {
        uint32_t temp_lpd; // 000
        uint32_t vcc_psintlp; //004
        uint32_t vcc_psintfp; //008
        uint32_t vp_vn;  //00C
        uint32_t reserved1[2];  //010
        uint32_t vcc_psaux; // 018
        uint32_t reserved2[6]; // 01C
        uint32_t vcco_psddr; // 034
        uint32_t vcco_psio3; // 038
        uint32_t vcco_psio0; // 03C
        uint32_t reserved3[16]; // 040
        uint32_t max_temp_lpd; // 080
        uint32_t max_vcc_psintlp; // 084
        uint32_t max_vcc_psintfp; // 088
        uint32_t max_vcc_psaux; // 08C
        uint32_t min_temp_lpd; // 090
        uint32_t min_vcc_psintlp; // 094
        uint32_t min_vcc_psintfp; // 098
        uint32_t min_vcc_psaux; // 09C
        uint32_t max_vcco_psddr; // 0A0
        uint32_t max_vcco_psio3;  // 0A4
        uint32_t max_vcco_psio0; // 0A8
        uint32_t reserved11;     // 0AC
        uint32_t min_vcco_psddr; // 0B0
        uint32_t min_vcco_psio3; // 0B4
        uint32_t min_vcco_psio0; // 0B8
        uint32_t reserved4[16]; // 0BC
        uint32_t status_flag; //0FC
        uint32_t config_reg0; //100
        uint32_t config_reg1; //104
        uint32_t config_reg2; //108
        uint32_t config_reg3; //10C
        uint32_t config_reg4; //110
        uint32_t reserved5; //114
        uint32_t seq_channel2; //118
        uint32_t seq_average2; //11C
        uint32_t seq_channel0; //120
        uint32_t reserved6; //124
        uint32_t seq_average0; //128
        uint32_t reserved7[4]; //12C
        uint32_t seq_acq1; //13C
        uint32_t alarm_temp_lpd_upper; //140
        uint32_t alarm_vcc_psintlp_upper; //144
        uint32_t alarm_vcc_psintfp_upper; //148
        uint32_t alarm_ot_upper; //14C
        uint32_t alarm_temp_lpd_lower; //150
        uint32_t alarm_lower_vcc_psintlp; //154
        uint32_t alarm_vcc_psintfp_lower; //158
        uint32_t alarm_ot_lower; //15C
        uint32_t alarm_vcc_psaux_upper; //160
        uint32_t alarm_vcco_psddr_upper;//164
        uint32_t alarm_vcco_psio3_upper; //168
        uint32_t alarm_vcco_psio0_upper; //16C
        uint32_t alarm_vcc_psaux_lower; //170
        uint32_t alarm_vcco_psddr_lower; //174
        uint32_t alarm_vcco_psio3_lower; //178
        uint32_t alarm_vcco_psio0_lower; //17C
        uint32_t alarm_vcco_psio1_upper; //180
        uint32_t alarm_vcco_psio2_upper; //184
        uint32_t alarm_mgtravcc_upper; //188
        uint32_t alarm_mgtravtt_upper; //18C
        uint32_t alarm_vcc_psadc_upper; //190
        uint32_t alarm_temp_fpd_upper; //194
        uint32_t reserved8[2]; //198
        uint32_t alarm_vcco_psio1_lower; //1A0
        uint32_t alarm_vcco_psio2_lower; //1A4
        uint32_t alarm_mgtravcc_lower; //1A8
        uint32_t alarm_mgtravtt_lower; //1AC
        uint32_t alarm_vcc_psadc_lower; //1B0
        uint32_t alarm_temp_fpd_lower; //1B4
        uint32_t reserved12[12]; //1B8
        uint32_t seq_low_rate_channel0; //1E8
        uint32_t reserved13; //1EC
        uint32_t seq_low_rate_channel2; //1F0
        uint32_t reserved14[3]; //1F4
        uint32_t vcco_psio1; //200
        uint32_t vcco_psio2; //204
        uint32_t vcc_psgtr; //208
        uint32_t vtt_psgtr; //20C
        uint32_t vcc_psadc; //210
        uint32_t temp_fpd; //214
        uint32_t reserved9[26]; //218
        uint32_t max_vcco_psio1; //280
        uint32_t max_vcco_psio2; //284
        uint32_t max_vcc_psgtr; //288
        uint32_t max_vtt_psgtr; //28C
        uint32_t max_psadc; //290
        uint32_t max_temp_fpd;  //294
        uint32_t reserved10[2]; //298
        uint32_t min_vcco_psio1; //2A0
        uint32_t min_vcco_psio2; //2A4
        uint32_t min_vcc_psgtr; //2A8
        uint32_t min_vtt_psgtr; //2AC
        uint32_t min_vcc_psadc; //2B0
        uint32_t min_temp_fpd; //2B4
    };
};