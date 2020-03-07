/*********************************************************************
 * @file          diamonds.c
 * @version
 * @brief
 * @author        badnack <n.redini@gmail.com>
 * @date          Thu Nov 21 20:42:50 2013
 * Modified at:   Tue Jan 14 19:00:06 2020
 * Modified by:   badnack <n.redini@gmail.com>
 ********************************************************************/
#include "diamonds.h"
#include "errors.h"
#include "screens.h"
#include "dbhelper.h"

int logged = 0, user_id = -1;
char delimiter = ':';

Message
show_user_diamonds()
{
  diamond *ptr=NULL;
  int n = 0;
  char * buff;
  if (get_diamonds(&ptr, &n) != 0){
    return -1;
  }
  buff = malloc((n * sizeof(diamond) + 1) * sizeof(char));

  int cnt = 0;
  for (int i = 0; i< n; i++) {
    sprintf(buff+cnt, "%d%c%d%c%d%c%d%c%f%c%f%c%s%c", ptr[i].timestamp, delimiter,ptr[i].id, delimiter, ptr[i].descriptionLen, delimiter, ptr[i].userId, delimiter, ptr[i].price, delimiter, ptr[i].size, delimiter, ptr[i].descr, delimiter);
    cnt = strlen(buff);
  }
  
  send_msg(buff);
  free(ptr);

  return 0;
}

Message
show_my_diamonds(int id)
{
  diamond *list;
  int tot, i, n, count;
  // max char per text + max len double * 2 + 3 delimiters
  char buff[((MAX_USER_LEN + 1) + (11 * 2) +  3) * MAX_DIAMONDS_PER_USER + 1];

  n = 0;
  if (id == -1) {
    send_msg(err_not_logged);
    return NOT_LOGGED;
  }
  if ((tot = list_diamonds(id, &list)) < 0) {
    send_msg(err_undefined);
    return FATAL_ERROR;
  }

  send_msg(ack_ok);

  count = 0;
  for (i = 0; i < tot; i++) {
    if ((n = snprintf(&buff[count], 11 + 1 + 1, "%f%c", list[i].price, delimiter)) < 0 ||
        n > MAX_USER_LEN) {
      return FATAL_ERROR;
    }

    count += n;
    if ((n = snprintf(&buff[count], 11 + 1 + 1, "%f%c", list[i].size, delimiter)) < 0 ||
        n > MAX_USER_LEN) {
      return FATAL_ERROR;
    }
    count += n;
    if ((n = snprintf(&buff[count], MAX_USER_LEN + 1, "%s\n", list[i].descr)) < 0 ||
        n > MAX_USER_LEN) {
      return FATAL_ERROR;
    }

    count += n;
  }
  buff[count] = '\0';

  if (!tot) {
    if ((n = snprintf(buff, strlen(no_diamonds), "%s", no_diamonds)) < 0 ||
        n > strlen(no_diamonds) + 1) {
      return FATAL_ERROR;
    }
  }

  send_msg(buff);
  return OK;
}

Message
add_diamond(int v)
{
  char *err_size, *err_price;
  double size, price;
  int set1, set2, set3, len;
  int ret;
  char* recv_data = malloc(255);
  diamond *d = (diamond*) malloc(sizeof(diamond));

  d->insert = &insert_diamond;
  
  if (!logged) {
    send_msg(err_not_logged);
    free(recv_data);
    free(d);
    return NOT_LOGGED;
  }

  send_msg(ack_ok);

  send_msg("\nInsert price diamond in $ [format (DDD.dddd)]: ");

  set1 = 0;
  set2 = 0;
  set3 = 0;

  if (recv_msg(recv_data, MAX_USER_LEN) == OK) {
    price = strtod(recv_data, &err_price);
    len = strnlen(recv_data, MAX_USER_LEN);
    set1 = (&recv_data[len] == err_price);
    d->price = price;
  }
 
  send_msg("\nInsert size in cm [format (DDD.dddd)]: ");
  if (recv_msg(recv_data, MAX_USER_LEN) == OK) {
    size = strtod(recv_data, &err_size);
    len = strnlen(recv_data, MAX_USER_LEN);
    set2 = (&recv_data[len] == err_size);
    d->size = size;
  }
 
  send_msg("\nNotes: ");
  if (recv_msg(recv_data, MAX_USER_LEN) == OK) {
    set3 = 1;
    strncpy(d->descr, recv_data, MAX_USER_LEN);
  }
  
  /* if some error occurred */
  if (!(set1 & set2 & set3)) {
    send_msg(err_invalid_info);
    free(d);
    free(recv_data);
    return WRONG_SELECTION;
  }

  send_msg(ack_ok);

  if ((ret = d->insert(user_id, d->price, d->size, d->descr)) < 0) {
    if (ret == DB_ERROR_TOO_MANY_DIAMONDS) {
      send_msg(err_too_diamonds);
    } else {
      send_msg(err_undefined);
    }
    free(d);
    free(recv_data);
    return FATAL_ERROR;
  }
  free(d);
  free(recv_data);
  send_msg("Diamond added.");

  return OK;
}

Message
send_login(int v)
{
  int uid, len;
  char userid[MAX_USER_LEN], password[MAX_USER_LEN], *tmp, *buf;
  Message retu, retp;

  send_msg("User Id: ");
  retu = recv_msg(userid, MAX_USER_LEN);
  send_msg("\nPassword: ");
  retp = recv_msg(password, MAX_USER_LEN);

  if (retu < 0 || retp < 0) {
    send_msg(err_invalid_cmd);
    return FATAL_ERROR;
  }

  uid = strtol(userid, &tmp, 10);
  len = strnlen(userid, MAX_USER_LEN);

  if (tmp != &userid[len] || (check_cred(uid, password, &tmp) < 0)) {
    send_msg(err_not_exists);
    return WRONG_SELECTION;
  }
  if (tmp == NULL) {
    send_msg(err_undefined);
    return FATAL_ERROR;
  }

  if ((buf = (char*) malloc(((strlen(welcome_back) + strlen(tmp)) * sizeof(char)) + 1)) == NULL) {
    send_msg(err_undefined);
    return FATAL_ERROR;
  }
  sprintf(buf, welcome_back, tmp);

  user_id = uid;
  logged = 1;
  send_msg(ack_ok);
  send_msg(buf);
  free(buf);
  free(tmp);
  return OK;
}
