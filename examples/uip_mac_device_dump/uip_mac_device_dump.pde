#include <NanodeUNIO.h>
#include <NanodeUIP.h>
#include <psock.h>

struct macdump_state {
  struct psock p;
  byte inputbuffer[1];
  word offset;
  /* lbuf does not actually need to be here, but there's plenty of
     space and it's better than allocating it on the stack in
     macdump_connection() */
  char lbuf[80];
};
UIPASSERT(sizeof(struct macdump_state)<=TCP_APP_STATE_SIZE)

static void dumppage(int offset,char lbuf[])
{
  byte buf[16];
  char *x;
  int j;
  NanodeUNIO unio(NANODE_MAC_DEVICE);

  memset(buf,0,16);
  unio.read(buf,offset,16);
  x=lbuf;
  sprintf(x,"%02X: ",offset);
  x+=4;
  for (j=0; j<16; j++) {
    sprintf(x,"%02X",buf[j]);
    x+=2;
  }
  *x=32; // space
  x+=1;
  for (j=0; j<16; j++) {
    if (buf[j]>=32 && buf[j]<127) *x=buf[j];
    else *x=46; // dot
    x++;
  }
  *x++=10; // linefeed
  *x++=0;
}
  
static int macdump_connection(struct macdump_state *s)
{
  PSOCK_BEGIN(&s->p);

  PSOCK_SEND_STR(&s->p, "MAC device dump\n\n");

  while (s->offset<256) {
    dumppage(s->offset,s->lbuf);
    PSOCK_SEND_STR(&s->p,s->lbuf);
    s->offset+=16;
  }
  
  PSOCK_CLOSE(&s->p);
  
  PSOCK_END(&s->p);
}

static void macdump_appcall(void)
{
  struct macdump_state *s = (struct macdump_state *)&(uip_conn->appstate);

  if(uip_connected()) {
    PSOCK_INIT(&s->p, s->inputbuffer, sizeof(s->inputbuffer)-1);
    s->offset=0;
  }

  macdump_connection(s);
}

void dhcp_status(int s,const uint16_t *dnsaddr) {
  char buf[20]="IP:";
  if (s==DHCP_STATUS_OK) {
    uip.get_ip_addr_str(buf+3);
    Serial.println(buf);
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
  uip.start_dhcp(dhcp_status);
  uip_listen(UIP_HTONS(1000),macdump_appcall);
  Serial.println("setup() done");
}

void loop() {
  uip.poll();
}

