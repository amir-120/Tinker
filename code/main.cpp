#include <stdio.h>
#include <wiringPi.h>

#include "StructsDS.hpp"

#define	CLK	29
#define DAT 28
#define CMD 27
#define MTR 26
#define ATT 25
#define ACK 24

struct DS2{
  DS2Packet ds2CMDPacketReady{ 0 };
  DS2Packet ds2CMDPacketInUse{ 0 };
  DS2Packet ds2DATPacketReady{ 0 };
  DS2Packet ds2DATPacketInUse{ 0 };

  size_t bitCursor{ 0 };
} g_ds2;

void OnClock();

unsigned long long int g_cycles { 0 };

int main (void) {
  wiringPiSetup();
  pullUpDnControl(CLK, PUD_UP);

  wiringPiISR(CLK, INT_EDGE_FALLING, OnClock);

  while(true){
    delay(998);
    printf("Cycles: %llu\n", g_cycles);
    g_cycles = 0;
  }

  return 0;
}

void OnClock(){
  g_cycles++;
}