#ifndef _NANODEUIP_LIB_H
#define _NANODEUIP_LIB_H

#include <WProgram.h>

extern "C" {
#include "timer.h"
}

#define DHCP_STATUS_OK 1
#define DHCP_STATUS_DOWN 0
typedef void dhcp_status_fn(int status);

class NanodeUIP {
 private:
  struct timer periodic_timer, arp_timer;
 public:
  dhcp_status_fn *dhcp_status_callback;
  NanodeUIP(void); // Constructor doesn't actually do anything
  void init();
  void getMACstr(char *buf); // buf must be at least 18 bytes
  void getIPstr(char *buf); // buf must be at least 16 bytes
  void poll(void);
};

extern NanodeUIP uip; // There can be only one!

#endif /* _NANODEUIP_LIB_H */
