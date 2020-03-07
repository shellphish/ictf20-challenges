#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char ** argv){
  if (argc != 3)
    exit(1);

  char session_id[100];
  char token[150];
  memset(session_id, 0, 100);
  memset(token, 0, 150);
  strcpy(session_id, argv[1]);
  strcpy(token, argv[2]);

  char dirname[86];
  strcpy(dirname, "data/");
  strcat(dirname, session_id);
  dirname[85] = '\0';

  int status = mkdir(dirname, 0700);

  if (status)
    exit(1);

  chdir(dirname);

  FILE *fp = fopen("token", "w");
  fprintf(fp, "%s\n", token);
  fclose(fp);

  exit(0);
}