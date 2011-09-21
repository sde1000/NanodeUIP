/**
 * \addtogroup helloworld
 * @{
 */

/**
 * \file
 *         An example of how to write uIP applications
 *         with protosockets.
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

/*
 * This is a short example of how to write uIP applications using
 * protosockets.
 */

/*
 * We define the application state (struct hello_world_state) in the
 * hello-world.h file, so we need to include it here. We also include
 * uip.h (since this cannot be included in hello-world.h) and
 * <string.h>, since we use the memcpy() function in the code.
 */
#include "hello-world.h"
#include "uip.h"
#include <string.h>

UIPASSERT(sizeof(struct hello_world_state)<=TCP_APP_STATE_SIZE)

/*
 * Declaration of the protosocket function that handles the connection
 * (defined at the end of the code).
 */
static int handle_connection(struct hello_world_state *s);
/*---------------------------------------------------------------------------*/
/*
 * The initialization function. We must explicitly call this function
 * from the system initialization code, some time after uip_init() is
 * called.
 */
int
hello_world_init(u16_t port)
{
  /* We start to listen for connections on the specified TCP port */
  return uip_listen(HTONS(port),hello_world_appcall);
}
/*---------------------------------------------------------------------------*/
/*
 * In hello-world.h we have defined the UIP_APPCALL macro to
 * hello_world_appcall so that this funcion is uIP's application
 * function. This function is called whenever an uIP event occurs
 * (e.g. when a new connection is established, new data arrives, sent
 * data is acknowledged, data needs to be retransmitted, etc.).
 */
void
hello_world_appcall(void)
{
  /*
   * The uip_conn structure has a field called "appstate" that holds
   * the application state of the connection. We make a pointer to
   * this to access it easier.
   */
  struct hello_world_state *s = (struct hello_world_state *)&(uip_conn->appstate);

  /*
   * If a new connection was just established, we should initialize
   * the protosocket in our applications' state structure.
   */
  if(uip_connected()) {
    PSOCK_INIT(&s->p, s->inputbuffer, sizeof(s->inputbuffer)-1);
  }

  /*
   * Finally, we run the protosocket function that actually handles
   * the communication. We pass it a pointer to the application state
   * of the current connection.
   */
  handle_connection(s);
}
/*---------------------------------------------------------------------------*/
/*
 * This is the protosocket function that handles the communication. A
 * protosocket function must always return an int, but must never
 * explicitly return - all return statements are hidden in the PSOCK
 * macros.
 */
static int
handle_connection(struct hello_world_state *s)
{
  PSOCK_BEGIN(&s->p);

  PSOCK_SEND_STR(&s->p, "Hello. What is your name?\n>>> ");
  PSOCK_READTO(&s->p, '\n');
  s->inputbuffer[PSOCK_DATALEN(&s->p)]=0;
  strncpy(s->name, (const char *)s->inputbuffer, sizeof(s->name));
  PSOCK_SEND_STR(&s->p, "What is your quest?\n>>> ");
  PSOCK_READTO(&s->p, '\n');
  s->inputbuffer[PSOCK_DATALEN(&s->p)]=0;
  strncpy(s->quest, (const char *)s->inputbuffer, sizeof(s->quest));
  PSOCK_SEND_STR(&s->p, "Hello ");
  PSOCK_SEND_STR(&s->p, s->name);
  PSOCK_SEND_STR(&s->p, "Your quest is: ");
  PSOCK_SEND_STR(&s->p, s->quest);
  PSOCK_CLOSE(&s->p);
  
  PSOCK_END(&s->p);
}
/*---------------------------------------------------------------------------*/
