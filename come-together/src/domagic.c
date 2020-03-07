#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char ** argv){
  char size[50], img[2], session_id[100], position[50], cmd[400];
  memset(size, 0, 50);
  memset(img, 0, 2);
  memset(session_id, 0, 100);
  memset(position, 0, 50);
  memset(cmd, 0, 400);
  strcpy(img, getenv("IMG"));
  strcpy(size, getenv("SIZE"));
  strcpy(session_id, getenv("SESSION_ID"));
  strcpy(position, getenv("POSITION"));

  char pic[94];
  strcpy(pic, "data/");
  strcat(pic, session_id);
  strcat(pic, "/img.png");
  pic[93] = '\0';

  char b_pic[50];
  memset(b_pic, 0, 50);
  snprintf(b_pic, 50, "images/%s.png", img);

  fprintf(stderr, "%s\n%s\n", pic, b_pic);

  snprintf(cmd, 400, "convert -resize %s %s %s", size, pic, pic);
  int status = WEXITSTATUS(system(cmd));

  fprintf(stderr, "%s\n%d\n", cmd, status);

  if (status != 0)
    exit(1);

  snprintf(cmd, 400, "convert -composite -geometry %s %s %s %s", position, b_pic, pic, pic);
  status = WEXITSTATUS(system(cmd));

  fprintf(stderr, "%s\n%d\n", cmd, status);

  if (status != 0)
    exit(1);

  exit(0);
}