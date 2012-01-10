#include <NanodeUNIO.h>
#include <NanodeUIP.h>
#include <psock.h>
#include "telnetd.h"
#include "printf.h"

void dhcp_status(int s,const uint16_t *) {
  char buf[20]="IP:";
  if (s==DHCP_STATUS_OK) {
    uip.get_ip_addr_str(buf+3);
    Serial.println(buf);

    nanode_log_P(PSTR("Bringing up telnet server..."));
    telnetd_init();
  }
}

void setup() {
  char buf[20];
  byte macaddr[6];
  NanodeUNIO unio(NANODE_MAC_DEVICE);

  Serial.begin(38400);
  printf_begin();
  printf_P(PSTR(__FILE__"\r\n"));
  
  unio.read(macaddr,NANODE_MAC_ADDRESS,6);
  uip.init(macaddr);
  uip.get_mac_str(buf);
  Serial.println(buf);
  uip.wait_for_link();
  nanode_log_P(PSTR("Link is up"));
  uip.start_dhcp(dhcp_status);
  nanode_log_P(PSTR("setup() done"));
}

void loop() {
  uip.poll();
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
