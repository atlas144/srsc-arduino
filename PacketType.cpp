#include "PacketType.h"

PacketType::PacketType(uint8_t packetTypeIdentifier, PayloadSize payloadSize, bool isCritical) : packetTypeIdentifier(packetTypeIdentifier), payloadSize(payloadSize), critical(isCritical) {}

uint8_t PacketType::getPacketTypeIdentifier() {
  return packetTypeIdentifier;
}

PayloadSize PacketType::getPayloadSize() {
  return payloadSize;
}

bool PacketType::isCritical() {
  return critical;
}