#include "PacketTypr.h"

PacketType::PacketType(uint8_t packetTypeIdentifier, int8_t payloadSize, bool isCritical = false) : packetTypeIdentifier(packetTypeIdentifier), payloadSize(payloadSize), isCritical(isCritical) {}

uint8_t PacketType::getPacketTypeIdentifier() {
  return packetTypeIdentifier;
}

int8_t PacketType::getPayloadSize() {
  return payloadSize;
}

bool PacketType::isCritical() {
  return isCritical;
}