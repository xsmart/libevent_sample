#include <stdio.h>
#include <event2/event.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

int pp[2];
struct event *evw;
struct event *evr;

void
read_cb(evutil_socket_t fd, short evtype, void *arg) {
        printf("read_cb\n");
        char buf[1024];
        int ret = read(fd, buf, 1024);
        buf[ret] = '\0';
        printf("read == %s\n", buf);

        /* test event_active and event_pending function. */
        if(event_pending(evw, EV_WRITE, NULL) == 0) {
                printf("evw being pending, make it active now.\n");
                event_active(evw, EV_WRITE, 1); /* no matter the event is pending nor non-pending. */
        }
}

void
write_cb(evutil_socket_t fd, short evtype, void *arg) {
        printf("write_cb\n");

        /* when callback is executed, event are active. */

        /* just write once */
        static bool bfirst = true;
        if(bfirst == true) {
                write(pp[1], "hello", 5);
                bfirst = false;
        }


}

void
cb_1 (evutil_socket_t fd, short events, void *arg)
{
    return;
}

void
test_num_events()
{
    struct event_base *base = event_base_new();
    struct event *ev = event_new (base, -1, EV_TIMEOUT, cb_1, NULL);

    struct timeval tv = {1,0};
    event_add(ev, &tv);
    event_add(ev, &tv); /* added events == 1 */

    /* only calculate the number of events we added */
    int num = event_base_get_num_events (base, EVENT_BASE_COUNT_ADDED);
    printf ("added events: <%d>\n", num);

    num = event_base_get_num_events (base, EVENT_BASE_COUNT_VIRTUAL);
    printf ("virtul events: <%d>\n", num);  /* 0 */

    event_active (ev, EV_TIMEOUT, 0);
    num = event_base_get_num_events (base, EVENT_BASE_COUNT_ACTIVE);
    printf ("active events: <%d>\n", num);

    int cc  = event_base_get_max_events (base, EVENT_BASE_COUNT_ADDED, 10);
    printf ("max active events: <%d>\n", cc);
}

int
main() {
        pipe(pp);
        struct event_base *base = event_base_new();
        evr = event_new(base, pp[0], EV_READ, read_cb, NULL);
        evw = event_new(base, pp[1], EV_WRITE  | EV_TIMEOUT, write_cb, NULL);

        event_add(evw, NULL);
        event_add(evr, NULL);

        struct event_base *outbase;
        int fdout;
        event_get_assignment(evw, NULL, &fdout, NULL, NULL, NULL);		/* Note fdout. */
        outbase = event_get_base(evw);		/* outbase == base */
        event_callback_fn cb = event_get_callback(evw);
        assert(write_cb == cb);
        assert(NULL == event_get_callback_arg(evw));

        assert(EV_WRITE|EV_TIMEOUT == event_get_events(evw));

        assert(pp[1] == event_get_fd(evw));
        assert(event_initialized(evw) == 1);

        test_num_events();
        event_base_dispatch(base);

        return 0;
}


