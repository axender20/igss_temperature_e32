#pragma once
#ifndef _IGGSE32_BOARD_PINOUT_H_
#define _IGGSE32_BOARD_PINOUT_H_

#include "stdint.h"

//> Pines para el sensor (I2C)
constexpr uint8_t s_temp_sda = 19u;
constexpr uint8_t s_temp_scl = 20u;

//> Pines para la SD (SPI)
constexpr int8_t sd_custom_cs = 21u;
constexpr int8_t sd_custom_sck = 26u;
constexpr int8_t sd_custom_mosi = 33u;
constexpr int8_t sd_custom_miso = 34u;

//> Pin para los led RGB  
constexpr uint8_t led_strip_pin = 38u;

//> Pin para el buzzer
constexpr uint8_t s_buzzer_pin = 15u;
#endif