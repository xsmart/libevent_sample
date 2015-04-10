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
    struct event_base *base = arg;
//    event_base_loopbreak(base);
    event_base_loopexit(base, NULL);

    printf("cb_1\n");
}

void
cb_2(evutil_socket_t fd, short evtype, void *arg)
{
    struct event_base *base = arg;
//    event_base_loopbreak(base);
    event_base_loopexit(base, NULL);
    printf("cb_2\n");
}

int
main()
{
    pipe(pp);
    struct event_base *base = event_base_new();
    event_base_priority_init(base, 20); 

    /* must be the same priority */
    ev1 = event_new(base, pp[1], EV_TIMEOUT|EV_PERSIST, cb_1, base);
    ev2 = event_new(base, pp[1], EV_TIMEOUT|EV_PERSIST, cb_2, base);

    struct timeval tv = {1,0};
    event_add(ev1, &tv);
    event_add(ev2, &tv);

    event_base_dispatch(base);

    return 0;
}

