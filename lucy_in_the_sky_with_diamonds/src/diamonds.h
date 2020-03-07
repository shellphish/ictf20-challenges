/*********************************************************************
 * @file          diamonds.h
 * @version
 * @brief
 * @author        badnack <n.redini@gmail.com>
 * @date          Thu Nov 21 20:41:30 2013
 * Modified at:   Mon Jan 13 17:26:52 2020
 * Modified by:   badnack <n.redini@gmail.com>
 ********************************************************************/
#include <stdio.h>

#include "util.h"

#ifndef __MY_POI_H
#define __MY_POI_H

extern int logged, user_id;
extern char* username, *password;

Message send_login(int);
Message show_my_diamonds(int);
Message add_diamond(int);

#endif
