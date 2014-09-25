#include <event.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

int pp[2];
struct event evw;
struct event evr;

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

	static bool bfirst = true;
	if(bfirst == true) {
		write(pp[1], "hello", 5);
		bfirst = false;
	}
}

int
main() {
	pipe(pp);
	struct event_base *base = event_base_new();
	event_assign(&evr, base, pp[0], EV_READ, read_cb, NULL);	/* event_new is recommended.  */
	event_assign(&evw, base, pp[1], EV_WRITE, write_cb, NULL);

	event_add(&evw, NULL);
	event_add(&evr, NULL);

	event_base_dispatch(base);

	return 0;
}

