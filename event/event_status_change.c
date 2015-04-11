#include <stdbool.h>
#include <assert.h>
#include <event2/event.h>

/* make event active by invoke functions */

int pp[2];
struct event *evw;
struct event *evr;

void
read_cb(evutil_socket_t fd, short evtype, void *arg)
{
    printf("read_cb\n");
    char buf[1024];
    int ret = read(fd, buf, 1024);
    buf[ret] = '\0';
    printf("read == %s\n", buf);

    assert(event_pending(evr, EV_READ, NULL));              /* being active. */
}

void
write_cb(evutil_socket_t fd, short evtype, void *arg)
{
    printf("write_cb\n");

    static int num = 2;
    if(num > 1) {
        write(pp[1], "hello", 5);

        /* In event_base, there is no evw again. If EV_PERSIST is set, get value 4.*/
        assert(event_pending(evw, EV_WRITE, NULL) == 0);    /* evw has been active */
        assert(event_pending(evr, EV_READ, NULL));  /* enr is pending */

        event_active(evw, EV_WRITE, 1);     /* 3rd param abandon. */
        assert(event_pending(evw, EV_WRITE, NULL));
        num--;
    }
}

int
main()
{
    pipe(pp);
    struct event_base *base = event_base_new();

    evr = event_new(base, pp[0], EV_READ | EV_PERSIST, read_cb, NULL);
    evw = event_new(base, pp[1], EV_WRITE, write_cb, NULL);

    struct timeval tv = {1,0};
    event_add(evw, &tv);
    event_add(evr, &tv);

    event_base_dispatch(base);

    return 0;
}

