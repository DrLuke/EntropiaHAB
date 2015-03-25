#include "dac.h"
#include "ax25.h"

void aprs_transmitPacket(uint8_t* dest, uint8_t destSSID, uint8_t* src, uint8_t srcSSID, uint8_t* info);
void aprs_sendPacket(uint8_t packet[], int packetLen);
