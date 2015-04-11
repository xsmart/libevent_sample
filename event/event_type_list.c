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
struct event *evsignal;

void
read2_cb(evutil_socket_t fd, short evtype, void *arg) {
    printf("read2222_cb\n");
    assert (pp[0] == fd);
    assert (evtype == EV_READ | EV_ET);
    assert (NULL == arg);
    
    char buf[4];
    int ret = read(fd, buf, 3);
    buf[ret] = '\0';
    printf ("read <%s>\n", buf);

}

void
read_cb(evutil_socket_t fd, short evtype, void *arg) {
    printf("read_cb\n");
    assert (pp[0] == fd);
    assert (evtype == EV_READ);
    assert (NULL == arg);

    char buf[4];
    int ret = read(fd, buf, 3);
    buf[ret] = '\0';
    printf ("read <%s>\n", buf);
}

void
write_cb(evutil_socket_t fd, short evtype, void *arg) {
    printf("write_cb invoked. \n");
    assert (pp[1] == fd);
    assert (evtype == EV_WRITE);
    assert (NULL == arg);

    write(pp[1], "hello", 5);
}

void
timeout_cb(evutil_socket_t fd, short evtype, void *arg) {
    printf ("timeout_cb invoked\n");
    assert (fd == -1);
    assert (evtype==EV_TIMEOUT);
    assert (NULL == arg);
}

void
signal_int_cb (evutil_socket_t fd, short evtype, void *arg) {
    printf ("signal_int_cb invoked\n");
    assert (fd == SIGINT);
    assert (evtype==EV_SIGNAL);
    assert (NULL == arg);
}

int
main() {
    pipe(pp);

    struct event_base *base = event_base_new();
//    evread  = event_new(base, pp[0], EV_READ | EV_PERSIST, read_cb, NULL);
    evread = event_new (base, pp[0], EV_READ | EV_PERSIST |EV_ET, read2_cb, NULL);
    evwrite = event_new(base, pp[1], EV_WRITE, write_cb, NULL);

    evtimeout = event_new (base, -1, EV_TIMEOUT | EV_PERSIST, timeout_cb, NULL);

    evsignal  = event_new (base, SIGINT, EV_SIGNAL, signal_int_cb, NULL);

    event_add(evwrite, NULL);
    event_add(evread, NULL);
    struct timeval tv = {1,0};
    event_add(evtimeout, &tv);
    event_add(evsignal, NULL);


    event_base_dispatch(base);

    event_free (evread);
    event_free (evwrite);
    event_free (evtimeout);
    event_free (evsignal);

    event_base_free (base);

    return 0;
}
