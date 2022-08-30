#include <stdio.h>
#include <wiringPi.h>

#define	CLK	0

void OnClock();

unsigned long long int g_cycles { 0 };

int main (void) {
  wiringPiSetup();
  pullUpDnControl(CLK, PUD_UP)

  wiringPiISR(CLK, INT_EDGE_FALLING, );


  while(true){
    delay(998);
    printf("%d", g_cycles);
    g_cycles = 0;
  }

  return 0;
}

void OnClock(){
  g_cycles++;
}