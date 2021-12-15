#ifndef PACKET_TYPE_H
#define PACKET_TYPE_H

#include <stdint.h>
#include "PayloadSize.h"

class PacketType {
  private:
    uint8_t packetTypeIdentifier;
    PayloadSize payloadSize;
    bool critical;

  public:
    PacketType(uint8_t packetTypeIdentifier, PayloadSize payloadSize, bool isCritical = false);
    uint8_t getPacketTypeIdentifier();
    PayloadSize getPayloadSize();
    bool isCritical();
};

#endif