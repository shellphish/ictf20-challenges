/*********************************************************************
 * @file          help.c
 * @version
 * @brief
 * @author        badnack <n.redini@gmail.com>
 * @date          Thu Nov 21 19:58:57 2013
 * Modified at:   Mon Jan 13 17:50:11 2020
 * Modified by:   badnack <n.redini@gmail.com>
 ********************************************************************/
#include "help.h"
#include "errors.h"

Message
send_pag_help(int page)
{
  int ret;

    send_msg("\nLucyInTheSky is a diamond register online service. Here you can" \
             "register your diamonds, and find interested buyers.\n"\
             "\nR: Register to LucyInTheSky."\
             "\nL: Log on to manage your personal account."\
             "\nH: This screen."\
             "\nE: Exit"\
             "\nG: Show user diamonds."\
             "\nA: Add a new diamond."\
             );
  return OK;
}
