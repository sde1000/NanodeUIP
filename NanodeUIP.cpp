#include "NanodeUIP.h"

#include <WProgram.h>
#include <inttypes.h>
#include "NanodeMAC.h"

extern "C" {
  #include "uip.h"
  #include "uip_arp.h"
  #include "enc28j60.h"
  #include "dhcpc.h"
  #include "hello-world.h"
}

extern "C" void nanode_log(char *msg);

void nanode_log(char *msg) {
  Serial.println(msg);
}

extern "C" void dhcpc_configured(const struct dhcpc_state *s);

void dhcpc_configured(const struct dhcpc_state *s) {
  uip_sethostaddr(s->ipaddr);
  uip_setnetmask(s->netmask);
  uip_setdraddr(s->default_router);
  //  resolv_conf(s->dnsaddr);
  if (uip.dhcp_status_callback!=NULL) {
    uip.dhcp_status_callback(DHCP_STATUS_OK);
  }
}

NanodeUIP::NanodeUIP(void) {
  // We don't initialise the UIP code in this constructor, because
  // apparently in some circumstances this can be called before the
  // init() function in arduino's wiring.c and several library
  // functions (like delay()) don't work.
  dhcp_status_callback=NULL;
}

void NanodeUIP::init(void) {
  struct uip_eth_addr mac;
  uip_ipaddr_t ipaddr;
  char buf[20];

  NanodeMAC(&mac);
  uip_setethaddr(mac);
  enc28j60SpiInit();
  enc28j60InitWithCs((uint8_t *)&mac, 8);
  enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
  delay(10);
  timer_set(&periodic_timer, CLOCK_SECOND / 2);
  timer_set(&arp_timer, CLOCK_SECOND * 10);
  uip_init();

#if 0
  /* We should eventually DHCP, but let's get other things working first */
  uip_ipaddr(ipaddr, 192,168,1,2);
  uip_sethostaddr(ipaddr);
  uip_ipaddr(ipaddr, 192,168,1,1);
  uip_setdraddr(ipaddr);
  uip_ipaddr(ipaddr, 255,255,255,0);
  uip_setnetmask(ipaddr);
#endif

  // Wait for link up
  while (!enc28j60linkup());
  Serial.println("Link up");
  dhcpc_init(&uip_ethaddr,6);

  hello_world_init();
}

// Requires a buffer of at least 18 bytes to format into
void NanodeUIP::getMACstr(char *buf) {
  sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X",
	  uip_ethaddr.addr[0], uip_ethaddr.addr[1], uip_ethaddr.addr[2],
	  uip_ethaddr.addr[3], uip_ethaddr.addr[4], uip_ethaddr.addr[5]);
}

// Requires a buffer of at least 16 bytes to format into
void NanodeUIP::getIPstr(char *buf) {
  sprintf(buf,"%d.%d.%d.%d",uip_hostaddr[0]&0xff,uip_hostaddr[0]>>8,
	  uip_hostaddr[1]&0xff,uip_hostaddr[1]>>8);
}

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

void NanodeUIP::poll(void) {
  int i;

  uip_len = enc28j60PacketReceive(UIP_BUFSIZE,uip_buf);
  if(uip_len > 0) {
    if(BUF->type == HTONS(UIP_ETHTYPE_IP)) {
      uip_arp_ipin();
      uip_input();
      /* If the above function invocation resulted in data that
	 should be sent out on the network, the global variable
	 uip_len is set to a value > 0. */
      if(uip_len > 0) {
	uip_arp_out();
	enc28j60PacketSend(uip_len,uip_buf);
      }
    } else if(BUF->type == HTONS(UIP_ETHTYPE_ARP)) {
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
