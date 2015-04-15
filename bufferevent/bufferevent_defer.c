#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#define SVRADDR "127.0.0.1"
#define PORT 8080

FILE *ff;
struct event_base *p_base;

/* the three callback will be called all.
 * NOTE: buff_read and buff_wirte have different evbuffer.
 */

static void
buff_read (struct bufferevent *bev, void* ctx)
{
    printf("***in %s\n", __func__);
    sleep (1);
}

static void
buff_write (struct bufferevent *bev, void* ctx)
{
    printf("***in %s\n", __func__);
    bufferevent_write (bev, "nihao", 5);

    sleep (1);
}

static void
timeout_cb (evutil_socket_t fd, short event, void *arg)
{
    printf("***in %s\n", __func__);
}

int
main ()
{
    int sockfd;
    struct bufferevent *p_event;
    struct sockaddr_in addr;

    ff = fopen("/tmp/log", "w");
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SVRADDR, &addr.sin_addr) <= 0) {
        printf("inet_pton");
        exit(1);
    }

    if ((p_base = event_base_new()) == NULL) {
        printf("event_base_new ");
        return 1;
    }

    /* we do not need invoke socket function to create socket */
    if ((p_event = bufferevent_socket_new(p_base, -1, BEV_OPT_CLOSE_ON_FREE)) == NULL) {
        printf("bufferevent_socket_new ");
        return 1;
    }

    /* client actually connecte to server at this time. */
    if ((sockfd = bufferevent_socket_connect(p_event, (struct sockaddr *) &addr,
                                             sizeof(addr))) < 0) {
        printf("bufferevent_socket_connect ");
        return 1;
    }

    /* If connection crupt here, EV_WRITE cb will not be invoked. */
    bufferevent_setcb(p_event, buff_read, buff_write, NULL, p_base);
    bufferevent_enable (p_event, EV_READ | EV_WRITE);
    bufferevent_write(p_event, "begin", 5);

    struct event *ev = event_new (p_base, -1, EV_TIMEOUT | EV_PERSIST , timeout_cb, NULL);
    struct timeval tv={1,0};
    event_add (ev, &tv);
    event_base_dispatch(p_base);

    return 0;
}
