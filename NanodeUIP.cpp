#include "NanodeUIP.h"

#include <inttypes.h>
#include <avr/pgmspace.h>

#include "uip.h"
#include "uip_arp.h"
#include "enc28j60.h"
#include "dhcpc.h"
#include "resolv.h"

void nanode_log(char *msg);

void nanode_log(char *msg) {
  Serial.println(msg);
}

void dhcpc_configured(const struct dhcpc_state *s) {
  uip_sethostaddr(&s->ipaddr);
  uip_setnetmask(&s->netmask);
  uip_setdraddr(&s->default_router);
  /* We don't call resolv_conf here, because that would drag in all
     the resolver code and state whether or not it's used by the
     sketch.  Instead we pass the address of the DNS server to the
     DHCP status callback code provided by the sketch and allow that
     to initialise the resolver if desired. */
  // resolv_conf(s->dnsaddr);
  if (uip.dhcp_status_callback!=NULL) {
    uip.dhcp_status_callback(DHCP_STATUS_OK, &s->dnsaddr);
  }
}

void resolv_found(char *name, uip_ipaddr_t *ipaddr);

void resolv_found(char *name, uip_ipaddr_t *ipaddr)
{
  if (uip.resolv_status_callback!=NULL) {
    uip.resolv_status_callback(name, ipaddr);
  }
}

NanodeUIP::NanodeUIP(void) {
  // We don't initialise the UIP code in this constructor, because
  // apparently in some circumstances this can be called before the
  // init() function in arduino's wiring.c and several library
  // functions (like delay()) don't work.
  dhcp_status_callback=NULL;
}

void NanodeUIP::init(const byte *macaddr) {
  const struct uip_eth_addr *mac=(struct uip_eth_addr *)macaddr;

  uip_setethaddr((*mac));
  enc28j60SpiInit();
  enc28j60InitWithCs(macaddr, 8);
  enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
  delay(10);
  timer_set(&periodic_timer, CLOCK_SECOND / 2);
  timer_set(&arp_timer, CLOCK_SECOND * 10);
  uip_init();
}

void NanodeUIP::wait_for_link(void) {
  while (!enc28j60linkup());
}

boolean NanodeUIP::link_is_up(void) {
  return enc28j60linkup()!=0;
}

void NanodeUIP::set_ip_addr(byte a, byte b, byte c, byte d) {
  uip_ipaddr_t ipaddr;
  uip_ipaddr(&ipaddr, a,b,c,d);
  uip_sethostaddr(&ipaddr);
}

void NanodeUIP::set_netmask(byte a, byte b, byte c, byte d) {
  uip_ipaddr_t ipaddr;
  uip_ipaddr(&ipaddr, a,b,c,d);
  uip_setnetmask(&ipaddr);
}

void NanodeUIP::set_gateway_addr(byte a, byte b, byte c, byte d) {
  uip_ipaddr_t ipaddr;
  uip_ipaddr(&ipaddr, a,b,c,d);
  uip_setdraddr(&ipaddr);
}

void NanodeUIP::set_nameserver_addr(byte a, byte b, byte c, byte d) {
  uip_ipaddr_t ipaddr;
  uip_ipaddr(&ipaddr, a,b,c,d);
  resolv_conf(&ipaddr);
}  

// Requires a buffer of at least 18 bytes to format into
void NanodeUIP::get_mac_str(char *buf) {
  sprintf_P(buf, PSTR("%02X:%02X:%02X:%02X:%02X:%02X"),
	  uip_ethaddr.addr[0], uip_ethaddr.addr[1], uip_ethaddr.addr[2],
	  uip_ethaddr.addr[3], uip_ethaddr.addr[4], uip_ethaddr.addr[5]);
}

// Requires a buffer of at least 16 bytes to format into
void NanodeUIP::format_ipaddr(char *buf, uip_ipaddr_t *addr) {
  sprintf_P(buf, PSTR("%d.%d.%d.%d"), uip_ipaddr_to_quad(addr));
}

void NanodeUIP::get_ip_addr_str(char *buf) {
  format_ipaddr(buf, &uip_hostaddr);
}

void NanodeUIP::get_netmask_str(char *buf) {
  format_ipaddr(buf, &uip_netmask);
}

void NanodeUIP::get_gateway_str(char *buf) {
  format_ipaddr(buf, &uip_draddr);
}

boolean NanodeUIP::start_dhcp(dhcp_status_fn *callback) {
  dhcp_status_callback=callback;
  return dhcpc_init(&uip_ethaddr, 6);
}

void NanodeUIP::init_resolv(resolv_result_fn *callback) {
  resolv_status_callback=callback;
  resolv_init();
}

void NanodeUIP::query_name(char *name) {
  resolv_query(name);
}

uip_ipaddr_t *lookup_name(char *name) {
  return resolv_lookup(name);
}

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

void NanodeUIP::poll(void) {
  int i;

  uip_len = enc28j60PacketReceive(UIP_BUFSIZE,uip_buf);
  if(uip_len > 0) {
    if(BUF->type == UIP_HTONS(UIP_ETHTYPE_IP)) {
      uip_arp_ipin();
      uip_input();
      /* If the above function invocation resulted in data that
	 should be sent out on the network, the global variable
	 uip_len is set to a value > 0. */
      if(uip_len > 0) {
	uip_arp_out();
	enc28j60PacketSend(uip_len,uip_buf);
      }
    } else if(BUF->type == UIP_HTONS(UIP_ETHTYPE_ARP)) {
      uip_arp_arpin();
      /* If the above function invocation resulted in data that
	 should be sent out on the network, the global variable
	 uip_len is set to a value > 0. */
      if(uip_len > 0) {
	enc28j60PacketSend(uip_len,uip_buf);
      }
    }
  } else if(timer_expired(&periodic_timer)) {
    timer_reset(&periodic_timer);
    for(i = 0; i < UIP_CONNS; i++) {
      uip_periodic(i);
      /* If the above function invocation resulted in data that
	 should be sent out on the network, the global variable
	 uip_len is set to a value > 0. */
      if(uip_len > 0) {
	uip_arp_out();
	enc28j60PacketSend(uip_len,uip_buf);
      }
    }
    
#if UIP_UDP
    for(i = 0; i < UIP_UDP_CONNS; i++) {
      uip_udp_periodic(i);
      /* If the above function invocation resulted in data that
	 should be sent out on the network, the global variable
	 uip_len is set to a value > 0. */
      if(uip_len > 0) {
	uip_arp_out();
	enc28j60PacketSend(uip_len,uip_buf);
      }
    }
#endif /* UIP_UDP */
    
    /* Call the ARP timer function every 10 seconds. */
    if(timer_expired(&arp_timer)) {
      timer_reset(&arp_timer);
      uip_arp_timer();
    }
  }
}

/* It seems ugly to define the only instance of the NanodeUIP class
   here, but it's a consequence of trying to wrap up the C UIP code in
   a C++ class so we can be an Arduino library.  If this wasn't here,
   callbacks from the UIP code wouldn't have the address of the
   NanodeUIP instance. */

NanodeUIP uip;
