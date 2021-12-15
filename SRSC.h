#ifndef SRSC_H
#define SRSC_H

#include <Arduino.h>
#include <stdint.h>
#include "PacketType.h"
#include "Packet.h"
#include "PayloadSize.h"

class SRSC {
  private:
    HardwareSerial& communicator;
    uint8_t packetTypesSize = 9, lastPacketId = 0; acceptedPacketIds[10] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    PacketType** packetTypes = new PacketType*[packetTypesSize] {
      new PacketType(0, INT),
      new PacketType(1, INT),
      new PacketType(2, COMMAND),
      new PacketType(64, BYTE),
      new PacketType(65, SHORT),
      new PacketType(66, INT),
      new PacketType(67, BYTE, true),
      new PacketType(68, SHORT, true),
      new PacketType(69, INT, true)
    };
    uint16_t semaphoreSize, semaphore;

    bool getPacketType(PacketType* packetTypeBuffer, uint8_t packetTypeIdentifier);
    bool isValid(PacketType* packetType, uint8_t checksum, uint8_t* payload, uint8_t id = 0);
    bool isValid(uint8_t* binaryPacket, uint8_t packetLength);
    uint8_t countChecksum(uint8_t packetType, uint8_t payload[], PayloadSize payloadSize, uint8_t id = 0);
    uint8_t countPacketId();
    bool getBinaryPayload(uint8_t buffer[], uint8_t binaryPacket[]);
    template <typename T>
    void parseBinaryPayload(T& payload, uint8_t* binaryPayload);/*
    void parseBinaryPayload(uint16_t& payload, uint8_t* binaryPayload);
    void parseBinaryPayload(uint32_t& payload, uint8_t* binaryPayload);*/
    template <typename T>
    void parsePayload(uint8_t* buffer, T payload);/*
    void parsePayload(uint8_t* buffer, uint16_t payload);
    void parsePayload(uint8_t* buffer, uint32_t payload);*/
    uint32_t countSemaphoreSize(uint32_t oponentBufferSize);
    bool buildBinaryPacket(uint8_t buffer[], uint8_t packetType, uint8_t payload[]);
    bool readConnect();
    bool readConnack();
    void readAcceptack();

  public:
    SRSC(HardwareSerial& communicator);
    bool begin(uint32_t bufferSize = 64);
    bool isAvailable();
    bool readPacket(*Packet incomingPacket);
    bool writePacket(uint8_t packetType);
    template <typename T>
    bool writePacket(uint8_t packetType, T payload);
    bool writeBinaryPacket(uint8_t packetType, uint8_t* payload);
    void definePacketType(uint8_t packetTypeIdentifier, PayloadSize payloadSize, bool isCritical = false);
    void definePacketTypes(uint8_t* packetTypeIdentifiers, PayloadSize* payloadSizes, bool* isCritical, uint8_t numberOfPacketTypes);
    void loop();
};

#endif