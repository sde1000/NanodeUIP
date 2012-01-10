#include "clock.h"
#include "uip-conf.h"

#if ARDUINO >= 100
  #include <Arduino.h> // Arduino 1.0
#else
  #include <WProgram.h> // Arduino 0022
#endif

void clock_init(void) {}

clock_time_t clock_time(void) {
  return millis();
}

void nullproc(void) {}

extern void nanode_log(char *msg);

void uip_log(char *msg) {
  nanode_log(msg);
}
