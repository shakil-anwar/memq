#include "memq.h"

typedef struct data_t
{
  uint8_t val1;
  uint8_t val2;
};

int data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
data_t dataStruct[8]; //main data buffer

data_t Data; //buffer for writing one sample
data_t readBuf;//buffer for reading one sample



memqPtr_t ring;
struct memq_t memq;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("Setup Done"));
  ring._head = 0;
  ring._tail = 0;

  Data.val1 = 0;
  Data.val2 = 1;
  //  memq = memq_create(0, sizeof(uint16_t), sizeof(data) / sizeof(uint16_t));
//  memq = memqNew(0, sizeof(data_t), 8);
//  memq -> setMemory(memq, ramReader, ramWriter, ramEraser, 4);
//  memq -> setPointer(memq, ringptrRead, ringptrWrite,10);
//  memq -> attachBusSafety(memq, enableOthersOnbus,disableOthersOnbus);
//  
//  memq -> reset(memq);
  

  
  memqBegin(&memq,0, sizeof(data_t), 8);
  memqSetMem(&memq, ramReader, ramWriter, ramEraser, 4);
  memqAttachBusSafety(&memq, enableOthersOnbus,disableOthersOnbus);
  memqSetMemPtr(&memq, ringptrRead, ringptrWrite,10);

  memqReset(&memq);

}

void loop()
{
  uint8_t cmd = getSerialCmd();
  Serial.println(F("------------------------------"));
  if (cmd == 1)
  {
    memqWrite(&memq, (uint8_t*)&Data); //writing single data point
    printBuffer((uint8_t*)&dataStruct, sizeof(dataStruct));
    Serial.print(F("Counter : "));Serial.println(memq._ptrEventCounter);
    Data.val1 += 2;
    Data.val2 += 2;
  }
  else if (cmd == 2)
  {
    memqRead(&memq, (uint8_t*)&readBuf);
    printBuffer((uint8_t*)&readBuf, sizeof(readBuf));
    printBuffer((uint8_t*)&dataStruct, sizeof(dataStruct));
    Serial.print(F("Counter : "));Serial.println(memq._ptrEventCounter);
  }
  memqSaveMemPtr(&memq);
}

void ramReader(uint32_t addr, uint8_t *buf, uint16_t len)
{
  uint8_t *flashPtr = (uint8_t*)&dataStruct + (uint16_t)addr;
  memcpy(buf, flashPtr, len);
}

void ramWriter(uint32_t addr, uint8_t *buf, uint16_t len)
{
  uint8_t *flashPtr = (uint8_t*)&dataStruct + (uint16_t)addr;
  memcpy(flashPtr, buf, len);
}
void ramEraser(uint32_t addr, uint16_t len)
{
  uint8_t *flashPtr = (uint8_t*)&dataStruct + (uint16_t)addr;
  memset(flashPtr, 0, len);
}

void ringptrRead(memqPtr_t *ptr)
{
  memcpy(ptr, &ring, sizeof(memqPtr_t));
}

void ringptrWrite(memqPtr_t *ptr)
{
  Serial.println(F("MemQPtr Writer called"));
  memcpy(&ring, ptr, sizeof(memqPtr_t));
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

void printdata()
{
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print(data[i]); Serial.print(F("  "));
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



void disableOthersOnbus()
{
  Serial.println(F("Disable others on bus"));
}

void enableOthersOnbus()
{
  Serial.println(F("Enable others on bus"));
}