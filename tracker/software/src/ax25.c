#include "ax25.h"

static uint16_t crc16(uint8_t *data, unsigned int len)
{
        uint16_t crc = 0xffff;
        while (len--)
	{
                uint8_t c = *data++;

                for (uint8_t i = 0x01; i & 0xff; i <<= 1)
		{
			crc ^= !!(c & i);
                        if(crc & 1)
                                crc = (crc >> 1) ^ 0x8408;
                        else
                                crc = crc >> 1;
                }
        }

        return (~crc) & 0xffff;
}

int ax25_constructPacket(uint8_t* packet, struct aprsPacket* packetData)
{
	// Flag (1 Byte) (repeated 3 times to make sure that a listening device can lock on)
	packet[0] = 0x7e;	// First Byte is always 0x7e
	packet[1] = 0x7e;
	packet[2] = 0x7e;
	
	// Destination Address (7 Bytes)
	#define __DEST_OFFSET 3
	int i;
	for(i = 0; (i < 6) || packet[i + __DEST_OFFSET] != '\0'; i++)
	{
		packet[i + __DEST_OFFSET] = packetData->source[i] << 1;
	}
	for(; i < 6; i++)	// If the Address is shorter than 6 characters, pad the rest with spaces
	{
		packet[i + __DEST_OFFSET] = ' ' << 1;	// Pad the rest of the address field with spaces
	}
	packet[__DEST_OFFSET + 7] = (('0' + packetData->sourceID) << 1);	// Add SSID

	#define __SOURCE_OFFSET (__DEST_OFFSET + 7)
	// Source Address (7 Bytes)
	for(i = 0; (i < 6) || packet[i + __SOURCE_OFFSET] != '\0'; i++)
	{
		packet[i + __SOURCE_OFFSET] = packetData->source[i] << 1;
	}
	for(; i < 6; i++)
	{
		packet[i + __SOURCE_OFFSET] = ' ' << 1;	// Pad the rest of the address field with spaces
	}
	packet[__SOURCE_OFFSET + 7] = (('0' + packetData->sourceID) << 1) | 1;	// Add SSID and "Last in Sequence" Signal bit
	
	// Digipeater Adresses (0-58 Bytes)
	// No bytes, we don't need to set this

	// Control Field (UI) (1 Byte)
	packet[__SOURCE_OFFSET+8] = 0x03;

	// Protocol ID (1 Byte)
	packet[__SOURCE_OFFSET+9] = 0xf0;

	// Information Field (1-256 Bytes)
	#define __INFO_OFFSET (__SOURCE_OFFSET + 10)	
	int infoIndx = 0;
	while((infoIndx < 256) || (packetData->info[infoIndx] != '\0') )
	{
		packet[infoIndx++ + __INFO_OFFSET] = packetData->info[infoIndx];
	}
	
	// FCS (2 Bytes)
	int crc = crc16((uint8_t*)packet+3, infoIndx + 17);	// CRC between first flag and FCS field
	packet[17+infoIndx+1] = (crc & 0xFF);	// LSB first
	packet[17+infoIndx+2] = ((crc >> 8) & 0xFF);	// MSB last

	// Flag (1 Byte)
	packet[17+infoIndx+3] = 0x7e;	// Last byte is always 0x7e
	return 0;
}
