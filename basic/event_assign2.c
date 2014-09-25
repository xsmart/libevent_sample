#include <event2/event.h>
#include <event2/event_struct.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int pp[2];

void cb(evutil_socket_t fd, short what, void *ptr)
{
    /* We pass 'NULL' as the callback pointer for the heap allocated
     * event, and we pass the event itself as the callback pointer
     * for the stack-allocated event. */
    struct event *ev = ptr;
    if (ev){
    	printf("wirte_cb called\n");
        write(fd, "12345", 5);

    }else {
    	char buf[1024];
    	int ret = read(fd, buf, 1024);
    	buf[ret]='\0';
    	printf("read_cb %s\n", buf);

        event_debug_unassign(ev);		/* even if ev is persist. */
    }
}

/* Here's a simple mainloop that waits until fd1 and fd2 are both
 * ready to read. */
void mainloop(evutil_socket_t fd1, evutil_socket_t fd2, int debug_mode)
{
    struct event_base *base;
    struct event event_on_stack, *event_on_heap;

    if (debug_mode)
       event_enable_debug_mode();

    base = event_base_new();

    event_on_heap = event_new(base, fd1, EV_READ, cb, NULL);
    event_assign(&event_on_stack, base, fd2, EV_WRITE | EV_PERSIST, cb, &event_on_stack);

    event_add(event_on_heap, NULL);
    event_add(&event_on_stack, NULL);

    event_base_dispatch(base);

    event_free(event_on_heap);
    event_base_free(base);
}

int main() {

        pipe(pp);
        mainloop(pp[0], pp[1], 1);
}

