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
	pipe(pp);
	base = event_base_new();
	char * bb = event_base_get_method(base);	/* Get the kernel event notification mechanism used by Libevent. */
	printf("%s\n", bb);		/* epoll */
	int cnt = event_base_get_features(base);

	if(cnt & EV_FEATURE_O1) {
		printf("having one event triggered among many is [approximately] an O(1) operation.\n");
	}
	if(cnt & EV_FEATURE_O1) {
		printf("allows edge-triggered events with EV_ET");
	}

	if(cnt & EV_FEATURE_FDS) {
		printf("allows file descriptors as well as sockets. \n");
	}
	return 0;
}

