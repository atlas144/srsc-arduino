#ifndef PACKET_H
#define PACKET_H

class Packet {
  private:
    PacketType& packetType;
    uint8_t[] payload;
    
  public:
    Packet(PacketType& packetType, uint8_t[] payload);
    uint8_t getPacketType();
    int8_t getPayloadSize();
    uint8_t[] getPayload();
};

#endif