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

static void
buff_input_cb (struct bufferevent *bev,
               void *ctx)
{
    printf("***in %s\n", __func__);

    int len = evbuffer_get_length(bufferevent_get_input(bev));
    printf("len=%d\n", len);
    event_base_dump_events(p_base, ff);

    if (len==20) {
        char buff[1024];
        /* first read data, second make READ event pending */
        bufferevent_read(bev, buff, 1024);
        len = evbuffer_get_length(bufferevent_get_input(bev));
        printf("len=%d\n", len);
        event_base_dump_events(p_base, ff);
    }

    return;
}

static void
buff_ev_cb (struct bufferevent *bev,
            short events,
            void *ctx)
{

    printf("in %s\n", __func__);

    if (events & BEV_EVENT_CONNECTED) {
        /* equal to BEV_EVENT_CONNECTED */
        printf("CONNECTED, event is <%d>\n", events);   
    }else if (events & BEV_EVENT_ERROR) {
        printf("ERROR, error is <%d>\n", events);
    }else if (events & BEV_EVENT_EOF) {
        /* ADD BEV_EVENT_READING, if you want catch the EVENT, enable EV_READ */
        printf("EOF, error is <%d>\n", events);
    }
    return;
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

    /* */
    bufferevent_setcb(p_event, buff_input_cb, NULL, buff_ev_cb, p_base);
    bufferevent_enable (p_event, EV_READ);
    event_base_dispatch(p_base);

    return 0;
}
