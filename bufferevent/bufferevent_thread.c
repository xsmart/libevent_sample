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
struct bufferevent *g_bev;

void *thread_func (void * arg)
{
    printf("***in %s\n", __func__);
    bufferevent_free (g_bev); 
}

static void
buff_input_cb (struct bufferevent *bev,
               void *ctx)
{
    printf("***in %s\n", __func__);
    pthread_t thread;
    pthread_create (&thread, NULL, thread_func, NULL);
    sleep (5);
    int cc = bufferevent_write (bev, "hello", 5);   /* It can write the data to client, after the cb, send EOF */
    printf ("write return val=%d\n", cc);
    return;
}

static void
buff_ev_cb (struct bufferevent *bev,
            short events,
            void *ctx)
{
    printf("in %s\n", __func__);
}

int
main ()
{
    int sockfd;
    struct bufferevent *p_event;
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SVRADDR, &addr.sin_addr) <= 0) {
        printf("inet_pton");
        exit(1);
    }

    evthread_use_pthreads();
    if ((p_base = event_base_new()) == NULL) {
        printf("event_base_new ");
        return 1;
    }
    

    /* we do not need invoke socket function to create socket */
    if ((p_event = bufferevent_socket_new(p_base, -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE)) == NULL) {
        printf("bufferevent_socket_new ");
        return 1;
    }
    g_bev = p_event;

    /* client actually connecte to server at this time. */
    if ((sockfd = bufferevent_socket_connect(p_event, (struct sockaddr *) &addr,
                                             sizeof(addr))) < 0) {
        printf("bufferevent_socket_connect ");
        return 1;
    }

    /* EV_WRITE is default enabled, EV_READ is default disabled */
    /* So If we disable READ, evbuffer callback will not be added to base (read and write) */
    bufferevent_setcb(p_event, buff_input_cb, NULL, buff_ev_cb, p_base);
    bufferevent_enable(p_event, EV_READ | EV_WRITE);

    event_base_dispatch(p_base);

    return 0;
}
