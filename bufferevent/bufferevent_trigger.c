#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <assert.h>

#define SVRADDR "127.0.0.1"
#define PORT 8080

struct event_base *p_base;

static void
buff_input_cb (struct bufferevent *bev,
               void *ctx)
{
    printf("***in %s\n", __func__);

    int len = evbuffer_get_length(bufferevent_get_input(bev));
    printf("len=%d\n", len);

    char buff[1024];
    /* first read data, second make READ event pending */
    bufferevent_read(bev, buff, 1024);
    len = evbuffer_get_length(bufferevent_get_input(bev));
    printf("len=%d\n", len);

    /* It will works, but result in stack overflow. */
//    bufferevent_trigger (bev, EV_READ, BEV_TRIG_IGNORE_WATERMARKS);
    bufferevent_trigger_event (bev, BEV_EVENT_EOF, 0);

    return;
}

void
bev_event_cb (struct bufferevent *bev, short event, void *arg)
{
    printf ("bev_event_cb called\n");
    if (event & BEV_EVENT_EOF)
        bufferevent_free (bev);
}

int
main ()
{
    int sockfd;
    struct bufferevent *p_event;
    struct sockaddr_in addr;
    int cc;

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

    /* EV_WRITE is default enabled, EV_READ is default disabled */
    /* So If we disable READ, evbuffer callback will not be added to base (read and write) */
    bufferevent_setcb(p_event, buff_input_cb, NULL, bev_event_cb, p_base);
    bufferevent_enable(p_event, EV_WRITE | EV_READ);

    
    event_base_dispatch(p_base);

    return 0;
}
