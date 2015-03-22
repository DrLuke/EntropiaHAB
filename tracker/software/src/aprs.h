#include "dac.h"
#include "ax25.h"

void aprs_transmitPacket(struct aprsPacket* packetData);
void aprs_sendPacket(uint8_t packet[], int packetLen);
