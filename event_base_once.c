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

        /* test event_active and event_pending function. */
        if(event_pending(evw, EV_WRITE, NULL) == 0) {
                printf("evw being pending, make it active now.\n");
                event_active(evw, EV_WRITE, 1); /* no matter the event is pending nor non-pending. */
        }
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

        event_base_once(base, pp[0], EV_READ, read_cb, NULL, NULL);		/* must not specify EV_PERSIST */
        event_base_once(base, pp[1], EV_WRITE, write_cb, NULL, NULL);

        event_base_dispatch(base);

        return 0;
}


