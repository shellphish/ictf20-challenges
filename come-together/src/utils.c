#include "utils.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

filedata * read_file(char * filename){
  FILE * f = fopen(filename, "rb");

  if (f) {
    fseek(f, 0, SEEK_END);
    int length = ftell(f);
    fseek(f, 0, SEEK_SET);
    filedata * data = malloc(sizeof(filedata)); 
    char * buffer = (char *) malloc(length);

    if (buffer) {
      fread(buffer, 1, length, f);
      fclose (f);
      data->buffer = buffer;
      data->size = length;
      return data;
    }
  
    fclose (f);
  }

  return NULL;
}

int write_file(char * filename, char * content, int size){
  FILE * f = fopen(filename, "wb");

  if (f) {
    if (size == NULL)
      size = strlen(content);

    fwrite(content, 1, size, f);
    fclose(f);

    return 1;
  }

  fclose(f);
  return 0;
}

void rand_string(char *str, size_t size){
  struct timespec t;

  clock_gettime(CLOCK_REALTIME, &t);
  srand(t.tv_nsec);

  char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  size_t n;

  if (size) {
    for (n = 0; n < (size-1); n++) {
      /* sizeof charset - 1: '\0' is NOT part of the charset */
      int key = rand() % (int) (sizeof(charset) - 1);
      str[n] = charset[key];
    }
    str[n] = '\0';
  }
}


char * get_query_param(char * query, char * param_name){
  if (query == NULL || param_name == NULL)
    return NULL;

  char * param;
  int size;
  char * find = malloc(strlen(param_name) + 2);
  sprintf(find, "%s=\0", param_name);

  char * start = strstr(query, find);
  if (start == NULL)
    return NULL;
  start += strlen(param_name) + 1;

  char * end = strchr(start, '&');
  if (end == NULL)
    size = strlen(start);
  else
    size = end - start;

  param = malloc(size + 1);
  strncpy(param, start, size);
  param[size] = '\0';

  return param;
}

int count_substr(char * str, char * substr, int size){
  int count = 0;
  char *tmp = str;

  while(tmp = strstr(tmp, substr))
  {
    if (tmp >= str + size)
      break;
    count++;
    tmp++;
  }

  return count;
}
