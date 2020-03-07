#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>

#include <stdbool.h>

char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";

void rand_str(char *buf, int cnt) {
    int i;
    for(i = 0; i < cnt; i++) {
        buf[i] = letters[rand()%(sizeof(letters)-1)];
    }
}

void no_buffer() {
  alarm(30);
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
}

int readline(char *buf, int size) {
  int i;
  for(i = 0; i < size; i++) {
    if(read(0, buf+i, 1) <= 0) {
      return 0;
    }
    if (buf[i] == '\n') {
      buf[i] = '\x00';
      return i;
    }
  }
  buf[size-1] = '\x00';
  return size-1;
}

long read_int() {
  char buf[0x20];
  readline(buf, 0x10);
  return atoi(buf);
}

void read_n(int fd, char *buf, size_t size) {
  size_t n_read = 0;
  while (n_read < size) {
    ssize_t tmp = read(fd, buf+n_read, size-n_read);
    if (tmp <= 0) {
      return;
    }
    n_read += tmp;
  }
}

bool string_is_alnum(char *s) {
  while (*s != '\x00') {
    if (!isalnum(*s)) {
      return false;
    }
    s++;
  }
  return true;
}

void secure_rand(char *buf, size_t size) {
  int fd = open("/dev/urandom", O_RDONLY);
  if (fd <= 0) {
    printf("could not open /dev/urandom\n");
    exit(1);
  }
  read_n(fd, buf, size);
  close(fd);
}

void print_hex(const unsigned char *s, unsigned int len)
{
  for(unsigned int i = 0; i < len; i++)
    printf("%02x", (unsigned int) s[i]);
  printf("\n");
}

void test_exit() {
    printf("should exit\n");
    exit(1);
    return;
}


