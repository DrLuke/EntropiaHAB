#pragma once

#include <libopencm3/stm32/crc.h>
#include "settings.h"



struct aprsPacket
{
	// Destination Address and SSID
	uint8_t dest[7];
	uint8_t destID;

	// Source Address and SSID
	uint8_t source[7];
	uint8_t sourceID;

	// Null terminated string that shall be transmitted as info
	uint8_t* info;
};

int ax25_constructPacket(uint8_t* packet, struct aprsPacket* packetData);
