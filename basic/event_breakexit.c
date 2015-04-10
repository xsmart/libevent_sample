#include <stdio.h>
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

        /* when callback is executed, event are active. */

        /* just write once */
        static bool bfirst = true;
        if(bfirst == true) {
                write(pp[1], "hello", 5);
                bfirst = false;
        }

        struct timeval tv = {1,0};
        event_base_loopexit(base, &tv);	/*NULL==tv, exit after running all currently active events. */

//        assert(event_base_got_break(base) == 0);
//        event_base_loopbreak(base);
//        assert(event_base_got_break(base) != 0);	/* read_cb have no chance to invoke. */


}

int
main() {
        pipe(pp);
        base = event_base_new();
        evr = event_new(base, pp[0], EV_READ | EV_PERSIST, read_cb, NULL);
        evw = event_new(base, pp[1], EV_WRITE, write_cb, NULL);

        event_add(evw, NULL);
        event_add(evr, NULL);

        event_base_dispatch(base);
        printf("done\n");
        return 0;
}

