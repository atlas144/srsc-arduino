#include "Packet.h"

Packet::Packet(PacketType& packetType, uint8_t& payload) : packetType(packetType), payload(payload) {}

uint8_t Packet::getPacketType()  {
  return packetType.getPacketTypeIdentifier();
}

int8_t Packet::getPayloadSize() {
  return packetType.getPayloadSize();
}

uint8_t& Packet::getPayload() {
  return payload;
}