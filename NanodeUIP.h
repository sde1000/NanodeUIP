#ifndef _NANODEUIP_LIB_H
#define _NANODEUIP_LIB_H

#include <WProgram.h>

extern "C" {
#include "timer.h"
}

class NanodeUIP {
 private:
  struct timer periodic_timer, arp_timer;
 public:
  NanodeUIP(void); // Constructor doesn't actually do anything
  void init(void);
  void getMACstr(char *buf); // Fill buf with string version of MAC addr
  void poll(void);
};

#endif /* _NANODEUIP_LIB_H */
