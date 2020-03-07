#define _GNU_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(){
  char * fname;
  fname = getenv("PEACE_FNAME");
  FILE * f = fopen(fname, "rb");

  if (f) {
    fseek(f, 0, SEEK_END);
    int length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char * buffer = (char *) malloc(length);

    if (buffer) {
      fread(buffer, 1, length, f);
      fclose (f);

      char * start = (char *) memmem(buffer, length, "\r\n\r\n", 4);
      start += 4;
      int new_size = length - (start - buffer);
      char * end = (char *) memmem(start, length - (start - buffer) , "-----", 5);

      if (start != NULL && end != NULL && !strncmp(start + 1, "PNG", 3)){
        int size = end - start;
        fclose(f);
        f = fopen(fname, "wb");
        fwrite(start, 1, size, f);
        fclose(f);
        fprintf(stderr, "He got early warning");
        exit(WEXITSTATUS(system("./setupimage")));
      }
    }
  }

  fprintf(stderr, "He got muddy water");

  fclose(f);
  exit(1);
}