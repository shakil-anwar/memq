#ifndef __MEMQ__H__
#define __MEMQ__H__

#ifdef __cplusplus 
extern "C" {
#endif
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define MEMQ_DEBUG

enum qState_t
{
  RESET = 0,
  RUNNING = 1,
  NO_DATA = 3,
};

struct memqPtr_t
{
  uint32_t _head;
  uint32_t _tail;
  uint32_t willEraseAddr;
  enum qState_t qState;
  bool _isLock;
  uint8_t checkSum;
};

typedef void (*memFun_t)(uint32_t addr, uint8_t *buf, uint16_t len);
typedef void (*memEraser_t)(uint32_t blobAddr, uint16_t len);
typedef void (*ringFun_t)(struct memqPtr_t *ptr);


struct memq_t
{
  memFun_t _memReader;
  memFun_t _memWriter;
  memEraser_t _memBlobEraser;

  ringFun_t _ptrRead;
  ringFun_t _ptrWrite;
  
  void (*_disableOthers)(void);
  void (*_enableOthers)(void);
  
  uint16_t _ptrEventCounter;
  uint16_t _maxPtrEvent;
  uint16_t _packetLen;
  uint16_t _blobSize;

  uint32_t _baseAddr;
  uint32_t _lastAddr;
  struct memqPtr_t ringPtr;
};


struct memq_t *memqNew(uint32_t baseAddr, uint32_t packetLen, uint32_t totalPacket); //to create memq object dynamically
void memqBegin(struct memq_t *memq,uint32_t baseAddr, uint32_t packetLen, uint32_t totalPacket); //to begin memq when object created before program start
void memqSetMem(struct memq_t *memq, memFun_t memReader, memFun_t memWriter, memEraser_t memEraser, uint16_t blobSz);
void memqSetMemPtr(struct memq_t *memq, ringFun_t reader, ringFun_t writer, uint16_t maxPtrEvent);
void memqAttachBusSafety(struct memq_t *memq, void (*enableOthers)(void), void (*disableOthers)(void));

void memqReset(struct memq_t *memq);
void memqWrite(struct memq_t *memq, uint8_t *buf);
uint8_t *memqRead(struct memq_t *memq, uint8_t *buf);
void memqSaveMemPtr(struct memq_t *memq);


bool memqIsLock(struct memq_t *memq);
uint32_t memqAvailable(struct memq_t *memq);
bool memqIsLock(struct memq_t *memq);
void memqPrintLog(struct memq_t *memq);



#ifdef __cplusplus 
}
#endif

#endif
