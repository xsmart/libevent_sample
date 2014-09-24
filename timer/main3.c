#include <event.h>
#include <assert.h>

void
timer_cb(evutil_socket_t fd, short event, void *arg) {
	printf("hello\n");
}

int
main(void) {

	struct timeval tv1;
	tv1.tv_sec=2;
	tv1.tv_usec=200;

	struct timeval tv2;
	tv2.tv_sec=8;
	tv2.tv_usec=800;

	struct timeval tv3;

	evutil_timeradd(&tv1, &tv2, &tv3);
	assert(tv3.tv_sec == 10);
	assert(tv3.tv_usec == 1000);

	evutil_timersub(&tv2, &tv1, &tv3);
	assert(tv3.tv_sec == 6);
	assert(tv3.tv_usec == 600);

	evutil_timerclear(&tv3);
	assert(tv3.tv_sec == 0);
	assert(tv3.tv_usec == 0);

	assert(evutil_timercmp(&tv1, &tv2, <) != 0 );
	assert(evutil_timerisset(&tv1) != 0);
}

