#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SVRADDR "127.0.0.1"
#define PORT 8080

/* copy from UNPv1e3 */
ssize_t
readline (int fd,
          void *vptr,
          size_t maxlen)
{

    ssize_t n, rc;
    char c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        again:
        if ((rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break; /* newline is stored, like fgets() */
        }else
            if (rc == 0) {
                *ptr = 0;
                return (n - 1); /* EOF, n - 1 bytes were read */
            }else {
                if (errno == EINTR)
                    goto again;
                return (-1); /* error, errno set by read() */
            }
    }

    *ptr = 0; /* null terminate like fgets() */
    return (n);
}

int
main ()
{

    int ret;
    int listenfd, connfd, sock_reuse;
    struct sockaddr_in servaddr;
    char buf[4096];

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket ");
        return 1;
    }

    sock_reuse = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *) &sock_reuse,
               sizeof(sock_reuse));

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("bind ");
        return 1;
    }

    listen(listenfd, 1024);

    if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) < 0) {
        perror("accept ");
        return 1;
    }
    printf("accept one \n");

    while (1) {
        readline(STDIN_FILENO, buf, sizeof(buf));
        printf("buf=[%s]\n", buf);
        ret = write(connfd, buf, strlen(buf));

#if 0
    while (1) {
        int n = read (connfd, buf, 1024);
        if (n==0) {
            printf ("client close the connection\n");
        }
        else if (n>0) {
            buf[n]=0;
            printf ("read <%s>\n", buf);
        }
        sleep (2);
    }
#endif
    }

    return 0;
}
