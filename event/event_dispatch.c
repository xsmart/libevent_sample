#include <stdio.h>
#include <event2/event.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

int pp[2];
struct event *ev1;
struct event *ev2;
struct event *ev3;

void
cb_1(evutil_socket_t fd, short evtype, void *arg)
{
    printf("cb_1\n");
}


void
cb_2(evutil_socket_t fd, short evtype, void *arg)
{
    printf("cb_2\n");
}

void
cb_3(evutil_socket_t fd, short evtype, void *arg)
{
    printf("cb_3\n");
}

void
loop_once_test()
{
    struct event_base *base = event_base_new();
    ev1 = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, cb_1, NULL);
    ev2 = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, cb_2, NULL);

    struct timeval tv = {1,0};
    event_add (ev1, &tv);
    event_add (ev2, &tv);

    /* callback cb_1 cb_2 run */
    event_base_loop (base, EVLOOP_ONCE);
}

void
loop_nonblock_test()
{
    struct event_base *base = event_base_new();
    event_base_priority_init(base, 20); 

    ev1 = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, cb_1, NULL);
    ev2 = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, cb_2, NULL);

    event_priority_set(ev2, 18);    /* make no effective */

    struct timeval tv = {1,0};
    event_add (ev1, &tv);
    event_add (ev2, &tv);

    /* callback cb_1 cb_2 run */

    event_active (ev1, EV_TIMEOUT, 1);
    event_active (ev2, EV_TIMEOUT, 1);
    event_base_loop (base, EVLOOP_NONBLOCK);
}


void
loop_NO_EXIT_ON_EMPTY()
{
    struct event_base *base = event_base_new();
    event_base_priority_init(base, 20); 

    ev1 = event_new(base, -1, EV_TIMEOUT, cb_1, NULL);
    ev2 = event_new(base, -1, EV_TIMEOUT, cb_2, NULL);

    struct timeval tv = {1,0};
    event_add (ev1, &tv);
    event_add (ev2, &tv);

    /* callback cb_1 cb_2 run */

    event_base_loop (base, EVLOOP_NO_EXIT_ON_EMPTY);
//    event_base_dispatch(base);
}

int
main()
{
//    loop_once_test();
//    loop_nonblock_test();
    loop_NO_EXIT_ON_EMPTY();
    return 0;
}

