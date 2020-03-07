/*********************************************************************
 * @file          exit.c
 * @version
 * @brief
 * @author        badnack <n.redini@gmail.com>
 * @date          Thu Nov 21 00:20:14 2013
 * Modified at:   Mon Jan 13 17:25:49 2020
 * Modified by:   badnack <n.redini@gmail.com>
 ********************************************************************/
#include "exit.h"

Message
send_exit()
{
  send_msg("bye.");
  return BYE_BYE;
}
