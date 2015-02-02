#include <stdbool.h>
#include <assert.h>
#include <event.h>
#include <sys/types.h>
#include <sys/syscall.h>

int pp[2];
struct event *evw;
struct event *evr;
struct event_base *base;

void
read_cb(evutil_socket_t fd, short evtype, void *arg) {
        printf("thread : %d, read_cb\n", syscall(SYS_gettid));
        /*
        char buf[1024];
        int ret = read(fd, buf, 1024);
        buf[ret] = '\0';
        printf("read == %s\n", buf);
        */
}

void
write_cb(evutil_socket_t fd, short evtype, void *arg) {
        printf("thread : %d, write_cb\n", syscall(SYS_gettid));

        static int num = 2;
        if(num > 0) {
                write(pp[1], "hello", 5);
                /* In event_base, there is no evw again. If EV_PERSIST is set, get value 4.*/
                event_active(evw, EV_WRITE, 1); /* evw from non-pending to pending. */
                num--;
        }

        //assert(event_pending(evr, EV_READ, NULL) == 1);
}

int
main() {
        pipe(pp);
        base = event_base_new();
        evr = event_new(base, pp[0], EV_READ , read_cb, NULL); /* EV_PERSIST make event pending always, except at callback function. */

        struct timeval tv = {0,0};
        event_add(evr, &tv);

        event_base_loop(base, EVLOOP_NONBLOCK);
        return 0;
}

