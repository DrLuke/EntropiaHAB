#pragma once

#include <libopencm3/stm32/crc.h>
#include "settings.h"

int ax25_constructPacket(uint8_t* packet, uint8_t* dest, uint8_t destSSID, uint8_t* src, uint8_t srcSSID, uint8_t* info);
