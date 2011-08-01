#include "clock.h"

#include <WProgram.h>

clock_time_t clock_time(void) {
  return millis();
}

void nullproc(void) {}

extern void nanode_log(char *msg);

void uip_log(char *msg) {
  nanode_log(msg);
}
