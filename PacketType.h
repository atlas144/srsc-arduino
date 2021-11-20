#ifndef PACKET_TYPE_H
#define PACKET_TYPE_H

class PacketType {
  private:
    uint8_t packetTypeIdentifier;
    int8_t payloadSize;
    bool isCritical;

  public:
    PacketType(uint8_t packetTypeIdentifier, int8_t payloadSize, bool isCritical = false);
    uint8_t getPacketTypeIdentifier();
    int8_t getPayloadSize();
    bool isCritical();
};

#endif