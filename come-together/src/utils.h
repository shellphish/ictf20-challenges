#include <stdio.h>

typedef struct{
  int size;
  char * buffer;
} filedata;

filedata * read_file(char * filename);
int write_file(char * filename, char * content, int size);

void rand_string(char *str, size_t size);
char * get_query_param(char * query, char * param_name);
int count_substr(char * str, char * substr, int size);
