#include "NanodeUIP.h"

#include <WProgram.h>
#include <inttypes.h>
#include "NanodeMAC.h"

extern "C" {
  #include "uip.h"
  #include "uip_arp.h"
  #include "enc28j60.h"
}

extern "C" void nanode_log(char *msg);

void nanode_log(char *msg) {
  Serial.println(msg);
}

NanodeUIP::NanodeUIP(void) {
  // We don't initialise in the constructor, because apparently in
  // some circumstances this can be called before the init() function
  // in arduino's wiring.c and several library functions (like
  // delay()) don't work.
}

void NanodeUIP::init(void) {
  struct uip_eth_addr mac;
  uip_ipaddr_t ipaddr;
  char buf[20];

  Serial.println("Get MAC");
  NanodeMAC(&mac);
  sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X",
	  mac.addr[0], mac.addr[1], mac.addr[2],
	  mac.addr[3], mac.addr[4], mac.addr[5]);
  Serial.println(buf);
  uip_setethaddr(mac);
  Serial.println("Eth init");
  enc28j60SpiInit();
  enc28j60InitWithCs((uint8_t *)&mac, 8);
  enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
  delay(10);
  Serial.println("Timer and UIP init");
  timer_set(&periodic_timer, CLOCK_SECOND / 2);
  timer_set(&arp_timer, CLOCK_SECOND * 10);
  uip_init();

  /* We should eventually DHCP, but let's get other things working first */
  uip_ipaddr(ipaddr, 192,168,73,200);
  uip_sethostaddr(ipaddr);
  uip_ipaddr(ipaddr, 192,168,73,4);
  uip_setdraddr(ipaddr);
  uip_ipaddr(ipaddr, 255,255,255,0);
  uip_setnetmask(ipaddr);
}

// Requires a buffer of at least 18 bytes to format into
void NanodeUIP::getMACstr(char *buf) {
  sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X",
	  uip_ethaddr.addr[0], uip_ethaddr.addr[1], uip_ethaddr.addr[2],
	  uip_ethaddr.addr[3], uip_ethaddr.addr[4], uip_ethaddr.addr[5]);
}

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

void NanodeUIP::poll(void) {
  int i;

  uip_len = enc28j60PacketReceive(UIP_BUFSIZE,uip_buf);
  if(uip_len > 0) {
    Serial.println("Got packet");
    if(BUF->type == htons(UIP_ETHTYPE_IP)) {
      uip_arp_ipin();
      uip_input();
      /* If the above function invocation resulted in data that
	 should be sent out on the network, the global variable
	 uip_len is set to a value > 0. */
      if(uip_len > 0) {
	uip_arp_out();
	enc28j60PacketSend(uip_len,uip_buf);
      }
    } else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
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

