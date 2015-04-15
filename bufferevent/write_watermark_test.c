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

/* verify the work manner of water-mark */

static void
event_output (struct bufferevent *bev, void *ctx)
{
    printf("***in %s\n", __func__);
}

static void
buff_ev_cb (struct bufferevent *bev,
            short events,
            void *ctx)
{

    printf("in %s\n", __func__);

    if (events & BEV_EVENT_CONNECTED) {
        printf("***BEV_EVENT_CONNECTED\n");
    }else if (events & BEV_EVENT_ERROR) {
        printf("***BEV_EVENT_ERROR\n");
    }else if (events & BEV_EVENT_EOF) {
        printf ("EOF, event <%d>", events);
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

    /* EV_WRITE is enbaled by default, so the underlying evbuffer writecb will be called when dispatch.
     * evbuffer writecb FOUND that there is no data to be sent, return.
     * evbuffer writecb is not persisted. (If persisted, evbuffer writecb will be called again and agian, see event_write
     * So there is no pending event in base, base will exit.
     */
    bufferevent_setcb(p_event, NULL, event_output, buff_ev_cb, p_base);

    /* write data to evbuffer, So when evbuffer cb FOUND there is data, send it out. 
     * And, call the bufferevent cb based on water mark.
     */
    bufferevent_write (p_event, "nisssssssssdssssssssa", 20);

    /*
     * normally, If evbuffer cb send data, FOUND that : the left data len is < 10, then bufferevent writecb
     * will be invoked to tell ME.
     * hey, there almost no data!!
     */
    bufferevent_setwatermark(p_event, EV_WRITE, 10, -1);
    
    sleep (10);
    event_base_dispatch(p_base);

    return 0;
}
