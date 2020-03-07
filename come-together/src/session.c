#include "session.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

session * create_new_session() {
  char cmd[300];
  char * session_id = malloc(SESSION_SIZE);
  char * token = malloc(TOKEN_SIZE);

  memset(cmd, 0, 300);

  rand_string(session_id, SESSION_SIZE);
  rand_string(token, TOKEN_SIZE);

  session * s = malloc(sizeof(session));
  s->session_id = session_id;
  s->token = token;

  strcpy(cmd, "./setupsession ");
  strcat(cmd, session_id);
  strcat(cmd, " ");
  strcat(cmd, token);

  int status = WEXITSTATUS(system(cmd));

  if (!status)
    return s;
  else
    return NULL;
}

void free_session(session * s) {
  if (s != NULL){
    free(s->session_id);
    free(s->token);
    free(s);
  }
}

int verify_session(session * s) {
  char fname[92];
  char token[TOKEN_SIZE];
  FILE * f;

  // TODO path traversal
  snprintf(fname, 92, "data/%s/token", s->session_id);
  fname[91] = '\0';

  if ((f = fopen(fname, "r"))) {
    fgets(token, TOKEN_SIZE, f);

    if (!strncmp(token, s->token, TOKEN_SIZE))
      return 1;
    else
      return 0;

  } else {
    return 0;
  }
}

session * get_session(char * qs){
  session * s = malloc(sizeof(session));
  s->session_id = get_query_param(qs, "session");
  s->token = get_query_param(qs, "token");

  if (s->session_id != NULL && s->token != NULL){
    if (verify_session(s))
      return s;
  }

  free_session(s);
  return NULL;
}
