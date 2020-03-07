#include <stdio.h>

#define SESSION_SIZE 80
#define TOKEN_SIZE 120

typedef struct{
  char * session_id;
  char * token;
} session;

session * create_new_session();

void free_session(session * s);

int verify_session(session * s);

session * get_session(char * qs);
