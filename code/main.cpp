#include <stdio.h>
#include <wiringPi.h>

#define	CLK	0

void OnClock();

unsigned long long int g_cycles { 0 };

int main (void) {
  wiringPiSetup();

  wiringPiISR(CLK, INT_EDGE_FALLING, OnClock);

  while(true){
    delay(998);
    printf("%llu", g_cycles);
    g_cycles = 0;
  }

  return 0;
}

void OnClock(){
  g_cycles++;
}