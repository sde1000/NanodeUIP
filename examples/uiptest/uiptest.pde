#include <NanodeUIP.h>
#include <NanodeUNIO.h>

static void resolv_found(char *name, uip_ipaddr_t *addr) {
  char buf[30]=": addr=";
  Serial.print(name);
  uip.format_ipaddr(buf+7,addr);
  Serial.println(buf);
}

void dhcp_status(int s,const uip_ipaddr_t *dnsaddr) {
  char buf[20]="IP:";
  if (s==DHCP_STATUS_OK) {
    resolv_conf(dnsaddr);
    uip.get_ip_addr_str(buf+3);
    Serial.println(buf);
    uip.query_name("www.greenend.org.uk");
  }
}

void setup() {
  char buf[20];
  byte macaddr[6];
  NanodeUNIO unio(NANODE_MAC_DEVICE);

  Serial.begin(38400);
  Serial.println("UIP test");
  
  unio.read(macaddr,NANODE_MAC_ADDRESS,6);
  uip.init(macaddr);
  uip.get_mac_str(buf);
  Serial.println(buf);
  uip.wait_for_link();
  Serial.println("Link is up");
  uip.init_resolv(resolv_found);
  uip.start_dhcp(dhcp_status);
  Serial.println("setup() done");
}

void loop() {
  uip.poll();
}
