#include <stdio.h>
#include <event.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

int pp[2];
struct event *evw;
struct event *evr;
struct event_base *base;

void
read_cb(evutil_socket_t fd, short evtype, void *arg) {
}

void
write_cb(evutil_socket_t fd, short evtype, void *arg) {
	struct timeval tv1;
	struct timeval tv2;
	struct timeval tv3;
	sleep(2);
	event_base_gettimeofday_cached(base, &tv1);
	evutil_gettimeofday(&tv2, NULL);

	evutil_timersub(&tv2, &tv1, &tv3);
	printf("%d(s), %d(us)\n", tv3.tv_sec, tv3.tv_usec);
}

int
main() {
        pipe(pp);
        base = event_base_new();
        evr = event_new(base, pp[0], EV_READ, read_cb, NULL);
        evw = event_new(base, pp[1], EV_WRITE, write_cb, NULL);

        event_add(evw, NULL);
        event_add(evr, NULL);

        event_base_dispatch(base);

        return 0;
}

