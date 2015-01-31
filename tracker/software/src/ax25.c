#include "ax25.h"

int ax25_constructPacket(char* packet, char* dest, char* source, char* info, int infolen)
{
	// Assemble AX.25 UI-Frame
	// See http://www.aprs.org/doc/APRS101.PDF for information on how the packet is concstructed
	
	// Flag (1 Byte)
	packet[0] = 0x7e;	// First Byte is always 0x7e

	// Destination Address (7 Bytes)
	for(int i = 0; i < 7; i++)
	{
		// Offset packet index by 1
		packet[i+1] = dest[i];
	}

	// Source Address (7 Bytes)
	for(int i = 0; i < 7; i++)
	{
		// Offset packet index by 8
		packet[i+8] = source[i];
	}

	// Digipeater Adresses (0-58 Bytes)
	// No bytes, we don't need to set this

	// Control Field (UI) (1 Byte)
	packet[15] = 0x03;

	// Protocol ID (1 Byte)
	packet[16] = 0xf0;

	// Information Field (1-256 Bytes)
	if(infolen > 0 && infolen < 256)	// Check if the information field length is correct
	{
		for(int i = 0; i < infolen; i++)
		{
			// Offset packet index by 17
			packet[i+17] = source[i];
		}
	}
	else
	{
		return 1;
	}

	// FCS (2 Bytes)
	// TODO: Actually calculate CRC for packet here
	packet[17+infolen+1] = 0x00;
	packet[17+infolen+2] = 0x00;

	// Flag (1 Byte)
	packet[17+infolen+3] = 0x7e;	// Last byte is always 0x7e
	return 0;
}
