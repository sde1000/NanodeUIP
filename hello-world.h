/**
 * \addtogroup apps
 * @{
 */

/**
 * \defgroup helloworld Hello, world
 * @{
 *
 * A small example showing how to write applications with
 * \ref psock "protosockets".
 */

/**
 * \file
 *         Header file for an example of how to write uIP applications
 *         with protosockets.
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#ifndef __HELLO_WORLD_H__
#define __HELLO_WORLD_H__

#include <WProgram.h>

/* Since this file will be included by uip.h, we cannot include uip.h
   here. But we might need to include uipopt.h if we need the u8_t and
   u16_t datatypes. */
#include "uipopt.h"

#include "psock.h"

struct hello_world_state {
  struct psock p;
  byte inputbuffer[20];
  char name[20];
  char quest[20];
};

/* Finally we define the application function to be called by uIP. */
void hello_world_appcall(void);

int hello_world_init(u16_t port);

#endif /* __HELLO_WORLD_H__ */
/** @} */
/** @} */
