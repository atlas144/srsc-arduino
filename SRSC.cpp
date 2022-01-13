#include "SRSC.h"

bool SRSC::getPacketType(PacketType* packetTypeBuffer, uint8_t packetTypeIdentifier) {
  for (uint8_t i = 0; i < packetTypesSize; i++) {
    if (packetTypes[i]->getPacketTypeIdentifier() == packetTypeIdentifier) {
      packetTypeBuffer = packetTypes[i];

      return true;
    }
  }

  return false;
}

bool SRSC::isValid(PacketType* packetType, uint8_t checksum, uint8_t* payload, uint8_t id) {
  uint8_t validationSum = packetType->getPacketTypeIdentifier() + checksum + id;

  for (uint8_t i = 0; i < packetType->getPayloadSize(); i++) {
    validationSum += payload[i];
  }

  return validationSum == 255 ? true : false;
}

bool SRSC::isValid(uint8_t* binaryPacket, uint8_t packetLength) {
  uint8_t validationSum = 0;

  for (uint8_t i = 0; i < packetLength; i++) {
    validationSum += binaryPacket[i];
  }

  return validationSum == 255 ? true : false;
}

uint8_t SRSC::countChecksum(uint8_t packetType, uint8_t payload[], PayloadSize payloadSize, uint8_t id) {
  uint8_t checkSum = packetType + id;

  for (uint8_t i = 0; i < payloadSize; i++) {
    checkSum += payload[i];
  }

  return checkSum;
}

uint8_t SRSC::countPacketId() {
  if (lastPacketId == 255) {
    lastPacketId = 1;
  } else {
    lastPacketId++;
  }

  return lastPacketId;
}

bool SRSC::getBinaryPayload(uint8_t buffer[], uint8_t binaryPacket[]) {
  PacketType* packetType;

  if (getPacketType(packetType, binaryPacket[0])) {
    uint8_t offset = 2 + packetType->isCritical() ? 1 : 0;

    for (uint8_t i = 0; i < packetType->getPayloadSize(); i++) {
      buffer[i] = binaryPacket[offset + i];
    }
    
    return true;
  } else {
    return false;
  }
}

/*void SRSC::parseBinaryPayload(uint8_t& payload, uint8_t* binaryPayload) {
  payload = binaryPayload[0];
}*/

/*template <typename T>
void SRSC::parseBinaryPayload(T& payload, uint8_t* binaryPayload) {
  payload = 0;

  for (uint8_t i = 0; i < sizeof(payload); i++) {
    payload += binaryPayload[i] * (255 * i);
  }
}*/
/*
void SRSC::parseBinaryPayload(uint32_t& payload, uint8_t* binaryPayload) {
  payload = 0;

  for (uint8_t i = 0; i < 4; i++) {
    payload += binaryPayload[i] * (255 * i);
  }
}*/

/*void SRSC::parsePayload(uint8_t* buffer, uint8_t payload) {
  buffer[0] = payload;
}*/

/*template <typename T>
void SRSC::parsePayload(uint8_t* buffer, T payload) {
  for (uint8_t i = 0; i < sizeof(payload); i++) {
    buffer[i] = payload >> (8 * i);
  }
}*/

/*void SRSC::parsePayload(uint8_t* buffer, uint32_t payload) {
  for (uint8_t i = 0; i < 4; i++) {
    buffer[i] = (payload & (255 * (i + 1))) >> (8 * i);
  }
}*/

uint32_t SRSC::countSemaphoreSize(uint32_t oponentBufferSize) {
  return oponentBufferSize / 7 - 1;
}

bool SRSC::buildBinaryPacket(uint8_t buffer[], uint8_t packetType, uint8_t payload[]) {
  PacketType* packetTypeObject;
  
  if (getPacketType(packetTypeObject, packetType)) {
    uint8_t payloadOffset = 2;
  
    buffer[0] = packetType;

    if (packetTypeObject->isCritical()) {
      payloadOffset++;

      buffer[2] = countPacketId();
      buffer[1] = countChecksum(packetType, payload, packetTypeObject->getPayloadSize(), buffer[2]);
    } else {
      buffer[1] = countChecksum(packetType, payload, packetTypeObject->getPayloadSize());
    }  
    
    for (uint8_t i = 0; i < packetTypeObject->getPayloadSize(); i++) {
      buffer[payloadOffset + i] = payload[i];
    }

    return true;
  } else {
    return false;
  }
}

bool SRSC::readConnect() {
  uint8_t connectPacket[6], connectPacketBinaryPayload[4], connackPacket[6], connackPacketBinaryPayload[4];

  communicator.readBytes(connectPacket, 6);
  
  if (isValid(connectPacket, 6)) {
    uint32_t& payload = *(new uint32_t());

    if (getBinaryPayload(connectPacketBinaryPayload, connectPacket)) {
      parseBinaryPayload(payload, connectPacketBinaryPayload);
      semaphoreSize = countSemaphoreSize(payload);

      semaphore = 0;
      lastPacketId = 0;

      for (uint8_t i = 0; i < 10; i++) {
        acceptedPacketIds[i] = 0;
      }

      parsePayload(connackPacketBinaryPayload, bufferSize);
  
      if (buildBinaryPacket(connackPacket, 0x01, connackPacketBinaryPayload)) {
        if (communicator.write(connackPacket, 6)) {
          return true;
        }
      }
    }
  }

  return false;
}

bool SRSC::readConnack() {
  uint8_t connackPacket[6], connackPacketPayload[4];

  //parsePayload(connectPacketPayload, bufferSize);
  
  communicator.readBytes(connackPacket, 6);

  if (isValid(connackPacket, 6)) {
    uint32_t& payload = *(new uint32_t());

    if (!getBinaryPayload(connackPacketPayload, connackPacket)) {
      return false;
    }
    
    parseBinaryPayload(payload, connackPacketPayload);
    semaphoreSize = countSemaphoreSize(payload);

    return true;
  } else {
    return false;
  }
}

void SRSC::readAcceptack() {
  communicator.read();
  semaphore--;
}

SRSC::SRSC(HardwareSerial& communicator) : communicator(communicator) {}

bool SRSC::begin(uint32_t bufferSize) {
  uint8_t connectPacket[6], connectPacketPayload[4];

  parsePayload(connectPacketPayload, bufferSize);
  
  if (!buildBinaryPacket(connectPacket, 0, connectPacketPayload)) {
    return false;
  }
  
  communicator.write(connectPacket, 6);
  
  return readConnack();
}

bool SRSC::isAvailable() {
  return communicator.available() > 0;
}

bool SRSC::readPacket(Packet* incomingPacket) {
  if (communicator.available() > 0) {
    switch (communicator.peek()) {
      case 0x00: {
        readConnect();
        return readPacket(incomingPacket);
      } case 0x01: {
        readConnack();
        return readPacket(incomingPacket);
      } case 0x02: {
        readAcceptack();
        return readPacket(incomingPacket);
      } default: {
        uint8_t packetTypeIdentifier = communicator.read();
        PacketType* packetType;
        
        if (getPacketType(packetType, packetTypeIdentifier)) {
          uint8_t checksum = communicator.read(), id = 0, payloadSize = packetType->getPayloadSize();

          if (packetType->isCritical()) {
            id = communicator.read();

            if (acceptedPacketIds[id % 10] == id) {
              return readPacket(incomingPacket);
            }
          }
          
          uint8_t* payload = new uint8_t[payloadSize];
          
          communicator.readBytes(payload, payloadSize);
          
          if (isValid(packetType, checksum, payload, id)) {
            incomingPacket = new Packet(*packetType, *payload);

            if (packetType->isCritical()) {
              acceptedPacketIds[id % 10] = id;
            }        

            return true;
          } else {
            return readPacket(incomingPacket);
          }
        } else {
          return false;
        }
      }
    }
  }
  return false;
}

bool SRSC::writePacket(uint8_t packetType) {
  PacketType* packetTypeObject;
  
  if (getPacketType(packetTypeObject, packetType)) {
    uint8_t sendCounter = packetTypeObject->isCritical() ? 3 : 1;

    if (semaphore + sendCounter > semaphoreSize) {
      uint8_t packetSize = 2 + (packetTypeObject->isCritical() ? 1 : 0) + packetTypeObject->getPayloadSize();

      uint8_t binaryPacket[packetSize];
      uint8_t binaryPayload[packetTypeObject->getPayloadSize()];

      parsePayload(binaryPayload, (uint8_t) 0);
      
      if (buildBinaryPacket(binaryPacket, packetType, binaryPayload)) {
        for (sendCounter; sendCounter > 0; sendCounter--) {        
          communicator.write(binaryPacket, packetSize);
          semaphore++;
        }

        return true;
      }
    }
  }
  
  return false;
}

/*template <typename T> bool SRSC::writePacket(uint8_t packetType, T payload) {
  PacketType* packetTypeObject;
  
  if (getPacketType(packetTypeObject, packetType)) {
    uint8_t sendCounter = packetTypeObject->isCritical() ? 3 : 1;

    if (semaphore + sendCounter > semaphoreSize) {
      uint8_t packetSize = 2 + (packetTypeObject->isCritical() ? 1 : 0) + packetTypeObject->getPayloadSize();

      uint8_t binaryPacket[packetSize];
      uint8_t binaryPayload[packetTypeObject->getPayloadSize()];

      parsePayload(binaryPayload, payload);
      
      if (buildBinaryPacket(binaryPacket, packetType, binaryPayload)) {
        for (sendCounter; sendCounter > 0; sendCounter--) {        
          communicator.write(binaryPacket, packetSize);
          semaphore++;
        }
  
        return true;
      }
    }
  }

  return false;
}*/

bool SRSC::writeBinaryPacket(uint8_t packetType, uint8_t* payload) {
  PacketType* packetTypeObject;
  
  if (getPacketType(packetTypeObject, packetType)) {
    uint8_t sendCounter = packetTypeObject->isCritical() ? 3 : 1;

    if (semaphore + sendCounter > semaphoreSize) {
      uint8_t packetSize = 2 + (packetTypeObject->isCritical() ? 1 : 0) + packetTypeObject->getPayloadSize();

      uint8_t binaryPacket[packetSize];

      if (buildBinaryPacket(binaryPacket, packetType, payload)) {
        communicator.write(binaryPacket, packetSize);
        semaphore++;

        return true;
      }
    }
  }

  return false;
}

void SRSC::definePacketType(uint8_t packetTypeIdentifier, PayloadSize payloadSize, bool isCritical) {
  PacketType** newPacketTypes = new PacketType*[packetTypesSize + 1];

  for (uint8_t i = 0; i < packetTypesSize; i++) {
    newPacketTypes[i] = packetTypes[i];
    delete packetTypes[i];
  }

  delete [] packetTypes;
  
  packetTypes = newPacketTypes;
  packetTypes[++packetTypesSize] = new PacketType(packetTypeIdentifier, payloadSize, isCritical);
}

void SRSC::definePacketTypes(uint8_t* packetTypeIdentifiers, PayloadSize* payloadSizes, bool* isCritical, uint8_t numberOfPacketTypes) {
  PacketType** newPacketTypes = new PacketType*[packetTypesSize + numberOfPacketTypes];

  for (uint8_t i = 0; i < packetTypesSize; i++) {
    newPacketTypes[i] = packetTypes[i];
    delete packetTypes[i];
  }

  delete [] packetTypes;

  for (uint8_t i = 0; i < numberOfPacketTypes; i++) {
    newPacketTypes[packetTypesSize + i] = new PacketType(packetTypeIdentifiers[i], payloadSizes[i], isCritical[i]);
  }
  
  packetTypesSize += numberOfPacketTypes;
}

void SRSC::loop() {
  while (communicator.peek() == 0x02) {
    readAcceptack();
  }
}