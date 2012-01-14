#ifndef _NANODEUIP_LIB_H
#define _NANODEUIP_LIB_H

#if ARDUINO >= 100
  #include <Arduino.h> // Arduino 1.0
#else
  #include <WProgram.h> // Arduino 0022
#endif

#include "uip.h"
#include "timer.h"

extern void resolv_conf(const uip_ipaddr_t *dnsserver);

#define DHCP_STATUS_OK 1
#define DHCP_STATUS_DOWN 0
/* If called with DHCP_STATUS_OK, dnsaddr is the address discovered
   for the nameserver.  If you want to use the resolver library, call
   resolv_conf(dnsaddr) in your implementation of this callback. */
typedef void dhcp_status_fn(int status,const uip_ipaddr_t *dnsaddr);

typedef void resolv_result_fn(char *name, uip_ipaddr_t *addr);

class NanodeUIP {
 private:
  struct timer periodic_timer, arp_timer;
 public:
  dhcp_status_fn *dhcp_status_callback;
  resolv_result_fn *resolv_status_callback;
  /* This constructor can't actually do anything, because it gets
     called before usable amounts of Arduino infrastructure are
     initialised.   Call the init() function from your sketch instead. */
  NanodeUIP(void);

  void init(const byte *macaddr); // Call in setup()
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
  void get_mac_str(char *buf); // buf must be at least 18 bytes
  void format_ipaddr(char *buf, uip_ipaddr_t *addr); // 16 byte buf
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

  /* Call this in setup() if you intend to use the resolver library. */
  void init_resolv(resolv_result_fn *callback);

  /* Start looking up a name; when found it will be cached, and the
     resolver callback function will be called. */
  void query_name(char *name);
  
  /* Return an address from the cache */
  uip_ipaddr_t *lookup_name(char *name);
};

extern NanodeUIP uip; // There can be only one!

#endif /* _NANODEUIP_LIB_H */
