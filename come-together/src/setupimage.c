#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char ** argv){
  char peace_fname[50], session_id[100], cmd[200];
  memset(peace_fname, 0, 50);
  memset(session_id, 0, 100);
  memset(cmd, 0, 200);
  strcpy(peace_fname, getenv("PEACE_FNAME"));
  strcpy(session_id, getenv("SESSION_ID"));

  char dest[94];
  strcpy(dest, "data/");
  strcat(dest, session_id);
  strcat(dest, "/img.png");
  dest[93] = '\0';

  strcpy(cmd, "mv ");
  strcat(cmd, peace_fname);
  strcat(cmd, " ");
  strcat(cmd, dest);

  exit(WEXITSTATUS(system(cmd)));
}