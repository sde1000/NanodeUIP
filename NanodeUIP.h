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
  /* This constructor can't actually do anything, because it gets
     called before usable amounts of Arduino infrastructure are
     initialised.   Call the init() function from your sketch instead. */
  NanodeUIP(void);

  void init(void); // Call in setup()
  void poll(void); // Must be called regularly in your sketch's loop()

  /* Link status functions */
  void wait_for_link(void); // Block until there's a link
  boolean link_is_up(void); // Returns true if link is up, otherwise false

  /* Manual address setting */
  void set_ip_addr(byte a, byte b, byte c, byte d);
  void set_netmask(byte a, byte b, byte c, byte d);
  void set_gateway_addr(byte a, byte b, byte c, byte d);
  void set_nameserver_addr(byte a, byte b, byte c, byte d);

  /* Output addresses to string buffers */
  void getMACstr(char *buf); // buf must be at least 18 bytes
  void get_ip_addr_str(char *buf); // buf must be at least 16 bytes
  void get_netmask_str(char *buf); // buf must be at least 16 bytes
  void get_gateway_str(char *buf); // buf must be at least 16 bytes

  /* Application startup functions all return 1 for success and 0 for
     failure; most common cause of failure is lack of listening port
     slots */

  /* Don't use manual address setting functions if you're going to
     start DHCP.  If you want to override the nameserver address, do
     so in the callback function when you're called with
     DHCP_STATUS_OK. */
  boolean start_dhcp(dhcp_status_fn *callback);

  boolean start_hello_world(word port);
  
};

extern NanodeUIP uip; // There can be only one!

#endif /* _NANODEUIP_LIB_H */
