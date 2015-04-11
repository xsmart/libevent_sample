#include <event2/event.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

int pp[2];
struct event *evw;
struct event *evr;
struct event_base *base;
FILE *f = NULL;

void
read_cb(evutil_socket_t fd, short evtype, void *arg) {
	printf("read_cb\n");
	char buf[1024];
	int ret = read(fd, buf, 1024);
	buf[ret] = '\0';
    printf ("read:<%s>\n", buf);

}

void
write_cb(evutil_socket_t fd, short evtype, void *arg) {
	printf("write_cb\n");
    /* Writes a human-readable description of all inserted and/or active events. */
    /* must be called after dispatch */
    event_active (evr, EV_READ, 1);
	event_base_dump_events(base, f);	
    write(pp[1], "hello", 5);
}

int
main() {
	f = fopen("/tmp/log", "w");
	pipe(pp);
	base = event_base_new();
	evr = event_new(base, pp[0], EV_READ, read_cb, NULL);
	evw = event_new(base, pp[1], EV_WRITE, write_cb, NULL);

    /* the callback will be called after you input data on STDIN */
	struct event *oev = event_new(base, STDIN_FILENO, EV_READ, read_cb, NULL);

	event_add(evw, NULL);
	event_add(evr, NULL);
	event_add(oev, NULL);

	event_base_dispatch(base);

	return 0;
}

