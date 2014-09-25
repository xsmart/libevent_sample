#include <event.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

int pp[2];
struct event *evw;
struct event *evr;
struct event_base *base;

void
read_cb(evutil_socket_t fd, short evtype, void *arg) {
	printf("read_cb\n");
	char buf[1024];
	int ret = read(fd, buf, 1024);
	buf[ret] = '\0';
	printf("read == %s\n", buf);

}

void
write_cb(evutil_socket_t fd, short evtype, void *arg) {
	printf("write_cb\n");

	static int num = 1;
	if(num > 0) {
		write(pp[1], "hello", 5);
		num--;
	}
}

int
main() {
	FILE *f = fopen("/tmp/log", "w");
	pipe(pp);
	base = event_base_new();
	evr = event_new(base, pp[0], EV_READ, read_cb, NULL);
	evw = event_new(base, pp[1], EV_WRITE, write_cb, NULL);
	struct event *oev = event_new(base, STDIN_FILENO, EV_READ, read_cb, NULL);

	struct timeval tv = {1,0};
	event_add(evw, NULL);
	event_add(evr, NULL);
	event_add(oev, NULL);

	event_base_dump_events(base, f);	/* Writes a human-readable description of all inserted and/or active events. */
	event_base_dispatch(base);

	return 0;
}

