#include <stdio.h>
#include <event2/event.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <signal.h>

int pp[2];
struct event *evwrite;
struct event *evread;
struct event *evtimeout;
struct event_base *base;

void
read_cb(evutil_socket_t fd, short evtype, void *arg) {
    printf("read_cb\n");
    assert (pp[0] == fd);
    assert (evtype == EV_READ);
    assert (NULL == arg);

    char buf[1024];
    int ret = read(fd, buf, 1024);
    buf[ret] = '\0';
    printf ("read <%s>\n", buf);
}

void
write_cb(evutil_socket_t fd, short evtype, void *arg) {
    printf("write_cb invoked. \n");

    write(pp[1], "hello", 5);

#if 1
    static int first=1;
    if (first) {
        printf("active\n");
        event_base_active_by_fd(base, fd, EV_WRITE);    /* dose not work */
        first=0;
    }
#endif
}

int
main() {
    pipe(pp);

    base = event_base_new();
    evread  = event_new(base, pp[0], EV_READ|EV_PERSIST, read_cb, NULL);
    evtimeout = event_new (base, pp[0], EV_TIMEOUT, write_cb, NULL);
    evwrite = event_new(base, pp[1], EV_WRITE, write_cb, NULL);

    event_add(evwrite, NULL);
    event_add(evread, NULL);
    struct timeval tv = {1,0};
    event_add(evtimeout, &tv);

    event_base_dispatch(base);

    event_free (evread);
    event_free (evwrite);
    event_free (evtimeout);

    event_base_free (base);

    return 0;
}
