#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <pthread.h>
#define SVRADDR "127.0.0.1"
#define PORT 8080

FILE *ff;
struct event_base *p_base;
struct event_base *thread_base;
pthread_t thread_id;

static void
buff_input_cb (struct bufferevent *bev,
               void *ctx)
{
    printf("***in %s\n", __func__);
    /* goto another base */
    /* use bufferevent_disable/enable to remove it from main thread. or base_set not work */
    bufferevent_disable (bev, EV_WRITE|EV_READ);
    bufferevent_base_set (thread_base, bev);
    bufferevent_enable (bev, EV_WRITE|EV_READ);

    if (pthread_equal (thread_id, pthread_self())) {
        printf ("run at new thread\n");
    }

    int len = evbuffer_get_length(bufferevent_get_input(bev));
    printf("len=%d\n", len);
    event_base_dump_events(p_base, ff);

    char buff[1024];
    /* first read data, second make READ event pending */
    bufferevent_read(bev, buff, 1024);
    len = evbuffer_get_length(bufferevent_get_input(bev));
    printf("len=%d\n", len);
    event_base_dump_events(p_base, ff);

    return;
}

void*
thread_func (void *arg)
{
    printf ("new thread begin\n");
    event_base_loop (thread_base, EVLOOP_NO_EXIT_ON_EMPTY);
    printf ("new thread end\n");
}

int
main ()
{

    thread_base = event_base_new();
    pthread_create (&thread_id, NULL, thread_func, NULL);
    
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
    /* So If we disable READ, evbuffer callback will not be added to base (read and write) */
    bufferevent_setcb(p_event, buff_input_cb, NULL, NULL, NULL);
    bufferevent_enable(p_event, EV_WRITE | EV_READ);

    event_base_dispatch(p_base);
    printf ("main thread join\n");
    
    pthread_join (thread_id, NULL);
    return 0;
}
