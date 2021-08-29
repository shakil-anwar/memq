#include "memq.h"
#include "FlashMemory.h"
#include "RingEEPROM.h"

#define FLASH_CS  8
#define OTHER_CS 7 //consider other spi device is connected in bus
#define RING_EEPROM_ADDR  0
#define RING_BUF_LEN      4

Flash flash(FLASH_CS);
RingEEPROM ringObj(RING_EEPROM_ADDR);
uint8_t pageBuf[256];

typedef struct payload_t
{
  uint32_t value[8];
};

payload_t pldWrite;
payload_t pldRead;
struct memq_t *memq;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("Setup Done"));
  flash.begin();
  ringObj.begin(RING_BUF_LEN, sizeof(struct memqPtr_t)); //begin eeprom.
  
  memq = memqNew(0, sizeof(payload_t), 384);
  memq -> setMemory(memq, memReader, memWriter, memEraser, 4096);
  memq -> setPointer(memq, memPtrReader, memPtrWriter,10);
  memq -> attachBusSafety(memq, enableOthersOnbus,disableOthersOnbus);  
  memq -> reset(memq); 
}

void loop()
{
  uint8_t cmd = getSerialCmd();
  Serial.println(F("------------------------------"));
  if (cmd == 1)
  {
    generatePld(&pldWrite);
    memq -> write(memq, (uint8_t*)&pldWrite); //writing single data point
    printPld(&pldWrite);
    uint32_t curPage = (memq ->ringPtr._head)>>8;
    flash.dumpPage(curPage, pageBuf);
    Serial.print(F("Counter : "));Serial.println(memq -> _ptrEventCounter);
  }
  else if (cmd == 2)
  {
    memq -> read(memq, (uint8_t*)&pldRead);
    printPld(&pldRead);
    Serial.print(F("Counter : "));Serial.println(memq -> _ptrEventCounter);
  }
  memq -> saveMemQPtr(memq);
}

void memReader(uint32_t addr, uint8_t *buf, uint16_t len)
{
//  uint8_t *flashPtr = (uint8_t*)&dataStruct + (uint16_t)addr;
//  memcpy(buf, flashPtr, len);
  
  flash.read(addr, buf, sizeof(payload_t));
}

void memWriter(uint32_t addr, uint8_t *buf, uint16_t len)
{
//  uint8_t *flashPtr = (uint8_t*)&dataStruct + (uint16_t)addr;
//  memcpy(flashPtr, buf, len);
  flash.write(addr,buf,sizeof(payload_t));
}
void memEraser(uint32_t addr, uint16_t len)
{
//  uint8_t *flashPtr = (uint8_t*)&dataStruct + (uint16_t)addr;
//  memset(flashPtr, 0, len);
 flash.eraseSector(addr);
 uint32_t curPage = addr>>8;
 flash.dumpPage(curPage, pageBuf);
}

void memPtrReader(memqPtr_t *ptr)
{
  Serial.println(F("MemQPtr Reader called"));
//  memcpy(ptr, &ring, sizeof(memqPtr_t));
  ringObj.readPacket((byte *)ptr); 
}

void memPtrWriter(memqPtr_t *ptr)
{
  Serial.println(F("MemQPtr Writer called"));
   ringObj.savePacket((byte *)ptr);
  //memcpy(&ring, ptr, sizeof(memqPtr_t));
}

void printBuffer(uint8_t *buf, uint8_t len)
{
  Serial.print(F("Buffer :"));
  for (uint8_t i = 0; i < len; i++)
  {
    Serial.print(*(buf + i));
    Serial.print(F("  "));
  }
  Serial.println();
}

uint8_t getSerialCmd()
{
  Serial.print(F("Input Cmd :"));
  while (!Serial.available())
  {
    delay(10);
  }
  int cmd = Serial.parseInt();
  Serial.println(cmd);
  return cmd;
}

void generatePld(payload_t *pld)
{
  static uint32_t data;
  for(uint8_t i = 0; i< 8; i++)
  {
    pld ->value[i] = ++data;
  }
}
void printPld(payload_t *pld)
{
  Serial.print(F("Payload :"));
  for(uint8_t i = 0; i<8 ; i++)
  {
    Serial.print(pld -> value[i]);Serial.print(F("  "));
  }
  Serial.println();
}

void disableOthersOnbus()
{
  Serial.println(F("Disable others on bus"));
}

void enableOthersOnbus()
{
  Serial.println(F("Enable others on bus"));
}
