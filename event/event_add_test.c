#include <stdbool.h>
#include <assert.h>
#include <event2/event.h>
#include <sys/types.h>
#include <sys/syscall.h>

int pp[2];
struct event *evw;
struct event *evr;
struct event_base *base;

/* It will be called right now */
void
read_cb(evutil_socket_t fd, short evtype, void *arg) {
    printf("thread : %d, read_cb\n", syscall(SYS_gettid));
}

int main() {
    pipe(pp);
    base = event_base_new();
    evr = event_new(base, pp[0], EV_READ , read_cb, NULL);

    /* different from NULL */
    struct timeval tv = {0,0};
    event_add(evr, &tv);

    event_base_loop(base, EVLOOP_NONBLOCK);
    return 0;
}

