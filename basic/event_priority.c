#include <stdio.h>
#include <event.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

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
}

void
write_cb(evutil_socket_t fd, short evtype, void *arg) {
        printf("write_cb\n");

        /* when callback is executed, event are active. */

        /* just write once */
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
        evr = event_new(base, pp[0], EV_READ, read_cb, NULL);
        evw = event_new(base, pp[1], EV_WRITE, write_cb, NULL);

        event_add(evw, NULL);
        event_add(evr, NULL);

        write(pp[1], "-----", 5);
        event_base_priority_init(base, 20);		/* all event should set priority, or you may free the default priority is 0. */
        event_priority_set(evr, 0);
        event_priority_set(evw, 19);	/* not 20 */
        event_base_dispatch(base);

        return 0;
}


