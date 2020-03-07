#define _GNU_SOURCE
// Vulnerabilities:
// normal pwn -> overflow seccomp rules -> make fopen to return stdin -> buffer overflow
// namespace sandbox -> net sandbox escape
// chroot sandbox -> open a folder and do not pass CLOSE_EXEC
// use of uninitialized data when parsing
//
// So it is a story about an apple scruff trying to break into Lennon-McCartney's house.
// State can be saved into a file and the saved file contains flags.
// How to organize the challenge 
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <time.h>

#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include <linux/filter.h>
#include <linux/seccomp.h>
#include <linux/audit.h>
#include <sys/ptrace.h>
#include <sys/capability.h>

#define OPEN_TRIAL_MAX 5
#define timeout 5
#define input() printf(">> ")
#define append_dir "/home/chall/service/append/"
#define fname_len 32
#define passwd_len 32
struct BadStruct
{
	char name[100];
	struct sock_filter filter[0x40];
};

typedef struct Tool
{
	char *name;
	union {
		unsigned height;
		unsigned weight;
	};
} Tool;

int attempt_times;
struct BadStruct bstruct;
Tool *my_tools[4];
char *tool_name_list[4] = {"Ladder", "Wrench", "Screw_driver", "Hammer"};
int seccomp_len;
int dirfd;

int get_rand();
void update_attempt_str(void);
#define Allow(syscall) \
	BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, SYS_##syscall, 0, 1), \
	BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ALLOW)
#define Disallow(syscall) \
	BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, SYS_##syscall, 0, 1), \
	BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_KILL)
__attribute__((constructor))
void init_func()
{
	struct sock_filter filter[] = {
		/* validate arch */
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS, 4),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, AUDIT_ARCH_X86_64, 1, 0),
		BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_KILL),

		/* load syscall */
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS, offsetof(struct seccomp_data, nr)),

		/* black list some syscalls */
		Disallow(fork),
		Disallow(vfork),
		Disallow(clone),
		Disallow(creat),

		/* disallow file creation */
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, SYS_open, 0, 5),
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS, offsetof(struct seccomp_data, args)+8),
		BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 0x40),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x40, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_KILL),
		BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ALLOW),

		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, SYS_openat, 0, 5),
		BPF_STMT(BPF_LD+BPF_W+BPF_ABS, offsetof(struct seccomp_data, args)+0x10),
		BPF_STMT(BPF_ALU | BPF_AND | BPF_K, 0x40),
		BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, 0x40, 0, 1),
		BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_KILL),
		BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ALLOW),


		/* allow any syscall <= 100(for exploits) */
		BPF_JUMP(BPF_JMP+BPF_JGT+BPF_K, 100, 1, 0),
		BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_ALLOW),

		//* list of allowed syscalls */
		Allow(execveat),       /* called by execve*/
		Allow(prctl),       /* called by prctl */
		Allow(openat),       /* called by prctl */
		Allow(memfd_create),       /* called by prctl */
		Allow(arch_prctl),       /* called by prctl */

		/* and if we don't match above, die */
		BPF_STMT(BPF_RET+BPF_K, SECCOMP_RET_KILL),
	};
	seccomp_len = sizeof(filter)/sizeof(filter[0]);
	attempt_times = 0;
	update_attempt_str();
	memcpy(bstruct.filter, filter, sizeof(filter));
	srand(get_rand());

	// VULNERABILITY: there is a fd leak here
	// can be used for chroot escape
	fclose(stderr);
	dirfd = open("/tmp/bath_window", O_RDONLY);
}

/* Utilities */
void init_proc()
{
	setvbuf(stdin,  0LL, 2LL, 0LL);
	setvbuf(stdout, 0LL, 2LL, 0LL);
	setvbuf(stderr, 0LL, 2LL, 0LL);
}

void error_out(char *s)
{
	fprintf(stderr, "%s\n", s);
	exit(-1);
}

void readline(char *buf, unsigned len)
{
	int ret;
	int num_read = 0;
	for(int i=0; i<len; i++) {
		ret = read(0, &buf[i], 1);
		if(ret <= 0)error_out("read error");
		if(buf[i] == '\n') {
			buf[i] = 0;
			return;
		}
		num_read += ret;
	}
	if(num_read > 0)buf[num_read-1] = 0;
}

int read_int()
{
	char buf[20];
	readline(buf, sizeof(buf));
	return atoi(buf);
}

void update_attempt_str(void)
{
	snprintf(bstruct.name, sizeof(bstruct.name)-1, "attempt-%d", attempt_times);
}

int execveat(int dirfd, const char *pathname, char *argv[], char *envp[], int flags)
{
	return syscall(SYS_execveat, dirfd, pathname, argv, envp, flags);
}

/* Functionalities */
void show_banner()
{
	puts(" _  _____ _______ ______   ___   ___ ___   ___  ");
	puts("(_)/ ____|__   __|  ____| |__ \\ / _ \\__ \\ / _ \\ ");
	puts(" _| |       | |  | |__       ) | | | | ) | | | |");
	puts("| | |       | |  |  __|     / /| | | |/ /| | | |");
	puts("| | |____   | |  | |       / /_| |_| / /_| |_| |");
	puts("|_|\\_____|  |_|  |_|      |____|\\___/____|\\___/ ");
	puts("                                                ");
}

void intro()
{
	puts("As an Apple Scruff, you are tired of being in the group just waiting outside with cameras and autograph books.");
	puts("Now, it is time to join the break-in group.");
	puts("I heard Lennon McCartney has a precious picture in his room.");
	puts("So, let's go for it!");
	puts("                      ");
}

void action_menu()
{
	printf("Attempt: %s\n", bstruct.name);
	puts("What to do now?");
	puts("[0] Hide my tools somewhere. I will come again later.");
	puts("[1] Recover my tools and continue breaking in.");
	puts("[2] Bring more tools with me...Just in case...");// open a fd -> which is an ladder from the chroot jail
	puts("[3] Show me my tools.");
	puts("[4] Give a fancy name to this attempt.");// overwrite seccomp rule
	puts("[5] Let's go..Sneaky..Sneaky..");
	input();
}

void list_tools()
{
	puts("Tools available:");
	puts("[0] Ladder");
	puts("[1] Wrench");
	puts("[2] Screw driver");
	puts("[3] Hammer");
	puts("Which to choose");
	input();
};

int ask_get_rand()
{
	int height = -1;
	int fd = -1;
	char buf[8];
	memset(buf, 0, sizeof(buf));

	puts("How high do you want it to be?");
	input();
	scanf("%d", &height);
	
	if(height < 0) {
		puts("You are a cheater.");
		height = 0;
		fd = openat(dirfd, "height", O_RDONLY);
		if(fd < 0)error_out("open error");
		read(fd, buf, sizeof(buf)-1);
		height = atoi(buf);
	}
	if(fd != -1)close(fd);
	return height;
}

int get_rand()
{
	int fd;
	int weight = 0;
	fd = open("/dev/urandom", O_RDONLY);
	if(fd < 0)error_out("open error");
	read(fd, &weight, 2);
	close(fd);
	return weight;
}

void set_seccomp()
{
	struct sock_fprog filterprog = {
		.len = seccomp_len,
		.filter = bstruct.filter
	};

	if(prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0))
		error_out("PR_SET_NO_NEW_PRIVS error");
	if(prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &filterprog) == -1) {
		perror("[seccomp]");
		exit(-1);
	}
}

int real_get_rand()
{
	int fd;
	int num = 0;
	fd = open("/dev/urandom", O_RDONLY);
	if(fd < 0)error_out("open error");
	read(fd, &num, sizeof(int));
	close(fd);
	return num;
}

void rand_str(char *dest, size_t length) {
	char charset[] = "0123456789"
					 "abcdefghijklmnopqrstuvwxyz"
					 "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	while (length-- > 0) {
		size_t index = real_get_rand() % (sizeof(charset)/sizeof(char) - 1 );
		*dest++ = charset[index];
	}
	*dest = '\0';
}

void clear_my_tools(void)
{
	for(int i=0; i<4; i++) {
		Tool* tool = my_tools[i];
		free(tool);
		my_tools[i] = NULL;
	}
}

void save()
{
	char path[fname_len + sizeof(append_dir) + 1];
	char filename[fname_len + 1];
	char passwd[passwd_len + 1];
	FILE *f;
	Tool *tool;

	memset(passwd, 0, sizeof(passwd));

	// build path and open flag file
	for(int i=0; i < OPEN_TRIAL_MAX; i++) {
		memset(path, 0, sizeof(path));
		rand_str(filename, fname_len);
		strcpy(path, append_dir);
		strcat(path, filename);

		f = fopen(path, "w");
		if(!f) {
			//int i;
			//for(i=0; i<30; i++) {
			//	printf("open %s fail\n", path);
			//	sleep(1);
			//}
			error_out("save error");
		}
	}

	// build passwd
	rand_str(passwd, passwd_len);


	fprintf(f, "%s\n", passwd);
	fprintf(f, "%s\n", bstruct.name);
	fprintf(f, "%d\n", attempt_times);

	for(int i=0; i<4; i++) {
		tool = my_tools[i];
		if(tool)fprintf(f, "%s %d\n", tool->name, tool->height);
		else{
			fprintf(f, "(null)\n");
		}
	}

	fclose(f);

	printf("You buried your tools at: %s\n", filename);
	printf("And it's password: %s\n", passwd);
}

void check_filename(char *path)
{
	if(strstr(path, ".") || strstr(path, "/")) error_out("oh no!!!");
}

void load()
{
	char path[fname_len + sizeof(append_dir) + 1];
	char filename[fname_len + 1];
	char passwd[passwd_len + 1];
	char real_passwd[passwd_len + 1];
	FILE *f;
	char name[100+1];
	char buf[80];

	memset(path, 0, sizeof(path));
	memset(filename, 0, sizeof(filename));
	memset(name, 0, sizeof(name));

	puts("Where did you bury your tools?");
	input();
	scanf("%32s", filename);

	puts("What's your password?");
	input();
	if(!scanf("%32s", passwd)) error_out("scanf error");
	getchar();

	// construct path
	check_filename(filename);
	strcpy(path, append_dir);
	strcat(path, filename);

	// open file
	f = fopen(path, "r");
	if(!f) {
		puts("Oh no! Your tools are gone!");
		return;
	}

	fscanf(f, "%32s", real_passwd);
	if(strcmp(real_passwd, passwd)) error_out("Don't try to steal. OK?");
	fscanf(f, "%99s", name);
	fscanf(f, "%d", &attempt_times);

	// load tools
	clear_my_tools();
	for(int i=0; i<4; i++) {
		unsigned weight;
		memset(buf, 0, sizeof(buf));
		fscanf(f, "%60s", buf);
		if(!strncmp(buf, "(null)", 6)) {
			//puts("null");
			continue;
		}
		fscanf(f, "%d", &weight);

		if(strcmp(tool_name_list[i], buf)) error_out("malformed tool list");
		my_tools[i] = realloc(my_tools[i], sizeof(Tool));
		my_tools[i]->name = tool_name_list[i];
		my_tools[i]->weight = weight;
	}

	strcpy(bstruct.name, name);
}

void more_tools()
{
	int idx= -1;
	Tool* tool;
	list_tools();
	idx = read_int();
	if(idx < 0 || idx > 3)error_out("Nice try");
	tool = realloc(my_tools[idx], sizeof(Tool));
	tool->name = tool_name_list[idx];

	switch (idx) {
		case 0:
			tool->height = ask_get_rand();
			break;
		case 1:
		case 2:
		case 3:
			tool->weight = get_rand();
			break;
		default:
			puts("I don't understand you");
	}
	my_tools[idx] = tool;
}

void show_tools()
{
	Tool *tool;
	for(int i=0; i<4; i++) {
		tool = my_tools[i];
		if(tool == NULL)continue;

		if(!strcmp(tool->name, "Ladder")) {
			printf("[*] Ladder, height: %d\n", tool->height);
		}
		else {
			printf("[*] %s, weight: %d\n", tool->name, tool->height);
		}
	}
}

// VULNERABILITY: buffer overflow
void rename_attempt()
{
	puts("What do you want to name this attempt?");
	input();
	readline(bstruct.name, 0x100);
}

void setup_chroot(const char *path)
{
	if(chroot(path))
		error_out("chroot error");
	if(chdir("/"))
		error_out("chdir error");
}

void hardening()
{
	set_seccomp();
	for(int i=3; i<0x400; i++) {
		close(i);
	}
	// TODO: setup namespace sandbox
}

// read from input and write it to the fd file
void read_to_fd(int fd, size_t size) {
	char buf[0x1000];
	ssize_t x;

	while (size) {
		x = read(0, buf, sizeof(buf) < size ? sizeof(buf) : size);
		if (x <= 0) {
			error_out("read error2");
		}
		if (write(fd, buf, x) != x) {
			error_out("write error");
		}
		size -= x;
	}
}

int get_elf()
{
	int size;
	int fd;
	puts("ELF size?");
	input();

	size = read_int();
	if(size <= 0 || size >= 10*1024*1024)
		error_out("size error");

	fd = memfd_create("EXE", MFD_CLOEXEC);
	if(fd < 0)error_out("memfd_create error");

	puts("Content?");
	input();

	read_to_fd(fd, size);
	return fd;
}

void exec_input_elf()
{
	int fd;
	hardening();
  	fd = get_elf();

	if(execveat(fd, "", NULL, NULL, AT_EMPTY_PATH))
		error_out("execveat error");
}

int print_owned_tools()
{
	Tool *tool;
	int printed = 0;
	for(int i=0; i<4; i++) {
		tool = my_tools[i];
		if(tool == NULL)continue;
		printed = 1;
		printf("[%d] use %s\n", i, tool->name);
	}
	return printed;
}

void try_break_in()
{
	int action, pid, status = 0;
	int start;
	int in;
	puts("Shhhhh. We are in the garden now.");
	puts("What to do now?");
	if(!print_owned_tools()) {
		puts("Nah, you don't have any tools with you");
		goto out;
	};
	input();
	action = read_int();
	if(action < 0 || action >=4 || !my_tools[action]) error_out("Oh No!");

	pid = fork();
	if(pid == -1) error_out("fork error");
	else if(pid == 0) {
		switch (action) {
			case 0:
				puts("We are in!");
				setup_chroot("/tmp/room");
				exec_input_elf();
				break;
			case 1:
			case 2:
				puts("Oh no! We are caught!");
				setup_chroot("/tmp/garden");
				exec_input_elf();
				break;
			case 3:
				puts("It's a terrible crime to use a hammer to break in.");
				setup_chroot("/tmp/jail");
				exec_input_elf();
				break;
			default:
				error_out("I don't understand you");
		}
	}

	in = dup(0);
	close(0);

	start = time(NULL);
	while(time(NULL) - start < timeout && status == 0)waitpid(pid, &status, WNOHANG);

	dup2(in, 0);
	close(in);

out:
	attempt_times += 1;
	update_attempt_str();
	return;
}

void main_func()
{
	int choice;
	intro();
	while(1) {
		action_menu();
		choice = read_int();
		switch (choice) {
			case 0:
				save();
				break;
			case 1:
				load();
				break;
			case 2:
				more_tools();
				break;
			case 3:
				show_tools();
				break;
			case 4:
				rename_attempt();
				break;
			case 5:
				try_break_in();
				break;
			default:
				puts("I don't understand you.");
		}
	}

}

int main()
{
	init_proc();
	show_banner();
	main_func();
}
