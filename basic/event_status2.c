#include <event.h>
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

	assert(event_pending(evr, EV_READ, NULL) == 0);		/* being active. */
}

void
write_cb(evutil_socket_t fd, short evtype, void *arg) {
	printf("write_cb\n");

	static int num = 2;
	if(num > 0) {
		write(pp[1], "hello", 5);
		assert(event_pending(evw, EV_WRITE, NULL) == 0);	/*being active.*/
		assert(event_pending(evr, EV_READ, NULL) == 0);
		event_active(evr, EV_READ, 1);	/* evr from non-pending to pending.*/
		assert(event_pending(evr, EV_READ, NULL) != 0);
		event_active(evw, EV_WRITE, 1);	/* evw from non-pending to pending. */
		num--;
	}

	//assert(event_pending(evr, EV_READ, NULL) == 1);
}

int
main() {
	pipe(pp);
	struct event_base *base = event_base_new();
	evr = event_new(base, pp[0], EV_READ | EV_TIMEOUT | EV_PERSIST, read_cb, NULL);	/* EV_PERSIST make event pending always, except at callback function. */
	evw = event_new(base, pp[1], EV_WRITE, write_cb, NULL);

	struct timeval tv = {1,0};
	event_add(evw, NULL);

	event_base_dispatch(base);

	return 0;
}

