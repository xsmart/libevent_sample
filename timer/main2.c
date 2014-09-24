#include <event.h>
#include <event_struct.h>
void
timer_cb(evutil_socket_t fd, short event, void *arg) {
	printf("hello\n");
}

int
main(void) {
	struct event_base * base = event_base_new();

	struct event ev;
	event_assign(&ev, base, -1, EV_TIMEOUT | EV_PERSIST, timer_cb, &ev);

	struct timeval tv;
	tv.tv_sec=1;
	tv.tv_usec=0;
	evtimer_add(&ev, &tv);
	event_base_dispatch(base);
}

