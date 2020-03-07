//FIXME need to block syscalls over value 300ish
//FIXME make sure results are well checked

#define _GNU_SOURCE 1
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "util.h"

#include "seccomp-bpf.h"

#define MAX_RESULTS 0x20

//void *results = NULL;
int has_used = 0;
char *g_filter = NULL;
char g_name[0x20];

unsigned int g_len = 0;

short g_result = 0;


struct sock_filter safe_filter[] = {
	/* Validate architecture. */
	VALIDATE_ARCHITECTURE,
	/* Grab the system call number. */
	EXAMINE_SYSCALL,
    /* Block > 400 */
    BPF_JUMP(BPF_JMP+BPF_JGT+BPF_K, 400, 0, 1),
    BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_KILL),
    /* Block anything with high bits set */
    BPF_JUMP(BPF_JMP+BPF_JSET+BPF_K, 0xf0000000, 0, 1),
    BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_KILL),
	/* List allowed syscalls. */
	BLOCK_SYSCALL(sendfile),
	BLOCK_SYSCALL(fork),
	BLOCK_SYSCALL(vfork),
	BLOCK_SYSCALL(clone),
	BLOCK_SYSCALL(execve),
	BLOCK_SYSCALL(kill),
	BLOCK_SYSCALL(truncate),
	BLOCK_SYSCALL(ftruncate),
	BLOCK_SYSCALL(rename),
	BLOCK_SYSCALL(mkdir),
	BLOCK_SYSCALL(rmdir),
	BLOCK_SYSCALL(creat),
	BLOCK_SYSCALL(link),
	BLOCK_SYSCALL(symlink),
	BLOCK_SYSCALL(unlink),
	BLOCK_SYSCALL(chmod),
	BLOCK_SYSCALL(fchmod),
	BLOCK_SYSCALL(unlinkat),
	BLOCK_SYSCALL(renameat),
	BLOCK_SYSCALL(mkdirat),
	BLOCK_SYSCALL(linkat),
	BLOCK_SYSCALL(symlinkat),
	BLOCK_SYSCALL(fchmodat),
	BLOCK_SYSCALL(alarm),
	BLOCK_SYSCALL(rt_sigprocmask),
	BLOCK_SYSCALL(rt_sigaction),
    BLOCK_SYSCALL(splice),
    BLOCK_SYSCALL(tee),
    BLOCK_SYSCALL(renameat2),
    BLOCK_SYSCALL(ptrace),
    BLOCK_SYSCALL(socket),



  ALLOW_REST,
};


static int install_syscall_filter(struct sock_filter* filter, unsigned long nbytes)
{
	struct sock_fprog prog = {
		.len = (unsigned short)(nbytes/sizeof(filter[0])),
		.filter = filter,
	};

	if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog)) {
		perror("prctl(SECCOMP)");
		goto failed;
	}
	return 0;

failed:
	return 1;
}

void save_result() {
  char pass[16] = {0};
  printf("enter a password to protect your save file\n");
  readline(pass, 16);
  // create saveid
  unsigned long id;
  secure_rand((char*)&id, sizeof(id));
  char save_name[40];
  snprintf(save_name, 40, "./append/%lu", id);
  FILE *f = fopen(save_name, "w");
  if (!f) {
    printf("Error: Could not open %s for writing\n", save_name);
    exit(1);
  }
  fprintf(f, "%s\n%s\n", pass, g_name);
  fprintf(f, "%u\n%hd\n", g_len, g_result);
  if (g_len > 0x40000) {
    printf("check failed\n");
    exit(1);
  }

  fwrite(g_filter, 1, g_len, f);
  fclose(f);
  printf("saveid is: %lu\n", id);
  exit(1);
}

void load_result() {
  char name[0x40] = {0};
  char buf[0x40] = {0};
  char tmp[0x20] = {0};
  char pass[0x20] = {0};
  
  printf("Enter the saveid you'll be loading\n");
  readline(name, 0x20);
  if (!string_is_alnum(name)) {
    printf("saveid not alphanumeric\n");
    return;
  }

  snprintf(buf, 40, "./append/%s", name);

  // check password
  FILE *f = fopen(buf, "r");
  if (!f) {
    printf("Save %s does not exist!\n", name);
    return;
  }

  printf("Enter the password\n");
  if (readline(pass, 20) < 0) {
    printf("read error\n");
    fclose(f);
    return;
  }


  // FIXME THIS FGETS CAN'T BE ALLOWED TO FAIL
  fgets(tmp, 16, f);
  char *pos;
  if ((pos = strchr(tmp, '\n')) != NULL) {
    *pos = '\x00';
  }

  if (strlen(tmp) < 5) {
    puts("Error reading password");
    fclose(f);
    return;
  }

  register int res asm("r10") = strcmp(tmp, pass);
  if (res) {
    puts("Invalid password");
    if (fclose(f)) {
        puts("Error closing file");
    }
    return;
  }

  //FIXME this could be a buffer overflow with %s instead
  //FIXME I LIKE THIS BETTER
  fgets(g_name, 0x20, f);

  fscanf(f, "%u\n%hd", &g_len, &g_result);
  fgetc(f); // new line

  free(g_filter);
  g_filter = malloc(g_len);
  fread(g_filter, g_len, 1, f);

  printf("Welcome back %s\n", g_name);

  fclose(f);
  
}



void install_base_filter() {
  printf("The strawberry fields\n");
  printf("You thought they were forever\n");
  printf("But nothing is real\n");

	if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {
		perror("prctl(NO_NEW_PRIVS)");
		exit(-1);
	}
  install_syscall_filter(safe_filter, sizeof(safe_filter));
  printf("Seccomp filters, those are real though\n");
}

void print_filter() {
  printf("Your filter is %d bytes\n", g_len);
  printf("What index do you want to start printing from? ");
  unsigned int index = read_int();
  if (index > g_len) {
    printf("That index is out of bounds!\n");
    return;
  } 
  printf("Filter: ");
  print_hex((unsigned char *)(g_filter+index), g_len-index);
  printf("Last result: %u\n", g_result);
}

void read_filter() {
    printf("How many bytes is your filter? ");
    unsigned int len = read_int();
    if (len > 0x40000) {
      printf("Way too big!\n");
      return;
    }
    free(g_filter);
    g_filter = malloc(len);
    g_len = len;
    printf("data? ");
    read_n(0, g_filter, len);
  }


void install_filter() {
  install_syscall_filter((struct sock_filter*)g_filter, g_len);
  printf("Filter installed!\n");
}

// if we have already set g_len large, this results in small malloc + read before
void add_to_filter() {
    printf("how many bytes are you adding? ");
    unsigned int len = read_int();
    if (len > 0x40000 || len + g_len > 0x40000) {
      printf("Way too big!\n");
      return;
    }
    printf("data? ");
    g_filter = realloc(g_filter, len+g_len);
    read_n(0, g_filter+(int) g_len, len);
    g_len += len;
}

long do_filter(unsigned long arg0, unsigned long arg1, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5, unsigned long arg6) {
    long ret;
    register long rax asm("rax") = arg0;
    register long rdi asm("rdi") = arg1;
    register long rsi asm("rsi") = arg2;
    register long rdx asm("rdx") = arg3;
    register long r10 asm("r10") = arg4;
    register long r8 asm("r8") = arg5;
    register long r9 asm("r9") = arg6;

    asm(
        "syscall\n\t"
        :"=a"(ret)
        :"r" (rax), "r"(rdi), "r"(rsi), "r"(rdx), "r" (r10), "r" (r8), "r" (r9));
    return (long)ret;
}

void test_filter() {
  if (has_used) {
    printf("Sorry you've already had one valid test once in this demo version\n");
    return;
  }
  printf("Enter the args\n");
  printf("arg0) ");
  short arg0 = read_int();
  printf("arg1) ");
  short arg1 = read_int();
  printf("arg2) ");
  short arg2= read_int();
  printf("arg3) ");
  short arg3 = read_int();
  printf("arg4) ");
  short arg4 = read_int();
  printf("arg5) ");
  short arg5 = read_int();
  printf("arg6) ");
  short arg6 = read_int();
  long res = do_filter(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
  if (res >= 0) {
    has_used = 1;
    printf("Result: %hd\n", (short)res);
  } else {
    printf("Test failed with value: %hd\n", (short)res);
  }

}

void main_loop()
{
    int command = 0;
    while(command != 8) {
      printf("1) Read new filter\n");
      printf("2) Add to filter\n");
      printf("3) Install filter\n");
      printf("4) Print filter\n");
      printf("5) Test filter\n");
      printf("6) Save Result\n");
      printf("7) Load Previous Result\n");
      printf("8) Exit\n");
      printf("> ");
      command = read_int();
      switch(command) {
        case 1:
          read_filter();
          break;
        case 2:
          add_to_filter();
          break;
        case 3:
          install_filter();
          break;
        case 4:
          print_filter();
          break;
        case 5:
          test_filter();
          break;
        case 6:
          save_result();
          break;
        case 7:
          load_result();
          break;
      }
    }

	return;
}


int main(int argc, char *argv[])
{
    no_buffer();
    install_base_filter();

    printf("What is your name?\n");
    readline(g_name, 0x20);

    main_loop();
    return 0;
}



