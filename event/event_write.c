#include <stdio.h>
#include <event2/event.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

int pp[2];
struct event *ev1;

void
cb_1(evutil_socket_t fd, short evtype, void *arg)
{
    printf("event write\n");
    sleep(1);
}

int
main()
{
    pipe(pp);
    struct event_base *base = event_base_new();
    
    /* test flag EV_WRITE|EV_PERSIST, cb_1 will be called again and again */
    ev1 = event_new(base, pp[1], EV_WRITE | EV_PERSIST, cb_1, NULL);
    event_add(ev1, NULL);
    event_base_dispatch(base);

    return 0;
}

