#include <stdio.h>
#include <wiringPi.h>

// LED Pin - wiringPi pin 0 is ASSU_PI_GPIO 164.

#define	LED	0

int main (void) {
  printf("Raspberry Pi blink\n");

  wiringPiSetup();
  pinMode(LED, OUTPUT);

  while(true) {
    digitalWrite(LED, HIGH);	// On
    delay(500);		// mS
    digitalWrite(LED, LOW);	// Off
    delay(500);
  }
  return 0;
}
