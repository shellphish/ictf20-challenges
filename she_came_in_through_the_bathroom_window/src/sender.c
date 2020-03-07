#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/ptrace.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define STR2(x) #x
#define STR(x) STR2(x)

#define DECL_STR(name, ...) \
    char name[1024] = {0};  \
    snprintf(name, sizeof name, __VA_ARGS__);

#define CHECK_CALL(func, ...) check(func(__VA_ARGS__), #func)
#define MAIN sendfd

static void info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    printf("[" STR(MAIN) "]  ");
    vprintf(fmt, args);
    printf("\n");
    fflush(stdout);

    va_end(args);
}

static int64_t check(int64_t err, const char *msg) {
    if (err == -1) {
        info("%s: %s", msg, strerror(errno));
        exit(0);
    }
    return err;
}

static void create_socket(int *sock, struct sockaddr_un *addr, socklen_t *addrlen) {
    info("Creating socket");
    *sock = CHECK_CALL(socket, AF_UNIX, SOCK_STREAM, 0);

    info("Creating addr");
    memset(addr, 0, sizeof *addr);
    addr->sun_family = AF_UNIX;
    strncpy(addr->sun_path, "@" STR(RAND), sizeof addr->sun_path - 1);
    *addrlen = offsetof(struct sockaddr_un, sun_path) + strlen(addr->sun_path) + 1;
    addr->sun_path[0] = 0;
}

static void send_fd(int conn, int fd) {
    info("Preparing fd message");

    char iobuf[1] = {0};
    struct iovec io = {.iov_base = iobuf, .iov_len = sizeof iobuf};

    union {
        char buf[CMSG_SPACE(sizeof fd)];
        struct cmsghdr align;
    } u;
    memset(&u, 0, sizeof u);

    struct msghdr msg = {0};
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = u.buf;
    msg.msg_controllen = sizeof u.buf;

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof fd);

    memcpy(CMSG_DATA(cmsg), &fd, sizeof fd);

    info("Sending fd");
    CHECK_CALL(sendmsg, conn, &msg, 0);
}

static void setup_socket_and_send_fd(int fd) {
    int sock;
    struct sockaddr_un addr;
    socklen_t addrlen;
    create_socket(&sock, &addr, &addrlen);

    info("Binding");
    CHECK_CALL(bind, sock, &addr, addrlen);

    info("Listening");
    CHECK_CALL(listen, sock, 8);

    info("Accepting");
    int conn = CHECK_CALL(accept, sock, NULL, NULL);

    send_fd(conn, fd);

    close(conn);
    close(sock);
}

// ----- entrypoints -----
static void do_sendfd(void) {
    info("Opening fd");
    int fd = CHECK_CALL(open, "/", 0);

    setup_socket_and_send_fd(fd);
    close(fd);
}

int main(void) {
	close(0);
	do_sendfd();
    return 0;
}
